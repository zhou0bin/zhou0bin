#include "ProcessExecutor.h"

#include <QFile>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QProcess>
#include <QThread>

#include <iostream>
#include <iomanip>
#include <vector>
#include <string>
#include <cstring>
#include <cstdint>

#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <termios.h>
#include <sys/select.h>

using namespace std;

#define BAUDRATE B9600

#define DEVICE "/dev/ttyUSB0"

#define SLAVE 0xFF

//--------------------------------------------------
// CRC16
//--------------------------------------------------

uint16_t CRC16(const uint8_t *buf,int len)
{
    uint16_t crc=0xFFFF;

    for(int pos=0;pos<len;pos++)
    {
        crc^=buf[pos];

        for(int i=0;i<8;i++)
        {
            if(crc&1)
            {
                crc>>=1;
                crc^=0xA001;
            }
            else
            {
                crc>>=1;
            }
        }
    }

    return crc;
}

void AppendCRC(vector<uint8_t> &cmd)
{
    uint16_t crc=CRC16(cmd.data(),cmd.size());

    cmd.push_back(crc&0xff);
    cmd.push_back((crc>>8)&0xff);
}

bool CheckCRC(const vector<uint8_t> &rx)
{
    if(rx.size()<3)
        return false;

    uint16_t recv=
            rx[rx.size()-2]
          | (rx[rx.size()-1]<<8);

    uint16_t calc=
            CRC16(rx.data(),rx.size()-2);

    return recv==calc;
}

void Dump(const vector<uint8_t> &buf)
{
    for(auto b:buf)
        printf("%02X ",b);

    printf("\n");
}

//--------------------------------------------------
// F620 Parse
//--------------------------------------------------

uint16_t ParseWord(const uint8_t *p)
{
    return p[0] | (p[1] << 8);
}

// 文档要求
// 36 23 06 00
// ->
// 00062336

uint32_t ParseDWord(const uint8_t *p)
{
    return
            ((uint32_t)p[3]<<24)
          | ((uint32_t)p[2]<<16)
          | ((uint32_t)p[1]<<8)
          | ((uint32_t)p[0]);
}

double ParseValue(const uint8_t *p)
{
    return ParseDWord(p)/1000.0;
}

//--------------------------------------------------
// Serial Port
//--------------------------------------------------

class SerialPort
{
public:

    SerialPort()
    {
        fd = -1;
    }

    ~SerialPort()
    {
        closePort();
    }

    bool openPort(const string &dev = DEVICE)
    {
        fd = open(dev.c_str(), O_RDWR | O_NOCTTY);

        if(fd < 0)
        {
            perror("open");
            return false;
        }

        struct termios tty;

        memset(&tty, 0, sizeof(tty));

        if(tcgetattr(fd, &tty) != 0)
        {
            perror("tcgetattr");
            return false;
        }

        cfsetospeed(&tty, BAUDRATE);
        cfsetispeed(&tty, BAUDRATE);

        // 8 data bits
        tty.c_cflag &= ~CSIZE;
        tty.c_cflag |= CS8;

        // Even parity
        tty.c_cflag |= PARENB;
        tty.c_cflag &= ~PARODD;

        // 1 stop bit
        tty.c_cflag &= ~CSTOPB;

        tty.c_cflag |= CLOCAL;
        tty.c_cflag |= CREAD;

        tty.c_iflag = INPCK;
        tty.c_oflag = 0;
        tty.c_lflag = 0;

        tty.c_cc[VTIME] = 1;
        tty.c_cc[VMIN] = 0;

        tcflush(fd, TCIFLUSH);

        if(tcsetattr(fd, TCSANOW, &tty) != 0)
        {
            perror("tcsetattr");
            return false;
        }

        cout << "Open Serial : " << dev << endl;

        return true;
    }

    void closePort()
    {
        if(fd >= 0)
        {
            close(fd);
            fd = -1;
        }
    }

    bool send(const vector<uint8_t> &tx)
    {
        if(fd < 0)
            return false;

        tcflush(fd, TCIOFLUSH);

        int ret = write(fd, tx.data(), tx.size());

        if(ret != (int)tx.size())
            return false;

        tcdrain(fd);

        return true;
    }

