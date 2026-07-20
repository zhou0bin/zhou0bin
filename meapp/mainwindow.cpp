#include "mainwindow.h"
#include "ui_mainwindow.h"

struct UdsParseResult
{
    bool    ok = false;        // 是否解析成功
    quint16 did = 0;           // DID，例如 0xF195
    QString version;           // 版本号（仅 F195 有）
    QString rawHex;            // 原始 0xXXYYZZDD
    QString error;             // 错误原因（调试用）
};

UdsParseResult parseUdsSingleFrame(const QByteArray &data)
{
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

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    canComm(new CanCommPCAN(this)),
    canThread(new QThread(this)),
    model(new QStandardItemModel(this))
{

    ui->setupUi(this);
    ui->tabWidget->setTabText(0, "Tab 0");

    // 将 CanComm 移动到工作线程中
    canComm->moveToThread(canThread);

    // 连接信号与槽
//    connect(canThread, &QThread::started, canComm, &CanComm::openConnection);
    connect(this, &MainWindow::sendMessage, canComm, &CanCommPCAN::sendMessage);
    connect(this, &MainWindow::receiveMessage, canComm, &CanCommPCAN::startReceiveThread);
    connect(canComm, &CanCommPCAN::messageReceived, this, &MainWindow::on_receiveMessage);
    connect(this, &MainWindow::timerMake, this, &MainWindow::on_timerMake);
//    connect(this, &MainWindow::timerMake0, this, &MainWindow::on_timerMake0);

    sendThreadRunning = false;

    // 启动线程
    canThread->start();
    // 初始化 list view 的数据模型
    listModel = new QStringListModel(this);
    ui->listView->setModel(listModel);

    ui->treeView->setModel(model);
    ui->treeView->setHeaderHidden(true);  // 不显示表头
    ui->treeView->setAnimated(true);      // 展开时带动画
    QStringList steps = {
        "Step 1: 准备阶段",
        "Step 2: 启动电池阀1+雨刮1",
        "Step 3: 启动电池阀2+雨刮2",
        "Step 4: 启动电池阀3",
        "Step 5: 启动电池阀4",
        "Step 6: 启动电池阀5+雨刮5",
        "Step 7: 启动电池阀6+雨刮6",
        "Step 8: 启动电池阀7+雨刮7",
        "Step 9: 启动电池阀8",
        "Step 10: 结束阶段"
    };
    initSteps(steps);
//    // 初始化已有步骤（手动添加或读取）
//    model->appendRow(new QStandardItem("Step 1: Init CAN"));
//    model->appendRow(new QStandardItem("Step 2: Open Channel"));
//    model->appendRow(new QStandardItem("Step 3: Send Data"));

    Cdccontrol1680_Reset(&cdc_control1_680);
//    cdc_control1_680.cdc_lelidarwprpwr = Cdc_control1_680_Cdc_lelidarwprpwrType_CDC_LELIDARWPRPWR_OPEN_E2;
//    Cdccontrol1680_UpdateData(&cdc_control1_680);
//    QByteArray byteArray(Data, 32);
//    QString hexStr = byteArray.toHex(' ').toUpper();
//    qDebug() << hexStr;


    auto *parent1 = ui->treeWidget->addTopItem("FAC", 100);
    ui->treeWidget->addChildItem(parent1, "右补盲激光雷达", 101);
    ui->treeWidget->addChildItem(parent1, "右补盲鱼眼相机", 102);
    ui->treeWidget->addChildItem(parent1, "前补盲激光雷达", 103);
    ui->treeWidget->addChildItem(parent1, "前补盲鱼眼相机", 104);
    ui->treeWidget->addChildItem(parent1, "左补盲激光雷达", 105);
    ui->treeWidget->addChildItem(parent1, "左补盲鱼眼相机", 106);
    ui->treeWidget->addChildItem(parent1, "左翼子板周边相机", 107);
    ui->treeWidget->addChildItem(parent1, "前底盘电磁阀控制", 108);

    auto *parent2 = ui->treeWidget->addTopItem("RAC", 200);
    ui->treeWidget->addChildItem(parent2, "后保左侧周边相机", 201);
    ui->treeWidget->addChildItem(parent2, "后保右侧周边相机", 202);
    ui->treeWidget->addChildItem(parent2, "后补盲鱼眼相机", 203);
    ui->treeWidget->addChildItem(parent2, "后补盲激光雷达", 204);
    ui->treeWidget->addChildItem(parent2, "中部底盘电磁阀控制", 205);
    ui->treeWidget->addChildItem(parent2, "后部底盘电磁阀控制", 206);

    auto *parent3 = ui->treeWidget->addTopItem("TAC", 300);
    ui->treeWidget->addChildItem(parent3, "正后相机", 301);
    ui->treeWidget->addChildItem(parent3, "主激光雷达", 302);
    ui->treeWidget->addChildItem(parent3, "左前相机", 303);
    ui->treeWidget->addChildItem(parent3, "右前相机", 304);
    ui->treeWidget->addChildItem(parent3, "左侧广角相机", 305);
    ui->treeWidget->addChildItem(parent3, "右侧广角相机", 306);
    ui->treeWidget->addChildItem(parent3, "前相机", 307);

    auto *parent0 = ui->treeWidget_3->addTopItem("动作", 1000);
    ui->treeWidget_3->addChildItem(parent0, "雨刮", 1001);
    ui->treeWidget_3->addChildItem(parent0, "水阀", 1002);
    ui->treeWidget_3->addChildItem(parent0, "雨刮和水阀", 1003);
    ui->treeWidget_3->addChildItem(parent0, "加热", 1004);
    ui->treeWidget_3->addChildItem(parent0, "遮蔽", 1005);

    // 选中某项
//    ui->treeWidget->selectItem(parent2);


    ui->tableWidget->initTable(
        {
            "右补盲激光雷达水阀",
            "右补盲鱼眼相机水阀",
            "前补盲激光雷达水阀",
            "前补盲鱼眼相机水阀",
            "左补盲激光雷达水阀",
            "左补盲鱼眼相机水阀",
            "左翼子板周边相机电磁阀",
            "后保左侧周边相机电磁阀",
            "后保右侧周边相机电磁阀",
            "后补盲鱼眼相机电磁阀",
            "后补盲激光雷达水阀",
            "正后相机电磁阀",
            "主激光雷达水阀",
            "左前相机电磁阀",
            "右前相机电磁阀",
            "左侧广角相机电磁阀",
            "右侧广角相机电磁阀",
            "前底盘电磁阀",
            "中底盘电磁阀",
            "后底盘电磁阀"
        },
        {
            "右补盲激光雷达雨刮",
            "不涉及",
            "前补盲激光雷达雨刮",
            "不涉及",
            "左补盲激光雷达雨刮",
            "不涉及",
            "左翼子板周边相机雨刮",
            "后保左侧周边相机雨刮",
            "后保右侧周边相机雨刮",
            "不涉及",
            "后补盲激光雷达雨刮",
            "正后相机雨刮",
            "不涉及",
            "左前相机雨刮",
            "右前相机雨刮",
            "左侧广角相机雨刮",
            "右侧广角相机雨刮",
            "不涉及",
            "不涉及",
            "不涉及"
        }
    );

    Cdccontrol1680_UpdateData(&cdc_control1_680);
    Cdc15443_Reset(&cdc_15_443_i);
    lidartype = false;

    executor = new ProcessExecutor();
    QThread *worker = new QThread(this);

    executor->moveToThread(worker);

    worker->start();

    initTableView();

    initProcess();

    connect(ui->tableViewAssembly,
            &QTableView::clicked,
            this,
            &MainWindow::slotAssemblyClicked);

    connect(executor, &ProcessExecutor::stepStarted,
            this, [](int id, const QString &name){
                qDebug() << "STEP START:" << id << name;
            });

    connect(executor, &ProcessExecutor::stepFinished,
            this, &MainWindow::onStepFinished);

    connect(this, &MainWindow::startAssembly,
            executor, &ProcessExecutor::runAssembly);
    connect(executor,
            &ProcessExecutor::shellOutput,
            this,
            &MainWindow::appendShellOutput,
            Qt::QueuedConnection);
    connect(executor,
            &ProcessExecutor::shellOutputLidar,
            this,
            &MainWindow::appendShellOutputLidar,
            Qt::QueuedConnection);

    connect(executor,
            &ProcessExecutor::picOutput,
            this,
            &MainWindow::appendPicOutput);

    espTimer = new QTimer(this);
    dcuTimer = new QTimer(this);
    dkmTimer = new QTimer(this);


    // ESP_1A 10ms周期
    connect(espTimer, &QTimer::timeout, this, [=](){

        QByteArray data50(48, 0);
        data50[4] = 0x01;    // ESP_ABSVehSpdVld = 1

        executor->canCommPro->sendMessage(0x32, data50);

    });


    // DCU_FL_1 20ms周期
    connect(dcuTimer, &QTimer::timeout, this, [=](){

        QByteArray data780(32, 0);
        data780[8] = 0x08;   // DCU_FL_DoorSts = 1

        executor->canCommPro->sendMessage(0x30c, data780);

    });


    // DKM_1 50ms周期
    connect(dkmTimer, &QTimer::timeout, this, [=](){

        QByteArray data1029(32, 0);
        data1029[2] = 0x01;   // DKM_CardDetectArea = 1

        executor->canCommPro->sendMessage(0x405, data1029);

    });
}

MainWindow::~MainWindow() {
    delete ui;
    canThread->quit();
    canThread->wait();
}

void MainWindow::on_sendButton_clicked() {
    // 假设使用固定的 CAN ID，例如 0x123
//    quint32 canId = 0x123;

//    // 构造数据
//    uint8_t data[4] = {0x01, 0x02, 0x03, 0x04};
//    QByteArray message(reinterpret_cast<char*>(data), sizeof(data));
    quint32 canId = ui->canIdLineEdit->text().toUInt(nullptr, 16);  // 从文本框读取16进制ID
    QByteArray message = QByteArray::fromHex(ui->dataLineEdit->text().toUtf8());  // 从文本框读取数据
    // 发射信号（你需要确保有信号 sendMessage(quint32, QByteArray)）
    emit sendMessage(canId, message);

    // 打印调试信息
    qDebug() << "[UI] Send request -> CAN ID:" << QString::number(canId, 16).toUpper()
             << "Data:" << message.toHex(' ').toUpper();
}
//    canComm->sendMessage(0x7DF, QByteArray::fromHex("0201050000000000"));  // 发一条 UDS 请求


void MainWindow::on_receiveButton_clicked() {
    // 调用接收消息槽
    emit receiveMessage();
    canComm->setPaused(false);
}

int extractSignal(const QByteArray &data, int startBit, int length)
{
    const quint8 *raw = reinterpret_cast<const quint8*>(data.constData());
    quint32 value = 0;

    // 假设 DBC 是 Intel(LSB) 顺序，也就是 startBit 递增对应低位优先
    for (int i = 0; i < length; ++i) {
        int bitIndex = startBit + i;
        int byteIndex = bitIndex / 8;
        int bitInByte = bitIndex % 8;

        if (raw[byteIndex] & (1 << bitInByte)) {
            value |= (1u << i);
        }
    }
    return static_cast<int>(value);
}

void MainWindow::on_receiveMessage(quint32 canId, const QByteArray &message)
{
    if (canId == 1137) {
        qDebug() << "----------" <<message;
        int xLidar      = extractSignal(message, 0, 3);
        int bLidarFront = extractSignal(message, 3, 3);
        int bLidarLeft  = extractSignal(message, 8, 3);
        int bLidarRight = extractSignal(message, 11, 3);
        int bLidarRear  = extractSignal(message, 16, 3);

//        qDebug() << "XCU Lidar Pos:"
//                 << "xLidar=" << xLidar
//                 << "Front=" << bLidarFront
//                 << "Left=" << bLidarLeft
//                 << "Right=" << bLidarRight
//                 << "Rear=" << bLidarRear;
        // 判断是否为 1 xLidar == 1 &&
        if (xLidar == 1 && bLidarFront == 1 && bLidarLeft == 1 &&
            bLidarRight == 1 && bLidarRear == 1) {
            lidartype = true;
        }else{
            lidartype = false;
        }
    }

//    if (canId == 1755)
//    {
//        UdsParseResult res = parseUdsSingleFrame(message);

//        if (!res.ok) {
//            qDebug() << "UDS parse failed:" << res.error;
//            return;
//        }

//        if (res.did == 0xF195) {
//            qDebug() << "Software Version:"
//                     << res.version
//                     << "(" << res.rawHex << ")";
//        }
//    }

    onCanFrameReceived(canId, message);

    QString timestamp = QTime::currentTime().toString("HH:mm:ss.zzz");
    QString canIdStr = QString::number(canId, 16).toUpper().rightJustified(3, '0');
    QString dataStr = message.toHex(' ').toUpper();

    QString finalStr = QString("%1 - ID: %2 - DATA: %3").arg(timestamp, canIdStr, dataStr);

    // 限制最多保存 100 条
    if (messageList.size() >= 100) {
        messageList.removeFirst();  // 移除最旧的一条
    }

    messageList.append(finalStr);
    listModel->setStringList(messageList);

    QModelIndex lastIndex = listModel->index(messageList.size() - 1);
    ui->listView->scrollTo(lastIndex);
}

void MainWindow::on_connectButton_clicked()
{
    if(canComm->openConnection(true))
    {
        ui->connectButton->setStyleSheet("background-color: green;");
    }else{
        ui->connectButton->setStyleSheet("background-color: red;");

    }
}

void MainWindow::on_stopButton_clicked()
{
    canComm->setPaused(true);  // 通知 CanComm 是否暂停 emit
}

void MainWindow::on_cleanButton_clicked()
{
    messageList.clear();                      // 清空消息列表
    listModel->setStringList(messageList);    // 清空模型
}

QByteArray buildLidarDockCommand(quint8 dockValue)
{
    QByteArray data(8, 0x00); // 初始化8字节全0
    quint8 *raw = reinterpret_cast<quint8*>(data.data());

    // xLidar -> bit0-1
    raw[0] |= (dockValue & 0x3) << 0;

    // bLidar Front -> bit2-3
    raw[0] |= (dockValue & 0x3) << 2;

    // bLidar Left -> bit4-5
    raw[0] |= (dockValue & 0x3) << 4;

    // bLidar Right -> bit6-7
    raw[0] |= (dockValue & 0x3) << 6;

    // bLidar Rear -> bit8-9 (byte1的低两位)
    raw[1] |= (dockValue & 0x3) << 0;

    return data;
}

void MainWindow::on_startButton_clicked()
{

    if (sendThreadRunning) {
           qWarning() << "Send thread already running!";
           return;
       }

    sendThreadRunning = true;

    sendThread = QThread::create([=]() {
       emit receiveMessage();
       int count = ui->countLineEdit->text().toUInt();
//       int count_0 = 0;
       for (int i = 0; i < count && sendThreadRunning; ++i) {

            qDebug()<< "start times = " << i;
//            QByteArray dockCmd = buildLidarDockCommand(1);
//            canComm->enqueueMessage(0x470, dockCmd);
//            QThread::msleep(1000);
//            while(1)
//            {
//                qDebug() << "................"<<count_0;
//                if(lidartype)
//                {
//                    count_0 = 0;
//                    break;
//                }
//                QThread::msleep(500);
//                count_0++;
//                if(count_0 > 20)
//                {
//                    count_0 = 0;
//                    return ;
//                }
//            }
            Cdccontrol1680_Reset(&cdc_control1_680);
//            QString hexStr = byteArray.toHex(' ').toUpper();
//            qDebug() << hexStr;
            cdc_control1_680.cdc_cleanpumpecupwr = Cdc_control1_680_Cdc_cleanpumpecupwrType_CDC_CLEANPUMPECUPWR_OPEN_E2;
            cdc_control1_680.cdc_cleanpumpspd = ui->pressureEdit->text().toDouble();
            Cdccontrol1680_UpdateData(&cdc_control1_680);
            QByteArray byteArray(Data, 48);
            if (canComm->sendMessage(0x680, byteArray)) {
                appendResultToStep(0, true);
            }else{
                appendResultToStep(0,false);
            }
            QThread::msleep(1000);

//            canComm->enqueueMessage(0x470, dockCmd);
            cdc_15_443_i.ri_blid_wiper_ctl = Cdc_15_443_Ri_blid_wiper_ctlType_RI_BLID_WIPER_CTL_STANDBY_POSITION_E2;
            cdc_15_443_i.ri_blid_wiper_frequency_ctl = 0;
            cdc_15_443_i.ri_blid_wiper_number_ctl = 0;
            cdc_15_443_i.fr_blid_wiper_ctl = Cdc_15_443_Fr_blid_wiper_ctlType_FR_BLID_WIPER_CTL_STANDBY_POSITION_E2;
            cdc_15_443_i.fr_blid_wiper_frequency_ctl = 0;
            cdc_15_443_i.fr_blid_wiper_number_ctl = 0;
            cdc_15_443_i.le_blid_wiper_ctl = Cdc_15_443_Le_blid_wiper_ctlType_LE_BLID_WIPER_CTL_STANDBY_POSITION_E2;
            cdc_15_443_i.le_blid_wiper_frequency_ctl = 0;
            cdc_15_443_i.le_blid_wiper_number_ctl = 0;
            Cdc15443_UpdateData(&cdc_15_443_i);
            byteArray = QByteArray(Data443, 64);
            canComm->sendMessage(0x443, byteArray);
            QThread::msleep(2000);

            Cdccontrol1680_Reset(&cdc_control1_680);
            cdc_control1_680.cdc_cleanpumpspd = 2;
            Cdccontrol1680_UpdateData(&cdc_control1_680);
            byteArray = QByteArray(Data, 48);
            canComm->enqueueMessage(0x680, byteArray);

            Cdccontrol1683_Reset(&cdc_control1_683);
            cdc_control1_683.cdc_rilidarvalvepwr = Cdc_control2_683_Cdc_rilidarvalvepwrType_CDC_RILIDARVALVEPWR_ON_E2;
            cdc_control1_683.cdc_frlidarvalvepwr = Cdc_control2_683_Cdc_frlidarvalvepwrType_CDC_FRLIDARVALVEPWR_ON_E2;
            cdc_control1_683.cdc_lelidarvalvepwr = Cdc_control2_683_Cdc_lelidarvalvepwrType_CDC_LELIDARVALVEPWR_ON_E2;
            Cdccontrol1683_UpdateData(&cdc_control1_683);
            byteArray = QByteArray(Data683, 64);
            canComm->enqueueMessage(0x683, byteArray);

            cdc_15_443_i.ri_blid_wiper_ctl = Cdc_15_443_Ri_blid_wiper_ctlType_RI_BLID_WIPER_CTL_ACTIVE_E1;
            cdc_15_443_i.ri_blid_wiper_frequency_ctl = 30;
            cdc_15_443_i.ri_blid_wiper_number_ctl = 3;
            cdc_15_443_i.fr_blid_wiper_ctl = Cdc_15_443_Fr_blid_wiper_ctlType_FR_BLID_WIPER_CTL_ACTIVE_E1;
            cdc_15_443_i.fr_blid_wiper_frequency_ctl = 30;
            cdc_15_443_i.fr_blid_wiper_number_ctl = 3;
            cdc_15_443_i.le_blid_wiper_ctl = Cdc_15_443_Le_blid_wiper_ctlType_LE_BLID_WIPER_CTL_ACTIVE_E1;
            cdc_15_443_i.le_blid_wiper_frequency_ctl = 30;
            cdc_15_443_i.le_blid_wiper_number_ctl = 3;
            Cdc15443_UpdateData(&cdc_15_443_i);
            byteArray = QByteArray(Data443, 64);
            canComm->sendMessage(0x443, byteArray);

            QThread::msleep(200);

            Cdccontrol1683_Reset(&cdc_control1_683);
            cdc_control1_683.cdc_rilidarvalvepwr = Cdc_control2_683_Cdc_rilidarvalvepwrType_CDC_RILIDARVALVEPWR_OFF_E1;
            cdc_control1_683.cdc_frlidarvalvepwr = Cdc_control2_683_Cdc_frlidarvalvepwrType_CDC_FRLIDARVALVEPWR_OFF_E1;
            cdc_control1_683.cdc_lelidarvalvepwr = Cdc_control2_683_Cdc_lelidarvalvepwrType_CDC_LELIDARVALVEPWR_OFF_E1;
            Cdccontrol1683_UpdateData(&cdc_control1_683);
            byteArray = QByteArray(Data683, 64);
            canComm->enqueueMessage(0x683, byteArray);
            QThread::msleep(5800);

            cdc_15_443_i.ri_blid_wiper_ctl = Cdc_15_443_Ri_blid_wiper_ctlType_RI_BLID_WIPER_CTL_STANDBY_POSITION_E2;
            cdc_15_443_i.ri_blid_wiper_frequency_ctl = 0;
            cdc_15_443_i.ri_blid_wiper_number_ctl = 0;
            cdc_15_443_i.fr_blid_wiper_ctl = Cdc_15_443_Fr_blid_wiper_ctlType_FR_BLID_WIPER_CTL_STANDBY_POSITION_E2;
            cdc_15_443_i.fr_blid_wiper_frequency_ctl = 0;
            cdc_15_443_i.fr_blid_wiper_number_ctl = 0;
            cdc_15_443_i.le_blid_wiper_ctl = Cdc_15_443_Le_blid_wiper_ctlType_LE_BLID_WIPER_CTL_STANDBY_POSITION_E2;
            cdc_15_443_i.le_blid_wiper_frequency_ctl = 0;
            cdc_15_443_i.le_blid_wiper_number_ctl = 0;
            Cdc15443_UpdateData(&cdc_15_443_i);
            byteArray = QByteArray(Data443, 64);
            if (canComm->sendMessage(0x443, byteArray)) {
                appendResultToStep(1, true);
            }else{
                appendResultToStep(1,false);
            }
            QThread::msleep(2000);
            QThread::msleep(ui->pressureEdit_5->text().toInt());

//            canComm->enqueueMessage(0x470, dockCmd);
            cdc_15_443_i.re_blid_rac_wiper_ctl = Cdc_15_443_Re_blid_rac_wiper_ctlType_RE_BLID_RAC_WIPER_CTL_STANDBY_POSITION_E2;
            cdc_15_443_i.re_blid_rac_wiper_frequency_ctl = 0;
            cdc_15_443_i.re_blid_rac_wiper_number_ctl = 0;
            Cdc15443_UpdateData(&cdc_15_443_i);
            byteArray = QByteArray(Data443, 64);
            canComm->sendMessage(0x443, byteArray);
            QThread::msleep(2000);

            Cdccontrol1680_Reset(&cdc_control1_680);
            cdc_control1_680.cdc_cleanpumpspd = 4;
            cdc_control1_680.cdc_mlidarshieldmove = Cdc_control1_680_Cdc_mlidarshieldmoveType_CDC_MLIDARSHIELDMOVE_OPEN_E1;
            Cdccontrol1680_UpdateData(&cdc_control1_680);
            byteArray = QByteArray(Data, 48);
            canComm->enqueueMessage(0x680, byteArray);

            Cdccontrol1683_Reset(&cdc_control1_683);
            cdc_control1_683.cdc_mlidarvalvepwr = Cdc_control2_683_Cdc_mlidarvalvepwrType_CDC_MLIDARVALVEPWR_ON_E2;
            cdc_control1_683.cdc_relidarvalvepwr = Cdc_control2_683_Cdc_relidarvalvepwrType_CDC_RELIDARVALVEPWR_ON_E2;
            Cdccontrol1683_UpdateData(&cdc_control1_683);
            byteArray = QByteArray(Data683, 64);
            canComm->enqueueMessage(0x683, byteArray);

            QThread::msleep(10);
            data470[2] = 0;
            data470[2] = data470[2] |(3 << 5);   // byte2 bit5~bit7
            data470[3] = 60;   //8bit
            canComm->enqueueMessage(0x470, data470);

            cdc_15_443_i.re_blid_rac_wiper_ctl = Cdc_15_443_Re_blid_rac_wiper_ctlType_RE_BLID_RAC_WIPER_CTL_ACTIVE_E1;
            cdc_15_443_i.re_blid_rac_wiper_frequency_ctl = 30;
            cdc_15_443_i.re_blid_rac_wiper_number_ctl = 3;
            Cdc15443_UpdateData(&cdc_15_443_i);
            byteArray = QByteArray(Data443, 64);
            canComm->sendMessage(0x443, byteArray);

            QThread::msleep(200);

            Cdccontrol1683_Reset(&cdc_control1_683);
            cdc_control1_683.cdc_relidarvalvepwr = Cdc_control2_683_Cdc_relidarvalvepwrType_CDC_RELIDARVALVEPWR_OFF_E1;
            Cdccontrol1683_UpdateData(&cdc_control1_683);
            byteArray = QByteArray(Data683, 64);
            canComm->enqueueMessage(0x683, byteArray);
            QThread::msleep(500);

            Cdccontrol1683_Reset(&cdc_control1_683);
            cdc_control1_683.cdc_mlidarvalvepwr = Cdc_control2_683_Cdc_mlidarvalvepwrType_CDC_MLIDARVALVEPWR_OFF_E1;
            Cdccontrol1683_UpdateData(&cdc_control1_683);
            byteArray = QByteArray(Data683, 64);
            canComm->enqueueMessage(0x683, byteArray);
            QThread::msleep(5300);

            Cdccontrol1680_Reset(&cdc_control1_680);
            cdc_control1_680.cdc_cleanpumpspd = 4;
            cdc_control1_680.cdc_mlidarshieldmove = Cdc_control1_680_Cdc_mlidarshieldmoveType_CDC_MLIDARSHIELDMOVE_CLOSE_E2;
            Cdccontrol1680_UpdateData(&cdc_control1_680);
            byteArray = QByteArray(Data, 48);
            canComm->enqueueMessage(0x680, byteArray);

            cdc_15_443_i.re_blid_rac_wiper_ctl = Cdc_15_443_Re_blid_rac_wiper_ctlType_RE_BLID_RAC_WIPER_CTL_STANDBY_POSITION_E2;
            cdc_15_443_i.re_blid_rac_wiper_frequency_ctl = 0;
            cdc_15_443_i.re_blid_rac_wiper_number_ctl = 0;
            Cdc15443_UpdateData(&cdc_15_443_i);
            byteArray = QByteArray(Data443, 64);
            if (canComm->sendMessage(0x443, byteArray)) {
                appendResultToStep(2, true);
            }else{
                appendResultToStep(2,false);
            }
            QThread::msleep(2000);
            QThread::msleep(ui->pressureEdit_5->text().toInt());

            Cdccontrol1680_Reset(&cdc_control1_680);
            cdc_control1_680.cdc_cleanpumpspd = 6;
            Cdccontrol1680_UpdateData(&cdc_control1_680);
            byteArray = QByteArray(Data, 48);
            canComm->enqueueMessage(0x680, byteArray);

            Cdccontrol1683_Reset(&cdc_control1_683);
            cdc_control1_683.cdc_fisheye_f_cmr_valve_pwr_req = Cdc_control2_683_Cdc_fisheye_f_cmr_valve_pwr_reqType_CDC_FISHEYE_F_CMR_VALVE_PWR_REQ_ON_E2;
            cdc_control1_683.cdc_fisheye_b_cmr_valve_pwr_req = Cdc_control2_683_Cdc_fisheye_b_cmr_valve_pwr_reqType_CDC_FISHEYE_B_CMR_VALVE_PWR_REQ_ON_E2;
            Cdccontrol1683_UpdateData(&cdc_control1_683);
            byteArray = QByteArray(Data683, 64);
            canComm->enqueueMessage(0x683, byteArray);

            QThread::msleep(500);

            Cdccontrol1683_Reset(&cdc_control1_683);
            cdc_control1_683.cdc_fisheye_f_cmr_valve_pwr_req = Cdc_control2_683_Cdc_fisheye_f_cmr_valve_pwr_reqType_CDC_FISHEYE_F_CMR_VALVE_PWR_REQ_OFF_E1;
            Cdccontrol1683_UpdateData(&cdc_control1_683);
            byteArray = QByteArray(Data683, 64);
            canComm->enqueueMessage(0x683, byteArray);
            QThread::msleep(500);

            Cdccontrol1683_Reset(&cdc_control1_683);
            cdc_control1_683.cdc_fisheye_b_cmr_valve_pwr_req = Cdc_control2_683_Cdc_fisheye_b_cmr_valve_pwr_reqType_CDC_FISHEYE_B_CMR_VALVE_PWR_REQ_OFF_E1;
            Cdccontrol1683_UpdateData(&cdc_control1_683);
            byteArray = QByteArray(Data683, 64);
            if (canComm->sendMessage(0x683, byteArray)) {
                appendResultToStep(3, true);
            }else{
                appendResultToStep(3,false);
            }
            QThread::msleep(ui->pressureEdit_5->text().toInt());

            Cdccontrol1680_Reset(&cdc_control1_680);
            cdc_control1_680.cdc_cleanpumpspd = 5;
            Cdccontrol1680_UpdateData(&cdc_control1_680);
            byteArray = QByteArray(Data, 48);
            canComm->enqueueMessage(0x680, byteArray);

            Cdccontrol1683_Reset(&cdc_control1_683);
            cdc_control1_683.cdc_fisheye_r_cmr_valve_pwr_req = Cdc_control2_683_Cdc_fisheye_r_cmr_valve_pwr_reqType_CDC_FISHEYE_R_CMR_VALVE_PWR_REQ_ON_E2;
            cdc_control1_683.cdc_fisheye_l_cmr_valve_pwr_req = Cdc_control2_683_Cdc_fisheye_l_cmr_valve_pwr_reqType_CDC_FISHEYE_L_CMR_VALVE_PWR_REQ_ON_E2;
            Cdccontrol1683_UpdateData(&cdc_control1_683);
            byteArray = QByteArray(Data683, 64);
            canComm->enqueueMessage(0x683, byteArray);

            QThread::msleep(500);

            Cdccontrol1683_Reset(&cdc_control1_683);
            cdc_control1_683.cdc_fisheye_r_cmr_valve_pwr_req = Cdc_control2_683_Cdc_fisheye_r_cmr_valve_pwr_reqType_CDC_FISHEYE_R_CMR_VALVE_PWR_REQ_OFF_E1;
            cdc_control1_683.cdc_fisheye_l_cmr_valve_pwr_req = Cdc_control2_683_Cdc_fisheye_l_cmr_valve_pwr_reqType_CDC_FISHEYE_L_CMR_VALVE_PWR_REQ_OFF_E1;
            Cdccontrol1683_UpdateData(&cdc_control1_683);
            byteArray = QByteArray(Data683, 64);
            if (canComm->sendMessage(0x683, byteArray)) {
                appendResultToStep(4, true);
            }else{
                appendResultToStep(4,false);
            }
            QThread::msleep(ui->pressureEdit_5->text().toInt());

            Cdccontrol1680_Reset(&cdc_control1_680);
            cdc_control1_680.cdc_cleanpumpspd = 2;
            Cdccontrol1680_UpdateData(&cdc_control1_680);
            byteArray = QByteArray(Data, 48);
            canComm->enqueueMessage(0x680, byteArray);

            Cdccontrol1683_Reset(&cdc_control1_683);
            cdc_control1_683.cdc_auto_rl_cmr_valve_pwr_req = Cdc_control2_683_Cdc_auto_rl_cmr_valve_pwr_reqType_CDC_AUTO_RL_CMR_VALVE_PWR_REQ_ON_E2;
            cdc_control1_683.cdc_auto_rr_cmr_valve_pwr_req = Cdc_control2_683_Cdc_auto_rr_cmr_valve_pwr_reqType_CDC_AUTO_RR_CMR_VALVE_PWR_REQ_ON_E2;
            cdc_control1_683.cdc_auto_lp_cmr_valve_pwr_req = Cdc_control2_683_Cdc_auto_lp_cmr_valve_pwr_reqType_CDC_AUTO_LP_CMR_VALVE_PWR_REQ_ON_E2;
            Cdccontrol1683_UpdateData(&cdc_control1_683);
            byteArray = QByteArray(Data683, 64);
            canComm->enqueueMessage(0x683, byteArray);

            cdc_15_443_i.lf_pcam_wiper_ctl = Cdc_15_443_Lf_pcam_wiper_ctlType_LF_PCAM_WIPER_CTL_ACTIVE_E1;
            cdc_15_443_i.lf_pcam_wiper_number_ctl = 3;
            cdc_15_443_i.lf_pcam_wiper_frequency_ctl = 60;
            cdc_15_443_i.rrb_pcam_wiper_ctl = Cdc_15_443_Rrb_pcam_wiper_ctlType_RRB_PCAM_WIPER_CTL_ACTIVE_E1;
            cdc_15_443_i.rrb_pcam_wiper_number_ctl = 3;
            cdc_15_443_i.rrb_pcam_wiper_frequency_ctl = 60;
            cdc_15_443_i.lrb_pcam_wiper_ctl = Cdc_15_443_Lrb_pcam_wiper_ctlType_LRB_PCAM_WIPER_CTL_ACTIVE_E1;
            cdc_15_443_i.lrb_pcam_wiper_number_ctl = 3;
            cdc_15_443_i.lrb_pcam_wiper_frequency_ctl = 60;
            Cdc15443_UpdateData(&cdc_15_443_i);
            byteArray = QByteArray(Data443, 64);
            canComm->enqueueMessage(0x443, byteArray);

            QThread::msleep(150);

            Cdccontrol1683_Reset(&cdc_control1_683);
            cdc_control1_683.cdc_auto_rl_cmr_valve_pwr_req = Cdc_control2_683_Cdc_auto_rl_cmr_valve_pwr_reqType_CDC_AUTO_RL_CMR_VALVE_PWR_REQ_OFF_E1;
            cdc_control1_683.cdc_auto_rr_cmr_valve_pwr_req = Cdc_control2_683_Cdc_auto_rr_cmr_valve_pwr_reqType_CDC_AUTO_RR_CMR_VALVE_PWR_REQ_OFF_E1;
            cdc_control1_683.cdc_auto_lp_cmr_valve_pwr_req = Cdc_control2_683_Cdc_auto_lp_cmr_valve_pwr_reqType_CDC_AUTO_LP_CMR_VALVE_PWR_REQ_OFF_E1;
            Cdccontrol1683_UpdateData(&cdc_control1_683);
            byteArray = QByteArray(Data683, 64);
            canComm->enqueueMessage(0x683, byteArray);
            QThread::msleep(4850);

            cdc_15_443_i.lf_pcam_wiper_ctl = Cdc_15_443_Lf_pcam_wiper_ctlType_LF_PCAM_WIPER_CTL_STANDBY_POSITION_E2;
            cdc_15_443_i.lf_pcam_wiper_number_ctl = 0;
            cdc_15_443_i.lf_pcam_wiper_frequency_ctl = 0;
            cdc_15_443_i.rrb_pcam_wiper_ctl = Cdc_15_443_Rrb_pcam_wiper_ctlType_RRB_PCAM_WIPER_CTL_STANDBY_POSITION_E2;
            cdc_15_443_i.rrb_pcam_wiper_number_ctl = 0;
            cdc_15_443_i.rrb_pcam_wiper_frequency_ctl = 0;
            cdc_15_443_i.lrb_pcam_wiper_ctl = Cdc_15_443_Lrb_pcam_wiper_ctlType_LRB_PCAM_WIPER_CTL_STANDBY_POSITION_E2;
            cdc_15_443_i.lrb_pcam_wiper_number_ctl = 0;
            cdc_15_443_i.lrb_pcam_wiper_frequency_ctl = 0;
            Cdc15443_UpdateData(&cdc_15_443_i);
            byteArray = QByteArray(Data443, 64);
            if (canComm->sendMessage(0x443, byteArray)) {
                appendResultToStep(5, true);
            }else{
                appendResultToStep(5,false);
            }
            QThread::msleep(ui->pressureEdit_5->text().toInt());

            Cdccontrol1680_Reset(&cdc_control1_680);
            cdc_control1_680.cdc_cleanpumpspd = 2;
            Cdccontrol1680_UpdateData(&cdc_control1_680);
            byteArray = QByteArray(Data, 48);
            canComm->enqueueMessage(0x680, byteArray);

            Cdccontrol1683_Reset(&cdc_control1_683);
            cdc_control1_683.cdc_auto_lf_cmr_valve_pwr_req = Cdc_control2_683_Cdc_auto_lf_cmr_valve_pwr_reqType_CDC_AUTO_LF_CMR_VALVE_PWR_REQ_ON_E2;
            cdc_control1_683.cdc_auto_rf_cmr_valve_pwr_req = Cdc_control2_683_Cdc_auto_rf_cmr_valve_pwr_reqType_CDC_AUTO_RF_CMR_VALVE_PWR_REQ_ON_E2;
            Cdccontrol1683_UpdateData(&cdc_control1_683);
            byteArray = QByteArray(Data683, 64);
            canComm->enqueueMessage(0x683, byteArray);

            cdc_15_443_i.fl_cam_wiper_ctl = Cdc_15_443_Fl_cam_wiper_ctlType_FL_CAM_WIPER_CTL_ACTIVE_E1;
            cdc_15_443_i.fl_cam_wiper_number_ctl = 3;
            cdc_15_443_i.fl_cam_wiper_frequency_ctl = 60;
            cdc_15_443_i.fr_cam_wiper_ctl = Cdc_15_443_Fr_cam_wiper_ctlType_FR_CAM_WIPER_CTL_ACTIVE_E1;
            cdc_15_443_i.fr_cam_wiper_number_ctl = 3;
            cdc_15_443_i.fr_cam_wiper_frequency_ctl = 60;
            Cdc15443_UpdateData(&cdc_15_443_i);
            byteArray = QByteArray(Data443, 64);
            canComm->enqueueMessage(0x443, byteArray);

            QThread::msleep(150);

            Cdccontrol1683_Reset(&cdc_control1_683);
            cdc_control1_683.cdc_auto_lf_cmr_valve_pwr_req = Cdc_control2_683_Cdc_auto_lf_cmr_valve_pwr_reqType_CDC_AUTO_LF_CMR_VALVE_PWR_REQ_OFF_E1;
            cdc_control1_683.cdc_auto_rf_cmr_valve_pwr_req = Cdc_control2_683_Cdc_auto_rf_cmr_valve_pwr_reqType_CDC_AUTO_RF_CMR_VALVE_PWR_REQ_OFF_E1;
            Cdccontrol1683_UpdateData(&cdc_control1_683);
            byteArray = QByteArray(Data683, 64);
            canComm->enqueueMessage(0x683, byteArray);
            QThread::msleep(4850);

            cdc_15_443_i.fl_cam_wiper_ctl = Cdc_15_443_Fl_cam_wiper_ctlType_FL_CAM_WIPER_CTL_STANDBY_POSITION_E2;
            cdc_15_443_i.fl_cam_wiper_number_ctl = 0;
            cdc_15_443_i.fl_cam_wiper_frequency_ctl = 0;
            cdc_15_443_i.fr_cam_wiper_ctl = Cdc_15_443_Fr_cam_wiper_ctlType_FR_CAM_WIPER_CTL_STANDBY_POSITION_E2;
            cdc_15_443_i.fr_cam_wiper_number_ctl = 0;
            cdc_15_443_i.fr_cam_wiper_frequency_ctl = 0;
            Cdc15443_UpdateData(&cdc_15_443_i);
            byteArray = QByteArray(Data443, 64);
            if (canComm->sendMessage(0x443, byteArray)) {
                appendResultToStep(6, true);
            }else{
                appendResultToStep(6,false);
            }
            QThread::msleep(ui->pressureEdit_5->text().toInt());

            Cdccontrol1680_Reset(&cdc_control1_680);
            cdc_control1_680.cdc_cleanpumpspd = 2;
            Cdccontrol1680_UpdateData(&cdc_control1_680);
            byteArray = QByteArray(Data, 48);
            canComm->enqueueMessage(0x680, byteArray);

            Cdccontrol1683_Reset(&cdc_control1_683);
            cdc_control1_683.cdc_auto_l_cmr_valve_pwr_req = Cdc_control2_683_Cdc_auto_l_cmr_valve_pwr_reqType_CDC_AUTO_L_CMR_VALVE_PWR_REQ_ON_E2;
            cdc_control1_683.cdc_auto_r_cmr_valve_pwr_req = Cdc_control2_683_Cdc_auto_r_cmr_valve_pwr_reqType_CDC_AUTO_R_CMR_VALVE_PWR_REQ_ON_E2;
            cdc_control1_683.cdc_auto_b_cmr_valve_pwr_req = Cdc_control2_683_Cdc_auto_b_cmr_valve_pwr_reqType_CDC_AUTO_B_CMR_VALVE_PWR_REQ_ON_E2;
            Cdccontrol1683_UpdateData(&cdc_control1_683);
            byteArray = QByteArray(Data683, 64);
            canComm->enqueueMessage(0x683, byteArray);

            cdc_15_443_i.le_cam_wiper_ctl = Cdc_15_443_Le_cam_wiper_ctlType_LE_CAM_WIPER_CTL_ACTIVE_E1;
            cdc_15_443_i.le_cam_wiper_number_ctl = 3;
            cdc_15_443_i.le_cam_wiper_frequency_ctl = 60;
            cdc_15_443_i.re_cam_wiper_ctl = Cdc_15_443_Re_cam_wiper_ctlType_RE_CAM_WIPER_CTL_ACTIVE_E1;
            cdc_15_443_i.re_cam_wiper_number_ctl = 3;
            cdc_15_443_i.re_cam_wiper_frequency_ctl = 60;
            cdc_15_443_i.ri_cam_wiper_ctl = Cdc_15_443_Ri_cam_wiper_ctlType_RI_CAM_WIPER_CTL_ACTIVE_E1;
            cdc_15_443_i.ri_cam_wiper_number_ctl = 3;
            cdc_15_443_i.ri_cam_wiper_frequency_ctl = 60;
            Cdc15443_UpdateData(&cdc_15_443_i);
            byteArray = QByteArray(Data443, 64);
            canComm->enqueueMessage(0x443, byteArray);

            QThread::msleep(150);

            Cdccontrol1683_Reset(&cdc_control1_683);
            cdc_control1_683.cdc_auto_l_cmr_valve_pwr_req = Cdc_control2_683_Cdc_auto_l_cmr_valve_pwr_reqType_CDC_AUTO_L_CMR_VALVE_PWR_REQ_OFF_E1;
            cdc_control1_683.cdc_auto_r_cmr_valve_pwr_req = Cdc_control2_683_Cdc_auto_r_cmr_valve_pwr_reqType_CDC_AUTO_R_CMR_VALVE_PWR_REQ_OFF_E1;
            cdc_control1_683.cdc_auto_b_cmr_valve_pwr_req = Cdc_control2_683_Cdc_auto_b_cmr_valve_pwr_reqType_CDC_AUTO_B_CMR_VALVE_PWR_REQ_OFF_E1;
            Cdccontrol1683_UpdateData(&cdc_control1_683);
            byteArray = QByteArray(Data683, 64);
            canComm->enqueueMessage(0x683, byteArray);
            QThread::msleep(4850);

            cdc_15_443_i.le_cam_wiper_ctl = Cdc_15_443_Le_cam_wiper_ctlType_LE_CAM_WIPER_CTL_STANDBY_POSITION_E2;
            cdc_15_443_i.le_cam_wiper_number_ctl = 0;
            cdc_15_443_i.le_cam_wiper_frequency_ctl = 0;
            cdc_15_443_i.re_cam_wiper_ctl = Cdc_15_443_Re_cam_wiper_ctlType_RE_CAM_WIPER_CTL_STANDBY_POSITION_E2;
            cdc_15_443_i.re_cam_wiper_number_ctl = 0;
            cdc_15_443_i.re_cam_wiper_frequency_ctl = 0;
            cdc_15_443_i.ri_cam_wiper_ctl = Cdc_15_443_Ri_cam_wiper_ctlType_RI_CAM_WIPER_CTL_STANDBY_POSITION_E2;
            cdc_15_443_i.ri_cam_wiper_number_ctl = 0;
            cdc_15_443_i.ri_cam_wiper_frequency_ctl = 0;
            Cdc15443_UpdateData(&cdc_15_443_i);
            byteArray = QByteArray(Data443, 64);
            if (canComm->sendMessage(0x443, byteArray)) {
                appendResultToStep(7, true);
            }else{
                appendResultToStep(7,false);
            }
            QThread::msleep(ui->pressureEdit_5->text().toInt());

            Cdccontrol1680_Reset(&cdc_control1_680);
            cdc_control1_680.cdc_cleanpumpspd = 2;

            cdc_control1_680.cdc_frchassisctrl = Cdc_control1_680_Cdc_frchassisctrlType_CDC_FRCHASSISCTRL_MOVE_E1;
            cdc_control1_680.cdc_frchassistgtctrl = 50;
            cdc_control1_680.cdc_midchassisctrl = Cdc_control1_680_Cdc_midchassisctrlType_CDC_MIDCHASSISCTRL_MOVE_E1;
            cdc_control1_680.cdc_midchassistgtctrl = 50;
            cdc_control1_680.cdc_rechassisctrl = Cdc_control1_680_Cdc_rechassisctrlType_CDC_RECHASSISCTRL_MOVE_E1;
            cdc_control1_680.cdc_rechassistgtctrl = 50;
            Cdccontrol1680_UpdateData(&cdc_control1_680);
            byteArray = QByteArray(Data, 48);
            canComm->enqueueMessage(0x680, byteArray);

            Cdccontrol1683_Reset(&cdc_control1_683);
            cdc_control1_683.cdc_frchassisvvctrl = Cdc_control2_683_Cdc_frchassisvvctrlType_CDC_FRCHASSISVVCTRL_ON_E2;
            cdc_control1_683.cdc_midchassisvvctrl = Cdc_control2_683_Cdc_midchassisvvctrlType_CDC_MIDCHASSISVVCTRL_ON_E2;
            cdc_control1_683.cdc_rechassisvvctrl = Cdc_control2_683_Cdc_rechassisvvctrlType_CDC_RECHASSISVVCTRL_ON_E2;
            Cdccontrol1683_UpdateData(&cdc_control1_683);
            byteArray = QByteArray(Data683, 64);
            canComm->enqueueMessage(0x683, byteArray);

            QThread::msleep(200);

            Cdccontrol1680_Reset(&cdc_control1_680);
            cdc_control1_680.cdc_cleanpumpspd = 2;
            cdc_control1_680.cdc_frchassisctrl = Cdc_control1_680_Cdc_frchassisctrlType_CDC_FRCHASSISCTRL_MOVE_E1;
            cdc_control1_680.cdc_frchassistgtctrl = 0;
            cdc_control1_680.cdc_midchassisctrl = Cdc_control1_680_Cdc_midchassisctrlType_CDC_MIDCHASSISCTRL_MOVE_E1;
            cdc_control1_680.cdc_midchassistgtctrl = 0;
            cdc_control1_680.cdc_rechassisctrl = Cdc_control1_680_Cdc_rechassisctrlType_CDC_RECHASSISCTRL_MOVE_E1;
            cdc_control1_680.cdc_rechassistgtctrl = 0;
            Cdccontrol1680_UpdateData(&cdc_control1_680);
            byteArray = QByteArray(Data, 48);
            canComm->enqueueMessage(0x680, byteArray);

            Cdccontrol1683_Reset(&cdc_control1_683);
            cdc_control1_683.cdc_frchassisvvctrl = Cdc_control2_683_Cdc_frchassisvvctrlType_CDC_FRCHASSISVVCTRL_OFF_E1;
            cdc_control1_683.cdc_midchassisvvctrl = Cdc_control2_683_Cdc_midchassisvvctrlType_CDC_MIDCHASSISVVCTRL_OFF_E1;
            cdc_control1_683.cdc_rechassisvvctrl = Cdc_control2_683_Cdc_rechassisvvctrlType_CDC_RECHASSISVVCTRL_OFF_E1;
            Cdccontrol1683_UpdateData(&cdc_control1_683);
            byteArray = QByteArray(Data683, 64);
            if (canComm->sendMessage(0x683, byteArray)) {
                appendResultToStep(8, true);
            }else{
                appendResultToStep(8,false);
            }
            QThread::msleep(ui->pressureEdit_5->text().toInt());

            Cdccontrol1680_Reset(&cdc_control1_680);
            cdc_control1_680.cdc_cleanpumpspd = 0.0;
            cdc_control1_680.cdc_cleanpumpecupwr = Cdc_control1_680_Cdc_cleanpumpecupwrType_CDC_CLEANPUMPECUPWR_CLOSE_E1;
            Cdccontrol1680_UpdateData(&cdc_control1_680);
            byteArray = QByteArray(Data, 48);
            if (canComm->sendMessage(0x680, byteArray)) {
                appendResultToStep(9, true);
            }else{
                appendResultToStep(9,false);
            }
            QThread::msleep(ui->pressureEdit_6->text().toInt());
            qDebug()<< "over times = " << i;

            int runCount = count - i -1;
            ui->countLineEdit->setText(QString::number(runCount));
       }
       sendThreadRunning = false;
    });

    sendThread->start();
}

void MainWindow::on_MEstopButton_clicked()
{
    if (!sendThreadRunning) return;

       sendThreadRunning = false;

       if (sendThread) {
           sendThread->quit();
           sendThread->wait();
           delete sendThread;
           sendThread = nullptr;
       }

    qDebug() << "Send thread stopped.";
}

void MainWindow::markStepResult(int stepIndex, bool success)
{
    if (stepIndex < 0 || stepIndex >= model->rowCount()) return;

    // 当前时间戳
    QString timestamp = QDateTime::currentDateTime().toString("HH:mm:ss");

    // 构造结果文本
    QString statusText = QString("[%1] %2").arg(timestamp, success ? "finish" : "error");

    QStandardItem *resultItem = new QStandardItem(statusText);

    if (success) {
        resultItem->setForeground(QBrush(Qt::darkGreen));
    } else {
        resultItem->setForeground(QBrush(Qt::red));
        resultItem->setFont(QFont("Arial", -1, QFont::Bold));
    }

    // 插入到步骤下方
    model->insertRow(stepIndex + 1, resultItem);
    ui->listView->scrollToBottom();
}

void MainWindow::markStepResultSameLine(int stepIndex, bool success)
{
    if (stepIndex < 0 || stepIndex >= model->rowCount()) return;

    QStandardItem *item = model->item(stepIndex);
    if (!item) return;

    QString timestamp = QDateTime::currentDateTime().toString("HH:mm:ss");
    QString statusText = QString(" [%1] %2").arg(timestamp, success ? "finish" : "error");

    QString newText = item->text() + statusText;
    item->setText(newText);

    // 设置颜色
    if (success) {
        item->setForeground(QBrush(Qt::darkGreen));
    } else {
        item->setForeground(QBrush(Qt::red));
        item->setFont(QFont("Arial", -1, QFont::Bold));
    }
}

void MainWindow::initSteps(const QStringList &steps)
{
    for (const QString &step : steps) {
        QStandardItem *stepItem = new QStandardItem(step);
        stepItem->setEditable(false);
        model->appendRow(stepItem);
        stepItems.append(stepItem);
    }
}

void MainWindow::appendResultToStep(int stepIndex, bool success)
{
    if (stepIndex < 0 || stepIndex >= stepItems.size()) return;

    QString timestamp = QDateTime::currentDateTime().toString("HH:mm:ss");
    QString statusText = QString("[%1] %2").arg(timestamp, success ? "finish" : "error");

    QStandardItem *resultItem = new QStandardItem(statusText);
    resultItem->setEditable(false);

    if (success) {
        resultItem->setForeground(QBrush(Qt::darkGreen));
    } else {
        resultItem->setForeground(QBrush(Qt::red));
        resultItem->setFont(QFont("Arial", -1, QFont::Bold));
    }

    // 添加子项（不强制 UI 动作）
    stepItems[stepIndex]->appendRow(resultItem);

    // 可选：不自动展开，不自动滚动，避免 UI 负担
    // ui->treeView->expand(stepItems[stepIndex]->index());
    // ui->treeView->scrollTo(resultItem->index());
}


void MainWindow::on_makeButton_clicked()
{
    QByteArray byteArray;
    if(ui->treeWidget_3->currentSelectedId() == 1002 || ui->treeWidget_3->currentSelectedId() == 1003){
        Cdccontrol1680_Reset(&cdc_control1_680);
        cdc_control1_680.cdc_cleanpumpecupwr = Cdc_control1_680_Cdc_cleanpumpecupwrType_CDC_CLEANPUMPECUPWR_OPEN_E2;
        cdc_control1_680.cdc_cleanpumpspd = ui->pressureEdit->text().toDouble();
        Cdccontrol1680_UpdateData(&cdc_control1_680);
        byteArray = QByteArray(Data, 48);
        canComm->sendMessage(0x680, byteArray);
        QThread::sleep(1);
    }

    Cdccontrol1683_Reset(&cdc_control1_683);
    this->selectControl1_680(&cdc_control1_683, &cdc_control1_680, true);
    if(ui->treeWidget_3->currentSelectedId() == 1001 || ui->treeWidget_3->currentSelectedId() == 1003)
    {
        Cdc15443_UpdateData(&cdc_15_443_i);
        byteArray = QByteArray(Data443, 64);
        canComm->sendMessage(0x443, byteArray);
    }
    if(ui->treeWidget_3->currentSelectedId() == 1002 || ui->treeWidget_3->currentSelectedId() == 1003)
    {
        Cdccontrol1683_UpdateData(&cdc_control1_683);
        byteArray = QByteArray(Data683, 64);
        canComm->enqueueMessage(0x683, byteArray);
    }
    if(ui->treeWidget_3->currentSelectedId() == 1004 || ui->treeWidget_3->currentSelectedId() == 1005)
    {
        Cdccontrol1680_UpdateData(&cdc_control1_680);
        byteArray = QByteArray(Data, 48);
        canComm->sendMessage(0x680, byteArray);
    }

    if(ui->treeWidget_3->currentSelectedId() == 1001){
        if(ui->lineEdit_7->text().toInt() > 0){
                QTimer::singleShot((ui->lineEdit_7->text().toInt()*2000), this, [=](){
                emit timerMake(1);
            });
        }
    }else if(ui->treeWidget_3->currentSelectedId() == 1002)
    {
        if(ui->lineEdit_6->text().toInt() > 0){
                QTimer::singleShot(ui->lineEdit_6->text().toInt(), this, [=](){
                emit timerMake(2);
            });
        }
    }else if(ui->treeWidget_3->currentSelectedId() == 1003){
        if(ui->lineEdit_7->text().toInt() > 0){
                QTimer::singleShot((ui->lineEdit_7->text().toInt()*2000), this, [=](){
                emit timerMake(1);
            });
        }
        if(ui->lineEdit_6->text().toInt() > 0){
                QTimer::singleShot(ui->lineEdit_6->text().toInt(), this, [=](){
                emit timerMake(2);
            });
        }
    }else{

    }
}

void MainWindow::on_makeButton_2_clicked()
{
    Cdccontrol1683_Reset(&cdc_control1_683);
    QByteArray byteArray;
    this->selectControl1_680(&cdc_control1_683, &cdc_control1_680, false);
    if(ui->treeWidget_3->currentSelectedId() == 1001 || ui->treeWidget_3->currentSelectedId() == 1003)
    {
        Cdc15443_UpdateData(&cdc_15_443_i);
        byteArray = QByteArray(Data443, 64);
        canComm->sendMessage(0x443, byteArray);
    }
    if(ui->treeWidget_3->currentSelectedId() == 1002 || ui->treeWidget_3->currentSelectedId() == 1003)
    {
        Cdccontrol1680_Reset(&cdc_control1_680);
        cdc_control1_680.cdc_cleanpumpecupwr = Cdc_control1_680_Cdc_cleanpumpecupwrType_CDC_CLEANPUMPECUPWR_CLOSE_E1;
        Cdccontrol1680_UpdateData(&cdc_control1_680);
        byteArray = QByteArray(Data, 48);
    //    QString hexStr = byteArray.toHex(' ').toUpper();
    //    qDebug() << hexStr;
        canComm->enqueueMessage(0x680, byteArray);

        Cdccontrol1683_UpdateData(&cdc_control1_683);
        byteArray = QByteArray(Data683, 64);
        canComm->enqueueMessage(0x683, byteArray);
    }
    if(ui->treeWidget_3->currentSelectedId() == 1004 || ui->treeWidget_3->currentSelectedId() == 1005)
    {
        Cdccontrol1680_UpdateData(&cdc_control1_680);
        byteArray = QByteArray(Data, 48);
        canComm->sendMessage(0x680, byteArray);
    }
}

void selectItemById(QTreeWidget *tree, int id) {
    QTreeWidgetItemIterator it(tree);
    while (*it) {
        if ((*it)->data(0, Qt::UserRole).toInt() == id) {
            tree->setCurrentItem(*it);
            (*it)->setSelected(true);   // 确保选中高亮
            tree->scrollToItem(*it);    // 滚动到可见位置（可选）
            break;
        }
        ++it;
    }
}

void MainWindow::on_timerMake(int value)
{
    if(value == 1){
        selectItemById(ui->treeWidget_3, 1001);
        this->selectControl1_680(&cdc_control1_683, &cdc_control1_680, false);
        Cdc15443_UpdateData(&cdc_15_443_i);
        QByteArray byteArray(Data443, 64);
        canComm->sendMessage(0x443, byteArray);
    }else if(value == 2){
        selectItemById(ui->treeWidget_3, 1002);
        Cdccontrol1680_Reset(&cdc_control1_680);
        if(ui->treeWidget_3->currentSelectedId() == 1002 || ui->treeWidget_3->currentSelectedId() == 1003){
            cdc_control1_680.cdc_cleanpumpecupwr = Cdc_control1_680_Cdc_cleanpumpecupwrType_CDC_CLEANPUMPECUPWR_CLOSE_E1;
        }
        Cdccontrol1680_UpdateData(&cdc_control1_680);
        QByteArray byteArray(Data, 48);
        canComm->enqueueMessage(0x680, byteArray);

        Cdccontrol1683_Reset(&cdc_control1_683);
        this->selectControl1_680(&cdc_control1_683, &cdc_control1_680, false);
        Cdccontrol1683_UpdateData(&cdc_control1_683);
        byteArray = QByteArray(Data683, 64);
        canComm->enqueueMessage(0x683, byteArray);
    }else{

    }
}

void MainWindow::selectControl1_680(Cdc_control2_683 *sle_control1_680, Cdc_control1_680 *sle_control1_683, bool bControl1)
{
    if(bControl1){
        switch (ui->treeWidget->currentSelectedId()) {
        case 101:
            if(ui->treeWidget_3->currentSelectedId() == 1001)
            {
                cdc_15_443_i.ri_blid_wiper_ctl = Cdc_15_443_Ri_blid_wiper_ctlType_RI_BLID_WIPER_CTL_ACTIVE_E1;
                cdc_15_443_i.ri_blid_wiper_number_ctl = ui->lineEdit_7->text().toInt();
                cdc_15_443_i.ri_blid_wiper_frequency_ctl = 30;
            }else if(ui->treeWidget_3->currentSelectedId() == 1002){
                sle_control1_680->cdc_rilidarvalvepwr = Cdc_control2_683_Cdc_rilidarvalvepwrType_CDC_RILIDARVALVEPWR_ON_E2;
            }else if(ui->treeWidget_3->currentSelectedId() == 1003){
                cdc_15_443_i.ri_blid_wiper_ctl = Cdc_15_443_Ri_blid_wiper_ctlType_RI_BLID_WIPER_CTL_ACTIVE_E1;
                cdc_15_443_i.ri_blid_wiper_number_ctl = ui->lineEdit_7->text().toInt();
                cdc_15_443_i.ri_blid_wiper_frequency_ctl = 30;
                sle_control1_680->cdc_rilidarvalvepwr = Cdc_control2_683_Cdc_rilidarvalvepwrType_CDC_RILIDARVALVEPWR_ON_E2;
            }else{
            }
            break;
        case 102:
            if(ui->treeWidget_3->currentSelectedId() == 1001)
            {
            }else if(ui->treeWidget_3->currentSelectedId() == 1002){
                sle_control1_680->cdc_fisheye_r_cmr_valve_pwr_req = Cdc_control2_683_Cdc_fisheye_r_cmr_valve_pwr_reqType_CDC_FISHEYE_R_CMR_VALVE_PWR_REQ_ON_E2;
            }else if(ui->treeWidget_3->currentSelectedId() == 1003){
                sle_control1_680->cdc_fisheye_r_cmr_valve_pwr_req = Cdc_control2_683_Cdc_fisheye_r_cmr_valve_pwr_reqType_CDC_FISHEYE_R_CMR_VALVE_PWR_REQ_ON_E2;
            }else if(ui->treeWidget_3->currentSelectedId() == 1004){
                sle_control1_683->cdc_fisheye_r_cmr_heat_req = Cdc_control1_680_Cdc_fisheye_r_cmr_heat_reqType_CDC_FISHEYE_R_CMR_HEAT_REQ_OPEN_E2;
            }else if(ui->treeWidget_3->currentSelectedId() == 1005){
                sle_control1_683->cdc_fisheye_r_cmr_shield_req = Cdc_control1_680_Cdc_fisheye_r_cmr_shield_reqType_CDC_FISHEYE_R_CMR_SHIELD_REQ_OPEN_E1;
            }else{
            }
            break;
        case 103:
            if(ui->treeWidget_3->currentSelectedId() == 1001)
            {
                cdc_15_443_i.fr_blid_wiper_ctl = Cdc_15_443_Fr_blid_wiper_ctlType_FR_BLID_WIPER_CTL_ACTIVE_E1;
                cdc_15_443_i.fr_blid_wiper_number_ctl = ui->lineEdit_7->text().toInt();
                cdc_15_443_i.fr_blid_wiper_frequency_ctl = 30;
            }else if(ui->treeWidget_3->currentSelectedId() == 1002){
                sle_control1_680->cdc_frlidarvalvepwr = Cdc_control2_683_Cdc_frlidarvalvepwrType_CDC_FRLIDARVALVEPWR_ON_E2;
            }else if(ui->treeWidget_3->currentSelectedId() == 1003){
                cdc_15_443_i.fr_blid_wiper_ctl = Cdc_15_443_Fr_blid_wiper_ctlType_FR_BLID_WIPER_CTL_ACTIVE_E1;
                cdc_15_443_i.fr_blid_wiper_number_ctl = ui->lineEdit_7->text().toInt();
                cdc_15_443_i.fr_blid_wiper_frequency_ctl = 30;
                sle_control1_680->cdc_frlidarvalvepwr = Cdc_control2_683_Cdc_frlidarvalvepwrType_CDC_FRLIDARVALVEPWR_ON_E2;
            }else{
            }
            break;
        case 104:
            if(ui->treeWidget_3->currentSelectedId() == 1001)
            {
            }else if(ui->treeWidget_3->currentSelectedId() == 1002){
                sle_control1_680->cdc_fisheye_f_cmr_valve_pwr_req = Cdc_control2_683_Cdc_fisheye_f_cmr_valve_pwr_reqType_CDC_FISHEYE_F_CMR_VALVE_PWR_REQ_ON_E2;
            }else if(ui->treeWidget_3->currentSelectedId() == 1003){
                sle_control1_680->cdc_fisheye_f_cmr_valve_pwr_req = Cdc_control2_683_Cdc_fisheye_f_cmr_valve_pwr_reqType_CDC_FISHEYE_F_CMR_VALVE_PWR_REQ_ON_E2;
            }else if(ui->treeWidget_3->currentSelectedId() == 1004){
                sle_control1_683->cdc_fisheye_f_cmr_heat_req = Cdc_control1_680_Cdc_fisheye_f_cmr_heat_reqType_CDC_FISHEYE_F_CMR_HEAT_REQ_OPEN_E2;
            }else if(ui->treeWidget_3->currentSelectedId() == 1005){
                sle_control1_683->cdc_fisheye_f_cmr_shield_req = Cdc_control1_680_Cdc_fisheye_f_cmr_shield_reqType_CDC_FISHEYE_F_CMR_SHIELD_REQ_OPEN_E1;
            }else{
            }
            break;
        case 105:
            if(ui->treeWidget_3->currentSelectedId() == 1001)
            {
                cdc_15_443_i.le_blid_wiper_ctl = Cdc_15_443_Le_blid_wiper_ctlType_LE_BLID_WIPER_CTL_ACTIVE_E1;
                cdc_15_443_i.le_blid_wiper_number_ctl = ui->lineEdit_7->text().toInt();
                cdc_15_443_i.le_blid_wiper_frequency_ctl = 30;
            }else if(ui->treeWidget_3->currentSelectedId() == 1002){
                sle_control1_680->cdc_lelidarvalvepwr = Cdc_control2_683_Cdc_lelidarvalvepwrType_CDC_LELIDARVALVEPWR_ON_E2;
            }else if(ui->treeWidget_3->currentSelectedId() == 1003){
                cdc_15_443_i.le_blid_wiper_ctl = Cdc_15_443_Le_blid_wiper_ctlType_LE_BLID_WIPER_CTL_ACTIVE_E1;
                cdc_15_443_i.le_blid_wiper_number_ctl = ui->lineEdit_7->text().toInt();
                cdc_15_443_i.le_blid_wiper_frequency_ctl = 30;
                sle_control1_680->cdc_lelidarvalvepwr = Cdc_control2_683_Cdc_lelidarvalvepwrType_CDC_LELIDARVALVEPWR_ON_E2;
            }else{
            }
            break;
        case 106:
            if(ui->treeWidget_3->currentSelectedId() == 1001)
            {
            }else if(ui->treeWidget_3->currentSelectedId() == 1002){
                sle_control1_680->cdc_fisheye_l_cmr_valve_pwr_req = Cdc_control2_683_Cdc_fisheye_l_cmr_valve_pwr_reqType_CDC_FISHEYE_L_CMR_VALVE_PWR_REQ_ON_E2;
            }else if(ui->treeWidget_3->currentSelectedId() == 1003){
                sle_control1_680->cdc_fisheye_l_cmr_valve_pwr_req = Cdc_control2_683_Cdc_fisheye_l_cmr_valve_pwr_reqType_CDC_FISHEYE_L_CMR_VALVE_PWR_REQ_ON_E2;
            }else if(ui->treeWidget_3->currentSelectedId() == 1004){
                sle_control1_683->cdc_fisheye_l_cmr_heat_req = Cdc_control1_680_Cdc_fisheye_l_cmr_heat_reqType_CDC_FISHEYE_L_CMR_HEAT_REQ_OPEN_E2;
            }else if(ui->treeWidget_3->currentSelectedId() == 1005){
                sle_control1_683->cdc_fisheye_l_cmr_shield_req = Cdc_control1_680_Cdc_fisheye_l_cmr_shield_reqType_CDC_FISHEYE_L_CMR_SHIELD_REQ_OPEN_E1;
            }else{
            }
            break;
        case 107:
            if(ui->treeWidget_3->currentSelectedId() == 1001)
            {
                cdc_15_443_i.lf_pcam_wiper_ctl = Cdc_15_443_Lf_pcam_wiper_ctlType_LF_PCAM_WIPER_CTL_ACTIVE_E1;
                cdc_15_443_i.lf_pcam_wiper_number_ctl = ui->lineEdit_7->text().toInt();
                cdc_15_443_i.lf_pcam_wiper_frequency_ctl = 30;
            }else if(ui->treeWidget_3->currentSelectedId() == 1002){
                sle_control1_680->cdc_auto_lp_cmr_valve_pwr_req = Cdc_control2_683_Cdc_auto_lp_cmr_valve_pwr_reqType_CDC_AUTO_LP_CMR_VALVE_PWR_REQ_ON_E2;
            }else if(ui->treeWidget_3->currentSelectedId() == 1003){
                cdc_15_443_i.lf_pcam_wiper_ctl = Cdc_15_443_Lf_pcam_wiper_ctlType_LF_PCAM_WIPER_CTL_ACTIVE_E1;
                cdc_15_443_i.lf_pcam_wiper_number_ctl = ui->lineEdit_7->text().toInt();
                cdc_15_443_i.lf_pcam_wiper_frequency_ctl = 30;
                sle_control1_680->cdc_auto_lp_cmr_valve_pwr_req = Cdc_control2_683_Cdc_auto_lp_cmr_valve_pwr_reqType_CDC_AUTO_LP_CMR_VALVE_PWR_REQ_ON_E2;
            }else if(ui->treeWidget_3->currentSelectedId() == 1004){
                sle_control1_683->cdc_auto_lp_cmr_heat_req = Cdc_control1_680_Cdc_auto_lp_cmr_heat_reqType_CDC_AUTO_LP_CMR_HEAT_REQ_OPEN_E2;
            }else{
            }
            break;
        case 108:
            if(ui->treeWidget_3->currentSelectedId() == 1002){
                sle_control1_680->cdc_frchassisvvctrl = Cdc_control2_683_Cdc_frchassisvvctrlType_CDC_FRCHASSISVVCTRL_ON_E2;
            }else if(ui->treeWidget_3->currentSelectedId() == 1003){
                sle_control1_680->cdc_frchassisvvctrl = Cdc_control2_683_Cdc_frchassisvvctrlType_CDC_FRCHASSISVVCTRL_ON_E2;
            }else{
            }
            break;
        case 201:
            if(ui->treeWidget_3->currentSelectedId() == 1001)
            {
                cdc_15_443_i.lrb_pcam_wiper_ctl = Cdc_15_443_Lrb_pcam_wiper_ctlType_LRB_PCAM_WIPER_CTL_ACTIVE_E1;
                cdc_15_443_i.lrb_pcam_wiper_number_ctl = ui->lineEdit_7->text().toInt();
                cdc_15_443_i.lrb_pcam_wiper_frequency_ctl = 30;
            }else if(ui->treeWidget_3->currentSelectedId() == 1002){
                sle_control1_680->cdc_auto_rl_cmr_valve_pwr_req = Cdc_control2_683_Cdc_auto_rl_cmr_valve_pwr_reqType_CDC_AUTO_RL_CMR_VALVE_PWR_REQ_ON_E2;
            }else if(ui->treeWidget_3->currentSelectedId() == 1003){
                cdc_15_443_i.lrb_pcam_wiper_ctl = Cdc_15_443_Lrb_pcam_wiper_ctlType_LRB_PCAM_WIPER_CTL_ACTIVE_E1;
                cdc_15_443_i.lrb_pcam_wiper_number_ctl = ui->lineEdit_7->text().toInt();
                cdc_15_443_i.lrb_pcam_wiper_frequency_ctl = 30;
                sle_control1_680->cdc_auto_rl_cmr_valve_pwr_req = Cdc_control2_683_Cdc_auto_rl_cmr_valve_pwr_reqType_CDC_AUTO_RL_CMR_VALVE_PWR_REQ_ON_E2;
            }else if(ui->treeWidget_3->currentSelectedId() == 1004){
                sle_control1_683->cdc_auto_lb_cmr_heat_req = Cdc_control1_680_Cdc_auto_lb_cmr_heat_reqType_CDC_AUTO_LB_CMR_HEAT_REQ_OPEN_E2;
            }else{
            }
            break;
        case 202:
            if(ui->treeWidget_3->currentSelectedId() == 1001)
            {
                cdc_15_443_i.rrb_pcam_wiper_ctl = Cdc_15_443_Rrb_pcam_wiper_ctlType_RRB_PCAM_WIPER_CTL_ACTIVE_E1;
                cdc_15_443_i.rrb_pcam_wiper_number_ctl = ui->lineEdit_7->text().toInt();
                cdc_15_443_i.rrb_pcam_wiper_frequency_ctl = 30;
            }else if(ui->treeWidget_3->currentSelectedId() == 1002){
                sle_control1_680->cdc_auto_rr_cmr_valve_pwr_req = Cdc_control2_683_Cdc_auto_rr_cmr_valve_pwr_reqType_CDC_AUTO_RR_CMR_VALVE_PWR_REQ_ON_E2;
            }else if(ui->treeWidget_3->currentSelectedId() == 1003){
                cdc_15_443_i.rrb_pcam_wiper_ctl = Cdc_15_443_Rrb_pcam_wiper_ctlType_RRB_PCAM_WIPER_CTL_ACTIVE_E1;
                cdc_15_443_i.rrb_pcam_wiper_number_ctl = ui->lineEdit_7->text().toInt();
                cdc_15_443_i.rrb_pcam_wiper_frequency_ctl = 30;
                sle_control1_680->cdc_auto_rr_cmr_valve_pwr_req = Cdc_control2_683_Cdc_auto_rr_cmr_valve_pwr_reqType_CDC_AUTO_RR_CMR_VALVE_PWR_REQ_ON_E2;
            }else if(ui->treeWidget_3->currentSelectedId() == 1004){
                sle_control1_683->cdc_auto_rb_cmr_heat_req = Cdc_control1_680_Cdc_auto_rb_cmr_heat_reqType_CDC_AUTO_RB_CMR_HEAT_REQ_OPEN_E2;
            }else{
            }
            break;
        case 203:
            if(ui->treeWidget_3->currentSelectedId() == 1001)
            {
            }else if(ui->treeWidget_3->currentSelectedId() == 1002){
                sle_control1_680->cdc_fisheye_b_cmr_valve_pwr_req = Cdc_control2_683_Cdc_fisheye_b_cmr_valve_pwr_reqType_CDC_FISHEYE_B_CMR_VALVE_PWR_REQ_ON_E2;
            }else if(ui->treeWidget_3->currentSelectedId() == 1003){
                sle_control1_680->cdc_fisheye_b_cmr_valve_pwr_req = Cdc_control2_683_Cdc_fisheye_b_cmr_valve_pwr_reqType_CDC_FISHEYE_B_CMR_VALVE_PWR_REQ_ON_E2;
            }else if(ui->treeWidget_3->currentSelectedId() == 1004){
                sle_control1_683->cdc_fisheye_b_cmr_heat_req = Cdc_control1_680_Cdc_fisheye_b_cmr_heat_reqType_CDC_FISHEYE_B_CMR_HEAT_REQ_OPEN_E2;
            }else if(ui->treeWidget_3->currentSelectedId() == 1005){
                sle_control1_683->cdc_fisheye_b_cmr_shield_req = Cdc_control1_680_Cdc_fisheye_b_cmr_shield_reqType_CDC_FISHEYE_B_CMR_SHIELD_REQ_OPEN_E1;
            }else{
            }
            break;
        case 204:
            if(ui->treeWidget_3->currentSelectedId() == 1001)
            {
                cdc_15_443_i.re_blid_rac_wiper_ctl = Cdc_15_443_Re_blid_rac_wiper_ctlType_RE_BLID_RAC_WIPER_CTL_ACTIVE_E1;
                cdc_15_443_i.re_blid_rac_wiper_number_ctl = ui->lineEdit_7->text().toInt();
                cdc_15_443_i.re_blid_rac_wiper_frequency_ctl = 30;
            }else if(ui->treeWidget_3->currentSelectedId() == 1002){
                sle_control1_680->cdc_relidarvalvepwr = Cdc_control2_683_Cdc_relidarvalvepwrType_CDC_RELIDARVALVEPWR_ON_E2;
            }else if(ui->treeWidget_3->currentSelectedId() == 1003){
                cdc_15_443_i.re_blid_rac_wiper_ctl = Cdc_15_443_Re_blid_rac_wiper_ctlType_RE_BLID_RAC_WIPER_CTL_ACTIVE_E1;
                cdc_15_443_i.re_blid_rac_wiper_number_ctl = ui->lineEdit_7->text().toInt();
                cdc_15_443_i.re_blid_rac_wiper_frequency_ctl = 30;
                sle_control1_680->cdc_relidarvalvepwr = Cdc_control2_683_Cdc_relidarvalvepwrType_CDC_RELIDARVALVEPWR_ON_E2;
            }else{
            }
            break;
        case 205:
            if(ui->treeWidget_3->currentSelectedId() == 1002){
                sle_control1_680->cdc_midchassisvvctrl = Cdc_control2_683_Cdc_midchassisvvctrlType_CDC_MIDCHASSISVVCTRL_ON_E2;
            }else if(ui->treeWidget_3->currentSelectedId() == 1003){
                sle_control1_680->cdc_midchassisvvctrl = Cdc_control2_683_Cdc_midchassisvvctrlType_CDC_MIDCHASSISVVCTRL_ON_E2;
            }else{
            }
            break;
        case 206:
            if(ui->treeWidget_3->currentSelectedId() == 1002){
                sle_control1_680->cdc_rechassisvvctrl = Cdc_control2_683_Cdc_rechassisvvctrlType_CDC_RECHASSISVVCTRL_ON_E2;
            }else if(ui->treeWidget_3->currentSelectedId() == 1003){
                sle_control1_680->cdc_rechassisvvctrl = Cdc_control2_683_Cdc_rechassisvvctrlType_CDC_RECHASSISVVCTRL_ON_E2;
            }else{
            }
            break;
        case 301:
            if(ui->treeWidget_3->currentSelectedId() == 1001)
            {
                cdc_15_443_i.re_cam_wiper_ctl = Cdc_15_443_Re_cam_wiper_ctlType_RE_CAM_WIPER_CTL_ACTIVE_E1;
                cdc_15_443_i.re_cam_wiper_number_ctl = ui->lineEdit_7->text().toInt();
                cdc_15_443_i.re_cam_wiper_frequency_ctl = 30;
            }else if(ui->treeWidget_3->currentSelectedId() == 1002){
                sle_control1_680->cdc_auto_b_cmr_valve_pwr_req = Cdc_control2_683_Cdc_auto_b_cmr_valve_pwr_reqType_CDC_AUTO_B_CMR_VALVE_PWR_REQ_ON_E2;
            }else if(ui->treeWidget_3->currentSelectedId() == 1003){
                cdc_15_443_i.re_cam_wiper_ctl = Cdc_15_443_Re_cam_wiper_ctlType_RE_CAM_WIPER_CTL_ACTIVE_E1;
                cdc_15_443_i.re_cam_wiper_number_ctl = ui->lineEdit_7->text().toInt();
                cdc_15_443_i.re_cam_wiper_frequency_ctl = 30;
                sle_control1_680->cdc_auto_b_cmr_valve_pwr_req = Cdc_control2_683_Cdc_auto_b_cmr_valve_pwr_reqType_CDC_AUTO_B_CMR_VALVE_PWR_REQ_ON_E2;
            }else if(ui->treeWidget_3->currentSelectedId() == 1004){
                sle_control1_683->cdc_auto_b_cmr_heat_req = Cdc_control1_680_Cdc_auto_b_cmr_heat_reqType_CDC_AUTO_B_CMR_HEAT_REQ_OPEN_E2;
            }else if(ui->treeWidget_3->currentSelectedId() == 1005){
                sle_control1_683->cdc_auto_b_cmr_shield_req = Cdc_control1_680_Cdc_auto_b_cmr_shield_reqType_CDC_AUTO_B_CMR_SHIELD_REQ_OPEN_E1;
            }else{
            }
            break;
        case 302:
            if(ui->treeWidget_3->currentSelectedId() == 1001)
            {
            }else if(ui->treeWidget_3->currentSelectedId() == 1002){
                sle_control1_680->cdc_mlidarvalvepwr = Cdc_control2_683_Cdc_mlidarvalvepwrType_CDC_MLIDARVALVEPWR_ON_E2;
            }else if(ui->treeWidget_3->currentSelectedId() == 1003){
                sle_control1_680->cdc_mlidarvalvepwr = Cdc_control2_683_Cdc_mlidarvalvepwrType_CDC_MLIDARVALVEPWR_ON_E2;
            }else if(ui->treeWidget_3->currentSelectedId() == 1005){
                sle_control1_683->cdc_mlidarshieldmove = Cdc_control1_680_Cdc_mlidarshieldmoveType_CDC_MLIDARSHIELDMOVE_OPEN_E1;
            }else{
            }
            break;
        case 303:
            if(ui->treeWidget_3->currentSelectedId() == 1001)
            {
                cdc_15_443_i.fl_cam_wiper_ctl = Cdc_15_443_Fl_cam_wiper_ctlType_FL_CAM_WIPER_CTL_ACTIVE_E1;
                cdc_15_443_i.fl_cam_wiper_number_ctl = ui->lineEdit_7->text().toInt();
                cdc_15_443_i.fl_cam_wiper_frequency_ctl = 30;
            }else if(ui->treeWidget_3->currentSelectedId() == 1002){
                sle_control1_680->cdc_auto_lf_cmr_valve_pwr_req = Cdc_control2_683_Cdc_auto_lf_cmr_valve_pwr_reqType_CDC_AUTO_LF_CMR_VALVE_PWR_REQ_ON_E2;
            }else if(ui->treeWidget_3->currentSelectedId() == 1003){
                cdc_15_443_i.fl_cam_wiper_ctl = Cdc_15_443_Fl_cam_wiper_ctlType_FL_CAM_WIPER_CTL_ACTIVE_E1;
                cdc_15_443_i.fl_cam_wiper_number_ctl = ui->lineEdit_7->text().toInt();
                cdc_15_443_i.fl_cam_wiper_frequency_ctl = 30;
                sle_control1_680->cdc_auto_lf_cmr_valve_pwr_req = Cdc_control2_683_Cdc_auto_lf_cmr_valve_pwr_reqType_CDC_AUTO_LF_CMR_VALVE_PWR_REQ_ON_E2;
            }else if(ui->treeWidget_3->currentSelectedId() == 1004){
                sle_control1_683->cdc_auto_lf_cmr_heat_req = Cdc_control1_680_Cdc_auto_lf_cmr_heat_reqType_CDC_AUTO_LF_CMR_HEAT_REQ_OPEN_E2;
            }else{
            }
            break;
        case 304:
            if(ui->treeWidget_3->currentSelectedId() == 1001)
            {
                cdc_15_443_i.fr_cam_wiper_ctl = Cdc_15_443_Fr_cam_wiper_ctlType_FR_CAM_WIPER_CTL_ACTIVE_E1;
                cdc_15_443_i.fr_cam_wiper_number_ctl = ui->lineEdit_7->text().toInt();
                cdc_15_443_i.fr_cam_wiper_frequency_ctl = 30;
            }else if(ui->treeWidget_3->currentSelectedId() == 1002){
                sle_control1_680->cdc_auto_rf_cmr_valve_pwr_req = Cdc_control2_683_Cdc_auto_rf_cmr_valve_pwr_reqType_CDC_AUTO_RF_CMR_VALVE_PWR_REQ_ON_E2;
            }else if(ui->treeWidget_3->currentSelectedId() == 1003){
                cdc_15_443_i.fr_cam_wiper_ctl = Cdc_15_443_Fr_cam_wiper_ctlType_FR_CAM_WIPER_CTL_ACTIVE_E1;
                cdc_15_443_i.fr_cam_wiper_number_ctl = ui->lineEdit_7->text().toInt();
                cdc_15_443_i.fr_cam_wiper_frequency_ctl = 30;
                sle_control1_680->cdc_auto_rf_cmr_valve_pwr_req = Cdc_control2_683_Cdc_auto_rf_cmr_valve_pwr_reqType_CDC_AUTO_RF_CMR_VALVE_PWR_REQ_ON_E2;
            }else if(ui->treeWidget_3->currentSelectedId() == 1004){
                sle_control1_683->cdc_auto_rf_cmr_heat_req = Cdc_control1_680_Cdc_auto_rf_cmr_heat_reqType_CDC_AUTO_RF_CMR_HEAT_REQ_OPEN_E2;
            }else{
            }
            break;
        case 305:
            if(ui->treeWidget_3->currentSelectedId() == 1001)
            {
                cdc_15_443_i.le_cam_wiper_ctl = Cdc_15_443_Le_cam_wiper_ctlType_LE_CAM_WIPER_CTL_ACTIVE_E1;
                cdc_15_443_i.le_cam_wiper_number_ctl = ui->lineEdit_7->text().toInt();
                cdc_15_443_i.le_cam_wiper_frequency_ctl = 30;
            }else if(ui->treeWidget_3->currentSelectedId() == 1002){
                sle_control1_680->cdc_auto_l_cmr_valve_pwr_req = Cdc_control2_683_Cdc_auto_l_cmr_valve_pwr_reqType_CDC_AUTO_L_CMR_VALVE_PWR_REQ_ON_E2;
            }else if(ui->treeWidget_3->currentSelectedId() == 1003){
                cdc_15_443_i.le_cam_wiper_ctl = Cdc_15_443_Le_cam_wiper_ctlType_LE_CAM_WIPER_CTL_ACTIVE_E1;
                cdc_15_443_i.le_cam_wiper_number_ctl = ui->lineEdit_7->text().toInt();
                cdc_15_443_i.le_cam_wiper_frequency_ctl = 30;
                sle_control1_680->cdc_auto_l_cmr_valve_pwr_req = Cdc_control2_683_Cdc_auto_l_cmr_valve_pwr_reqType_CDC_AUTO_L_CMR_VALVE_PWR_REQ_ON_E2;
            }else if(ui->treeWidget_3->currentSelectedId() == 1004){
                sle_control1_683->cdc_auto_l_cmr_heat_req = Cdc_control1_680_Cdc_auto_l_cmr_heat_reqType_CDC_AUTO_L_CMR_HEAT_REQ_OPEN_E2;
            }else if(ui->treeWidget_3->currentSelectedId() == 1005){
                sle_control1_683->cdc_auto_l_cmr_shield_req = Cdc_control1_680_Cdc_auto_l_cmr_shield_reqType_CDC_AUTO_L_CMR_SHIELD_REQ_OPEN_E1;
            }else{
            }
            break;
        case 306:
            if(ui->treeWidget_3->currentSelectedId() == 1001)
            {
                cdc_15_443_i.ri_cam_wiper_ctl = Cdc_15_443_Ri_cam_wiper_ctlType_RI_CAM_WIPER_CTL_ACTIVE_E1;
                cdc_15_443_i.ri_cam_wiper_number_ctl = ui->lineEdit_7->text().toInt();
                cdc_15_443_i.ri_cam_wiper_frequency_ctl = 30;
            }else if(ui->treeWidget_3->currentSelectedId() == 1002){
                sle_control1_680->cdc_auto_r_cmr_valve_pwr_req = Cdc_control2_683_Cdc_auto_r_cmr_valve_pwr_reqType_CDC_AUTO_R_CMR_VALVE_PWR_REQ_ON_E2;
            }else if(ui->treeWidget_3->currentSelectedId() == 1003){
                cdc_15_443_i.ri_cam_wiper_ctl = Cdc_15_443_Ri_cam_wiper_ctlType_RI_CAM_WIPER_CTL_ACTIVE_E1;
                cdc_15_443_i.ri_cam_wiper_number_ctl = ui->lineEdit_7->text().toInt();
                cdc_15_443_i.ri_cam_wiper_frequency_ctl = 30;
                sle_control1_680->cdc_auto_r_cmr_valve_pwr_req = Cdc_control2_683_Cdc_auto_r_cmr_valve_pwr_reqType_CDC_AUTO_R_CMR_VALVE_PWR_REQ_ON_E2;
            }else if(ui->treeWidget_3->currentSelectedId() == 1004){
                sle_control1_683->cdc_auto_r_cmr_heat_req = Cdc_control1_680_Cdc_auto_r_cmr_heat_reqType_CDC_AUTO_R_CMR_HEAT_REQ_OPEN_E2;
            }else if(ui->treeWidget_3->currentSelectedId() == 1005){
                sle_control1_683->cdc_auto_r_cmr_shield_req = Cdc_control1_680_Cdc_auto_r_cmr_shield_reqType_CDC_AUTO_R_CMR_SHIELD_REQ_OPEN_E1;
            }else{
            }
            break;
        case 307:
            if(ui->treeWidget_3->currentSelectedId() == 1005)
            {
                sle_control1_683->cdc_front_cmr_sreen_shield_req = Cdc_control1_680_Cdc_front_cmr_sreen_shield_reqType_CDC_FRONT_CMR_SREEN_SHIELD_REQ_OPEN_E1;
            }else{
            }
            break;
        default: break;
        }
    }else {
        switch (ui->treeWidget->currentSelectedId()) {
        case 101:
            if(ui->treeWidget_3->currentSelectedId() == 1001)
            {
                cdc_15_443_i.ri_blid_wiper_ctl = Cdc_15_443_Ri_blid_wiper_ctlType_RI_BLID_WIPER_CTL_STANDBY_POSITION_E2;
                cdc_15_443_i.ri_blid_wiper_number_ctl = 0;
                cdc_15_443_i.ri_blid_wiper_frequency_ctl = 0;
            }else if(ui->treeWidget_3->currentSelectedId() == 1002){
                sle_control1_680->cdc_rilidarvalvepwr = Cdc_control2_683_Cdc_rilidarvalvepwrType_CDC_RILIDARVALVEPWR_OFF_E1;
            }else if(ui->treeWidget_3->currentSelectedId() == 1003){
                cdc_15_443_i.ri_blid_wiper_ctl = Cdc_15_443_Ri_blid_wiper_ctlType_RI_BLID_WIPER_CTL_STANDBY_POSITION_E2;
                cdc_15_443_i.ri_blid_wiper_number_ctl = 0;
                cdc_15_443_i.ri_blid_wiper_frequency_ctl = 0;
                sle_control1_680->cdc_rilidarvalvepwr = Cdc_control2_683_Cdc_rilidarvalvepwrType_CDC_RILIDARVALVEPWR_OFF_E1;
            }else{
            }
            break;
        case 102:
            if(ui->treeWidget_3->currentSelectedId() == 1001)
            {
            }else if(ui->treeWidget_3->currentSelectedId() == 1002){
                sle_control1_680->cdc_fisheye_r_cmr_valve_pwr_req = Cdc_control2_683_Cdc_fisheye_r_cmr_valve_pwr_reqType_CDC_FISHEYE_R_CMR_VALVE_PWR_REQ_OFF_E1;
            }else if(ui->treeWidget_3->currentSelectedId() == 1003){
                sle_control1_680->cdc_fisheye_r_cmr_valve_pwr_req = Cdc_control2_683_Cdc_fisheye_r_cmr_valve_pwr_reqType_CDC_FISHEYE_R_CMR_VALVE_PWR_REQ_OFF_E1;
            }else if(ui->treeWidget_3->currentSelectedId() == 1004){
                sle_control1_683->cdc_fisheye_r_cmr_heat_req = Cdc_control1_680_Cdc_fisheye_r_cmr_heat_reqType_CDC_FISHEYE_R_CMR_HEAT_REQ_CLOSE_E1;
            }else if(ui->treeWidget_3->currentSelectedId() == 1005){
                sle_control1_683->cdc_fisheye_r_cmr_shield_req = Cdc_control1_680_Cdc_fisheye_r_cmr_shield_reqType_CDC_FISHEYE_R_CMR_SHIELD_REQ_CLOSE_E2;
            }else{
            }
            break;
        case 103:
            if(ui->treeWidget_3->currentSelectedId() == 1001)
            {
                cdc_15_443_i.fr_blid_wiper_ctl = Cdc_15_443_Fr_blid_wiper_ctlType_FR_BLID_WIPER_CTL_STANDBY_POSITION_E2;
                cdc_15_443_i.fr_blid_wiper_number_ctl = 0;
                cdc_15_443_i.fr_blid_wiper_frequency_ctl = 0;
            }else if(ui->treeWidget_3->currentSelectedId() == 1002){
                sle_control1_680->cdc_frlidarvalvepwr = Cdc_control2_683_Cdc_frlidarvalvepwrType_CDC_FRLIDARVALVEPWR_OFF_E1;
            }else if(ui->treeWidget_3->currentSelectedId() == 1003){
                cdc_15_443_i.fr_blid_wiper_ctl = Cdc_15_443_Fr_blid_wiper_ctlType_FR_BLID_WIPER_CTL_STANDBY_POSITION_E2;
                cdc_15_443_i.fr_blid_wiper_number_ctl = 0;
                cdc_15_443_i.fr_blid_wiper_frequency_ctl = 0;
                sle_control1_680->cdc_frlidarvalvepwr = Cdc_control2_683_Cdc_frlidarvalvepwrType_CDC_FRLIDARVALVEPWR_OFF_E1;
            }else{
            }
            break;
        case 104:
            if(ui->treeWidget_3->currentSelectedId() == 1001)
            {
            }else if(ui->treeWidget_3->currentSelectedId() == 1002){
                sle_control1_680->cdc_fisheye_f_cmr_valve_pwr_req = Cdc_control2_683_Cdc_fisheye_f_cmr_valve_pwr_reqType_CDC_FISHEYE_F_CMR_VALVE_PWR_REQ_OFF_E1;
            }else if(ui->treeWidget_3->currentSelectedId() == 1003){
                sle_control1_680->cdc_fisheye_f_cmr_valve_pwr_req = Cdc_control2_683_Cdc_fisheye_f_cmr_valve_pwr_reqType_CDC_FISHEYE_F_CMR_VALVE_PWR_REQ_OFF_E1;
            }else if(ui->treeWidget_3->currentSelectedId() == 1004){
                sle_control1_683->cdc_fisheye_f_cmr_heat_req = Cdc_control1_680_Cdc_fisheye_f_cmr_heat_reqType_CDC_FISHEYE_F_CMR_HEAT_REQ_CLOSE_E1;
            }else if(ui->treeWidget_3->currentSelectedId() == 1005){
                sle_control1_683->cdc_fisheye_f_cmr_shield_req = Cdc_control1_680_Cdc_fisheye_f_cmr_shield_reqType_CDC_FISHEYE_F_CMR_SHIELD_REQ_CLOSE_E2;
            }else{
            }
            break;
        case 105:
            if(ui->treeWidget_3->currentSelectedId() == 1001)
            {
                cdc_15_443_i.le_blid_wiper_ctl = Cdc_15_443_Le_blid_wiper_ctlType_LE_BLID_WIPER_CTL_STANDBY_POSITION_E2;
                cdc_15_443_i.le_blid_wiper_number_ctl = 0;
                cdc_15_443_i.le_blid_wiper_frequency_ctl = 0;
            }else if(ui->treeWidget_3->currentSelectedId() == 1002){
                sle_control1_680->cdc_lelidarvalvepwr = Cdc_control2_683_Cdc_lelidarvalvepwrType_CDC_LELIDARVALVEPWR_OFF_E1;
            }else if(ui->treeWidget_3->currentSelectedId() == 1003){
                cdc_15_443_i.le_blid_wiper_ctl = Cdc_15_443_Le_blid_wiper_ctlType_LE_BLID_WIPER_CTL_STANDBY_POSITION_E2;
                cdc_15_443_i.le_blid_wiper_number_ctl = 0;
                cdc_15_443_i.le_blid_wiper_frequency_ctl = 0;
                sle_control1_680->cdc_lelidarvalvepwr = Cdc_control2_683_Cdc_lelidarvalvepwrType_CDC_LELIDARVALVEPWR_OFF_E1;
            }else{
            }
            break;
        case 106:
            if(ui->treeWidget_3->currentSelectedId() == 1001)
            {
            }else if(ui->treeWidget_3->currentSelectedId() == 1002){
                sle_control1_680->cdc_fisheye_l_cmr_valve_pwr_req = Cdc_control2_683_Cdc_fisheye_l_cmr_valve_pwr_reqType_CDC_FISHEYE_L_CMR_VALVE_PWR_REQ_OFF_E1;
            }else if(ui->treeWidget_3->currentSelectedId() == 1003){
                sle_control1_680->cdc_fisheye_l_cmr_valve_pwr_req = Cdc_control2_683_Cdc_fisheye_l_cmr_valve_pwr_reqType_CDC_FISHEYE_L_CMR_VALVE_PWR_REQ_OFF_E1;
            }else if(ui->treeWidget_3->currentSelectedId() == 1004){
                sle_control1_683->cdc_fisheye_l_cmr_heat_req = Cdc_control1_680_Cdc_fisheye_l_cmr_heat_reqType_CDC_FISHEYE_L_CMR_HEAT_REQ_CLOSE_E1;
            }else if(ui->treeWidget_3->currentSelectedId() == 1005){
                sle_control1_683->cdc_fisheye_l_cmr_shield_req = Cdc_control1_680_Cdc_fisheye_l_cmr_shield_reqType_CDC_FISHEYE_L_CMR_SHIELD_REQ_CLOSE_E2;
            }else{
            }
            break;
        case 107:
            if(ui->treeWidget_3->currentSelectedId() == 1001)
            {
                cdc_15_443_i.lf_pcam_wiper_ctl = Cdc_15_443_Lf_pcam_wiper_ctlType_LF_PCAM_WIPER_CTL_STANDBY_POSITION_E2;
                cdc_15_443_i.lf_pcam_wiper_number_ctl = 0;
                cdc_15_443_i.lf_pcam_wiper_frequency_ctl = 0;
            }else if(ui->treeWidget_3->currentSelectedId() == 1002){
                sle_control1_680->cdc_auto_lp_cmr_valve_pwr_req = Cdc_control2_683_Cdc_auto_lp_cmr_valve_pwr_reqType_CDC_AUTO_LP_CMR_VALVE_PWR_REQ_OFF_E1;
            }else if(ui->treeWidget_3->currentSelectedId() == 1003){
                cdc_15_443_i.lf_pcam_wiper_ctl = Cdc_15_443_Lf_pcam_wiper_ctlType_LF_PCAM_WIPER_CTL_STANDBY_POSITION_E2;
                cdc_15_443_i.lf_pcam_wiper_number_ctl = 0;
                cdc_15_443_i.lf_pcam_wiper_frequency_ctl = 0;
                sle_control1_680->cdc_auto_lp_cmr_valve_pwr_req = Cdc_control2_683_Cdc_auto_lp_cmr_valve_pwr_reqType_CDC_AUTO_LP_CMR_VALVE_PWR_REQ_OFF_E1;
            }else if(ui->treeWidget_3->currentSelectedId() == 1004){
                sle_control1_683->cdc_auto_lp_cmr_heat_req = Cdc_control1_680_Cdc_auto_lp_cmr_heat_reqType_CDC_AUTO_LP_CMR_HEAT_REQ_CLOSE_E1;
            }else{
            }
            break;
        case 108:
            if(ui->treeWidget_3->currentSelectedId() == 1002){
                sle_control1_680->cdc_frchassisvvctrl = Cdc_control2_683_Cdc_frchassisvvctrlType_CDC_FRCHASSISVVCTRL_OFF_E1;
            }else if(ui->treeWidget_3->currentSelectedId() == 1003){
                sle_control1_680->cdc_frchassisvvctrl = Cdc_control2_683_Cdc_frchassisvvctrlType_CDC_FRCHASSISVVCTRL_OFF_E1;
            }else{
            }
            break;
        case 201:
            if(ui->treeWidget_3->currentSelectedId() == 1001)
            {
                cdc_15_443_i.lrb_pcam_wiper_ctl = Cdc_15_443_Lrb_pcam_wiper_ctlType_LRB_PCAM_WIPER_CTL_STANDBY_POSITION_E2;
                cdc_15_443_i.lrb_pcam_wiper_number_ctl = 0;
                cdc_15_443_i.lrb_pcam_wiper_frequency_ctl = 0;
            }else if(ui->treeWidget_3->currentSelectedId() == 1002){
                sle_control1_680->cdc_auto_rl_cmr_valve_pwr_req = Cdc_control2_683_Cdc_auto_rl_cmr_valve_pwr_reqType_CDC_AUTO_RL_CMR_VALVE_PWR_REQ_OFF_E1;
            }else if(ui->treeWidget_3->currentSelectedId() == 1003){
                cdc_15_443_i.lrb_pcam_wiper_ctl = Cdc_15_443_Lrb_pcam_wiper_ctlType_LRB_PCAM_WIPER_CTL_STANDBY_POSITION_E2;
                cdc_15_443_i.lrb_pcam_wiper_number_ctl = 0;
                cdc_15_443_i.lrb_pcam_wiper_frequency_ctl = 0;
                sle_control1_680->cdc_auto_rl_cmr_valve_pwr_req = Cdc_control2_683_Cdc_auto_rl_cmr_valve_pwr_reqType_CDC_AUTO_RL_CMR_VALVE_PWR_REQ_OFF_E1;
            }else if(ui->treeWidget_3->currentSelectedId() == 1004){
                sle_control1_683->cdc_auto_lb_cmr_heat_req = Cdc_control1_680_Cdc_auto_lb_cmr_heat_reqType_CDC_AUTO_LB_CMR_HEAT_REQ_CLOSE_E1;
            }else{
            }
            break;
        case 202:
            if(ui->treeWidget_3->currentSelectedId() == 1001)
            {
                cdc_15_443_i.rrb_pcam_wiper_ctl = Cdc_15_443_Rrb_pcam_wiper_ctlType_RRB_PCAM_WIPER_CTL_STANDBY_POSITION_E2;
                cdc_15_443_i.rrb_pcam_wiper_number_ctl = 0;
                cdc_15_443_i.rrb_pcam_wiper_frequency_ctl = 0;
            }else if(ui->treeWidget_3->currentSelectedId() == 1002){
                sle_control1_680->cdc_auto_rr_cmr_valve_pwr_req = Cdc_control2_683_Cdc_auto_rr_cmr_valve_pwr_reqType_CDC_AUTO_RR_CMR_VALVE_PWR_REQ_OFF_E1;
            }else if(ui->treeWidget_3->currentSelectedId() == 1003){
                cdc_15_443_i.rrb_pcam_wiper_ctl = Cdc_15_443_Rrb_pcam_wiper_ctlType_RRB_PCAM_WIPER_CTL_STANDBY_POSITION_E2;
                cdc_15_443_i.rrb_pcam_wiper_number_ctl = 0;
                cdc_15_443_i.rrb_pcam_wiper_frequency_ctl = 0;
                sle_control1_680->cdc_auto_rr_cmr_valve_pwr_req = Cdc_control2_683_Cdc_auto_rr_cmr_valve_pwr_reqType_CDC_AUTO_RR_CMR_VALVE_PWR_REQ_OFF_E1;
            }else if(ui->treeWidget_3->currentSelectedId() == 1004){
                sle_control1_683->cdc_auto_rb_cmr_heat_req = Cdc_control1_680_Cdc_auto_rb_cmr_heat_reqType_CDC_AUTO_RB_CMR_HEAT_REQ_CLOSE_E1;
            }else{
            }
            break;
        case 203:
            if(ui->treeWidget_3->currentSelectedId() == 1001)
            {
            }else if(ui->treeWidget_3->currentSelectedId() == 1002){
                sle_control1_680->cdc_fisheye_b_cmr_valve_pwr_req = Cdc_control2_683_Cdc_fisheye_b_cmr_valve_pwr_reqType_CDC_FISHEYE_B_CMR_VALVE_PWR_REQ_OFF_E1;
            }else if(ui->treeWidget_3->currentSelectedId() == 1003){
                sle_control1_680->cdc_fisheye_b_cmr_valve_pwr_req = Cdc_control2_683_Cdc_fisheye_b_cmr_valve_pwr_reqType_CDC_FISHEYE_B_CMR_VALVE_PWR_REQ_OFF_E1;
            }else if(ui->treeWidget_3->currentSelectedId() == 1004){
                sle_control1_683->cdc_fisheye_b_cmr_heat_req = Cdc_control1_680_Cdc_fisheye_b_cmr_heat_reqType_CDC_FISHEYE_B_CMR_HEAT_REQ_CLOSE_E1;
            }else if(ui->treeWidget_3->currentSelectedId() == 1005){
                sle_control1_683->cdc_fisheye_b_cmr_shield_req = Cdc_control1_680_Cdc_fisheye_b_cmr_shield_reqType_CDC_FISHEYE_B_CMR_SHIELD_REQ_CLOSE_E2;
            }else{
            }
            break;
        case 204:
            if(ui->treeWidget_3->currentSelectedId() == 1001)
            {
                cdc_15_443_i.re_blid_rac_wiper_ctl = Cdc_15_443_Re_blid_rac_wiper_ctlType_RE_BLID_RAC_WIPER_CTL_STANDBY_POSITION_E2;
                cdc_15_443_i.re_blid_rac_wiper_number_ctl = 0;
                cdc_15_443_i.re_blid_rac_wiper_frequency_ctl = 0;
            }else if(ui->treeWidget_3->currentSelectedId() == 1002){
                sle_control1_680->cdc_relidarvalvepwr = Cdc_control2_683_Cdc_relidarvalvepwrType_CDC_RELIDARVALVEPWR_OFF_E1;
            }else if(ui->treeWidget_3->currentSelectedId() == 1003){
                cdc_15_443_i.re_blid_rac_wiper_ctl = Cdc_15_443_Re_blid_rac_wiper_ctlType_RE_BLID_RAC_WIPER_CTL_STANDBY_POSITION_E2;
                cdc_15_443_i.re_blid_rac_wiper_number_ctl = 0;
                cdc_15_443_i.re_blid_rac_wiper_frequency_ctl = 0;
                sle_control1_680->cdc_relidarvalvepwr = Cdc_control2_683_Cdc_relidarvalvepwrType_CDC_RELIDARVALVEPWR_OFF_E1;
            }else{
            }
            break;
        case 205:
            if(ui->treeWidget_3->currentSelectedId() == 1002){
                sle_control1_680->cdc_midchassisvvctrl = Cdc_control2_683_Cdc_midchassisvvctrlType_CDC_MIDCHASSISVVCTRL_OFF_E1;
            }else if(ui->treeWidget_3->currentSelectedId() == 1003){
                sle_control1_680->cdc_midchassisvvctrl = Cdc_control2_683_Cdc_midchassisvvctrlType_CDC_MIDCHASSISVVCTRL_OFF_E1;
            }else{
            }
            break;
        case 206:
            if(ui->treeWidget_3->currentSelectedId() == 1002){
                sle_control1_680->cdc_rechassisvvctrl = Cdc_control2_683_Cdc_rechassisvvctrlType_CDC_RECHASSISVVCTRL_OFF_E1;
            }else if(ui->treeWidget_3->currentSelectedId() == 1003){
                sle_control1_680->cdc_rechassisvvctrl = Cdc_control2_683_Cdc_rechassisvvctrlType_CDC_RECHASSISVVCTRL_OFF_E1;
            }else{
            }
            break;
        case 301:
            if(ui->treeWidget_3->currentSelectedId() == 1001)
            {
                cdc_15_443_i.re_cam_wiper_ctl = Cdc_15_443_Re_cam_wiper_ctlType_RE_CAM_WIPER_CTL_STANDBY_POSITION_E2;
                cdc_15_443_i.re_cam_wiper_number_ctl = 0;
                cdc_15_443_i.re_cam_wiper_frequency_ctl = 0;
            }else if(ui->treeWidget_3->currentSelectedId() == 1002){
                sle_control1_680->cdc_auto_b_cmr_valve_pwr_req = Cdc_control2_683_Cdc_auto_b_cmr_valve_pwr_reqType_CDC_AUTO_B_CMR_VALVE_PWR_REQ_OFF_E1;
            }else if(ui->treeWidget_3->currentSelectedId() == 1003){
                cdc_15_443_i.re_cam_wiper_ctl = Cdc_15_443_Re_cam_wiper_ctlType_RE_CAM_WIPER_CTL_STANDBY_POSITION_E2;
                cdc_15_443_i.re_cam_wiper_number_ctl = 0;
                cdc_15_443_i.re_cam_wiper_frequency_ctl = 0;
                sle_control1_680->cdc_auto_b_cmr_valve_pwr_req = Cdc_control2_683_Cdc_auto_b_cmr_valve_pwr_reqType_CDC_AUTO_B_CMR_VALVE_PWR_REQ_OFF_E1;
            }else if(ui->treeWidget_3->currentSelectedId() == 1004){
                sle_control1_683->cdc_auto_b_cmr_heat_req = Cdc_control1_680_Cdc_auto_b_cmr_heat_reqType_CDC_AUTO_B_CMR_HEAT_REQ_CLOSE_E1;
            }else if(ui->treeWidget_3->currentSelectedId() == 1005){
                sle_control1_683->cdc_auto_b_cmr_shield_req = Cdc_control1_680_Cdc_auto_b_cmr_shield_reqType_CDC_AUTO_B_CMR_SHIELD_REQ_OPEN_E1;
            }else{
            }
            break;
        case 302:
            if(ui->treeWidget_3->currentSelectedId() == 1001)
            {
            }else if(ui->treeWidget_3->currentSelectedId() == 1002){
                sle_control1_680->cdc_mlidarvalvepwr = Cdc_control2_683_Cdc_mlidarvalvepwrType_CDC_MLIDARVALVEPWR_OFF_E1;
            }else if(ui->treeWidget_3->currentSelectedId() == 1003){
                sle_control1_680->cdc_mlidarvalvepwr = Cdc_control2_683_Cdc_mlidarvalvepwrType_CDC_MLIDARVALVEPWR_OFF_E1;
            }else if(ui->treeWidget_3->currentSelectedId() == 1005){
                sle_control1_683->cdc_mlidarshieldmove = Cdc_control1_680_Cdc_mlidarshieldmoveType_CDC_MLIDARSHIELDMOVE_CLOSE_E2;
            }else{
            }
            break;
        case 303:
            if(ui->treeWidget_3->currentSelectedId() == 1001)
            {
                cdc_15_443_i.fl_cam_wiper_ctl = Cdc_15_443_Fl_cam_wiper_ctlType_FL_CAM_WIPER_CTL_STANDBY_POSITION_E2;
                cdc_15_443_i.fl_cam_wiper_number_ctl = 0;
                cdc_15_443_i.fl_cam_wiper_frequency_ctl = 0;
            }else if(ui->treeWidget_3->currentSelectedId() == 1002){
                sle_control1_680->cdc_auto_lf_cmr_valve_pwr_req = Cdc_control2_683_Cdc_auto_lf_cmr_valve_pwr_reqType_CDC_AUTO_LF_CMR_VALVE_PWR_REQ_OFF_E1;
            }else if(ui->treeWidget_3->currentSelectedId() == 1003){
                cdc_15_443_i.fl_cam_wiper_ctl = Cdc_15_443_Fl_cam_wiper_ctlType_FL_CAM_WIPER_CTL_STANDBY_POSITION_E2;
                cdc_15_443_i.fl_cam_wiper_number_ctl = 0;
                cdc_15_443_i.fl_cam_wiper_frequency_ctl = 0;
                sle_control1_680->cdc_auto_lf_cmr_valve_pwr_req = Cdc_control2_683_Cdc_auto_lf_cmr_valve_pwr_reqType_CDC_AUTO_LF_CMR_VALVE_PWR_REQ_OFF_E1;
            }else if(ui->treeWidget_3->currentSelectedId() == 1004){
                sle_control1_683->cdc_auto_lf_cmr_heat_req = Cdc_control1_680_Cdc_auto_lf_cmr_heat_reqType_CDC_AUTO_LF_CMR_HEAT_REQ_CLOSE_E1;
            }else{
            }
            break;
        case 304:
            if(ui->treeWidget_3->currentSelectedId() == 1001)
            {
                cdc_15_443_i.fr_cam_wiper_ctl = Cdc_15_443_Fr_cam_wiper_ctlType_FR_CAM_WIPER_CTL_STANDBY_POSITION_E2;
                cdc_15_443_i.fr_cam_wiper_number_ctl = 0;
                cdc_15_443_i.fr_cam_wiper_frequency_ctl = 0;
            }else if(ui->treeWidget_3->currentSelectedId() == 1002){
                sle_control1_680->cdc_auto_rf_cmr_valve_pwr_req = Cdc_control2_683_Cdc_auto_rf_cmr_valve_pwr_reqType_CDC_AUTO_RF_CMR_VALVE_PWR_REQ_OFF_E1;
            }else if(ui->treeWidget_3->currentSelectedId() == 1003){
                cdc_15_443_i.fr_cam_wiper_ctl = Cdc_15_443_Fr_cam_wiper_ctlType_FR_CAM_WIPER_CTL_STANDBY_POSITION_E2;
                cdc_15_443_i.fr_cam_wiper_number_ctl = 0;
                cdc_15_443_i.fr_cam_wiper_frequency_ctl = 0;
                sle_control1_680->cdc_auto_rf_cmr_valve_pwr_req = Cdc_control2_683_Cdc_auto_rf_cmr_valve_pwr_reqType_CDC_AUTO_RF_CMR_VALVE_PWR_REQ_OFF_E1;
            }else if(ui->treeWidget_3->currentSelectedId() == 1004){
                sle_control1_683->cdc_auto_rf_cmr_heat_req = Cdc_control1_680_Cdc_auto_rf_cmr_heat_reqType_CDC_AUTO_RF_CMR_HEAT_REQ_CLOSE_E1;
            }else{
            }
            break;
        case 305:
            if(ui->treeWidget_3->currentSelectedId() == 1001)
            {
                cdc_15_443_i.le_cam_wiper_ctl = Cdc_15_443_Le_cam_wiper_ctlType_LE_CAM_WIPER_CTL_STANDBY_POSITION_E2;
                cdc_15_443_i.le_cam_wiper_number_ctl = 0;
                cdc_15_443_i.le_cam_wiper_frequency_ctl = 0;
            }else if(ui->treeWidget_3->currentSelectedId() == 1002){
                sle_control1_680->cdc_auto_l_cmr_valve_pwr_req = Cdc_control2_683_Cdc_auto_l_cmr_valve_pwr_reqType_CDC_AUTO_L_CMR_VALVE_PWR_REQ_OFF_E1;
            }else if(ui->treeWidget_3->currentSelectedId() == 1003){
                cdc_15_443_i.le_cam_wiper_ctl = Cdc_15_443_Le_cam_wiper_ctlType_LE_CAM_WIPER_CTL_STANDBY_POSITION_E2;
                cdc_15_443_i.le_cam_wiper_number_ctl = 0;
                cdc_15_443_i.le_cam_wiper_frequency_ctl = 0;
                sle_control1_680->cdc_auto_l_cmr_valve_pwr_req = Cdc_control2_683_Cdc_auto_l_cmr_valve_pwr_reqType_CDC_AUTO_L_CMR_VALVE_PWR_REQ_OFF_E1;
            }else if(ui->treeWidget_3->currentSelectedId() == 1004){
                sle_control1_683->cdc_auto_l_cmr_heat_req = Cdc_control1_680_Cdc_auto_l_cmr_heat_reqType_CDC_AUTO_L_CMR_HEAT_REQ_CLOSE_E1;
            }else if(ui->treeWidget_3->currentSelectedId() == 1005){
                sle_control1_683->cdc_auto_l_cmr_shield_req = Cdc_control1_680_Cdc_auto_l_cmr_shield_reqType_CDC_AUTO_L_CMR_SHIELD_REQ_CLOSE_E2;
            }else{
            }
            break;
        case 306:
            if(ui->treeWidget_3->currentSelectedId() == 1001)
            {
                cdc_15_443_i.ri_cam_wiper_ctl = Cdc_15_443_Ri_cam_wiper_ctlType_RI_CAM_WIPER_CTL_STANDBY_POSITION_E2;
                cdc_15_443_i.ri_cam_wiper_number_ctl = 0;
                cdc_15_443_i.ri_cam_wiper_frequency_ctl = 0;
            }else if(ui->treeWidget_3->currentSelectedId() == 1002){
                sle_control1_680->cdc_auto_r_cmr_valve_pwr_req = Cdc_control2_683_Cdc_auto_r_cmr_valve_pwr_reqType_CDC_AUTO_R_CMR_VALVE_PWR_REQ_OFF_E1;
            }else if(ui->treeWidget_3->currentSelectedId() == 1003){
                cdc_15_443_i.ri_cam_wiper_ctl = Cdc_15_443_Ri_cam_wiper_ctlType_RI_CAM_WIPER_CTL_STANDBY_POSITION_E2;
                cdc_15_443_i.ri_cam_wiper_number_ctl = 0;
                cdc_15_443_i.ri_cam_wiper_frequency_ctl = 0;
                sle_control1_680->cdc_auto_r_cmr_valve_pwr_req = Cdc_control2_683_Cdc_auto_r_cmr_valve_pwr_reqType_CDC_AUTO_R_CMR_VALVE_PWR_REQ_OFF_E1;
            }else if(ui->treeWidget_3->currentSelectedId() == 1004){
                sle_control1_683->cdc_auto_r_cmr_heat_req = Cdc_control1_680_Cdc_auto_r_cmr_heat_reqType_CDC_AUTO_R_CMR_HEAT_REQ_CLOSE_E1;
            }else if(ui->treeWidget_3->currentSelectedId() == 1005){
                sle_control1_683->cdc_auto_r_cmr_shield_req = Cdc_control1_680_Cdc_auto_r_cmr_shield_reqType_CDC_AUTO_R_CMR_SHIELD_REQ_CLOSE_E2;
            }else{
            }
            break;
        case 307:
            if(ui->treeWidget_3->currentSelectedId() == 1005)
            {
                sle_control1_683->cdc_front_cmr_sreen_shield_req = Cdc_control1_680_Cdc_front_cmr_sreen_shield_reqType_CDC_FRONT_CMR_SREEN_SHIELD_REQ_CLOSE_E2;
            }else{
            }
            break;
        default: break;
        }
    }
}

void MainWindow::on_MEstopButton_5_clicked()
{
    QByteArray byteArray;
    Cdccontrol1680_Reset(&cdc_control1_680);
    cdc_control1_680.cdc_cleanpumpecupwr = Cdc_control1_680_Cdc_cleanpumpecupwrType_CDC_CLEANPUMPECUPWR_OPEN_E2;
    cdc_control1_680.cdc_cleanpumpspd = ui->pressureEdit->text().toDouble();
    Cdccontrol1680_UpdateData(&cdc_control1_680);
    byteArray = QByteArray(Data, 48);
    canComm->sendMessage(0x680, byteArray);
}

void MainWindow::on_MEstopButton_6_clicked()
{
    Cdccontrol1680_Reset(&cdc_control1_680);
    cdc_control1_680.cdc_cleanpumpecupwr = Cdc_control1_680_Cdc_cleanpumpecupwrType_CDC_CLEANPUMPECUPWR_CLOSE_E1;
    Cdccontrol1680_UpdateData(&cdc_control1_680);
    QByteArray byteArray(Data, 48);
    canComm->sendMessage(0x680, byteArray);
}

void MainWindow::on_makeButton_3_clicked()
{
    QByteArray byteArray;
    cdc_15_443_i.lm_control_move = Cdc_15_443_Lm_control_moveType_LM_CONTROL_MOVE__E1;
    cdc_15_443_i.lm_move_target_position = ui->pressureEdit_2->text().toInt();
    Cdc15443_UpdateData(&cdc_15_443_i);
    byteArray = QByteArray(Data443, 64);
    canComm->sendMessage(0x443, byteArray);
}

void MainWindow::on_makeButton_4_clicked()
{
    QByteArray byteArray;
    cdc_15_443_i.rac_control_move = Cdc_15_443_Rac_control_moveType_RAC_CONTROL_MOVE__E1;
    cdc_15_443_i.rac_4_movetarget_position = ui->pressureEdit_3->text().toInt();
    Cdc15443_UpdateData(&cdc_15_443_i);
    byteArray = QByteArray(Data443, 64);
    canComm->sendMessage(0x443, byteArray);
}

void MainWindow::on_makeButton_5_clicked()
{
    QByteArray byteArray;
    cdc_15_443_i.rac_5_control_move = Cdc_15_443_Rac_5_control_moveType_RAC_5_CONTROL_MOVE__E1;
    cdc_15_443_i.rac_5_movetarget_position = ui->pressureEdit_4->text().toInt();
    Cdc15443_UpdateData(&cdc_15_443_i);
    byteArray = QByteArray(Data443, 64);
    canComm->sendMessage(0x443, byteArray);
}

void MainWindow::on_makeButton_11_clicked()
{
    QByteArray byteArray;
    Cdccontrol1680_Reset(&cdc_control1_680);
    cdc_control1_680.cdc_fisheye_l_cmr_heat_req = Cdc_control1_680_Cdc_fisheye_l_cmr_heat_reqType_CDC_FISHEYE_L_CMR_HEAT_REQ_OPEN_E2;
    cdc_control1_680.cdc_fisheye_f_cmr_heat_req = Cdc_control1_680_Cdc_fisheye_f_cmr_heat_reqType_CDC_FISHEYE_F_CMR_HEAT_REQ_OPEN_E2;
    cdc_control1_680.cdc_fisheye_r_cmr_heat_req = Cdc_control1_680_Cdc_fisheye_r_cmr_heat_reqType_CDC_FISHEYE_R_CMR_HEAT_REQ_OPEN_E2;
    cdc_control1_680.cdc_auto_lp_cmr_heat_req = Cdc_control1_680_Cdc_auto_lp_cmr_heat_reqType_CDC_AUTO_LP_CMR_HEAT_REQ_OPEN_E2;
    cdc_control1_680.cdc_auto_rb_cmr_heat_req = Cdc_control1_680_Cdc_auto_rb_cmr_heat_reqType_CDC_AUTO_RB_CMR_HEAT_REQ_OPEN_E2;
    cdc_control1_680.cdc_auto_lb_cmr_heat_req = Cdc_control1_680_Cdc_auto_lb_cmr_heat_reqType_CDC_AUTO_LB_CMR_HEAT_REQ_OPEN_E2;
    cdc_control1_680.cdc_fisheye_b_cmr_heat_req = Cdc_control1_680_Cdc_fisheye_b_cmr_heat_reqType_CDC_FISHEYE_B_CMR_HEAT_REQ_OPEN_E2;
    cdc_control1_680.cdc_auto_b_cmr_heat_req = Cdc_control1_680_Cdc_auto_b_cmr_heat_reqType_CDC_AUTO_B_CMR_HEAT_REQ_OPEN_E2;
    cdc_control1_680.cdc_auto_r_cmr_heat_req = Cdc_control1_680_Cdc_auto_r_cmr_heat_reqType_CDC_AUTO_R_CMR_HEAT_REQ_OPEN_E2;
    cdc_control1_680.cdc_auto_l_cmr_heat_req = Cdc_control1_680_Cdc_auto_l_cmr_heat_reqType_CDC_AUTO_L_CMR_HEAT_REQ_OPEN_E2;
    cdc_control1_680.cdc_auto_rf_cmr_heat_req = Cdc_control1_680_Cdc_auto_rf_cmr_heat_reqType_CDC_AUTO_RF_CMR_HEAT_REQ_OPEN_E2;
    cdc_control1_680.cdc_auto_lf_cmr_heat_req = Cdc_control1_680_Cdc_auto_lf_cmr_heat_reqType_CDC_AUTO_LF_CMR_HEAT_REQ_OPEN_E2;
    Cdccontrol1680_UpdateData(&cdc_control1_680);
    byteArray = QByteArray(Data, 48);
    canComm->sendMessage(0x680, byteArray);
}

void MainWindow::on_makeButton_12_clicked()
{
    QByteArray byteArray;
    Cdccontrol1680_Reset(&cdc_control1_680);
    cdc_control1_680.cdc_mlidarshieldmove = Cdc_control1_680_Cdc_mlidarshieldmoveType_CDC_MLIDARSHIELDMOVE_OPEN_E1;
    cdc_control1_680.cdc_front_cmr_sreen_shield_req = Cdc_control1_680_Cdc_front_cmr_sreen_shield_reqType_CDC_FRONT_CMR_SREEN_SHIELD_REQ_OPEN_E1;
    cdc_control1_680.cdc_auto_l_cmr_shield_req = Cdc_control1_680_Cdc_auto_l_cmr_shield_reqType_CDC_AUTO_L_CMR_SHIELD_REQ_OPEN_E1;
    cdc_control1_680.cdc_auto_r_cmr_shield_req = Cdc_control1_680_Cdc_auto_r_cmr_shield_reqType_CDC_AUTO_R_CMR_SHIELD_REQ_OPEN_E1;
    cdc_control1_680.cdc_auto_b_cmr_shield_req = Cdc_control1_680_Cdc_auto_b_cmr_shield_reqType_CDC_AUTO_B_CMR_SHIELD_REQ_OPEN_E1;
    cdc_control1_680.cdc_fisheye_b_cmr_shield_req = Cdc_control1_680_Cdc_fisheye_b_cmr_shield_reqType_CDC_FISHEYE_B_CMR_SHIELD_REQ_OPEN_E1;
    cdc_control1_680.cdc_fisheye_r_cmr_shield_req = Cdc_control1_680_Cdc_fisheye_r_cmr_shield_reqType_CDC_FISHEYE_R_CMR_SHIELD_REQ_OPEN_E1;
    cdc_control1_680.cdc_fisheye_l_cmr_shield_req = Cdc_control1_680_Cdc_fisheye_l_cmr_shield_reqType_CDC_FISHEYE_L_CMR_SHIELD_REQ_OPEN_E1;
    cdc_control1_680.cdc_fisheye_f_cmr_shield_req = Cdc_control1_680_Cdc_fisheye_f_cmr_shield_reqType_CDC_FISHEYE_F_CMR_SHIELD_REQ_OPEN_E1;
    Cdccontrol1680_UpdateData(&cdc_control1_680);
    byteArray = QByteArray(Data, 48);
    canComm->sendMessage(0x680, byteArray);
}

void MainWindow::on_makeButton_27_clicked()
{
    QByteArray byteArray;
    Cdccontrol1680_Reset(&cdc_control1_680);
    cdc_control1_680.cdc_fisheye_l_cmr_heat_req = Cdc_control1_680_Cdc_fisheye_l_cmr_heat_reqType_CDC_FISHEYE_L_CMR_HEAT_REQ_CLOSE_E1;
    cdc_control1_680.cdc_fisheye_f_cmr_heat_req = Cdc_control1_680_Cdc_fisheye_f_cmr_heat_reqType_CDC_FISHEYE_F_CMR_HEAT_REQ_CLOSE_E1;
    cdc_control1_680.cdc_fisheye_r_cmr_heat_req = Cdc_control1_680_Cdc_fisheye_r_cmr_heat_reqType_CDC_FISHEYE_R_CMR_HEAT_REQ_CLOSE_E1;
    cdc_control1_680.cdc_auto_lp_cmr_heat_req = Cdc_control1_680_Cdc_auto_lp_cmr_heat_reqType_CDC_AUTO_LP_CMR_HEAT_REQ_CLOSE_E1;
    cdc_control1_680.cdc_auto_rb_cmr_heat_req = Cdc_control1_680_Cdc_auto_rb_cmr_heat_reqType_CDC_AUTO_RB_CMR_HEAT_REQ_CLOSE_E1;
    cdc_control1_680.cdc_auto_lb_cmr_heat_req = Cdc_control1_680_Cdc_auto_lb_cmr_heat_reqType_CDC_AUTO_LB_CMR_HEAT_REQ_CLOSE_E1;
    cdc_control1_680.cdc_fisheye_b_cmr_heat_req = Cdc_control1_680_Cdc_fisheye_b_cmr_heat_reqType_CDC_FISHEYE_B_CMR_HEAT_REQ_CLOSE_E1;
    cdc_control1_680.cdc_auto_b_cmr_heat_req = Cdc_control1_680_Cdc_auto_b_cmr_heat_reqType_CDC_AUTO_B_CMR_HEAT_REQ_CLOSE_E1;
    cdc_control1_680.cdc_auto_r_cmr_heat_req = Cdc_control1_680_Cdc_auto_r_cmr_heat_reqType_CDC_AUTO_R_CMR_HEAT_REQ_CLOSE_E1;
    cdc_control1_680.cdc_auto_l_cmr_heat_req = Cdc_control1_680_Cdc_auto_l_cmr_heat_reqType_CDC_AUTO_L_CMR_HEAT_REQ_CLOSE_E1;
    cdc_control1_680.cdc_auto_rf_cmr_heat_req = Cdc_control1_680_Cdc_auto_rf_cmr_heat_reqType_CDC_AUTO_RF_CMR_HEAT_REQ_CLOSE_E1;
    cdc_control1_680.cdc_auto_lf_cmr_heat_req = Cdc_control1_680_Cdc_auto_lf_cmr_heat_reqType_CDC_AUTO_LF_CMR_HEAT_REQ_CLOSE_E1;
    Cdccontrol1680_UpdateData(&cdc_control1_680);
    byteArray = QByteArray(Data, 48);
    canComm->sendMessage(0x680, byteArray);
}

void MainWindow::on_makeButton_28_clicked()
{
    QByteArray byteArray;
    Cdccontrol1680_Reset(&cdc_control1_680);
    cdc_control1_680.cdc_mlidarshieldmove = Cdc_control1_680_Cdc_mlidarshieldmoveType_CDC_MLIDARSHIELDMOVE_CLOSE_E2;
    cdc_control1_680.cdc_front_cmr_sreen_shield_req = Cdc_control1_680_Cdc_front_cmr_sreen_shield_reqType_CDC_FRONT_CMR_SREEN_SHIELD_REQ_CLOSE_E2;
    cdc_control1_680.cdc_auto_l_cmr_shield_req = Cdc_control1_680_Cdc_auto_l_cmr_shield_reqType_CDC_AUTO_L_CMR_SHIELD_REQ_CLOSE_E2;
    cdc_control1_680.cdc_auto_r_cmr_shield_req = Cdc_control1_680_Cdc_auto_r_cmr_shield_reqType_CDC_AUTO_R_CMR_SHIELD_REQ_CLOSE_E2;
    cdc_control1_680.cdc_auto_b_cmr_shield_req = Cdc_control1_680_Cdc_auto_b_cmr_shield_reqType_CDC_AUTO_B_CMR_SHIELD_REQ_CLOSE_E2;
    cdc_control1_680.cdc_fisheye_b_cmr_shield_req = Cdc_control1_680_Cdc_fisheye_b_cmr_shield_reqType_CDC_FISHEYE_B_CMR_SHIELD_REQ_CLOSE_E2;
    cdc_control1_680.cdc_fisheye_r_cmr_shield_req = Cdc_control1_680_Cdc_fisheye_r_cmr_shield_reqType_CDC_FISHEYE_R_CMR_SHIELD_REQ_CLOSE_E2;
    cdc_control1_680.cdc_fisheye_l_cmr_shield_req = Cdc_control1_680_Cdc_fisheye_l_cmr_shield_reqType_CDC_FISHEYE_L_CMR_SHIELD_REQ_CLOSE_E2;
    cdc_control1_680.cdc_fisheye_f_cmr_shield_req = Cdc_control1_680_Cdc_fisheye_f_cmr_shield_reqType_CDC_FISHEYE_F_CMR_SHIELD_REQ_CLOSE_E2;
    Cdccontrol1680_UpdateData(&cdc_control1_680);
    byteArray = QByteArray(Data, 48);
    canComm->sendMessage(0x680, byteArray);
}

void MainWindow::dataSend(Cdc_control1_680 cdc_control1_680_x)
{
    QByteArray byteArray;
    cdc_control1_680_x.cdc_cleanpumpspd = ui->pressureEdit_17->text().toDouble();
    QMutexLocker locker(&dataMutex);
    Cdccontrol1680_UpdateData(&cdc_control1_680_x);
    byteArray = QByteArray(Data, 48);
    locker.unlock();
    canComm->enqueueMessage(0x680, byteArray);
    qDebug()<< "680" << byteArray.toHex();
}

void MainWindow::dataSend683(Cdc_control2_683 cdc_control1_683_x)
{
    QByteArray byteArray;
    QMutexLocker locker(&dataMutex);
    Cdccontrol1683_UpdateData(&cdc_control1_683_x);
    byteArray = QByteArray(Data683, 64);
    locker.unlock();
    canComm->enqueueMessage(0x683, byteArray);
    qDebug()<< "683" << byteArray.toHex();
}

void MainWindow::dataSend443(Cdc_15_443 cdc_15_443)
{
    QByteArray byteArray;
    QMutexLocker locker(&dataMutex0);
    Cdc15443_UpdateData(&cdc_15_443);
    byteArray = QByteArray(Data443, 64);
    locker.unlock();
    canComm->enqueueMessage(0x443, byteArray);
    qDebug()<< byteArray.toHex();
}

void Delay_MSec(unsigned int msec)

{
    QEventLoop loop;//定义一个新的事件循环

    QTimer::singleShot(msec, &loop, SLOT(quit()));//创建单次定时器，槽函数为事件循环的退出函数

    loop.exec();//事件循环开始执行，程序会卡在这里，直到定时时间到，本循环被退出
}

void MainWindow::on_timerMake0(int value)
{
    switch (value) {
    case 0:{
        QVector<TableRowData> allRows = ui->tableWidget->getTableData();
        QString targetValve = "右补盲激光雷达水阀";
        auto it = std::find_if(allRows.begin(), allRows.end(), [&](const TableRowData &d){
            return d.valvePosition == targetValve;
        });
        if (it != allRows.end()) {
            if(it->cleanCount > 0){
                QMutexLocker locker(&dataMutex1);
                cdc_15_443_i.ri_blid_wiper_ctl = Cdc_15_443_Ri_blid_wiper_ctlType_RI_BLID_WIPER_CTL_STANDBY_POSITION_E2;
                cdc_15_443_i.ri_blid_wiper_frequency_ctl = 0;
                cdc_15_443_i.ri_blid_wiper_number_ctl = 0;
                dataSend443(cdc_15_443_i);
                locker.unlock();
                Delay_MSec(2000);

                locker.relock();
                cdc_15_443_i.ri_blid_wiper_ctl = Cdc_15_443_Ri_blid_wiper_ctlType_RI_BLID_WIPER_CTL_ACTIVE_E1;
                cdc_15_443_i.ri_blid_wiper_frequency_ctl = it->cleanFrequency;
                cdc_15_443_i.ri_blid_wiper_number_ctl = it->cleanCount;
                dataSend443(cdc_15_443_i);
                locker.unlock();

                Delay_MSec(it->cleanCount*(60/it->cleanFrequency*1000));
                locker.relock();
                cdc_15_443_i.ri_blid_wiper_ctl = Cdc_15_443_Ri_blid_wiper_ctlType_RI_BLID_WIPER_CTL_STANDBY_POSITION_E2;
                cdc_15_443_i.ri_blid_wiper_frequency_ctl = 0;
                cdc_15_443_i.ri_blid_wiper_number_ctl = 0;
                dataSend443(cdc_15_443_i);
                locker.unlock();
                Delay_MSec(2000);
            }

            if(it->waterCount > 0)
            {
                Delay_MSec(it->waterDelay);
                QMutexLocker locker(&dataMutex1);
                cdc_15_443_i.ri_blid_wiper_ctl = Cdc_15_443_Ri_blid_wiper_ctlType_RI_BLID_WIPER_CTL_ACTIVE_E1;
                cdc_15_443_i.ri_blid_wiper_frequency_ctl = it->waterFrequency;
                cdc_15_443_i.ri_blid_wiper_number_ctl = it->waterCount;
                dataSend443(cdc_15_443_i);
                locker.unlock();

                Delay_MSec(it->waterCount*(60/it->waterFrequency*1000));
                locker.relock();
                cdc_15_443_i.ri_blid_wiper_ctl = Cdc_15_443_Ri_blid_wiper_ctlType_RI_BLID_WIPER_CTL_STANDBY_POSITION_E2;
                cdc_15_443_i.ri_blid_wiper_frequency_ctl = 0;
                cdc_15_443_i.ri_blid_wiper_number_ctl = 0;
                dataSend443(cdc_15_443_i);
                locker.unlock();
                Delay_MSec(2000);
            }
        }
    }
        break;
    case 2:{
        QVector<TableRowData> allRows = ui->tableWidget->getTableData();
        QString targetValve = "前补盲激光雷达水阀";
        auto it = std::find_if(allRows.begin(), allRows.end(), [&](const TableRowData &d){
            return d.valvePosition == targetValve;
        });
        if (it != allRows.end()) {
            if(it->cleanCount > 0){
                QMutexLocker locker(&dataMutex1);
                cdc_15_443_i.fr_blid_wiper_ctl = Cdc_15_443_Fr_blid_wiper_ctlType_FR_BLID_WIPER_CTL_STANDBY_POSITION_E2;
                cdc_15_443_i.fr_blid_wiper_frequency_ctl = 0;
                cdc_15_443_i.fr_blid_wiper_number_ctl = 0;
                dataSend443(cdc_15_443_i);
                locker.unlock();
                Delay_MSec(2000);

                locker.relock();
                cdc_15_443_i.fr_blid_wiper_ctl = Cdc_15_443_Fr_blid_wiper_ctlType_FR_BLID_WIPER_CTL_ACTIVE_E1;
                cdc_15_443_i.fr_blid_wiper_frequency_ctl = it->cleanFrequency;
                cdc_15_443_i.fr_blid_wiper_number_ctl = it->cleanCount;
                dataSend443(cdc_15_443_i);
                locker.unlock();

                Delay_MSec(it->cleanCount*(60/it->cleanFrequency*1000));
                locker.relock();
                cdc_15_443_i.fr_blid_wiper_ctl = Cdc_15_443_Fr_blid_wiper_ctlType_FR_BLID_WIPER_CTL_STANDBY_POSITION_E2;
                cdc_15_443_i.fr_blid_wiper_frequency_ctl = 0;
                cdc_15_443_i.fr_blid_wiper_number_ctl = 0;
                dataSend443(cdc_15_443_i);
                locker.unlock();
                Delay_MSec(2000);
            }

            if(it->waterCount > 0)
            {
                Delay_MSec(it->waterDelay);
                QMutexLocker locker(&dataMutex1);
                cdc_15_443_i.fr_blid_wiper_ctl = Cdc_15_443_Fr_blid_wiper_ctlType_FR_BLID_WIPER_CTL_ACTIVE_E1;
                cdc_15_443_i.fr_blid_wiper_frequency_ctl = it->waterFrequency;
                cdc_15_443_i.fr_blid_wiper_number_ctl = it->waterCount;
                dataSend443(cdc_15_443_i);
                locker.unlock();

                Delay_MSec(it->waterCount*(60/it->waterFrequency*1000));
                locker.relock();
                cdc_15_443_i.fr_blid_wiper_ctl = Cdc_15_443_Fr_blid_wiper_ctlType_FR_BLID_WIPER_CTL_STANDBY_POSITION_E2;
                cdc_15_443_i.fr_blid_wiper_frequency_ctl = 0;
                cdc_15_443_i.fr_blid_wiper_number_ctl = 0;
                dataSend443(cdc_15_443_i);
                locker.unlock();
                Delay_MSec(2000);
            }
        }
    }
        break;
    case 4:{
        QVector<TableRowData> allRows = ui->tableWidget->getTableData();
        QString targetValve = "左补盲激光雷达水阀";
        auto it = std::find_if(allRows.begin(), allRows.end(), [&](const TableRowData &d){
            return d.valvePosition == targetValve;
        });
        if (it != allRows.end()) {
            if(it->cleanCount > 0){
                QMutexLocker locker(&dataMutex1);
                cdc_15_443_i.le_blid_wiper_ctl = Cdc_15_443_Le_blid_wiper_ctlType_LE_BLID_WIPER_CTL_STANDBY_POSITION_E2;
                cdc_15_443_i.le_blid_wiper_frequency_ctl = 0;
                cdc_15_443_i.le_blid_wiper_number_ctl = 0;
                dataSend443(cdc_15_443_i);
                locker.unlock();
                Delay_MSec(2000);

                locker.relock();
                cdc_15_443_i.le_blid_wiper_ctl = Cdc_15_443_Le_blid_wiper_ctlType_LE_BLID_WIPER_CTL_ACTIVE_E1;
                cdc_15_443_i.le_blid_wiper_frequency_ctl = it->cleanFrequency;
                cdc_15_443_i.le_blid_wiper_number_ctl = it->cleanCount;
                dataSend443(cdc_15_443_i);
                locker.unlock();

                Delay_MSec(it->cleanCount*(60/it->cleanFrequency*1000));
                locker.relock();
                cdc_15_443_i.le_blid_wiper_ctl = Cdc_15_443_Le_blid_wiper_ctlType_LE_BLID_WIPER_CTL_STANDBY_POSITION_E2;
                cdc_15_443_i.le_blid_wiper_frequency_ctl = 0;
                cdc_15_443_i.le_blid_wiper_number_ctl = 0;
                dataSend443(cdc_15_443_i);
                locker.unlock();
                Delay_MSec(2000);
            }

            if(it->waterCount > 0)
            {
                Delay_MSec(it->waterDelay);
                QMutexLocker locker(&dataMutex1);
                cdc_15_443_i.le_blid_wiper_ctl = Cdc_15_443_Le_blid_wiper_ctlType_LE_BLID_WIPER_CTL_ACTIVE_E1;
                cdc_15_443_i.le_blid_wiper_frequency_ctl = it->waterFrequency;
                cdc_15_443_i.le_blid_wiper_number_ctl = it->waterCount;
                dataSend443(cdc_15_443_i);
                locker.unlock();

                Delay_MSec(it->waterCount*(60/it->waterFrequency*1000));
                locker.relock();
                cdc_15_443_i.le_blid_wiper_ctl = Cdc_15_443_Le_blid_wiper_ctlType_LE_BLID_WIPER_CTL_STANDBY_POSITION_E2;
                cdc_15_443_i.le_blid_wiper_frequency_ctl = 0;
                cdc_15_443_i.le_blid_wiper_number_ctl = 0;
                dataSend443(cdc_15_443_i);
                locker.unlock();
                Delay_MSec(2000);
            }
        }
    }
        break;
    case 6:{
        QVector<TableRowData> allRows = ui->tableWidget->getTableData();
        QString targetValve = "左翼子板周边相机电磁阀";
        auto it = std::find_if(allRows.begin(), allRows.end(), [&](const TableRowData &d){
            return d.valvePosition == targetValve;
        });
        if (it != allRows.end()) {
            if(it->cleanCount > 0){
                QMutexLocker locker(&dataMutex1);
                cdc_15_443_i.lf_pcam_wiper_ctl = Cdc_15_443_Lf_pcam_wiper_ctlType_LF_PCAM_WIPER_CTL_ACTIVE_E1;
                cdc_15_443_i.lf_pcam_wiper_frequency_ctl = it->cleanFrequency;
                cdc_15_443_i.lf_pcam_wiper_number_ctl = it->cleanCount;
                dataSend443(cdc_15_443_i);
                locker.unlock();

                Delay_MSec(it->cleanCount*(60/it->cleanFrequency*1000));
                locker.relock();
                cdc_15_443_i.lf_pcam_wiper_ctl = Cdc_15_443_Lf_pcam_wiper_ctlType_LF_PCAM_WIPER_CTL_STANDBY_POSITION_E2;
                cdc_15_443_i.lf_pcam_wiper_frequency_ctl = 0;
                cdc_15_443_i.lf_pcam_wiper_number_ctl = 0;
                dataSend443(cdc_15_443_i);
                locker.unlock();
            }
            if(it->waterCount > 0)
            {
                Delay_MSec(it->waterDelay);
                QMutexLocker locker(&dataMutex1);
                cdc_15_443_i.lf_pcam_wiper_ctl = Cdc_15_443_Lf_pcam_wiper_ctlType_LF_PCAM_WIPER_CTL_ACTIVE_E1;
                cdc_15_443_i.lf_pcam_wiper_frequency_ctl = it->waterFrequency;
                cdc_15_443_i.lf_pcam_wiper_number_ctl = it->waterCount;
                dataSend443(cdc_15_443_i);
                locker.unlock();

                Delay_MSec(it->waterCount*(60/it->waterFrequency*1000));
                locker.relock();
                cdc_15_443_i.lf_pcam_wiper_ctl = Cdc_15_443_Lf_pcam_wiper_ctlType_LF_PCAM_WIPER_CTL_STANDBY_POSITION_E2;
                cdc_15_443_i.lf_pcam_wiper_frequency_ctl = 0;
                cdc_15_443_i.lf_pcam_wiper_number_ctl = 0;
                dataSend443(cdc_15_443_i);
                locker.unlock();
            }
        }
    }
        break;
    case 7:{
        QVector<TableRowData> allRows = ui->tableWidget->getTableData();
        QString targetValve = "后保左侧周边相机电磁阀";
        auto it = std::find_if(allRows.begin(), allRows.end(), [&](const TableRowData &d){
            return d.valvePosition == targetValve;
        });
        if (it != allRows.end()) {
            if(it->cleanCount > 0){
                QMutexLocker locker(&dataMutex1);
                cdc_15_443_i.lrb_pcam_wiper_ctl = Cdc_15_443_Lrb_pcam_wiper_ctlType_LRB_PCAM_WIPER_CTL_ACTIVE_E1;
                cdc_15_443_i.lrb_pcam_wiper_frequency_ctl = it->cleanFrequency;
                cdc_15_443_i.lrb_pcam_wiper_number_ctl = it->cleanCount;
                dataSend443(cdc_15_443_i);
                locker.unlock();

                Delay_MSec(it->cleanCount*(60/it->cleanFrequency*1000));
                locker.relock();
                cdc_15_443_i.lrb_pcam_wiper_ctl = Cdc_15_443_Lrb_pcam_wiper_ctlType_LRB_PCAM_WIPER_CTL_STANDBY_POSITION_E2;
                cdc_15_443_i.lrb_pcam_wiper_frequency_ctl = 0;
                cdc_15_443_i.lrb_pcam_wiper_number_ctl = 0;
                dataSend443(cdc_15_443_i);
                locker.unlock();
            }
            if(it->waterCount > 0)
            {
                Delay_MSec(it->waterDelay);
                QMutexLocker locker(&dataMutex1);
                cdc_15_443_i.lrb_pcam_wiper_ctl = Cdc_15_443_Lrb_pcam_wiper_ctlType_LRB_PCAM_WIPER_CTL_ACTIVE_E1;
                cdc_15_443_i.lrb_pcam_wiper_frequency_ctl = it->waterFrequency;
                cdc_15_443_i.lrb_pcam_wiper_number_ctl = it->waterCount;
                dataSend443(cdc_15_443_i);
                locker.unlock();

                Delay_MSec(it->waterCount*(60/it->waterFrequency*1000));
                locker.relock();
                cdc_15_443_i.lrb_pcam_wiper_ctl = Cdc_15_443_Lrb_pcam_wiper_ctlType_LRB_PCAM_WIPER_CTL_STANDBY_POSITION_E2;
                cdc_15_443_i.lrb_pcam_wiper_frequency_ctl = 0;
                cdc_15_443_i.lrb_pcam_wiper_number_ctl = 0;
                dataSend443(cdc_15_443_i);
                locker.unlock();
            }
        }
    }
        break;
    case 8:{
        QVector<TableRowData> allRows = ui->tableWidget->getTableData();
        QString targetValve = "后保右侧周边相机电磁阀";
        auto it = std::find_if(allRows.begin(), allRows.end(), [&](const TableRowData &d){
            return d.valvePosition == targetValve;
        });
        if (it != allRows.end()) {
            if(it->cleanCount > 0){
                QMutexLocker locker(&dataMutex1);
                cdc_15_443_i.rrb_pcam_wiper_ctl = Cdc_15_443_Rrb_pcam_wiper_ctlType_RRB_PCAM_WIPER_CTL_ACTIVE_E1;
                cdc_15_443_i.rrb_pcam_wiper_frequency_ctl = it->cleanFrequency;
                cdc_15_443_i.rrb_pcam_wiper_number_ctl = it->cleanCount;
                dataSend443(cdc_15_443_i);
                locker.unlock();

                Delay_MSec(it->cleanCount*(60/it->cleanFrequency*1000));
                locker.relock();
                cdc_15_443_i.rrb_pcam_wiper_ctl = Cdc_15_443_Rrb_pcam_wiper_ctlType_RRB_PCAM_WIPER_CTL_STANDBY_POSITION_E2;
                cdc_15_443_i.rrb_pcam_wiper_frequency_ctl = 0;
                cdc_15_443_i.rrb_pcam_wiper_number_ctl = 0;
                dataSend443(cdc_15_443_i);
                locker.unlock();
            }
            if(it->waterCount > 0)
            {
                Delay_MSec(it->waterDelay);
                QMutexLocker locker(&dataMutex1);
                cdc_15_443_i.rrb_pcam_wiper_ctl = Cdc_15_443_Rrb_pcam_wiper_ctlType_RRB_PCAM_WIPER_CTL_ACTIVE_E1;
                cdc_15_443_i.rrb_pcam_wiper_frequency_ctl = it->waterFrequency;
                cdc_15_443_i.rrb_pcam_wiper_number_ctl = it->waterCount;
                dataSend443(cdc_15_443_i);
                locker.unlock();

                Delay_MSec(it->waterCount*(60/it->waterFrequency*1000));
                locker.relock();
                cdc_15_443_i.rrb_pcam_wiper_ctl = Cdc_15_443_Rrb_pcam_wiper_ctlType_RRB_PCAM_WIPER_CTL_STANDBY_POSITION_E2;
                cdc_15_443_i.rrb_pcam_wiper_frequency_ctl = 0;
                cdc_15_443_i.rrb_pcam_wiper_number_ctl = 0;
                dataSend443(cdc_15_443_i);
                locker.unlock();
            }
        }
    }
        break;
    case 10:{
        QVector<TableRowData> allRows = ui->tableWidget->getTableData();
        QString targetValve = "后补盲激光雷达水阀";
        auto it = std::find_if(allRows.begin(), allRows.end(), [&](const TableRowData &d){
            return d.valvePosition == targetValve;
        });
        if (it != allRows.end()) {
            if(it->cleanCount > 0){
                QMutexLocker locker(&dataMutex1);
                cdc_15_443_i.re_blid_rac_wiper_ctl = Cdc_15_443_Re_blid_rac_wiper_ctlType_RE_BLID_RAC_WIPER_CTL_STANDBY_POSITION_E2;
                cdc_15_443_i.re_blid_rac_wiper_frequency_ctl = 0;
                cdc_15_443_i.re_blid_rac_wiper_number_ctl = 0;
                dataSend443(cdc_15_443_i);
                locker.unlock();
                Delay_MSec(2000);

                locker.relock();
                cdc_15_443_i.re_blid_rac_wiper_ctl = Cdc_15_443_Re_blid_rac_wiper_ctlType_RE_BLID_RAC_WIPER_CTL_ACTIVE_E1;
                cdc_15_443_i.re_blid_rac_wiper_frequency_ctl = it->cleanFrequency;
                cdc_15_443_i.re_blid_rac_wiper_number_ctl = it->cleanCount;
                dataSend443(cdc_15_443_i);
                locker.unlock();

                Delay_MSec(it->cleanCount*(60/it->cleanFrequency*1000));
                locker.relock();
                cdc_15_443_i.re_blid_rac_wiper_ctl = Cdc_15_443_Re_blid_rac_wiper_ctlType_RE_BLID_RAC_WIPER_CTL_STANDBY_POSITION_E2;
                cdc_15_443_i.re_blid_rac_wiper_frequency_ctl = 0;
                cdc_15_443_i.re_blid_rac_wiper_number_ctl = 0;
                dataSend443(cdc_15_443_i);
                locker.unlock();
                Delay_MSec(2000);
            }

            if(it->waterCount > 0)
            {
                Delay_MSec(it->waterDelay);
                QMutexLocker locker(&dataMutex1);
                cdc_15_443_i.re_blid_rac_wiper_ctl = Cdc_15_443_Re_blid_rac_wiper_ctlType_RE_BLID_RAC_WIPER_CTL_ACTIVE_E1;
                cdc_15_443_i.re_blid_rac_wiper_frequency_ctl = it->waterFrequency;
                cdc_15_443_i.re_blid_rac_wiper_number_ctl = it->waterCount;
                dataSend443(cdc_15_443_i);
                locker.unlock();

                Delay_MSec(it->waterCount*(60/it->waterFrequency*1000));
                locker.relock();
                cdc_15_443_i.re_blid_rac_wiper_ctl = Cdc_15_443_Re_blid_rac_wiper_ctlType_RE_BLID_RAC_WIPER_CTL_STANDBY_POSITION_E2;
                cdc_15_443_i.re_blid_rac_wiper_frequency_ctl = 0;
                cdc_15_443_i.re_blid_rac_wiper_number_ctl = 0;
                dataSend443(cdc_15_443_i);
                locker.unlock();
                Delay_MSec(2000);
            }
        }
    }
        break;
    case 11:{
        QVector<TableRowData> allRows = ui->tableWidget->getTableData();
        QString targetValve = "正后相机电磁阀";
        auto it = std::find_if(allRows.begin(), allRows.end(), [&](const TableRowData &d){
            return d.valvePosition == targetValve;
        });
        if (it != allRows.end()) {
            if(it->cleanCount > 0){
                QMutexLocker locker(&dataMutex1);
                cdc_15_443_i.re_cam_wiper_ctl = Cdc_15_443_Re_cam_wiper_ctlType_RE_CAM_WIPER_CTL_ACTIVE_E1;
                cdc_15_443_i.re_cam_wiper_frequency_ctl = it->cleanFrequency;
                cdc_15_443_i.re_cam_wiper_number_ctl = it->cleanCount;
                dataSend443(cdc_15_443_i);
                locker.unlock();

                Delay_MSec(it->cleanCount*(60/it->cleanFrequency*1000));
                locker.relock();
                cdc_15_443_i.re_cam_wiper_ctl = Cdc_15_443_Re_cam_wiper_ctlType_RE_CAM_WIPER_CTL_STANDBY_POSITION_E2;
                cdc_15_443_i.re_cam_wiper_frequency_ctl = 0;
                cdc_15_443_i.re_cam_wiper_number_ctl = 0;
                dataSend443(cdc_15_443_i);
                locker.unlock();
            }
            if(it->waterCount > 0)
            {
                Delay_MSec(it->waterDelay);
                QMutexLocker locker(&dataMutex1);
                cdc_15_443_i.re_cam_wiper_ctl = Cdc_15_443_Re_cam_wiper_ctlType_RE_CAM_WIPER_CTL_ACTIVE_E1;
                cdc_15_443_i.re_cam_wiper_frequency_ctl = it->waterFrequency;
                cdc_15_443_i.re_cam_wiper_number_ctl = it->waterCount;
                dataSend443(cdc_15_443_i);
                locker.unlock();

                Delay_MSec(it->waterCount*(60/it->waterFrequency*1000));
                locker.relock();
                cdc_15_443_i.re_cam_wiper_ctl = Cdc_15_443_Re_cam_wiper_ctlType_RE_CAM_WIPER_CTL_STANDBY_POSITION_E2;
                cdc_15_443_i.re_cam_wiper_frequency_ctl = 0;
                cdc_15_443_i.re_cam_wiper_number_ctl = 0;
                dataSend443(cdc_15_443_i);
                locker.unlock();
            }
        }
    }
        break;
    case 13:{
        QVector<TableRowData> allRows = ui->tableWidget->getTableData();
        QString targetValve = "左前相机电磁阀";
        auto it = std::find_if(allRows.begin(), allRows.end(), [&](const TableRowData &d){
            return d.valvePosition == targetValve;
        });
        if (it != allRows.end()) {
            if(it->cleanCount > 0){
                QMutexLocker locker(&dataMutex1);
                cdc_15_443_i.fl_cam_wiper_ctl = Cdc_15_443_Fl_cam_wiper_ctlType_FL_CAM_WIPER_CTL_ACTIVE_E1;
                cdc_15_443_i.fl_cam_wiper_frequency_ctl = it->cleanFrequency;
                cdc_15_443_i.fl_cam_wiper_number_ctl = it->cleanCount;
                dataSend443(cdc_15_443_i);
                locker.unlock();

                Delay_MSec(it->cleanCount*(60/it->cleanFrequency*1000));
                locker.relock();
                cdc_15_443_i.fl_cam_wiper_ctl = Cdc_15_443_Fl_cam_wiper_ctlType_FL_CAM_WIPER_CTL_STANDBY_POSITION_E2;
                cdc_15_443_i.fl_cam_wiper_frequency_ctl = 0;
                cdc_15_443_i.fl_cam_wiper_number_ctl = 0;
                dataSend443(cdc_15_443_i);
                locker.unlock();
            }
            if(it->waterCount > 0)
            {
                Delay_MSec(it->waterDelay);
                QMutexLocker locker(&dataMutex1);
                cdc_15_443_i.fl_cam_wiper_ctl = Cdc_15_443_Fl_cam_wiper_ctlType_FL_CAM_WIPER_CTL_ACTIVE_E1;
                cdc_15_443_i.fl_cam_wiper_frequency_ctl = it->waterFrequency;
                cdc_15_443_i.fl_cam_wiper_number_ctl = it->waterCount;
                dataSend443(cdc_15_443_i);
                locker.unlock();

                Delay_MSec(it->waterCount*(60/it->waterFrequency*1000));
                locker.relock();
                cdc_15_443_i.fl_cam_wiper_ctl = Cdc_15_443_Fl_cam_wiper_ctlType_FL_CAM_WIPER_CTL_STANDBY_POSITION_E2;
                cdc_15_443_i.fl_cam_wiper_frequency_ctl = 0;
                cdc_15_443_i.fl_cam_wiper_number_ctl = 0;
                dataSend443(cdc_15_443_i);
                locker.unlock();
            }
        }
    }
        break;
    case 14:{
        QVector<TableRowData> allRows = ui->tableWidget->getTableData();
        QString targetValve = "右前相机电磁阀";
        auto it = std::find_if(allRows.begin(), allRows.end(), [&](const TableRowData &d){
            return d.valvePosition == targetValve;
        });
        if (it != allRows.end()) {
            if(it->cleanCount > 0){
                QMutexLocker locker(&dataMutex1);
                cdc_15_443_i.fr_cam_wiper_ctl = Cdc_15_443_Fr_cam_wiper_ctlType_FR_CAM_WIPER_CTL_ACTIVE_E1;
                cdc_15_443_i.fr_cam_wiper_frequency_ctl = it->cleanFrequency;
                cdc_15_443_i.fr_cam_wiper_number_ctl = it->cleanCount;
                dataSend443(cdc_15_443_i);
                locker.unlock();

                Delay_MSec(it->cleanCount*(60/it->cleanFrequency*1000));
                locker.relock();
                cdc_15_443_i.fr_cam_wiper_ctl = Cdc_15_443_Fr_cam_wiper_ctlType_FR_CAM_WIPER_CTL_STANDBY_POSITION_E2;
                cdc_15_443_i.fr_cam_wiper_frequency_ctl = 0;
                cdc_15_443_i.fr_cam_wiper_number_ctl = 0;
                dataSend443(cdc_15_443_i);
                locker.unlock();
            }
            if(it->waterCount > 0)
            {
                Delay_MSec(it->waterDelay);
                QMutexLocker locker(&dataMutex1);
                cdc_15_443_i.fr_cam_wiper_ctl = Cdc_15_443_Fr_cam_wiper_ctlType_FR_CAM_WIPER_CTL_ACTIVE_E1;
                cdc_15_443_i.fr_cam_wiper_frequency_ctl = it->waterFrequency;
                cdc_15_443_i.fr_cam_wiper_number_ctl = it->waterCount;
                dataSend443(cdc_15_443_i);
                locker.unlock();

                Delay_MSec(it->waterCount*(60/it->waterFrequency*1000));
                locker.relock();
                cdc_15_443_i.fr_cam_wiper_ctl = Cdc_15_443_Fr_cam_wiper_ctlType_FR_CAM_WIPER_CTL_STANDBY_POSITION_E2;
                cdc_15_443_i.fr_cam_wiper_frequency_ctl = 0;
                cdc_15_443_i.fr_cam_wiper_number_ctl = 0;
                dataSend443(cdc_15_443_i);
                locker.unlock();
            }
        }
    }
        break;
    case 15:{
        QVector<TableRowData> allRows = ui->tableWidget->getTableData();
        QString targetValve = "左侧广角相机电磁阀";
        auto it = std::find_if(allRows.begin(), allRows.end(), [&](const TableRowData &d){
            return d.valvePosition == targetValve;
        });
        if (it != allRows.end()) {
            if(it->cleanCount > 0){
                QMutexLocker locker(&dataMutex1);
                cdc_15_443_i.le_cam_wiper_ctl = Cdc_15_443_Le_cam_wiper_ctlType_LE_CAM_WIPER_CTL_ACTIVE_E1;
                cdc_15_443_i.le_cam_wiper_frequency_ctl = it->cleanFrequency;
                cdc_15_443_i.le_cam_wiper_number_ctl = it->cleanCount;
                dataSend443(cdc_15_443_i);
                locker.unlock();

                Delay_MSec(it->cleanCount*(60/it->cleanFrequency*1000));
                locker.relock();
                cdc_15_443_i.le_cam_wiper_ctl = Cdc_15_443_Le_cam_wiper_ctlType_LE_CAM_WIPER_CTL_STANDBY_POSITION_E2;
                cdc_15_443_i.le_cam_wiper_frequency_ctl = 0;
                cdc_15_443_i.le_cam_wiper_number_ctl = 0;
                dataSend443(cdc_15_443_i);
                locker.unlock();
            }
            if(it->waterCount > 0)
            {
                Delay_MSec(it->waterDelay);
                QMutexLocker locker(&dataMutex1);
                cdc_15_443_i.le_cam_wiper_ctl = Cdc_15_443_Le_cam_wiper_ctlType_LE_CAM_WIPER_CTL_ACTIVE_E1;
                cdc_15_443_i.le_cam_wiper_frequency_ctl = it->waterFrequency;
                cdc_15_443_i.le_cam_wiper_number_ctl = it->waterCount;
                dataSend443(cdc_15_443_i);
                locker.unlock();

                Delay_MSec(it->waterCount*(60/it->waterFrequency*1000));
                locker.relock();
                cdc_15_443_i.le_cam_wiper_ctl = Cdc_15_443_Le_cam_wiper_ctlType_LE_CAM_WIPER_CTL_STANDBY_POSITION_E2;
                cdc_15_443_i.le_cam_wiper_frequency_ctl = 0;
                cdc_15_443_i.le_cam_wiper_number_ctl = 0;
                dataSend443(cdc_15_443_i);
                locker.unlock();
            }
        }
    }
        break;
    case 16:{
        QVector<TableRowData> allRows = ui->tableWidget->getTableData();
        QString targetValve = "右侧广角相机电磁阀";
        auto it = std::find_if(allRows.begin(), allRows.end(), [&](const TableRowData &d){
            return d.valvePosition == targetValve;
        });
        if (it != allRows.end()) {
            if(it->cleanCount > 0){
                QMutexLocker locker(&dataMutex1);
                cdc_15_443_i.ri_cam_wiper_ctl = Cdc_15_443_Ri_cam_wiper_ctlType_RI_CAM_WIPER_CTL_ACTIVE_E1;
                cdc_15_443_i.ri_cam_wiper_frequency_ctl = it->cleanFrequency;
                cdc_15_443_i.ri_cam_wiper_number_ctl = it->cleanCount;
                dataSend443(cdc_15_443_i);
                locker.unlock();

                Delay_MSec(it->cleanCount*(60/it->cleanFrequency*1000));
                locker.relock();
                cdc_15_443_i.ri_cam_wiper_ctl = Cdc_15_443_Ri_cam_wiper_ctlType_RI_CAM_WIPER_CTL_STANDBY_POSITION_E2;
                cdc_15_443_i.ri_cam_wiper_frequency_ctl = 0;
                cdc_15_443_i.ri_cam_wiper_number_ctl = 0;
                dataSend443(cdc_15_443_i);
                locker.unlock();
            }
            if(it->waterCount > 0)
            {
                Delay_MSec(it->waterDelay);
                QMutexLocker locker(&dataMutex1);
                cdc_15_443_i.ri_cam_wiper_ctl = Cdc_15_443_Ri_cam_wiper_ctlType_RI_CAM_WIPER_CTL_ACTIVE_E1;
                cdc_15_443_i.ri_cam_wiper_frequency_ctl = it->waterFrequency;
                cdc_15_443_i.ri_cam_wiper_number_ctl = it->waterCount;
                dataSend443(cdc_15_443_i);
                locker.unlock();

                Delay_MSec(it->waterCount*(60/it->waterFrequency*1000));
                locker.relock();
                cdc_15_443_i.ri_cam_wiper_ctl = Cdc_15_443_Ri_cam_wiper_ctlType_RI_CAM_WIPER_CTL_STANDBY_POSITION_E2;
                cdc_15_443_i.ri_cam_wiper_frequency_ctl = 0;
                cdc_15_443_i.ri_cam_wiper_number_ctl = 0;
                dataSend443(cdc_15_443_i);
                locker.unlock();
            }
        }
    }
        break;
    default: break;
    }
}

void MainWindow::myTaskFunction(int id)
{
    switch (id) {
    case 0:{
        QVector<TableRowData> allRows = ui->tableWidget->getTableData();
        QString targetValve = "右补盲激光雷达水阀";
        auto it = std::find_if(allRows.begin(), allRows.end(), [&](const TableRowData &d){
            return d.valvePosition == targetValve;
        });
        if (it != allRows.end()) {
            if(it->sprayDuration > 0)
            {
//                QTimer::singleShot(it->sprayDuration + it->cleanDelay, this, [=](){
//                    emit timerMake0(id);
//                });

//                QtConcurrent::run([=](){
//                    QThread::msleep(it->sprayDuration + it->cleanDelay);
//                    emit timerMake0(id);   // 从子线程发信号，Qt 会自动切到接收者所在的线程
//                });
//                QThread *t = QThread::create([=](){
//                    QThread::msleep(it->sprayDuration + it->cleanDelay);
//                    emit timerMake0(id);
//                });
//                t->start();

                QThread *t = QThread::create([=](){
                    QThread::msleep(it->sprayDuration + it->cleanDelay);
                    this->on_timerMake0(id);  // 直接运行在子线程
                });
                connect(t, &QThread::finished, t, &QObject::deleteLater);
                t->start();



                Cdccontrol1683_Reset(&cdc_control1_683_i[id]);
                cdc_control1_683_i[id].cdc_rilidarvalvepwr = Cdc_control2_683_Cdc_rilidarvalvepwrType_CDC_RILIDARVALVEPWR_ON_E2;
                dataSend683(cdc_control1_683_i[id]);

                Delay_MSec(it->sprayDuration);
                Cdccontrol1683_Reset(&cdc_control1_683_i[id]);
                cdc_control1_683_i[id].cdc_rilidarvalvepwr = Cdc_control2_683_Cdc_rilidarvalvepwrType_CDC_RILIDARVALVEPWR_OFF_E1;
                dataSend683(cdc_control1_683_i[id]);

            }else if(it->cycleCount > 0)
            {
//                QTimer::singleShot((it->closeDuration + it->openDuration)*it->cleanCount + it->cleanDelay, this, [=](){
//                    emit timerMake0(id);
//                });
                QThread *t = QThread::create([=](){
                    QThread::msleep((it->closeDuration + it->openDuration)*it->cleanCount + it->cleanDelay);
                    this->on_timerMake0(id);  // 直接运行在子线程
                });
                connect(t, &QThread::finished, t, &QObject::deleteLater);
                t->start();

                Cdccontrol1683_Reset(&cdc_control1_683_i[id]);
                cdc_control1_683_i[id].cdc_rilidarvalvepwr = Cdc_control2_683_Cdc_rilidarvalvepwrType_CDC_RILIDARVALVEPWR_NO_REQUEST_E0;
                dataSend683(cdc_control1_683_i[id]);

                Cdccontrol1683_Reset(&cdc_control1_683_i[id]);
                cdc_control1_683_i[id].cdc_rilidarvalvepwr = Cdc_control2_683_Cdc_rilidarvalvepwrType_CDC_RILIDARVALVEPWR_INTERMIT_SPRAY_E3;
                cdc_control1_683_i[id].cdc_rilidarvalvespraynum = it->cycleCount;
                cdc_control1_683_i[id].cdc_rilidarvalveopdrtn = it->openDuration;
                cdc_control1_683_i[id].cdc_rilidarvalveclsdrtn = it->closeDuration;
                dataSend683(cdc_control1_683_i[id]);
            }
        }
        break;
    }
    case 1:{
        QVector<TableRowData> allRows = ui->tableWidget->getTableData();
        QString targetValve = "右补盲鱼眼相机水阀";
        auto it = std::find_if(allRows.begin(), allRows.end(), [&](const TableRowData &d){
            return d.valvePosition == targetValve;
        });
        if (it != allRows.end()) {
            if(it->sprayDuration > 0)
            {
                Cdccontrol1683_Reset(&cdc_control1_683_i[id]);
                cdc_control1_683_i[id].cdc_fisheye_r_cmr_valve_pwr_req = Cdc_control2_683_Cdc_fisheye_r_cmr_valve_pwr_reqType_CDC_FISHEYE_R_CMR_VALVE_PWR_REQ_ON_E2;
                dataSend683(cdc_control1_683_i[id]);

                Delay_MSec(it->sprayDuration);
                Cdccontrol1683_Reset(&cdc_control1_683_i[id]);
                cdc_control1_683_i[id].cdc_fisheye_r_cmr_valve_pwr_req = Cdc_control2_683_Cdc_fisheye_r_cmr_valve_pwr_reqType_CDC_FISHEYE_R_CMR_VALVE_PWR_REQ_OFF_E1;
                dataSend683(cdc_control1_683_i[id]);
            }else if(it->cycleCount > 0)
            {
                Cdccontrol1683_Reset(&cdc_control1_683_i[id]);
                cdc_control1_683_i[id].cdc_fisheye_r_cmr_valve_pwr_req = Cdc_control2_683_Cdc_fisheye_r_cmr_valve_pwr_reqType_CDC_FISHEYE_R_CMR_VALVE_PWR_REQ_NO_REQUEST_E0;
                dataSend683(cdc_control1_683_i[id]);

                Cdccontrol1683_Reset(&cdc_control1_683_i[id]);
                cdc_control1_683_i[id].cdc_fisheye_r_cmr_valve_pwr_req = Cdc_control2_683_Cdc_fisheye_r_cmr_valve_pwr_reqType_CDC_FISHEYE_R_CMR_VALVE_PWR_REQ_INTERMIT_SPRAY_E3;
                cdc_control1_683_i[id].cdc_fisheye_r_cmrvvspraynum = it->cycleCount;
                cdc_control1_683_i[id].cdc_fisheye_r_cmrvvopdrtn = it->openDuration;
                cdc_control1_683_i[id].cdc_fisheye_r_cmrvvclsdrtn = it->closeDuration;
                dataSend683(cdc_control1_683_i[id]);
            }
        }
        break;
    }
    case 2:{
        QVector<TableRowData> allRows = ui->tableWidget->getTableData();
        QString targetValve = "前补盲激光雷达水阀";
        auto it = std::find_if(allRows.begin(), allRows.end(), [&](const TableRowData &d){
            return d.valvePosition == targetValve;
        });
        if (it != allRows.end()) {
            if(it->sprayDuration > 0)
            {
                QThread *t = QThread::create([=](){
                    QThread::msleep(it->sprayDuration + it->cleanDelay);
                    this->on_timerMake0(id);  // 直接运行在子线程
                });
                connect(t, &QThread::finished, t, &QObject::deleteLater);
                t->start();


                Cdccontrol1683_Reset(&cdc_control1_683_i[id]);
                cdc_control1_683_i[id].cdc_frlidarvalvepwr = Cdc_control2_683_Cdc_frlidarvalvepwrType_CDC_FRLIDARVALVEPWR_ON_E2;
                dataSend683(cdc_control1_683_i[id]);

                Delay_MSec(it->sprayDuration);
                Cdccontrol1683_Reset(&cdc_control1_683_i[id]);
                cdc_control1_683_i[id].cdc_frlidarvalvepwr = Cdc_control2_683_Cdc_frlidarvalvepwrType_CDC_FRLIDARVALVEPWR_OFF_E1;
                dataSend683(cdc_control1_683_i[id]);
            }else if(it->cycleCount > 0)
            {
                QThread *t = QThread::create([=](){
                    QThread::msleep((it->closeDuration + it->openDuration)*it->cleanCount + it->cleanDelay);
                    this->on_timerMake0(id);  // 直接运行在子线程
                });
                connect(t, &QThread::finished, t, &QObject::deleteLater);
                t->start();

                Cdccontrol1683_Reset(&cdc_control1_683_i[id]);
                cdc_control1_683_i[id].cdc_frlidarvalvepwr = Cdc_control2_683_Cdc_frlidarvalvepwrType_CDC_FRLIDARVALVEPWR_NO_REQUEST_E0;
                dataSend683(cdc_control1_683_i[id]);

                Cdccontrol1683_Reset(&cdc_control1_683_i[id]);
                cdc_control1_683_i[id].cdc_frlidarvalvepwr = Cdc_control2_683_Cdc_frlidarvalvepwrType_CDC_FRLIDARVALVEPWR_INTERMIT_SPRAY_E3;
                cdc_control1_683_i[id].cdc_frlidarvalvespraynum = it->cycleCount;
                cdc_control1_683_i[id].cdc_frlidarvalveopdrtn = it->openDuration;
                cdc_control1_683_i[id].cdc_frlidarvalveclsdrtn = it->closeDuration;
                dataSend683(cdc_control1_683_i[id]);
            }
        }
        break;
    }
    case 3:{
        QVector<TableRowData> allRows = ui->tableWidget->getTableData();
        QString targetValve = "前补盲鱼眼相机水阀";
        auto it = std::find_if(allRows.begin(), allRows.end(), [&](const TableRowData &d){
            return d.valvePosition == targetValve;
        });
        if (it != allRows.end()) {
            if(it->sprayDuration > 0)
            {
                Cdccontrol1683_Reset(&cdc_control1_683_i[id]);
                cdc_control1_683_i[id].cdc_fisheye_f_cmr_valve_pwr_req = Cdc_control2_683_Cdc_fisheye_f_cmr_valve_pwr_reqType_CDC_FISHEYE_F_CMR_VALVE_PWR_REQ_ON_E2;
                dataSend683(cdc_control1_683_i[id]);

                Delay_MSec(it->sprayDuration);
                Cdccontrol1683_Reset(&cdc_control1_683_i[id]);
                cdc_control1_683_i[id].cdc_fisheye_f_cmr_valve_pwr_req = Cdc_control2_683_Cdc_fisheye_f_cmr_valve_pwr_reqType_CDC_FISHEYE_F_CMR_VALVE_PWR_REQ_OFF_E1;
                dataSend683(cdc_control1_683_i[id]);
            }else if(it->cycleCount > 0)
            {
                Cdccontrol1683_Reset(&cdc_control1_683_i[id]);
                cdc_control1_683_i[id].cdc_fisheye_f_cmr_valve_pwr_req = Cdc_control2_683_Cdc_fisheye_f_cmr_valve_pwr_reqType_CDC_FISHEYE_F_CMR_VALVE_PWR_REQ_NO_REQUEST_E0;
                dataSend683(cdc_control1_683_i[id]);

                Cdccontrol1683_Reset(&cdc_control1_683_i[id]);
                cdc_control1_683_i[id].cdc_fisheye_f_cmr_valve_pwr_req = Cdc_control2_683_Cdc_fisheye_f_cmr_valve_pwr_reqType_CDC_FISHEYE_F_CMR_VALVE_PWR_REQ_INTERMIT_SPRAY_E3;
                cdc_control1_683_i[id].cdc_fisheye_f_cmrvvspraynum = it->cycleCount;
                cdc_control1_683_i[id].cdc_fisheye_f_cmrvvopdrtn = it->openDuration;
                cdc_control1_683_i[id].cdc_fisheye_f_cmrvvclsdrtn = it->closeDuration;
                dataSend683(cdc_control1_683_i[id]);
            }
        }
        break;
    }
    case 4:{
        QVector<TableRowData> allRows = ui->tableWidget->getTableData();
        QString targetValve = "左补盲激光雷达水阀";
        auto it = std::find_if(allRows.begin(), allRows.end(), [&](const TableRowData &d){
            return d.valvePosition == targetValve;
        });
        if (it != allRows.end()) {
            if(it->sprayDuration > 0)
            {
                QThread *t = QThread::create([=](){
                    QThread::msleep(it->sprayDuration + it->cleanDelay);
                    this->on_timerMake0(id);  // 直接运行在子线程
                });
                connect(t, &QThread::finished, t, &QObject::deleteLater);
                t->start();

                Cdccontrol1683_Reset(&cdc_control1_683_i[id]);
                cdc_control1_683_i[id].cdc_lelidarvalvepwr = Cdc_control2_683_Cdc_lelidarvalvepwrType_CDC_LELIDARVALVEPWR_ON_E2;
                dataSend683(cdc_control1_683_i[id]);

                Delay_MSec(it->sprayDuration);
                Cdccontrol1683_Reset(&cdc_control1_683_i[id]);
                cdc_control1_683_i[id].cdc_lelidarvalvepwr = Cdc_control2_683_Cdc_lelidarvalvepwrType_CDC_LELIDARVALVEPWR_OFF_E1;
                dataSend683(cdc_control1_683_i[id]);
            }else if(it->cycleCount > 0)
            {
                QThread *t = QThread::create([=](){
                    QThread::msleep((it->closeDuration + it->openDuration)*it->cleanCount + it->cleanDelay);
                    this->on_timerMake0(id);  // 直接运行在子线程
                });
                connect(t, &QThread::finished, t, &QObject::deleteLater);
                t->start();

                Cdccontrol1683_Reset(&cdc_control1_683_i[id]);
                cdc_control1_683_i[id].cdc_lelidarvalvepwr = Cdc_control2_683_Cdc_lelidarvalvepwrType_CDC_LELIDARVALVEPWR_NO_REQUEST_E0;
                dataSend683(cdc_control1_683_i[id]);

                Cdccontrol1683_Reset(&cdc_control1_683_i[id]);
                cdc_control1_683_i[id].cdc_lelidarvalvepwr = Cdc_control2_683_Cdc_lelidarvalvepwrType_CDC_LELIDARVALVEPWR_INTERMIT_SPRAY_E3;
                cdc_control1_683_i[id].cdc_lelidarvalveopdrtn = it->cycleCount;
                cdc_control1_683_i[id].cdc_lelidarvalvespraynum = it->openDuration;
                cdc_control1_683_i[id].cdc_lelidarvalveclsdrtn = it->closeDuration;
                dataSend683(cdc_control1_683_i[id]);
            }
        }
        break;
    }
    case 5:{
        QVector<TableRowData> allRows = ui->tableWidget->getTableData();
        QString targetValve = "左补盲鱼眼相机水阀";
        auto it = std::find_if(allRows.begin(), allRows.end(), [&](const TableRowData &d){
            return d.valvePosition == targetValve;
        });
        if (it != allRows.end()) {
            if(it->sprayDuration > 0)
            {
                Cdccontrol1683_Reset(&cdc_control1_683_i[id]);
                cdc_control1_683_i[id].cdc_fisheye_l_cmr_valve_pwr_req = Cdc_control2_683_Cdc_fisheye_l_cmr_valve_pwr_reqType_CDC_FISHEYE_L_CMR_VALVE_PWR_REQ_ON_E2;
                dataSend683(cdc_control1_683_i[id]);

                Delay_MSec(it->sprayDuration);
                Cdccontrol1683_Reset(&cdc_control1_683_i[id]);
                cdc_control1_683_i[id].cdc_fisheye_l_cmr_valve_pwr_req = Cdc_control2_683_Cdc_fisheye_l_cmr_valve_pwr_reqType_CDC_FISHEYE_L_CMR_VALVE_PWR_REQ_OFF_E1;
                dataSend683(cdc_control1_683_i[id]);
            }else if(it->cycleCount > 0)
            {
                Cdccontrol1683_Reset(&cdc_control1_683_i[id]);
                cdc_control1_683_i[id].cdc_fisheye_l_cmr_valve_pwr_req = Cdc_control2_683_Cdc_fisheye_l_cmr_valve_pwr_reqType_CDC_FISHEYE_L_CMR_VALVE_PWR_REQ_NO_REQUEST_E0;
                dataSend683(cdc_control1_683_i[id]);

                Cdccontrol1683_Reset(&cdc_control1_683_i[id]);
                cdc_control1_683_i[id].cdc_fisheye_l_cmr_valve_pwr_req = Cdc_control2_683_Cdc_fisheye_l_cmr_valve_pwr_reqType_CDC_FISHEYE_L_CMR_VALVE_PWR_REQ_INTERMIT_SPRAY_E3;
                cdc_control1_683_i[id].cdc_fisheye_l_cmrvvspraynum = it->cycleCount;
                cdc_control1_683_i[id].cdc_fisheye_l_cmrvvopdrtn = it->openDuration;
                cdc_control1_683_i[id].cdc_fisheye_l_cmrvvclsdrtn = it->closeDuration;
                dataSend683(cdc_control1_683_i[id]);
            }
        }
        break;
    }
    case 6:{
        QVector<TableRowData> allRows = ui->tableWidget->getTableData();
        QString targetValve = "左翼子板周边相机电磁阀";
        auto it = std::find_if(allRows.begin(), allRows.end(), [&](const TableRowData &d){
            return d.valvePosition == targetValve;
        });
        if (it != allRows.end()) {
            if(it->sprayDuration > 0)
            {
                QThread *t = QThread::create([=](){
                    QThread::msleep(it->sprayDuration + it->cleanDelay);
                    this->on_timerMake0(id);  // 直接运行在子线程
                });
                connect(t, &QThread::finished, t, &QObject::deleteLater);
                t->start();

                Cdccontrol1683_Reset(&cdc_control1_683_i[id]);
                cdc_control1_683_i[id].cdc_auto_lp_cmr_valve_pwr_req = Cdc_control2_683_Cdc_auto_lp_cmr_valve_pwr_reqType_CDC_AUTO_LP_CMR_VALVE_PWR_REQ_ON_E2;
                dataSend683(cdc_control1_683_i[id]);

                Delay_MSec(it->sprayDuration);
                Cdccontrol1683_Reset(&cdc_control1_683_i[id]);
                cdc_control1_683_i[id].cdc_auto_lp_cmr_valve_pwr_req = Cdc_control2_683_Cdc_auto_lp_cmr_valve_pwr_reqType_CDC_AUTO_LP_CMR_VALVE_PWR_REQ_OFF_E1;
                dataSend683(cdc_control1_683_i[id]);
            }else if(it->cycleCount > 0)
            {
                QThread *t = QThread::create([=](){
                    QThread::msleep((it->closeDuration + it->openDuration)*it->cleanCount + it->cleanDelay);
                    this->on_timerMake0(id);  // 直接运行在子线程
                });
                connect(t, &QThread::finished, t, &QObject::deleteLater);
                t->start();

                Cdccontrol1683_Reset(&cdc_control1_683_i[id]);
                cdc_control1_683_i[id].cdc_auto_lp_cmr_valve_pwr_req = Cdc_control2_683_Cdc_auto_lp_cmr_valve_pwr_reqType_CDC_AUTO_LP_CMR_VALVE_PWR_REQ_NO_REQUEST_E0;
                dataSend683(cdc_control1_683_i[id]);

                Cdccontrol1683_Reset(&cdc_control1_683_i[id]);
                cdc_control1_683_i[id].cdc_auto_lp_cmr_valve_pwr_req = Cdc_control2_683_Cdc_auto_lp_cmr_valve_pwr_reqType_CDC_AUTO_LP_CMR_VALVE_PWR_REQ_INTERMIT_SPRAY_E3;
                cdc_control1_683_i[id].cdc_auto_lp_cmrvvspraynum = it->cycleCount;
                cdc_control1_683_i[id].cdc_auto_lp_cmrvvopdrtn = it->openDuration;
                cdc_control1_683_i[id].cdc_auto_lp_cmrvvclsdrtn = it->closeDuration;
                dataSend683(cdc_control1_683_i[id]);
            }
        }
        break;
    }
    case 7:{
        QVector<TableRowData> allRows = ui->tableWidget->getTableData();
        QString targetValve = "后保左侧周边相机电磁阀";
        auto it = std::find_if(allRows.begin(), allRows.end(), [&](const TableRowData &d){
            return d.valvePosition == targetValve;
        });
        if (it != allRows.end()) {
            if(it->sprayDuration > 0)
            {
                QThread *t = QThread::create([=](){
                    QThread::msleep(it->sprayDuration + it->cleanDelay);
                    this->on_timerMake0(id);  // 直接运行在子线程
                });
                connect(t, &QThread::finished, t, &QObject::deleteLater);
                t->start();

                Cdccontrol1683_Reset(&cdc_control1_683_i[id]);
                cdc_control1_683_i[id].cdc_auto_rl_cmr_valve_pwr_req = Cdc_control2_683_Cdc_auto_rl_cmr_valve_pwr_reqType_CDC_AUTO_RL_CMR_VALVE_PWR_REQ_ON_E2;
                dataSend683(cdc_control1_683_i[id]);

                Delay_MSec(it->sprayDuration);
                Cdccontrol1683_Reset(&cdc_control1_683_i[id]);
                cdc_control1_683_i[id].cdc_auto_rl_cmr_valve_pwr_req = Cdc_control2_683_Cdc_auto_rl_cmr_valve_pwr_reqType_CDC_AUTO_RL_CMR_VALVE_PWR_REQ_OFF_E1;
                dataSend683(cdc_control1_683_i[id]);
            }else if(it->cycleCount > 0)
            {
                QThread *t = QThread::create([=](){
                    QThread::msleep((it->closeDuration + it->openDuration)*it->cleanCount + it->cleanDelay);
                    this->on_timerMake0(id);  // 直接运行在子线程
                });
                connect(t, &QThread::finished, t, &QObject::deleteLater);
                t->start();

                Cdccontrol1683_Reset(&cdc_control1_683_i[id]);
                cdc_control1_683_i[id].cdc_auto_rl_cmr_valve_pwr_req = Cdc_control2_683_Cdc_auto_rl_cmr_valve_pwr_reqType_CDC_AUTO_RL_CMR_VALVE_PWR_REQ_NO_REQUEST_E0;
                dataSend683(cdc_control1_683_i[id]);

                Cdccontrol1683_Reset(&cdc_control1_683_i[id]);
                cdc_control1_683_i[id].cdc_auto_rl_cmr_valve_pwr_req = Cdc_control2_683_Cdc_auto_rl_cmr_valve_pwr_reqType_CDC_AUTO_RL_CMR_VALVE_PWR_REQ_INTERMIT_SPRAY_E3;
                cdc_control1_683_i[id].cdc_auto_rl_cmrvvspraynum = it->cycleCount;
                cdc_control1_683_i[id].cdc_auto_rl_cmrvvopdrtn = it->openDuration;
                cdc_control1_683_i[id].cdc_auto_rl_cmrvvclsdrtn = it->closeDuration;
                dataSend683(cdc_control1_683_i[id]);
            }
        }
        break;
    }
    case 8:{
        QVector<TableRowData> allRows = ui->tableWidget->getTableData();
        QString targetValve = "后保右侧周边相机电磁阀";
        auto it = std::find_if(allRows.begin(), allRows.end(), [&](const TableRowData &d){
            return d.valvePosition == targetValve;
        });
        if (it != allRows.end()) {
            if(it->sprayDuration > 0)
            {
                QThread *t = QThread::create([=](){
                    QThread::msleep(it->sprayDuration + it->cleanDelay);
                    this->on_timerMake0(id);  // 直接运行在子线程
                });
                connect(t, &QThread::finished, t, &QObject::deleteLater);
                t->start();

                Cdccontrol1683_Reset(&cdc_control1_683_i[id]);
                cdc_control1_683_i[id].cdc_auto_rr_cmr_valve_pwr_req = Cdc_control2_683_Cdc_auto_rr_cmr_valve_pwr_reqType_CDC_AUTO_RR_CMR_VALVE_PWR_REQ_ON_E2;
                dataSend683(cdc_control1_683_i[id]);

                Delay_MSec(it->sprayDuration);
                Cdccontrol1683_Reset(&cdc_control1_683_i[id]);
                cdc_control1_683_i[id].cdc_auto_rr_cmr_valve_pwr_req = Cdc_control2_683_Cdc_auto_rr_cmr_valve_pwr_reqType_CDC_AUTO_RR_CMR_VALVE_PWR_REQ_OFF_E1;
                dataSend683(cdc_control1_683_i[id]);
            }else if(it->cycleCount > 0)
            {
                QThread *t = QThread::create([=](){
                    QThread::msleep((it->closeDuration + it->openDuration)*it->cleanCount + it->cleanDelay);
                    this->on_timerMake0(id);  // 直接运行在子线程
                });
                connect(t, &QThread::finished, t, &QObject::deleteLater);
                t->start();

                Cdccontrol1683_Reset(&cdc_control1_683_i[id]);
                cdc_control1_683_i[id].cdc_auto_rr_cmr_valve_pwr_req = Cdc_control2_683_Cdc_auto_rr_cmr_valve_pwr_reqType_CDC_AUTO_RR_CMR_VALVE_PWR_REQ_NO_REQUEST_E0;
                dataSend683(cdc_control1_683_i[id]);

                Cdccontrol1683_Reset(&cdc_control1_683_i[id]);
                cdc_control1_683_i[id].cdc_auto_rr_cmr_valve_pwr_req = Cdc_control2_683_Cdc_auto_rr_cmr_valve_pwr_reqType_CDC_AUTO_RR_CMR_VALVE_PWR_REQ_INTERMIT_SPRAY_E3;
                cdc_control1_683_i[id].cdc_auto_rr_cmrvvspraynum = it->cycleCount;
                cdc_control1_683_i[id].cdc_auto_rr_cmrvvopdrtn = it->openDuration;
                cdc_control1_683_i[id].cdc_auto_rr_cmrvvclsdrtn = it->closeDuration;
                dataSend683(cdc_control1_683_i[id]);
            }
        }
        break;
    }
    case 9:{
        QVector<TableRowData> allRows = ui->tableWidget->getTableData();
        QString targetValve = "后补盲鱼眼相机电磁阀";
        auto it = std::find_if(allRows.begin(), allRows.end(), [&](const TableRowData &d){
            return d.valvePosition == targetValve;
        });
        if (it != allRows.end()) {
            if(it->sprayDuration > 0)
            {
                Cdccontrol1683_Reset(&cdc_control1_683_i[id]);
                cdc_control1_683_i[id].cdc_fisheye_b_cmr_valve_pwr_req = Cdc_control2_683_Cdc_fisheye_b_cmr_valve_pwr_reqType_CDC_FISHEYE_B_CMR_VALVE_PWR_REQ_ON_E2;
                dataSend683(cdc_control1_683_i[id]);

                Delay_MSec(it->sprayDuration);
                Cdccontrol1683_Reset(&cdc_control1_683_i[id]);
                cdc_control1_683_i[id].cdc_fisheye_b_cmr_valve_pwr_req = Cdc_control2_683_Cdc_fisheye_b_cmr_valve_pwr_reqType_CDC_FISHEYE_B_CMR_VALVE_PWR_REQ_OFF_E1;
                dataSend683(cdc_control1_683_i[id]);
            }else if(it->cycleCount > 0)
            {
                Cdccontrol1683_Reset(&cdc_control1_683_i[id]);
                cdc_control1_683_i[id].cdc_fisheye_b_cmr_valve_pwr_req = Cdc_control2_683_Cdc_fisheye_b_cmr_valve_pwr_reqType_CDC_FISHEYE_B_CMR_VALVE_PWR_REQ_NO_REQUEST_E0;
                dataSend683(cdc_control1_683_i[id]);

                Cdccontrol1683_Reset(&cdc_control1_683_i[id]);
                cdc_control1_683_i[id].cdc_fisheye_b_cmr_valve_pwr_req = Cdc_control2_683_Cdc_fisheye_b_cmr_valve_pwr_reqType_CDC_FISHEYE_B_CMR_VALVE_PWR_REQ_INTERMIT_SPRAY_E3;
                cdc_control1_683_i[id].cdc_fisheye_b_cmrvvspraynum = it->cycleCount;
                cdc_control1_683_i[id].cdc_fisheye_b_cmrvvopdrtn = it->openDuration;
                cdc_control1_683_i[id].cdc_fisheye_b_cmrvvclsdrtn = it->closeDuration;
                dataSend683(cdc_control1_683_i[id]);
            }
        }
        break;
    }
    case 10:{
        QVector<TableRowData> allRows = ui->tableWidget->getTableData();
        QString targetValve = "后补盲激光雷达水阀";
        auto it = std::find_if(allRows.begin(), allRows.end(), [&](const TableRowData &d){
            return d.valvePosition == targetValve;
        });
        if (it != allRows.end()) {
            if(it->sprayDuration > 0)
            {
                QThread *t = QThread::create([=](){
                    QThread::msleep(it->sprayDuration + it->cleanDelay);
                    this->on_timerMake0(id);  // 直接运行在子线程
                });
                connect(t, &QThread::finished, t, &QObject::deleteLater);
                t->start();

                Cdccontrol1683_Reset(&cdc_control1_683_i[id]);
                cdc_control1_683_i[id].cdc_relidarvalvepwr = Cdc_control2_683_Cdc_relidarvalvepwrType_CDC_RELIDARVALVEPWR_ON_E2;
                dataSend683(cdc_control1_683_i[id]);

                Delay_MSec(it->sprayDuration);
                Cdccontrol1683_Reset(&cdc_control1_683_i[id]);
                cdc_control1_683_i[id].cdc_relidarvalvepwr = Cdc_control2_683_Cdc_relidarvalvepwrType_CDC_RELIDARVALVEPWR_OFF_E1;
                dataSend683(cdc_control1_683_i[id]);
            }else if(it->cycleCount > 0)
            {
                QThread *t = QThread::create([=](){
                    QThread::msleep((it->closeDuration + it->openDuration)*it->cleanCount + it->cleanDelay);
                    this->on_timerMake0(id);  // 直接运行在子线程
                });
                connect(t, &QThread::finished, t, &QObject::deleteLater);
                t->start();

                Cdccontrol1683_Reset(&cdc_control1_683_i[id]);
                cdc_control1_683_i[id].cdc_relidarvalvepwr = Cdc_control2_683_Cdc_relidarvalvepwrType_CDC_RELIDARVALVEPWR_NO_REQUEST_E0;
                dataSend683(cdc_control1_683_i[id]);

                Cdccontrol1683_Reset(&cdc_control1_683_i[id]);
                cdc_control1_683_i[id].cdc_relidarvalvepwr = Cdc_control2_683_Cdc_relidarvalvepwrType_CDC_RELIDARVALVEPWR_INTERMIT_SPRAY_E3;
                cdc_control1_683_i[id].cdc_relidarvvspraynum = it->cycleCount;
                cdc_control1_683_i[id].cdc_relidarvvopdrtn = it->openDuration;
                cdc_control1_683_i[id].cdc_relidarvvclsdrtn = it->closeDuration;
                dataSend683(cdc_control1_683_i[id]);
            }
        }
        break;
    }
    case 11:{
        QVector<TableRowData> allRows = ui->tableWidget->getTableData();
        QString targetValve = "正后相机电磁阀";
        auto it = std::find_if(allRows.begin(), allRows.end(), [&](const TableRowData &d){
            return d.valvePosition == targetValve;
        });
        if (it != allRows.end()) {
            if(it->sprayDuration > 0)
            {
                QThread *t = QThread::create([=](){
                    QThread::msleep(it->sprayDuration + it->cleanDelay);
                    this->on_timerMake0(id);  // 直接运行在子线程
                });
                connect(t, &QThread::finished, t, &QObject::deleteLater);
                t->start();

                Cdccontrol1683_Reset(&cdc_control1_683_i[id]);
                cdc_control1_683_i[id].cdc_auto_b_cmr_valve_pwr_req = Cdc_control2_683_Cdc_auto_b_cmr_valve_pwr_reqType_CDC_AUTO_B_CMR_VALVE_PWR_REQ_ON_E2;
                dataSend683(cdc_control1_683_i[id]);

                Delay_MSec(it->sprayDuration);
                Cdccontrol1683_Reset(&cdc_control1_683_i[id]);
                cdc_control1_683_i[id].cdc_auto_b_cmr_valve_pwr_req = Cdc_control2_683_Cdc_auto_b_cmr_valve_pwr_reqType_CDC_AUTO_B_CMR_VALVE_PWR_REQ_OFF_E1;
                dataSend683(cdc_control1_683_i[id]);
            }else if(it->cycleCount > 0)
            {
                QThread *t = QThread::create([=](){
                    QThread::msleep((it->closeDuration + it->openDuration)*it->cleanCount + it->cleanDelay);
                    this->on_timerMake0(id);  // 直接运行在子线程
                });
                connect(t, &QThread::finished, t, &QObject::deleteLater);
                t->start();

                Cdccontrol1683_Reset(&cdc_control1_683_i[id]);
                cdc_control1_683_i[id].cdc_auto_b_cmr_valve_pwr_req = Cdc_control2_683_Cdc_auto_b_cmr_valve_pwr_reqType_CDC_AUTO_B_CMR_VALVE_PWR_REQ_NO_REQUEST_E0;
                dataSend683(cdc_control1_683_i[id]);

                Cdccontrol1683_Reset(&cdc_control1_683_i[id]);
                cdc_control1_683_i[id].cdc_auto_b_cmr_valve_pwr_req = Cdc_control2_683_Cdc_auto_b_cmr_valve_pwr_reqType_CDC_AUTO_B_CMR_VALVE_PWR_REQ_INTERMIT_SPRAY_E3;
                cdc_control1_683_i[id].cdc_auto_b_cmrvvspraynum = it->cycleCount;
                cdc_control1_683_i[id].cdc_auto_b_cmrvvopdrtn = it->openDuration;
                cdc_control1_683_i[id].cdc_auto_b_cmrvvclsdrtn = it->closeDuration;
                dataSend683(cdc_control1_683_i[id]);
            }
        }
        break;
    }
    case 12:{
        QVector<TableRowData> allRows = ui->tableWidget->getTableData();
        QString targetValve = "主激光雷达水阀";
        auto it = std::find_if(allRows.begin(), allRows.end(), [&](const TableRowData &d){
            return d.valvePosition == targetValve;
        });
        if (it != allRows.end()) {
            if(it->sprayDuration > 0)
            {
                Cdccontrol1683_Reset(&cdc_control1_683_i[id]);
                cdc_control1_683_i[id].cdc_mlidarvalvepwr = Cdc_control2_683_Cdc_mlidarvalvepwrType_CDC_MLIDARVALVEPWR_ON_E2;
                dataSend683(cdc_control1_683_i[id]);

                Delay_MSec(it->sprayDuration);
                Cdccontrol1683_Reset(&cdc_control1_683_i[id]);
                cdc_control1_683_i[id].cdc_mlidarvalvepwr = Cdc_control2_683_Cdc_mlidarvalvepwrType_CDC_MLIDARVALVEPWR_OFF_E1;
                dataSend683(cdc_control1_683_i[id]);
            }else if(it->cycleCount > 0)
            {
                Cdccontrol1683_Reset(&cdc_control1_683_i[id]);
                cdc_control1_683_i[id].cdc_mlidarvalvepwr = Cdc_control2_683_Cdc_mlidarvalvepwrType_CDC_MLIDARVALVEPWR_NO_REQUEST_E0;
                dataSend683(cdc_control1_683_i[id]);

                Cdccontrol1683_Reset(&cdc_control1_683_i[id]);
                cdc_control1_683_i[id].cdc_mlidarvalvepwr = Cdc_control2_683_Cdc_mlidarvalvepwrType_CDC_MLIDARVALVEPWR_INTERMIT_SPRAY_E3;
                cdc_control1_683_i[id].cdc_mlidarvvspraynum = it->cycleCount;
                cdc_control1_683_i[id].cdc_mlidarvvopdrtn = it->openDuration;
                cdc_control1_683_i[id].cdc_mlidarvvclsdrtn = it->closeDuration;
                dataSend683(cdc_control1_683_i[id]);
            }
        }
        break;
    }
    case 13:{
        QVector<TableRowData> allRows = ui->tableWidget->getTableData();
        QString targetValve = "左前相机电磁阀";
        auto it = std::find_if(allRows.begin(), allRows.end(), [&](const TableRowData &d){
            return d.valvePosition == targetValve;
        });
        if (it != allRows.end()) {
            if(it->sprayDuration > 0)
            {
                QThread *t = QThread::create([=](){
                    QThread::msleep(it->sprayDuration + it->cleanDelay);
                    this->on_timerMake0(id);  // 直接运行在子线程
                });
                connect(t, &QThread::finished, t, &QObject::deleteLater);
                t->start();

                Cdccontrol1683_Reset(&cdc_control1_683_i[id]);
                cdc_control1_683_i[id].cdc_auto_lf_cmr_valve_pwr_req = Cdc_control2_683_Cdc_auto_lf_cmr_valve_pwr_reqType_CDC_AUTO_LF_CMR_VALVE_PWR_REQ_ON_E2;
                dataSend683(cdc_control1_683_i[id]);

                Delay_MSec(it->sprayDuration);
                Cdccontrol1683_Reset(&cdc_control1_683_i[id]);
                cdc_control1_683_i[id].cdc_auto_lf_cmr_valve_pwr_req = Cdc_control2_683_Cdc_auto_lf_cmr_valve_pwr_reqType_CDC_AUTO_LF_CMR_VALVE_PWR_REQ_OFF_E1;
                dataSend683(cdc_control1_683_i[id]);
            }else if(it->cycleCount > 0)
            {
                QThread *t = QThread::create([=](){
                    QThread::msleep((it->closeDuration + it->openDuration)*it->cleanCount + it->cleanDelay);
                    this->on_timerMake0(id);  // 直接运行在子线程
                });
                connect(t, &QThread::finished, t, &QObject::deleteLater);
                t->start();

                Cdccontrol1683_Reset(&cdc_control1_683_i[id]);
                cdc_control1_683_i[id].cdc_auto_lf_cmr_valve_pwr_req = Cdc_control2_683_Cdc_auto_lf_cmr_valve_pwr_reqType_CDC_AUTO_LF_CMR_VALVE_PWR_REQ_NO_REQUEST_E0;
                dataSend683(cdc_control1_683_i[id]);

                Cdccontrol1683_Reset(&cdc_control1_683_i[id]);
                cdc_control1_683_i[id].cdc_auto_lf_cmr_valve_pwr_req = Cdc_control2_683_Cdc_auto_lf_cmr_valve_pwr_reqType_CDC_AUTO_LF_CMR_VALVE_PWR_REQ_INTERMIT_SPRAY_E3;
                cdc_control1_683_i[id].cdc_auto_lf_cmrvvspraynum = it->cycleCount;
                cdc_control1_683_i[id].cdc_auto_lf_cmrvvopdrtn = it->openDuration;
                cdc_control1_683_i[id].cdc_auto_lf_cmrvvclsdrtn = it->closeDuration;
                dataSend683(cdc_control1_683_i[id]);
            }
        }
        break;
    }
    case 14:{
        QVector<TableRowData> allRows = ui->tableWidget->getTableData();
        QString targetValve = "右前相机电磁阀";
        auto it = std::find_if(allRows.begin(), allRows.end(), [&](const TableRowData &d){
            return d.valvePosition == targetValve;
        });
        if (it != allRows.end()) {
            if(it->sprayDuration > 0)
            {
                QThread *t = QThread::create([=](){
                    QThread::msleep(it->sprayDuration + it->cleanDelay);
                    this->on_timerMake0(id);  // 直接运行在子线程
                });
                connect(t, &QThread::finished, t, &QObject::deleteLater);
                t->start();

                Cdccontrol1683_Reset(&cdc_control1_683_i[id]);
                cdc_control1_683_i[id].cdc_auto_rf_cmr_valve_pwr_req = Cdc_control2_683_Cdc_auto_rf_cmr_valve_pwr_reqType_CDC_AUTO_RF_CMR_VALVE_PWR_REQ_ON_E2;
                dataSend683(cdc_control1_683_i[id]);

                Delay_MSec(it->sprayDuration);
                Cdccontrol1683_Reset(&cdc_control1_683_i[id]);
                cdc_control1_683_i[id].cdc_auto_rf_cmr_valve_pwr_req = Cdc_control2_683_Cdc_auto_rf_cmr_valve_pwr_reqType_CDC_AUTO_RF_CMR_VALVE_PWR_REQ_OFF_E1;
                dataSend683(cdc_control1_683_i[id]);
            }else if(it->cycleCount > 0)
            {
                QThread *t = QThread::create([=](){
                    QThread::msleep((it->closeDuration + it->openDuration)*it->cleanCount + it->cleanDelay);
                    this->on_timerMake0(id);  // 直接运行在子线程
                });
                connect(t, &QThread::finished, t, &QObject::deleteLater);
                t->start();

                Cdccontrol1683_Reset(&cdc_control1_683_i[id]);
                cdc_control1_683_i[id].cdc_auto_rf_cmr_valve_pwr_req = Cdc_control2_683_Cdc_auto_rf_cmr_valve_pwr_reqType_CDC_AUTO_RF_CMR_VALVE_PWR_REQ_NO_REQUEST_E0;
                dataSend683(cdc_control1_683_i[id]);

                Cdccontrol1683_Reset(&cdc_control1_683_i[id]);
                cdc_control1_683_i[id].cdc_auto_rf_cmr_valve_pwr_req = Cdc_control2_683_Cdc_auto_rf_cmr_valve_pwr_reqType_CDC_AUTO_RF_CMR_VALVE_PWR_REQ_INTERMIT_SPRAY_E3;
                cdc_control1_683_i[id].cdc_auto_rf_cmrvvspraynum = it->cycleCount;
                cdc_control1_683_i[id].cdc_auto_rf_cmrvvopdrtn = it->openDuration;
                cdc_control1_683_i[id].cdc_auto_rf_cmrvvclsdrtn = it->closeDuration;
                dataSend683(cdc_control1_683_i[id]);
            }
        }
        break;
    }
    case 15:{
        QVector<TableRowData> allRows = ui->tableWidget->getTableData();
        QString targetValve = "左侧广角相机电磁阀";
        auto it = std::find_if(allRows.begin(), allRows.end(), [&](const TableRowData &d){
            return d.valvePosition == targetValve;
        });
        if (it != allRows.end()) {
            if(it->sprayDuration > 0)
            {
                QThread *t = QThread::create([=](){
                    QThread::msleep(it->sprayDuration + it->cleanDelay);
                    this->on_timerMake0(id);  // 直接运行在子线程
                });
                connect(t, &QThread::finished, t, &QObject::deleteLater);
                t->start();

                Cdccontrol1683_Reset(&cdc_control1_683_i[id]);
                cdc_control1_683_i[id].cdc_auto_l_cmr_valve_pwr_req = Cdc_control2_683_Cdc_auto_l_cmr_valve_pwr_reqType_CDC_AUTO_L_CMR_VALVE_PWR_REQ_ON_E2;
                dataSend683(cdc_control1_683_i[id]);

                Delay_MSec(it->sprayDuration);
                Cdccontrol1683_Reset(&cdc_control1_683_i[id]);
                cdc_control1_683_i[id].cdc_auto_l_cmr_valve_pwr_req = Cdc_control2_683_Cdc_auto_l_cmr_valve_pwr_reqType_CDC_AUTO_L_CMR_VALVE_PWR_REQ_OFF_E1;
                dataSend683(cdc_control1_683_i[id]);
            }else if(it->cycleCount > 0)
            {
                QThread *t = QThread::create([=](){
                    QThread::msleep((it->closeDuration + it->openDuration)*it->cleanCount + it->cleanDelay);
                    this->on_timerMake0(id);  // 直接运行在子线程
                });
                connect(t, &QThread::finished, t, &QObject::deleteLater);
                t->start();

                Cdccontrol1683_Reset(&cdc_control1_683_i[id]);
                cdc_control1_683_i[id].cdc_auto_l_cmr_valve_pwr_req = Cdc_control2_683_Cdc_auto_l_cmr_valve_pwr_reqType_CDC_AUTO_L_CMR_VALVE_PWR_REQ_NO_REQUEST_E0;
                dataSend683(cdc_control1_683_i[id]);

                Cdccontrol1683_Reset(&cdc_control1_683_i[id]);
                cdc_control1_683_i[id].cdc_auto_l_cmr_valve_pwr_req = Cdc_control2_683_Cdc_auto_l_cmr_valve_pwr_reqType_CDC_AUTO_L_CMR_VALVE_PWR_REQ_INTERMIT_SPRAY_E3;
                cdc_control1_683_i[id].cdc_auto_l_cmrvvspraynum = it->cycleCount;
                cdc_control1_683_i[id].cdc_auto_l_cmrvvopdrtn = it->openDuration;
                cdc_control1_683_i[id].cdc_auto_l_cmrvvclsdrtn = it->closeDuration;
                dataSend683(cdc_control1_683_i[id]);
            }
        }
        break;
    }
    case 16:{
        QVector<TableRowData> allRows = ui->tableWidget->getTableData();
        QString targetValve = "右侧广角相机电磁阀";
        auto it = std::find_if(allRows.begin(), allRows.end(), [&](const TableRowData &d){
            return d.valvePosition == targetValve;
        });
        if (it != allRows.end()) {
            if(it->sprayDuration > 0)
            {
                QThread *t = QThread::create([=](){
                    QThread::msleep(it->sprayDuration + it->cleanDelay);
                    this->on_timerMake0(id);  // 直接运行在子线程
                });
                connect(t, &QThread::finished, t, &QObject::deleteLater);
                t->start();

                Cdccontrol1683_Reset(&cdc_control1_683_i[id]);
                cdc_control1_683_i[id].cdc_auto_r_cmr_valve_pwr_req = Cdc_control2_683_Cdc_auto_r_cmr_valve_pwr_reqType_CDC_AUTO_R_CMR_VALVE_PWR_REQ_ON_E2;
                dataSend683(cdc_control1_683_i[id]);

                Delay_MSec(it->sprayDuration);
                Cdccontrol1683_Reset(&cdc_control1_683_i[id]);
                cdc_control1_683_i[id].cdc_auto_r_cmr_valve_pwr_req = Cdc_control2_683_Cdc_auto_r_cmr_valve_pwr_reqType_CDC_AUTO_R_CMR_VALVE_PWR_REQ_OFF_E1;
                dataSend683(cdc_control1_683_i[id]);
            }else if(it->cycleCount > 0)
            {
                QThread *t = QThread::create([=](){
                    QThread::msleep((it->closeDuration + it->openDuration)*it->cleanCount + it->cleanDelay);
                    this->on_timerMake0(id);  // 直接运行在子线程
                });
                connect(t, &QThread::finished, t, &QObject::deleteLater);
                t->start();

                Cdccontrol1683_Reset(&cdc_control1_683_i[id]);
                cdc_control1_683_i[id].cdc_auto_r_cmr_valve_pwr_req = Cdc_control2_683_Cdc_auto_r_cmr_valve_pwr_reqType_CDC_AUTO_R_CMR_VALVE_PWR_REQ_NO_REQUEST_E0;
                dataSend683(cdc_control1_683_i[id]);

                Cdccontrol1683_Reset(&cdc_control1_683_i[id]);
                cdc_control1_683_i[id].cdc_auto_r_cmr_valve_pwr_req = Cdc_control2_683_Cdc_auto_r_cmr_valve_pwr_reqType_CDC_AUTO_R_CMR_VALVE_PWR_REQ_INTERMIT_SPRAY_E3;
                cdc_control1_683_i[id].cdc_auto_r_cmrvvspraynum = it->cycleCount;
                cdc_control1_683_i[id].cdc_auto_r_cmrvvopdrtn = it->openDuration;
                cdc_control1_683_i[id].cdc_auto_r_cmrvvclsdrtn = it->closeDuration;
                dataSend683(cdc_control1_683_i[id]);
            }
        }
        break;
    }
    case 17:{
        QVector<TableRowData> allRows = ui->tableWidget->getTableData();
        QString targetValve = "前底盘电磁阀";
        auto it = std::find_if(allRows.begin(), allRows.end(), [&](const TableRowData &d){
            return d.valvePosition == targetValve;
        });
        if (it != allRows.end()) {
            if(it->sprayDuration > 0)
            {
//                Cdccontrol1683_Reset(&cdc_control1_683_i[id]);
//                cdc_control1_680_i[id].cdc_frchassisvvctrl = Cdc_control1_680_Cdc_frchassisvvctrlType_CDC_FRCHASSISVVCTRL_OPEN_E2;
//                cdc_control1_680_i[id].cdc_frchassisctrl = Cdc_control1_680_Cdc_frchassisctrlType_CDC_FRCHASSISCTRL_MOVE_E1;
//                cdc_control1_680_i[id].cdc_frchassistgtctrl = 50;
//                dataSend(cdc_control1_680_i[id]);

//                Delay_MSec(it->sprayDuration);
//                Cdccontrol1683_Reset(&cdc_control1_683_i[id]);
//                cdc_control1_680_i[id].cdc_frchassisvvctrl = Cdc_control1_680_Cdc_frchassisvvctrlType_CDC_FRCHASSISVVCTRL_CLOSE_E1;
//                cdc_control1_680_i[id].cdc_frchassisctrl = Cdc_control1_680_Cdc_frchassisctrlType_CDC_FRCHASSISCTRL_MOVE_E1;
//                cdc_control1_680_i[id].cdc_frchassistgtctrl = 0;
//                dataSend(cdc_control1_680_i[id]);

                Cdccontrol1683_Reset(&cdc_control1_683_i[id]);
                cdc_control1_683_i[id].cdc_frchassisvvctrl = Cdc_control2_683_Cdc_frchassisvvctrlType_CDC_FRCHASSISVVCTRL_ON_E2;
                dataSend683(cdc_control1_683_i[id]);

                Delay_MSec(it->sprayDuration);
                Cdccontrol1683_Reset(&cdc_control1_683_i[id]);
                cdc_control1_683_i[id].cdc_frchassisvvctrl = Cdc_control2_683_Cdc_frchassisvvctrlType_CDC_FRCHASSISVVCTRL_OFF_E1;
                dataSend683(cdc_control1_683_i[id]);
            }else if(it->cycleCount > 0)
            {
//                cdc_control1_680_i[id].cdc_frchassisctrl = Cdc_control1_680_Cdc_frchassisctrlType_CDC_FRCHASSISCTRL_MOVE_E1;
//                cdc_control1_680_i[id].cdc_frchassistgtctrl = 50;
//                dataSend(cdc_control1_680_i[id]);
//                for (int i = 0; i < it->cycleCount; i++) {

//                    Cdccontrol1683_Reset(&cdc_control1_683_i[id]);
//                    cdc_control1_680_i[id].cdc_frchassisvvctrl = Cdc_control1_680_Cdc_frchassisvvctrlType_CDC_FRCHASSISVVCTRL_OPEN_E2;
//                    dataSend(cdc_control1_680_i[id]);

//                    Delay_MSec(it->openDuration);
//                    Cdccontrol1683_Reset(&cdc_control1_683_i[id]);
//                    cdc_control1_680_i[id].cdc_frchassisvvctrl = Cdc_control1_680_Cdc_frchassisvvctrlType_CDC_FRCHASSISVVCTRL_CLOSE_E1;
//                    dataSend(cdc_control1_680_i[id]);
//                    Delay_MSec(it->closeDuration);
//                }
//                cdc_control1_680_i[id].cdc_frchassisctrl = Cdc_control1_680_Cdc_frchassisctrlType_CDC_FRCHASSISCTRL_MOVE_E1;
//                cdc_control1_680_i[id].cdc_frchassistgtctrl = 0;
//                dataSend(cdc_control1_680_i[id]);
                Cdccontrol1683_Reset(&cdc_control1_683_i[id]);
                cdc_control1_683_i[id].cdc_frchassisvvctrl = Cdc_control2_683_Cdc_frchassisvvctrlType_CDC_FRCHASSISVVCTRL_NO_REQUEST_E0;
                dataSend683(cdc_control1_683_i[id]);

                Cdccontrol1683_Reset(&cdc_control1_683_i[id]);
                cdc_control1_683_i[id].cdc_frchassisvvctrl = Cdc_control2_683_Cdc_frchassisvvctrlType_CDC_FRCHASSISVVCTRL_INTERMIT_SPRAY_E3;
                cdc_control1_683_i[id].cdc_frchassisvvspraynum = it->cycleCount;
                cdc_control1_683_i[id].cdc_frchassisvvopdrtn = it->openDuration;
                cdc_control1_683_i[id].cdc_frchassisvvclsdrtn = it->closeDuration;
                dataSend683(cdc_control1_683_i[id]);
            }
        }
        break;
    }
    case 18:{
        QVector<TableRowData> allRows = ui->tableWidget->getTableData();
        QString targetValve = "中底盘电磁阀";
        auto it = std::find_if(allRows.begin(), allRows.end(), [&](const TableRowData &d){
            return d.valvePosition == targetValve;
        });
        if (it != allRows.end()) {
            if(it->sprayDuration > 0)
            {
//                Cdccontrol1683_Reset(&cdc_control1_683_i[id]);
//                cdc_control1_680_i[id].cdc_midchassisvvctrl = Cdc_control1_680_Cdc_midchassisvvctrlType_CDC_MIDCHASSISVVCTRL_OPEN_E2;
//                cdc_control1_680_i[id].cdc_midchassisctrl = Cdc_control1_680_Cdc_midchassisctrlType_CDC_MIDCHASSISCTRL_MOVE_E1;
//                cdc_control1_680_i[id].cdc_midchassistgtctrl = 50;
//                dataSend(cdc_control1_680_i[id]);

//                Delay_MSec(it->sprayDuration);
//                Cdccontrol1683_Reset(&cdc_control1_683_i[id]);
//                cdc_control1_680_i[id].cdc_midchassisvvctrl = Cdc_control1_680_Cdc_midchassisvvctrlType_CDC_MIDCHASSISVVCTRL_CLOSE_E1;
//                cdc_control1_680_i[id].cdc_midchassisctrl = Cdc_control1_680_Cdc_midchassisctrlType_CDC_MIDCHASSISCTRL_MOVE_E1;
//                cdc_control1_680_i[id].cdc_midchassistgtctrl = 0;
//                dataSend(cdc_control1_680_i[id]);
                Cdccontrol1683_Reset(&cdc_control1_683_i[id]);
                cdc_control1_683_i[id].cdc_midchassisvvctrl = Cdc_control2_683_Cdc_midchassisvvctrlType_CDC_MIDCHASSISVVCTRL_ON_E2;
                dataSend683(cdc_control1_683_i[id]);

                Delay_MSec(it->sprayDuration);
                Cdccontrol1683_Reset(&cdc_control1_683_i[id]);
                cdc_control1_683_i[id].cdc_midchassisvvctrl = Cdc_control2_683_Cdc_midchassisvvctrlType_CDC_MIDCHASSISVVCTRL_OFF_E1;
                dataSend683(cdc_control1_683_i[id]);
            }else if(it->cycleCount > 0)
            {
//                cdc_control1_680_i[id].cdc_midchassisctrl = Cdc_control1_680_Cdc_midchassisctrlType_CDC_MIDCHASSISCTRL_MOVE_E1;
//                cdc_control1_680_i[id].cdc_midchassistgtctrl = 50;
//                dataSend(cdc_control1_680_i[id]);
//                for (int i = 0; i < it->cycleCount; i++) {

//                    Cdccontrol1683_Reset(&cdc_control1_683_i[id]);
//                    cdc_control1_680_i[id].cdc_midchassisvvctrl = Cdc_control1_680_Cdc_midchassisvvctrlType_CDC_MIDCHASSISVVCTRL_OPEN_E2;
//                    dataSend(cdc_control1_680_i[id]);

//                    Delay_MSec(it->openDuration);
//                    Cdccontrol1683_Reset(&cdc_control1_683_i[id]);
//                    cdc_control1_680_i[id].cdc_midchassisvvctrl = Cdc_control1_680_Cdc_midchassisvvctrlType_CDC_MIDCHASSISVVCTRL_CLOSE_E1;
//                    dataSend(cdc_control1_680_i[id]);
//                    Delay_MSec(it->closeDuration);
//                }
//                cdc_control1_680_i[id].cdc_midchassisctrl = Cdc_control1_680_Cdc_midchassisctrlType_CDC_MIDCHASSISCTRL_MOVE_E1;
//                cdc_control1_680_i[id].cdc_midchassistgtctrl = 0;
//                dataSend(cdc_control1_680_i[id]);
                Cdccontrol1683_Reset(&cdc_control1_683_i[id]);
                cdc_control1_683_i[id].cdc_midchassisvvctrl = Cdc_control2_683_Cdc_midchassisvvctrlType_CDC_MIDCHASSISVVCTRL_NO_REQUEST_E0;
                dataSend683(cdc_control1_683_i[id]);

                Cdccontrol1683_Reset(&cdc_control1_683_i[id]);
                cdc_control1_683_i[id].cdc_midchassisvvctrl = Cdc_control2_683_Cdc_midchassisvvctrlType_CDC_MIDCHASSISVVCTRL_INTERMIT_SPRAY_E3;
                cdc_control1_683_i[id].cdc_midchassisvvspraynum = it->cycleCount;
                cdc_control1_683_i[id].cdc_midchassisvvopdrtn = it->openDuration;
                cdc_control1_683_i[id].cdc_midchassisvvclsdrtn = it->closeDuration;
                dataSend683(cdc_control1_683_i[id]);
            }
        }
        break;
    }
    case 19:{
        QVector<TableRowData> allRows = ui->tableWidget->getTableData();
        QString targetValve = "后底盘电磁阀";
        auto it = std::find_if(allRows.begin(), allRows.end(), [&](const TableRowData &d){
            return d.valvePosition == targetValve;
        });
        if (it != allRows.end()) {
            if(it->sprayDuration > 0)
            {
//                Cdccontrol1683_Reset(&cdc_control1_683_i[id]);
//                cdc_control1_680_i[id].cdc_rechassisvvctrl = Cdc_control1_680_Cdc_rechassisvvctrlType_CDC_RECHASSISVVCTRL_OPEN_E2;
//                cdc_control1_680_i[id].cdc_rechassisctrl = Cdc_control1_680_Cdc_rechassisctrlType_CDC_RECHASSISCTRL_MOVE_E1;
//                cdc_control1_680_i[id].cdc_rechassistgtctrl = 50;
//                dataSend(cdc_control1_680_i[id]);

//                Delay_MSec(it->sprayDuration);
//                Cdccontrol1683_Reset(&cdc_control1_683_i[id]);
//                cdc_control1_680_i[id].cdc_rechassisvvctrl = Cdc_control1_680_Cdc_rechassisvvctrlType_CDC_RECHASSISVVCTRL_CLOSE_E1;
//                cdc_control1_680_i[id].cdc_rechassisctrl = Cdc_control1_680_Cdc_rechassisctrlType_CDC_RECHASSISCTRL_MOVE_E1;
//                cdc_control1_680_i[id].cdc_rechassistgtctrl = 0;
//                dataSend(cdc_control1_680_i[id]);
                Cdccontrol1683_Reset(&cdc_control1_683_i[id]);
                cdc_control1_683_i[id].cdc_rechassisvvctrl = Cdc_control2_683_Cdc_rechassisvvctrlType_CDC_RECHASSISVVCTRL_ON_E2;
                dataSend683(cdc_control1_683_i[id]);

                Delay_MSec(it->sprayDuration);
                Cdccontrol1683_Reset(&cdc_control1_683_i[id]);
                cdc_control1_683_i[id].cdc_rechassisvvctrl = Cdc_control2_683_Cdc_rechassisvvctrlType_CDC_RECHASSISVVCTRL_OFF_E1;
                dataSend683(cdc_control1_683_i[id]);
            }else if(it->cycleCount > 0)
            {
//                cdc_control1_680_i[id].cdc_rechassisctrl = Cdc_control1_680_Cdc_rechassisctrlType_CDC_RECHASSISCTRL_MOVE_E1;
//                cdc_control1_680_i[id].cdc_rechassistgtctrl = 50;
//                dataSend(cdc_control1_680_i[id]);
//                for (int i = 0; i < it->cycleCount; i++) {

//                    Cdccontrol1683_Reset(&cdc_control1_683_i[id]);
//                    cdc_control1_680_i[id].cdc_rechassisvvctrl = Cdc_control1_680_Cdc_rechassisvvctrlType_CDC_RECHASSISVVCTRL_OPEN_E2;
//                    dataSend(cdc_control1_680_i[id]);

//                    Delay_MSec(it->openDuration);
//                    Cdccontrol1683_Reset(&cdc_control1_683_i[id]);
//                    cdc_control1_680_i[id].cdc_rechassisvvctrl = Cdc_control1_680_Cdc_rechassisvvctrlType_CDC_RECHASSISVVCTRL_CLOSE_E1;
//                    dataSend(cdc_control1_680_i[id]);
//                    Delay_MSec(it->closeDuration);
//                }
//                cdc_control1_680_i[id].cdc_rechassisctrl = Cdc_control1_680_Cdc_rechassisctrlType_CDC_RECHASSISCTRL_MOVE_E1;
//                cdc_control1_680_i[id].cdc_rechassistgtctrl = 0;
//                dataSend(cdc_control1_680_i[id]);
                Cdccontrol1683_Reset(&cdc_control1_683_i[id]);
                cdc_control1_683_i[id].cdc_rechassisvvctrl = Cdc_control2_683_Cdc_rechassisvvctrlType_CDC_RECHASSISVVCTRL_NO_REQUEST_E0;
                dataSend683(cdc_control1_683_i[id]);

                Cdccontrol1683_Reset(&cdc_control1_683_i[id]);
                cdc_control1_683_i[id].cdc_rechassisvvctrl = Cdc_control2_683_Cdc_rechassisvvctrlType_CDC_RECHASSISVVCTRL_INTERMIT_SPRAY_E3;
                cdc_control1_683_i[id].cdc_rechassisvvspraynum = it->cycleCount;
                cdc_control1_683_i[id].cdc_rechassisvvopdrtn = it->openDuration;
                cdc_control1_683_i[id].cdc_rechassisvvclsdrtn = it->closeDuration;
                dataSend683(cdc_control1_683_i[id]);
            }
        }
        break;
    }
    default: break;

    }
}

void MainWindow::on_makeButton_40_clicked()
{
    Cdc15443_Reset(&cdc_15_443_i);
    QByteArray byteArray;
    Cdccontrol1683_Reset(&cdc_control1_683);
    Cdccontrol1680_Reset(&cdc_control1_680);

    cdc_control1_680.cdc_cleanpumpecupwr = Cdc_control1_680_Cdc_cleanpumpecupwrType_CDC_CLEANPUMPECUPWR_OPEN_E2;
    cdc_control1_680.cdc_cleanpumpspd = ui->pressureEdit_17->text().toDouble();
    Cdccontrol1680_UpdateData(&cdc_control1_680);
    byteArray = QByteArray(Data, 48);
    canComm->sendMessage(0x680, byteArray);
    Delay_MSec(ui->pressureEdit_18->text().toInt());

    const int taskCount = 20;
    QThreadPool pool;
    pool.setMaxThreadCount(taskCount);

    QAtomicInt finishedCount(0);
    QEventLoop loop;

    // 提交20个任务
    for (int i = 0; i < taskCount; ++i) {
        pool.start(new WorkerTask(i, [&](int id){
            myTaskFunction(id);

            // 完成计数
            finishedCount.fetchAndAddRelaxed(1);
            if (finishedCount.load() >= taskCount) {
                loop.quit(); // 所有任务完成
            }
        }));
    }

    loop.exec();

    QThread::msleep(500);
    Cdccontrol1680_Reset(&cdc_control1_680);
    cdc_control1_680.cdc_cleanpumpecupwr = Cdc_control1_680_Cdc_cleanpumpecupwrType_CDC_CLEANPUMPECUPWR_CLOSE_E1;
    cdc_control1_680.cdc_cleanpumpspd = 0.0;
    Cdccontrol1680_UpdateData(&cdc_control1_680);
    byteArray = QByteArray(Data, 48);
    canComm->sendMessage(0x680, byteArray);

    qDebug() << "All tasks finished";

    return ;
}

void MainWindow::on_makeButton_39_clicked()
{
    QThread::usleep(100);
    QByteArray byteArray;
    Cdccontrol1680_Reset(&cdc_control1_680);
    cdc_control1_680.cdc_cleanpumpecupwr = Cdc_control1_680_Cdc_cleanpumpecupwrType_CDC_CLEANPUMPECUPWR_CLOSE_E1;
    cdc_control1_680.cdc_cleanpumpspd = 0.0;
    Cdccontrol1680_UpdateData(&cdc_control1_680);
    byteArray = QByteArray(Data, 48);
    canComm->sendMessage(0x680, byteArray);
}

void MainWindow::on_makeButton_38_clicked()
{
    ui->tableWidget->setTableDataToZero();
    memset(cdc_control1_680_i, 0, sizeof(cdc_control1_680_i));
    memset(cdc_control1_683_i, 0, sizeof(cdc_control1_683_i));

//    memset(cdc_15_443_i, 0, sizeof(cdc_15_443_i));
}

void MainWindow::on_makeButton_13_clicked()
{
    QByteArray dockCmd = buildLidarDockCommand(1);
    canComm->enqueueMessage(0x470, dockCmd);
}

void MainWindow::on_makeButton_14_clicked()
{
    QByteArray dockCmd = buildLidarDockCommand(2);
    canComm->enqueueMessage(0x470, dockCmd);
}

void MainWindow::on_horizontalSlider_sliderMoved(int position)
{
//    qDebug() << position;
    QByteArray message(8, 0);  // 初始化 8 字节报文

    // CDC_DSMACtrlReq = 1
    message[6] = static_cast<char>( static_cast<quint8>(message[6]) | (1 << 7) );

    // CDC_DSMACtrlTrgPos = position (7 bits)
    int pos = position & 0x7F;
    message[7] = static_cast<char>( static_cast<quint8>(message[7]) | pos );

    canComm->sendMessage(0x608, message);
}

void MainWindow::on_makeButton_34_clicked()
{
    //    qDebug() << position;
        QByteArray message(8, 0);  // 初始化 8 字节报文

        //CDC_StrwhlSelfLearnReq = 1
//        SG_ CDC_StrwhlSelfLearnReq : 18|1@1+ (1,0) [0|1] "" VDU,VDU_CCM
        message[2] = static_cast<char>(static_cast<quint8>(message[2]) | (1 << 2));

        // CDC_DSMACtrlReq = 1
        message[6] = static_cast<char>( static_cast<quint8>(message[6]) | (1 << 7) );

        // CDC_DSMACtrlTrgPos = position (7 bits)
        int pos = ui->pressureEdit_25->text().toInt() & 0x7F;
        message[7] = static_cast<char>( static_cast<quint8>(message[7]) | pos );

        canComm->sendMessage(0x608, message);
}

void MainWindow::on_makeButton_6_clicked()
{
    data470[2] = 0;

    if(ui->pressureEdit_7->text().toUInt() == 7)
    {
        quint32 value = 1 & 0x07;
        data470[2] = data470[2] | (value << 0);   // byte2 bit0~bit2
    }
    quint32 value1 = ui->pressureEdit_7->text().toUInt() & 0x07;
    data470[2] = data470[2] |(value1 << 5);   // byte2 bit5~bit7（bit21~23）

    quint32 value3 = ui->pressureEdit_8->text().toUInt() & 0xFF;
    data470[3] = static_cast<char>(value3);   // byte3 直接放 8bit

    canComm->enqueueMessage(0x470, data470);
    qDebug()<< data470.toHex();
}

void MainWindow::on_makeButton_29_clicked()
{
    if(ui->pressureEdit_7->text().toUInt() == 7)
    {
        quint32 value = 3 & 0x07;
        data470[2] = data470[2] | (value << 0);   // byte2 bit0~bit2

        canComm->enqueueMessage(0x470, data470);
        qDebug()<< data470.toHex();
    }
}

//DBC@0
QByteArray MainWindow::buildCanFrameBySignal(int startBit, int bitLength, const QByteArray &data, int frameLen)
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

//QByteArray udsData;
//udsData << char(0x03)
//        << char(0x22)
//        << char(0xF1)
//        << char(0x95);

//QByteArray frame = buildCanFrameBySignal(
//    7,          // Fr_bLid_ReqDiagData 起始位
//    8 * 8,      // 8 字节信号（DBC 里 BO_ 长度是 8）
//    udsData,
//    8           // 报文长度（BO_ 1747 : 8）
//);



void MainWindow::on_versionButtun_clicked()
{
    // 清空 listView
    auto model = ui->listView_2->model();

    if (model && model->rowCount() > 0)
    {
        model->removeRows(0, model->rowCount());
    }

    // 定义请求 CAN ID 和对应的响应 CAN ID
    struct UdsReqResp
    {
        quint32 reqId;
        quint32 respId;
        QString name;
    };

    QVector<UdsReqResp> udsList = {
        {1744, 1752, "LF_pCam"},
        {1745, 1753, "Ri_bLid"},
        {1746, 1754, "Le_bLid"},
        {1747, 1755, "Fr_bLid"},
        {1748, 1756, "FR_CAM_LM"}
    };

    // 保存发送的请求 CAN ID -> 名字映射，方便接收响应时匹配
    m_pendingUds.clear();
    for (const auto &item : udsList)
    {
        m_pendingUds[item.respId] = item.name;

        // 构造请求 UDS 数据：22 F1 95
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

        // 发送请求
        canComm->sendMessageCan(item.reqId, frame);
    }
}


void MainWindow::onCanFrameReceived(quint32 canId, const QByteArray &data)
{
    // 只处理在待处理列表里的响应 CAN ID
    if (!m_pendingUds.contains(canId))
        return;

    QString name = m_pendingUds[canId];

    // 使用之前封装好的解析函数
    UdsParseResult res = parseUdsSingleFrame(data);

    QString versionStr;
    if (res.ok)
    {
        versionStr = QString("%1 : %2").arg(name).arg(res.version);
    }
    else
    {
        versionStr = QString("%1 : %2").arg(name).arg(res.error);
    }

    // 添加到 listView
    QStandardItemModel *model = qobject_cast<QStandardItemModel*>(ui->listView_2->model());
    if (model)
    {
        QStandardItem *item = new QStandardItem(versionStr);
        model->appendRow(item);
    }

    // 收到响应后，可以从待处理列表删除
    m_pendingUds.remove(canId);
}

void MainWindow::initTableView()
{
    assemblyModel =
            new QStandardItemModel(this);

    stepModel =
            new QStandardItemModel(this);

    manualModel =
            new QStandardItemModel(this);

    ui->tableViewAssembly
            ->setModel(assemblyModel);

    ui->tableViewStep
            ->setModel(stepModel);

    ui->tableViewManual
            ->setModel(manualModel);

    assemblyModel
            ->setHorizontalHeaderLabels(
                {"Assembly"});

    stepModel
            ->setHorizontalHeaderLabels(
                {"Step",
                 "Test Item",
                 "Result"});

    manualModel
            ->setHorizontalHeaderLabels(
                {"Step",
                 "Phenomenon",
                 "Manual Result"});

    ui->tableViewAssembly
            ->horizontalHeader()
            ->setStretchLastSection(true);

    ui->tableViewStep
            ->horizontalHeader()
            ->setStretchLastSection(false);

    ui->tableViewManual
            ->horizontalHeader()
            ->setStretchLastSection(false);

    ui->tableViewAssembly
            ->verticalHeader()
            ->hide();

    ui->tableViewStep
            ->verticalHeader()
            ->hide();

    ui->tableViewManual
            ->verticalHeader()
            ->hide();

    ui->tableViewAssembly
            ->setSelectionBehavior(
                QAbstractItemView::SelectRows);

    ui->tableViewAssembly
            ->setEditTriggers(
                QAbstractItemView::NoEditTriggers);

    ui->tableViewStep->setModel(stepModel);

    ui->tableViewStep->horizontalHeader()
            ->setSectionResizeMode(0, QHeaderView::Fixed);

    ui->tableViewStep->horizontalHeader()
            ->setSectionResizeMode(1, QHeaderView::Stretch);

    ui->tableViewStep->horizontalHeader()
            ->setSectionResizeMode(2, QHeaderView::Fixed);

    ui->tableViewStep->setColumnWidth(0, 5);
    ui->tableViewStep->setColumnWidth(2, 65);

    ui->tableViewManual->horizontalHeader()
            ->setSectionResizeMode(0, QHeaderView::Fixed);

    ui->tableViewManual->horizontalHeader()
            ->setSectionResizeMode(1, QHeaderView::Stretch);

    ui->tableViewManual->horizontalHeader()
            ->setSectionResizeMode(2, QHeaderView::Fixed);

    ui->tableViewManual->setColumnWidth(0, 5);
    ui->tableViewManual->setColumnWidth(2, 110);

    ui->tableViewManual->setItemDelegateForColumn(
        2,
        new ComboBoxDelegate(this));
    ui->tableViewManual->setItemDelegateForColumn(
        1,
        new ComboBoxDele(this));
}

void MainWindow::initProcess()
{
    QFile file("process.json");

    if(!file.open(QIODevice::ReadOnly))
    {
        qDebug()<<"open process.json failed";
        return;
    }

    QJsonParseError err;

    QJsonDocument doc =
            QJsonDocument::fromJson(
                file.readAll(),
                &err);

    if(err.error != QJsonParseError::NoError)
    {
        qDebug()<<"json error"<<err.errorString();
        return;
    }

    processMap.clear();

    assemblyModel->removeRows(
                0,
                assemblyModel->rowCount());

    QJsonArray assemblies =
            doc.object()["assemblies"]
            .toArray();

    for(auto assemblyVal : assemblies)
    {
        QJsonObject assemblyObj =
                assemblyVal.toObject();

        QString assemblyName =
                assemblyObj["name"]
                .toString();

        QList<ProcessStep> steps;

        for(auto stepVal :
            assemblyObj["steps"].toArray())
        {
            QJsonObject stepObj =
                    stepVal.toObject();

            ProcessStep step;

            step.id =
                    stepObj["id"].toInt();

            step.name =
                    stepObj["name"].toString();

            step.type =
                    stepObj["type"].toString();

            step.expect =
                    stepObj["expect"].toObject();

            //--------------------------------
            // cmds
            //--------------------------------

            QJsonArray cmdArr =
                    stepObj["cmds"].toArray();

            for(auto cmdVal : cmdArr)
            {
                QJsonObject cmdObj =
                        cmdVal.toObject();

                CanCmd cmd;

                cmd.action =
                        cmdObj["action"].toString();

                cmd.bus =
                        cmdObj["bus"].toString();

                cmd.id =
                        cmdObj["id"].toString();

                cmd.ms =
                        cmdObj["ms"].toInt();

                cmd.shell =
                        cmdObj["shell"].toString();

                cmd.signalMap =
                        cmdObj["signals"]
                        .toObject();

                step.cmds.append(cmd);
            }

            steps.append(step);
        }

        processMap.insert(
                    assemblyName,
                    steps);

        assemblyModel->appendRow(
                    new QStandardItem(
                        assemblyName));
    }

    qDebug()
            <<"process loaded:"
            <<processMap.size();
}

void MainWindow::slotAssemblyClicked(
        const QModelIndex &index)
{
    if(!index.isValid())
        return;

    assemblyName =
            assemblyModel->data(index).toString();

    if(!processMap.contains(assemblyName))
    {
        qDebug() << "[ERROR] missing assembly:"
                 << assemblyName;
        return;
    }

    const QList<ProcessStep> &steps =
            processMap.value(assemblyName);

    //----------------------------------
    // Step Table
    //----------------------------------

    stepModel->removeRows(
                0,
                stepModel->rowCount());

    //----------------------------------
    // Manual Table
    //----------------------------------

    manualModel->removeRows(
                0,
                manualModel->rowCount());

    for(const ProcessStep &step : steps)
    {
        //----------------------------------
        // Step Table
        //----------------------------------

        QList<QStandardItem*> stepRow;

        stepRow
                << new QStandardItem(
                       QString::number(step.id))
                << new QStandardItem(
                       step.name)
                << new QStandardItem(
                       step.result);

        stepModel->appendRow(stepRow);

        //----------------------------------
        // Manual Table
        //----------------------------------

        QList<QStandardItem*> manualRow;

        manualRow
                << new QStandardItem(
                       QString::number(step.id));

        manualModel->appendRow(manualRow);
    }

    qDebug()
            << "[UI] load assembly:"
            << assemblyName
            << "steps:"
            << steps.size();
}

void MainWindow::on_startTemp_clicked()
{
    ui->startTemp->setStyleSheet("background-color: green;");
    ui->startTemp->repaint();
    if(assemblyName == "Front blind spot filling assembly" || assemblyName == "Truck lid blind spot filling assembly" || assemblyName == "Left blind spot filling assembly" || assemblyName == "Right blind spot filling assembly")
    {
        QByteArray dockCmd = buildLidarDockCommand(1);
        executor->canCommPro->enqueueMessage(0x470, dockCmd);
        QThread::sleep(10);
    }

    emit startAssembly(assemblyName);

    qDebug() << "executor =" << executor;
    qDebug() << "mainwindow thread =" << QThread::currentThread();
}

void MainWindow::on_stopTemp_clicked()
{
    executor->stop();
    ui->startTemp->setStyleSheet("background-color: white;");
}

void MainWindow::onStepFinished(int stepId, const QString &name, const QString &result)
{
    int row_i = 0;
    for(int i = 0; i < stepModel->rowCount(); i++)
    {
        qDebug() << result;
        auto idItem = stepModel->item(i, 0);
        if(!idItem) continue;

        if(idItem->text().toInt() == stepId)
        {
            auto resultItem = new QStandardItem(result);

            if(result == "SUBMIT")
                resultItem->setBackground(Qt::green);
            else if(result == "FAIL")
                resultItem->setBackground(Qt::red);
            else if(result == "SKIP")
                resultItem->setBackground(Qt::yellow);

            stepModel->setItem(i, 2, resultItem);
            row_i = i;
            break;
        }
    }

    if(row_i == (stepModel->rowCount() - 1)){
        ui->startTemp->setStyleSheet("background-color: white;");
    }
}

void MainWindow::on_pcanConnect_clicked()
{
    if(executor->canCommPro->openConnection(true))
    {
        ui->pcanConnect->setStyleSheet("background-color: green;");

        //================ ESP_1A =================
        QByteArray data50_lidar(48, 0);
        data50_lidar[4] = 0x01;    // ESP_ABSVehSpdVld = 1
        executor->canCommPro->sendMessage2(0x32, data50_lidar);
        //================ DCU_FL_1 =================
        QByteArray data780_lidar(32, 0);
        data780_lidar[8] = 0x08;   // DCU_FL_DoorSts = 1
        executor->canCommPro->sendMessage2(0x30c, data780_lidar);
        //================ DKM_1 =================
        QByteArray data1029_lidar(32, 0);
        data1029_lidar[2] = 0x01;        // DKM_CardDetectArea = 1
        executor->canCommPro->sendMessage2(0x405, data1029_lidar);

        executor->canCommPro->startReceiveThread();
        executor->canCommPro->setPaused(true);

//        espTimer->start(10);
//        dcuTimer->start(20);
//        dkmTimer->start(50);

//        espTimer->stop();
//        dcuTimer->stop();
//        dkmTimer->stop();
    }else{
        ui->pcanConnect->setStyleSheet("background-color: red;");

    }
}

void MainWindow::on_MEstopButton_10_clicked()
{
    // 点击按钮先变绿色
    ui->MEstopButton_10->setStyleSheet("background-color: green;");
    ui->MEstopButton_10->repaint();
    QThread::sleep(1);

    QString dirPath = QDir::currentPath() + "/eolTest";
    QDir dir;

    // 创建目录
    if (!dir.exists(dirPath) && !dir.mkpath(dirPath))
    {
        ui->MEstopButton_10->setStyleSheet("background-color: red;");
        return;
    }

    QString sn = ui->lineEdit->text().trimmed();
    QString time = QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss");

    // 文件名时间不要带 : ，Windows 不允许
    QString fileTime = QDateTime::currentDateTime().toString("yyyyMMdd_HHmmss");

    QString fileName = QString("%1/%2_%3.txt")
            .arg(dirPath)
            .arg(sn)
            .arg(fileTime);

    QFile file(fileName);

    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        ui->MEstopButton_10->setStyleSheet("background-color: red;");
        return;
    }

    QTextStream out(&file);

    out << "SN: " << sn << "\n";
    out << "Time: " << time << "\n";

    QString assembly;
    QModelIndex idx = ui->tableViewAssembly->currentIndex();
    if (idx.isValid())
        assembly = assemblyModel->item(idx.row(),0)->text();

    out << "Assembly: " << assembly << "\n";
    out << "Step|Item|Result|MStep|Phenomenon|MResult\n";

    int rows = stepModel->rowCount();

    auto get = [](QStandardItemModel *m, int r, int c)->QString
    {
        auto item = m->item(r,c);
        return item ? item->text() : "";
    };

    for (int i = 0; i < rows; ++i)
    {
        out << get(stepModel,i,0) << "|"
            << get(stepModel,i,1) << "|"
            << get(stepModel,i,2) << "|"
            << get(manualModel,i,0) << "|"
            << get(manualModel,i,1) << "|"
            << get(manualModel,i,2)
            << "\n";
    }

    file.close();

    // 保存成功恢复白色
    ui->MEstopButton_10->setStyleSheet("");
}