    bool recv(vector<uint8_t> &rx, int timeout = 1000)
    {
        rx.clear();

        uint8_t buf[256];

        while (true)
        {
            fd_set rfds;
            FD_ZERO(&rfds);
            FD_SET(fd, &rfds);

            struct timeval tv;
            tv.tv_sec = timeout / 1000;
            tv.tv_usec = (timeout % 1000) * 1000;

            int ret = select(fd + 1, &rfds, nullptr, nullptr, &tv);

            if (ret <= 0)
                break;

            int len = read(fd, buf, sizeof(buf));

            if (len > 0)
            {
                rx.insert(rx.end(), buf, buf + len);

                // 根据功能码判断是否收完整
                if (rx.size() >= 3)
                {
                    if (rx[1] == 0x03)
                    {
                        int expect = 3 + rx[2] + 2;

                        if ((int)rx.size() >= expect)
                            return true;
                    }
                    else if (rx[1] == 0x05)
                    {
                        // 你的F620写命令ACK只有6字节
                        if (rx.size() >= 6)
                            return true;
                    }
                }
            }

            usleep(1000);   // 1ms
        }

        return !rx.empty();
    }

    bool transceive(const vector<uint8_t> &tx,
                    vector<uint8_t> &rx,
                    int timeout = 1000)
    {
        cout << "TX : ";
        Dump(tx);

        if(!send(tx))
        {
            cout << "Send Failed" << endl;
            return false;
        }

        if(!recv(rx, timeout))
        {
            cout << "Receive Timeout" << endl;
            return false;
        }

        cout << "RX : ";
        Dump(rx);

        if(rx.size() >= 8)
        {
            if(!CheckCRC(rx))
            {
                cout << "CRC ERROR" << endl;
                return false;
            }
        }

        return true;
    }

private:

    int fd;
};

//--------------------------------------------------
// Unit
//--------------------------------------------------

string PressureUnitName(uint32_t unit)
{
    switch(unit)
    {
        case 10000: return "bar";
        case 11000: return "MPa";
        case 12000: return "kPa";
        default:
            return "Unknown(" + to_string(unit) + ")";
    }
}

string LeakUnitName(uint32_t unit)
{
    switch(unit)
    {
        case 6000:
            return "Pa";

        case 7000:
            return "Pa/min";

        case 8000:
            return "Pa/s";

        case 9000:
            return "kPa/s";

        default:
            return "Unknown(" + to_string(unit) + ")";
    }
}

//--------------------------------------------------
// F620
//--------------------------------------------------

class F620
{
public:

    explicit F620(SerialPort &sp):serial(sp)
    {

    }

    //--------------------------------------------------
    // 选择程序(PR0~PR99)
    //--------------------------------------------------
    bool selectProgram(uint16_t program)
    {
        vector<uint8_t> cmd=
        {
            SLAVE,
            0x10,
            0x02,
            0x00,
            0x00,
            0x01,
            0x02
        };

        cmd.push_back(program&0xff);
        cmd.push_back((program>>8)&0xff);

        AppendCRC(cmd);

        vector<uint8_t> rx;

        cout<<"Select Program "<<program<<endl;

        return serial.transceive(cmd,rx);
    }

    //--------------------------------------------------
    // 启动
    //--------------------------------------------------
    bool start()
    {
        vector<uint8_t> cmd=
        {
            SLAVE,
            0x05,
            0x00,
            0x01,
            0xFF,
            0x00
        };

        AppendCRC(cmd);

        vector<uint8_t> rx;

        cout<<"Start Test"<<endl;

        return serial.transceive(cmd,rx);
    }

    //--------------------------------------------------
    // 停止
    //--------------------------------------------------
    bool stop()
    {
        vector<uint8_t> cmd=
        {
            SLAVE,
            0x05,
            0x00,
            0x00,
            0xFF,
            0x00
        };

        AppendCRC(cmd);

        vector<uint8_t> rx;

        return serial.transceive(cmd,rx);
    }

    //--------------------------------------------------
    // 状态
    //--------------------------------------------------
    bool readStatus(uint16_t &status)
    {
        vector<uint8_t> cmd=
        {
            SLAVE,
            0x03,
            0x22,
            0x04,
            0x00,
            0x01
        };

        AppendCRC(cmd);

        vector<uint8_t> rx;

        if(!serial.transceive(cmd,rx))
            return false;

        if(rx.size()<7)
            return false;

//        status = rx[3] | (rx[4] << 8);
        status = ParseWord(&rx[3]);

        return true;
    }

    //--------------------------------------------------
    // 等待结束
    //--------------------------------------------------
    bool waitFinish()
    {
        cout << "Waiting..." << endl;

        while(true)
        {
            uint16_t status;

            if(!readStatus(status))
            {
                usleep(300000);
                continue;
            }

            printf("STATUS : %04X\n", status);

            // 8020：测试中止
            if(status == 0x8020)
            {
                cout << "Test Abort" << endl;
                return false;
            }

            // bit5=1：测试结束
            if(status & (1 << 5))
            {
                cout << "Test Finished" << endl;
                return true;
            }

            usleep(500000);
        }
    }

    //--------------------------------------------------
    // Last Result
    //--------------------------------------------------
    bool readLastResult()
    {
        vector<uint8_t> cmd=
        {
            SLAVE,
            0x03,
            0x00,
            0x11,
            0x00,
            0x0C
        };

        AppendCRC(cmd);

        vector<uint8_t> rx;

        if(!serial.transceive(cmd,rx,2000))
            return false;

        Dump(rx);

//        if(rx.size()<29)
//            return false;

        if (rx.size() < 29)   // 3 + 24 + 2
            return false;

        if (rx[1] != 0x03)
            return false;

        if (rx[2] != 0x18)
            return false;

        uint16_t program=
                ParseWord(&rx[3]);

        uint16_t mode=
                ParseWord(&rx[5]);

        uint16_t result=
                ParseWord(&rx[7]);

        uint16_t alarm=
                ParseWord(&rx[9]);

        uint32_t pressure=
                ParseDWord(&rx[11]);

        uint32_t pressureUnit=
                ParseDWord(&rx[15]);

        uint32_t leak=
                ParseDWord(&rx[19]);

        uint32_t leakUnit=
                ParseDWord(&rx[23]);

        cout<<endl;
        cout<<"============================"<<endl;

        cout<<"Program : PR"<<program<<endl;

        cout<<"Mode    : "<<mode<<endl;

        //--------------------------------------------------
        // Alarm优先
        //--------------------------------------------------

        if(alarm!=0)
        {
            cout<<"Result  : ALARM"<<endl;
            cout<<"Alarm   : "<<alarm<<endl;

            return true;
        }

        //--------------------------------------------------
        // Result
        //--------------------------------------------------

        if(result&0x01)
            cout<<"Result  : PASS"<<endl;

        else if(result&0x02)
            cout<<"Result  : FAIL"<<endl;

        else if(result&0x04)
            cout<<"Result  : FAIL"<<endl;

        else if(result&0x08)
            cout<<"Result  : ALARM"<<endl;

        else
            cout<<"Result  : UNKNOWN"<<endl;

            cout<<fixed<<setprecision(3);

            cout
            <<"Pressure : "
            <<pressure/1000.0
            <<" "
            <<PressureUnitName(pressureUnit)
            <<endl;

            cout
            <<"Leak     : "
            <<leak/1000.0
            <<" "
            <<LeakUnitName(leakUnit)
            <<endl;

//        cout<<"Pressure Unit : "<<pressureUnit<<endl;

//        cout<<"Leak Unit     : "<<leakUnit<<endl;

        cout<<"============================"<<endl;

        if(result&0x01)
            return true;
        else
            return false;
    }

private:

    SerialPort &serial;

};


ProcessExecutor::ProcessExecutor(QObject *parent)
    : QObject(parent)
{
    canCommPro = new CanCommPCAN();
    connect(canCommPro, &CanCommPCAN::messageReceived, this, &ProcessExecutor::on_receiveMessage0);

    load("process.json");
}