void MainWindow::on_palyback_clicked()
{
    QString dirPath = QDir::currentPath() + "/eolTest";
    QDir dir(dirPath);

    QString sn = ui->lineEdit->text().trimmed();

    // 找到所有该SN对应的文件
    QStringList files = dir.entryList(
                QStringList() << QString("%1_*.txt").arg(sn),
                QDir::Files,
                QDir::Name);      // 文件名带时间，按名字排序即可

    if (files.isEmpty())
        return;

    // 最新文件
    QString latestFile = dir.absoluteFilePath(files.last());

    QFile file(latestFile);

    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return;

    stepModel->removeRows(0, stepModel->rowCount());
    manualModel->removeRows(0, manualModel->rowCount());

    QTextStream in(&file);

    while (!in.atEnd())
    {
        QString line = in.readLine().trimmed();

        if (line.isEmpty())
            continue;

        // 跳过头信息
        if (line.startsWith("SN:"))
            continue;

        if (line.startsWith("Time:"))
            continue;

        if (line.startsWith("Assembly:"))
            continue;

        if (line.startsWith("Step|"))
            continue;

        QStringList p = line.split("|");

        if (p.size() < 6)
            continue;

        stepModel->appendRow({
            new QStandardItem(p[0]),
            new QStandardItem(p[1]),
            new QStandardItem(p[2])
        });

        manualModel->appendRow({
            new QStandardItem(p[3]),
            new QStandardItem(p[4]),
            new QStandardItem(p[5])
        });
    }

    file.close();

    ui->tableViewStep->scrollToTop();
    ui->tableViewManual->scrollToTop();
}