struct UdsParseResult
{
    bool    ok = false;        // 是否解析成功
    quint16 did = 0;           // DID，例如 0xF195
    QString version;           // 版本号（仅 F195 有）
    QString rawHex;            // 原始 0xXXYYZZDD
    QString error;             // 错误原因（调试用）
};


UdsParseResult parseUdsSingleFrame0(const QByteArray &data)
{
    qDebug() << "...... ......version data:" << data.toHex();

    UdsParseResult result;

    // ===== 1. 基本长度校验 =====
    if (data.size() < 8) {
        result.error = "Frame length < 8";
        return result;
    }

    // ===== 2. ISO-TP SF 校验 =====
    quint8 pci = (quint8)data[0];
    quint8 frameType = (pci & 0xF0) >> 4;
    quint8 udsLen    = (pci & 0x0F);

    // 只支持 Single Frame
    if (frameType != 0x0) {
        result.error = "Not Single Frame";
        return result;
    }

    // UDS Payload 至少要有 SID + DID
    if (udsLen < 3) {
        result.error = "UDS length too short";
        return result;
    }

    // ===== 3. UDS 响应校验 =====
    quint8 sid = (quint8)data[1];
    if (sid != 0x62) {
        result.error = "Not ReadDataByIdentifier response";
        return result;
    }

    quint16 did = ((quint8)data[2] << 8) | (quint8)data[3];
    result.did = did;

    // ===== 4. 按 DID 解析 =====
    switch (did)
    {
    case 0xF195:   // 软件版本
    {
        // 62 F1 95 XX YY ZZ DD → 共 7 字节
        if (udsLen != 7) {
            result.error = "Invalid F195 length";
            return result;
        }

        quint8 xx = (quint8)data[4];
        quint8 yy = (quint8)data[5];
        quint8 zz = (quint8)data[6];
        quint8 dd = (quint8)data[7];

        result.version = QString("%1.%2.%3.%4")
                             .arg(xx)
                             .arg(yy)
                             .arg(zz)
                             .arg(dd);

        result.rawHex = QString("0x%1%2%3%4")
                            .arg(xx, 2, 16, QChar('0'))
                            .arg(yy, 2, 16, QChar('0'))
                            .arg(zz, 2, 16, QChar('0'))
                            .arg(dd, 2, 16, QChar('0'))
                            .toUpper();

        result.ok = true;
        break;
    }

    default:
        result.error = QString("Unsupported DID: 0x%1")
                           .arg(did, 4, 16, QChar('0'))
                           .toUpper();
        break;
    }

    return result;
}

void ProcessExecutor::on_receiveMessage0(quint32 canId, const QByteArray &message)
{
    qDebug() << QString("✅ 接收 CAN-FD 成功: ID=0x%1 LEN=%2 DATA=%3")
                .arg(canId, 3, 16, QLatin1Char('0'))
                .arg(message.size())
                .arg(QString(message.toHex(' ').toUpper()));
    if (canId == waitCanid)
    {
        qDebug()<< "ghfdhfdgkd---------------------";
        UdsParseResult res = parseUdsSingleFrame0(message);

        if (res.ok)
        {
            qDebug()<< "ghfdhfdgkd---------------------ok";
            m_waitVersion = res.version;
            m_waitVersionReady = true;
        }
    }
}


void ProcessExecutor::load(QString filePath)
{
    QFile file(filePath);
    if(!file.open(QIODevice::ReadOnly))
        return;

    QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
    QJsonArray assemblies = doc.object()["assemblies"].toArray();

    processMap.clear();

    for(const QJsonValue &a : assemblies)
    {
        QJsonObject ao = a.toObject();
        QString name = ao["name"].toString();

        QList<ProcessStep> stepList;

        for(const QJsonValue &s : ao["steps"].toArray())
        {
            QJsonObject so = s.toObject();

            ProcessStep step;
            step.id = so["id"].toInt();
            step.name = so["name"].toString();
            step.type = so["type"].toString();

            QJsonArray cmds = so["cmd"].toArray();
            for(const QJsonValue &c : cmds)
            {
                QJsonObject co = c.toObject();

                CanCmd cmd;
                cmd.bus = co["bus"].toString();
                cmd.id = co["id"].toString();
                cmd.signalMap = co["signals"].toObject();
                cmd.action = co["action"].toString();
                cmd.shell = co["bash"].toString();
                cmd.ms = co["ms"].toInt();

                step.cmds.append(cmd);
            }

            stepList.append(step);
        }

        processMap[name] = stepList;
    }

//    qDebug() << "==== processMap keys ====";
//    for(auto it = processMap.begin(); it != processMap.end(); ++it)
//    {
//        qDebug() << "[" << it.key() << "]";
//    }
}

void ProcessExecutor::runAssembly(
        const QString &assemblyName)
{
    qDebug()
            << "runAssembly:"
            << assemblyName;

    if(!processMap.contains(
                assemblyName))
    {
        qDebug()
                << "assembly not found";

        return;
    }

    m_stopFlag = false;

    const QList<ProcessStep> &steps =
            processMap[assemblyName];

    emit log(
        QString("Start Assembly: %1")
        .arg(assemblyName));

    for(const ProcessStep &step : steps)
    {
        if(m_stopFlag)
        {
            emit log(
                "Assembly stopped");

            break;
        }

        emit stepStarted(
                    step.id,
                    step.name);

        runStep(step);
    }

    emit log(
        QString("Finish Assembly: %1")
        .arg(assemblyName));
}

void ProcessExecutor::runStep(
        const ProcessStep &step)
{
    emit log(
                QString("Step %1 : %2")
                .arg(step.id)
                .arg(step.name));

    int ok = 1;
    if(step.type == "NONE")
    {
        ok = 2;
    }else{
        for(const CanCmd &cmd : step.cmds)
        {
            //--------------------------------
            // CAN
            //--------------------------------

            if(cmd.action == "can")
            {
                if(executeCan(cmd) == 0)
                    ok = 0;
                else if(executeCan(cmd) == 3)
                    ok = 3;
            }

            //--------------------------------
            // SLEEP
            //--------------------------------

            else if(cmd.action == "sleep")
            {
                QThread::msleep(cmd.ms);
            }

            //--------------------------------
            // SHELL
            //--------------------------------

            else if(cmd.action == "shell")
            {
                if(!executeShell(cmd))
                    ok = 0;
                else{
//                    switch (cmd.id.toUInt(nullptr,16)) {
//                    case 9:
//                        emit picOutput("video9-001.raw", 3000, 3190);
//                        break;
//                    default:
//                        break;
//                    }
                    if(cmd.id.toUInt() != 99 && cmd.id.toUInt() != 88 && cmd.id.toUInt() != 77){
                        QString fileName = QString("/home/autox/platsw/tools/camera/video%1-001.raw")
                                               .arg(cmd.id);
                        int width  = cmd.signalMap["width"].toInt();
                        int height = cmd.signalMap["height"].toInt();

                        emit picOutput(fileName, width, height);
                    }
                }
            }
        }
    }

    if(ok == 1){
        emit stepFinished(step.id,step.name,"SUBMIT");
    }else if(ok == 0){
        emit stepFinished(step.id,step.name,"FAIL");
    }else if(ok == 2){
        emit stepFinished(step.id,step.name,"SKIP");
    }else if(ok == 3){
        emit stepFinished(step.id,step.name,m_waitVersion);
    }
}

QByteArray buildCanFrameBySignal(int startBit, int bitLength, const QByteArray &data, int frameLen)
{
    QByteArray frame(frameLen, 0x00);

    if (bitLength % 8 != 0) {
        qWarning() << "bitLength must be multiple of 8";
        return frame;
    }

    int startByte = startBit / 8;
    int byteCount = bitLength / 8;

    for (int i = 0; i < byteCount && i < data.size(); ++i) {
        int idx = startByte + i;
        if (idx >= frameLen)
            break;
        frame[idx] = data[i];
    }

    return frame;
}