void MainWindow::appendShellOutput(const QString &text)
{
    ui->textEdit->moveCursor(QTextCursor::End);
    ui->textEdit->insertPlainText(text);
    ui->textEdit->moveCursor(QTextCursor::End);
}

void MainWindow::appendShellOutputLidar(const QString &text)
{
    ui->textEdit_lidar->moveCursor(QTextCursor::End);
    ui->textEdit_lidar->insertPlainText(text);
    ui->textEdit_lidar->moveCursor(QTextCursor::End);
}

void MainWindow::appendPicOutput(QString fileName, const int width, const int height)
{
    qDebug() << "appendPicOutput thread =" << QThread::currentThread();
    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly))
    {
        qDebug() << "Open raw file failed:" << fileName;
        return;
    }

    QByteArray raw = file.readAll();

    if (raw.size() != width * height * 2)
    {
        qDebug() << "Invalid raw file size:" << raw.size();
//        return;
    }

    QImage image(width, height, QImage::Format_RGB888);

    const unsigned char *src =
        reinterpret_cast<const unsigned char *>(raw.constData());

    auto clamp = [](int v) -> int
    {
        if (v < 0) return 0;
        if (v > 255) return 255;
        return v;
    };

    for (int y = 0; y < height; ++y)
    {
        uchar *dst = image.scanLine(y);

        for (int x = 0; x < width; x += 2)
        {
            int index = (y * width + x) * 2;

            int U  = src[index + 0];
            int Y0 = src[index + 1];
            int V  = src[index + 2];
            int Y1 = src[index + 3];

            auto convert = [&](int Y, uchar *rgb)
            {
                int C = Y - 16;
                int D = U - 128;
                int E = V - 128;

                int R = (298 * C + 409 * E + 128) >> 8;
                int G = (298 * C - 100 * D - 208 * E + 128) >> 8;
                int B = (298 * C + 516 * D + 128) >> 8;

                rgb[0] = clamp(R);
                rgb[1] = clamp(G);
                rgb[2] = clamp(B);
            };

            convert(Y0, dst + x * 3);
            convert(Y1, dst + (x + 1) * 3);
        }
    }

    ui->label_6->setAlignment(Qt::AlignCenter);

    ui->label_6->setPixmap(
        QPixmap::fromImage(image).scaled(
            ui->label_6->size(),
            Qt::KeepAspectRatio,
            Qt::SmoothTransformation));
}

void MainWindow::on_resetTemp_clicked()
{
    // 表2：Result -> WAIT
    for (int row = 0; row < stepModel->rowCount(); ++row)
    {
        QStandardItem *item = stepModel->item(row, 2);
        if (item){
            item->setBackground(Qt::white);
            item->setText("WAIT");
        }

    }

    // 表3：Phenomenon、Manual Result 清空
    for (int row = 0; row < manualModel->rowCount(); ++row)
    {
        QStandardItem *item1 = manualModel->item(row, 1);
        if (item1)
            item1->setText("");

        QStandardItem *item2 = manualModel->item(row, 2);
        if (item2)
            item2->setText("");
    }
    ui->textEdit->clear();
    ui->label_6->clear();
    ui->textEdit_lidar->clear();
}