bool ProcessExecutor::executeCan(
        const CanCmd &cmd)
{
    uint32_t canId =
            cmd.id.toUInt(
                nullptr,
                16);

    int ret = 0;

    switch(canId)
    {
        case 0x443:
        {
            Cdc_15_443 msg{};
            Cdc15443_Reset(&msg);

            for (auto it = cmd.signalMap.begin(); it != cmd.signalMap.end(); ++it)
            {
                const QString signalName = it.key();
                const QJsonValue signalValue = it.value();

                SignalMapper443::SetSignal(msg, signalName.toStdString(), signalValue);
            }

            Cdc15443_UpdateData(&msg);
            QByteArray byteArray = QByteArray(Data443, 64);
            if(canCommPro->sendMessage(0x443, byteArray))
                ret = 1;
//            qDebug() << "----------443:"<< QString(byteArray.toHex());

            break;
        }
        case 0x680:
        {
            Cdc_control1_680 msg{};
            Cdccontrol1680_Reset(&msg);

            for (auto it = cmd.signalMap.begin(); it != cmd.signalMap.end(); ++it)
            {
                const QString signalName = it.key();
                const QJsonValue signalValue = it.value();

                SignalMapper680::SetSignal(msg, signalName.toStdString(), signalValue);
            }

            Cdccontrol1680_UpdateData(&msg);
            QByteArray byteArray = QByteArray(Data, 48);
            if(canCommPro->sendMessage(0x680, byteArray))
                ret = 1;
//            qDebug() << "----------680:"<< QString(byteArray.toHex());


            break;
        }
        case 0x683:
        {
            Cdc_control2_683 msg{};
            Cdccontrol1683_Reset(&msg);

            for (auto it = cmd.signalMap.begin(); it != cmd.signalMap.end(); ++it)
            {
                const QString signalName = it.key();
                const QJsonValue signalValue = it.value();

                SignalMapper683::SetSignal(msg, signalName.toStdString(), signalValue);
            }

            Cdccontrol1683_UpdateData(&msg);
            QByteArray byteArray = QByteArray(Data683, 48);
            if(canCommPro->sendMessage(0x683, byteArray))
                ret = 1;
//            qDebug() << "----------683:"<< QString(byteArray.toHex());

            break;
        }
        case 0x470:
        {
            for (auto it = cmd.signalMap.begin(); it != cmd.signalMap.end(); ++it)
            {
                const QJsonValue signalValue = it.value();

                if(signalValue == 3)
                {
                    QByteArray xlidar470;
                    xlidar470[2] = 0;
                    quint32 value = 3 & 0x07;
                    xlidar470[2] = xlidar470[2] | (value << 0);   // byte2 bit0~bit2

                    if(canCommPro->sendMessage(0x470, xlidar470))
                        ret = 1;
                    qDebug() << "----------470:"<< QString(xlidar470.toHex());
                    break;
                }else if(signalValue == 0){
                    QByteArray xlidar470;
                    xlidar470[2] = 0;
                    quint32 value = 1 & 0x07;
                    xlidar470[2] = xlidar470[2] |(value << 5);   // byte2 bit5~bit7（bit21~23）

                    quint32 value3 = 30 & 0xFF;
                    xlidar470[3] = static_cast<char>(value3);   // byte3 直接放 8bit

                    if(canCommPro->sendMessage(0x470, xlidar470))
                        ret = 1;
                    qDebug() << "----------470:"<< QString(xlidar470.toHex());
                    break;
                }else if(signalValue == 2){
                    QByteArray xlidar470;
                    xlidar470[2] = 0;
                    quint32 value = 2 & 0x07;
                    xlidar470[2] = xlidar470[2] | (value << 0);   // byte2 bit0~bit2

                    if(canCommPro->sendMessage(0x470, xlidar470))
                        ret = 1;
                    qDebug() << "----------470:"<< QString(xlidar470.toHex());
                    break;
                }
            }
            break;
        }
        default:
        {
            QByteArray udsData;
            udsData.append(char(0x03));  // PCI
            udsData.append(char(0x22));  // SID
            udsData.append(char(0xF1));  // DID High
            udsData.append(char(0x95));  // DID Low

            // 使用 DBC 信号构造完整 8 字节 CAN 帧
            QByteArray frame = buildCanFrameBySignal(
                7,          // 起始位
                8*8,        // 信号长度 8 字节
                udsData,
                8           // BO_ 长度
            );
            canCommPro->setPaused(false);
            if(canCommPro->sendMessageCan(canId, frame))
            {
                qDebug() << "----------"<< canId << QString(frame.toHex());
                m_waitVersionReady = false;
                m_waitVersion = " ";
                waitCanid = canId + 8;

                QElapsedTimer timer;
                timer.start();

                while(timer.elapsed() < 1000)
                {
                    QCoreApplication::processEvents();
                    qDebug() << "----------QCoreApplication";

                    if(m_waitVersionReady)
                    {
                        qDebug() << "----------m_waitVersionReady";
                        ret = 3;
                        break;
                    }

                    QThread::msleep(5);
                }
            }
            canCommPro->setPaused(true);
            break;
        }
    }

//    if(!ret)
//    {
//        QByteArray payload;
//        canCommPro->sendMessage(
//                    canId,
//                    payload);

//        emit log(
//                    QString(
//                        "send can %1")
//                    .arg(cmd.id));
//    }

    return ret;
}

bool ProcessExecutor::executeShell(const CanCmd &cmd)
{
    emit log(QString("shell: %1").arg(cmd.shell));

    if(cmd.id.toUInt() == 88)
    {
        cout<<"===================================="<<endl;
        cout<<" F620 Leak Test"<<endl;
        cout<<"===================================="<<endl;

        SerialPort serial;

        if(!serial.openPort())
        {
            cout<<"Open Serial Failed"<<endl;
            return false;
        }

        F620 tester(serial);

        //------------------------------------------------
        // 如果需要切程序
        //------------------------------------------------

        // tester.selectProgram(21);

        //------------------------------------------------
        // Start
        //------------------------------------------------

        if(!tester.start())
        {
            cout<<"Start Failed"<<endl;
            return false;
        }

        //------------------------------------------------
        // Wait Finish
        //------------------------------------------------

        if(!tester.waitFinish())
        {
            cout<<"Test Failed"<<endl;
            return false;
        }

        QThread::sleep(1);
        //------------------------------------------------
        // Read Result
        //------------------------------------------------

        if(!tester.readLastResult())
        {
            cout<<"Read Result Failed"<<endl;
            return false;
        }

        cout<<endl;
        cout<<"Done"<<endl;

        return true;
    }

    QProcess proc;

    if(cmd.id.toUInt() == 99){
        connect(&proc, &QProcess::readyReadStandardOutput,
                this, [&]()
        {
            QString out = QString::fromLocal8Bit(proc.readAllStandardOutput());
            emit shellOutput(out);
        });

        connect(&proc, &QProcess::readyReadStandardError,
                this, [&]()
        {
            QString err = QString::fromLocal8Bit(proc.readAllStandardError());
            emit shellOutput(err);      // stderr也显示到TextEdit
        });
    }

    if(cmd.id.toUInt() == 77){
        connect(&proc, &QProcess::readyReadStandardOutput,
                this, [&]()
        {
            QString outLidar = QString::fromLocal8Bit(proc.readAllStandardOutput());
            emit shellOutputLidar(outLidar);
        });

        connect(&proc, &QProcess::readyReadStandardError,
                this, [&]()
        {
            QString errLidar = QString::fromLocal8Bit(proc.readAllStandardError());
            emit shellOutputLidar(errLidar);      // stderr也显示到TextEdit
        });
    }

    proc.start("bash", QStringList() << "-c" << cmd.shell);

    if (!proc.waitForStarted(3000))
    {
        emit log("shell start failed");
        return false;
    }

    if (!proc.waitForFinished(15000))
    {
        proc.kill();
        proc.waitForFinished();

        emit log("shell timeout");
        return false;
    }

    QString out1 = proc.readAllStandardOutput();
    QString err1 = proc.readAllStandardError();

    qDebug() << "stdout =" << out1;
    qDebug() << "stderr =" << err1;

    qDebug() << proc.exitCode();
    qDebug() << proc.exitStatus();

    return proc.exitCode() == 0;
}

bool ProcessExecutor::checkExpect(
        const ProcessStep &step)
{
    Q_UNUSED(step);

    //----------------------------------
    // TODO:
    // version compare
    // signal compare
    // lidar status compare
    //----------------------------------

    return true;
}

void ProcessExecutor::stop()
{
    m_stopFlag = true;
}
