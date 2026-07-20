#include "cancomm_pcanfd.h"
#include <QDebug>
#include <QThread>

CanCommPCAN::CanCommPCAN(QObject *parent) : QObject(parent) {}
CanCommPCAN::~CanCommPCAN() { closeConnection(); }

bool CanCommPCAN::openConnection(bool useFd)
{
    TPCANStatus status;

    if (useFd)
    {
        // 初始化 CAN-FD 模式 (500k/2M)
//        TPCANStatus status = CAN_InitializeFD(
//            PCAN_USBBUS1,
//            "f_clock=80000000, nom_brp=4, nom_tseg1=63, nom_tseg2=16, nom_sjw=16, "
//            "data_brp=1, data_tseg1=15, data_tseg2=4, data_sjw=4, "
//            "fdoe=1, brse=1, mode=0"
//        );
//        TPCANStatus status = CAN_InitializeFD(
//            PCAN_USBBUS1,
//            "f_clock=80000000, nom_brp=2, nom_tseg1=63, nom_tseg2=16, nom_sjw=16, "
//            "data_brp=2, data_tseg1=15, data_tseg2=4, data_sjw=4, "
//        );
        status = CAN_InitializeFD(
            PCAN_USBBUS1,
            "f_clock=80000000, "
            "nom_brp=2, nom_tseg1=63, nom_tseg2=16, nom_sjw=16, "
            "data_brp=2, data_tseg1=15, data_tseg2=4, data_sjw=4, "
            "fdoe=1, brse=1"
        );

        TPCANStatus status2 = CAN_InitializeFD(
            PCAN_USBBUS2,
            "f_clock=80000000, "
            "nom_brp=2, nom_tseg1=63, nom_tseg2=16, nom_sjw=16, "
            "data_brp=2, data_tseg1=15, data_tseg2=4, data_sjw=4, "
            "fdoe=1, brse=1"
        );

        if (status2 == PCAN_ERROR_OK) {
            qDebug() << "✅ PCAN-FD 通道2打开成功 (500k/2M ISO)";
        }


        if (status == PCAN_ERROR_OK) {
            qDebug() << "✅ PCAN-FD 通道打开成功 (500k/2M ISO)";
            m_isFdMode = true;
            sendThread = QThread::create([this]() { sendLoop(); });
            sendThread->start();
            m_running = true;
            return true;
        }
    }

    // 回退：初始化标准 CAN
    status = CAN_Initialize(PCAN_USBBUS1, PCAN_BAUD_500K, 0, 0, 0);
    if (status == PCAN_ERROR_OK) {
        qDebug() << "✅ PCAN 通道打开成功 (500k)";
        m_isFdMode = false;
        m_running = true;
        return true;
    }

    qDebug() << "❌ 初始化失败, 错误码:" << QString("0x%1").arg(status, 0, 16);
    return false;
}

void CanCommPCAN::closeConnection() {
    if (m_running) {
        m_running = false;
        receiveThreadRunning = false;
        m_cond.wakeAll();

        if (receiveThread && receiveThread->isRunning()) {
            receiveThread->quit();
            receiveThread->wait();
        }
        if (sendThread && sendThread->isRunning()) {
            sendThread->quit();
            sendThread->wait();
        }

        CAN_Uninitialize(PCAN_USBBUS1);
        qDebug() << "🧹 PCAN 通道已关闭";
    }
}

quint8 lengthToDLC(int len)
{
    if (len <= 8)  return len;
    if (len <= 12) return 9;
    if (len <= 16) return 10;
    if (len <= 20) return 11;
    if (len <= 24) return 12;
    if (len <= 32) return 13;
    if (len <= 48) return 14;
    return 15; // 64 bytes
}

bool CanCommPCAN::sendMessageCan(quint32 canId, const QByteArray &data)
{
    if (!m_running)
        return false;

    if (data.size() > 8)
    {
        qDebug() << "❌ Classical CAN 数据长度不能超过8字节";
        return false;
    }

    TPCANMsgFD msg{};
    TPCANStatus status;

    msg.ID = canId;

    // 注意：这里不要带 PCAN_MESSAGE_FD
    msg.MSGTYPE = (canId > 0x7FF)
                    ? PCAN_MESSAGE_EXTENDED
                    : PCAN_MESSAGE_STANDARD;

    msg.DLC = data.size();     // 0~8 时 DLC 与长度一致
    memcpy(msg.DATA, data.constData(), data.size());

    status = CAN_WriteFD(PCAN_USBBUS1, &msg);

    if (status != PCAN_ERROR_OK)
    {
        qDebug() << QString("❌ 发送 CAN 失败, 错误码: 0x%1")
                    .arg(status, 0, 16);
        return false;
    }

    qDebug() << QString("✅ 发送 CAN 成功: ID=0x%1 LEN=%2 DATA=%3")
                .arg(canId, 3, 16, QLatin1Char('0'))
                .arg(data.size())
                .arg(QString(data.toHex(' ')));

    return true;
}

bool CanCommPCAN::sendMessage(quint32 canId, const QByteArray &data)
{
    if (!m_running)
        return false;

    TPCANStatus status;

    if (m_isFdMode) {
        // --- CAN FD 发送 ---
        TPCANMsgFD msg;
        memset(&msg, 0, sizeof(msg));

        msg.ID = canId;
        msg.MSGTYPE = PCAN_MESSAGE_FD | PCAN_MESSAGE_BRS;
        msg.DLC = lengthToDLC(data.size()); // 自动匹配DLC
        memcpy(msg.DATA, data.constData(), qMin(data.size(), 64));

        status = CAN_WriteFD(PCAN_USBBUS1, &msg);
        if (status != PCAN_ERROR_OK) {
            qDebug() << QString("❌ 发送 CAN-FD 失败, 错误码: 0x%1").arg(status, 0, 16);
            if (status & PCAN_ERROR_BUSOFF)
                qDebug() << "⚠️ 总线已离线 (Bus-Off)，请检查波特率或复位设备";
            return false;
        }

        qDebug() << QString("✅ 发送 CAN-FD 成功: ID=0x%1 LEN=%2 DATA=%3")
                    .arg(canId, 3, 16, QLatin1Char('0'))
                    .arg(data.size())
                    .arg(QString(data.toHex(' ')));
    }
    else {
        // --- 普通 CAN ---
        TPCANMsg msg;
        memset(&msg, 0, sizeof(msg));
        msg.ID = canId;
        msg.MSGTYPE = PCAN_MESSAGE_STANDARD;
        msg.LEN = qMin(data.size(), 8);
        memcpy(msg.DATA, data.constData(), msg.LEN);

        status = CAN_Write(PCAN_USBBUS1, &msg);
        if (status != PCAN_ERROR_OK) {
            qDebug() << QString("❌ 发送 CAN 失败, 错误码: 0x%1").arg(status, 0, 16);
            return false;
        }

        qDebug() << QString("✅ 发送 CAN 成功: ID=0x%1 LEN=%2 DATA=%3")
                    .arg(canId, 3, 16, QLatin1Char('0'))
                    .arg(data.size())
                    .arg(QString(data.toHex(' ')));
    }

    return true;
}

bool CanCommPCAN::sendMessage2(quint32 canId, const QByteArray &data)
{
    TPCANStatus status;

        // --- CAN FD 发送 ---
        TPCANMsgFD msg;
        memset(&msg, 0, sizeof(msg));

        msg.ID = canId;
        msg.MSGTYPE = PCAN_MESSAGE_FD | PCAN_MESSAGE_BRS;
        msg.DLC = lengthToDLC(data.size()); // 自动匹配DLC
        memcpy(msg.DATA, data.constData(), qMin(data.size(), 64));

        status = CAN_WriteFD(PCAN_USBBUS2, &msg);
        if (status != PCAN_ERROR_OK) {
            qDebug() << QString("❌ 发送 CAN-FD 失败, 错误码: 0x%1").arg(status, 0, 16);
            if (status & PCAN_ERROR_BUSOFF)
                qDebug() << "⚠️ 总线已离线 (Bus-Off)，请检查波特率或复位设备";
            return false;
        }

        qDebug() << QString("✅ 发送 CAN-FD2 成功: ID=0x%1 LEN=%2 DATA=%3")
                    .arg(canId, 3, 16, QLatin1Char('0'))
                    .arg(data.size())
                    .arg(QString(data.toHex(' ')));
    return true;
}

void CanCommPCAN::enqueueMessage(quint32 canId, const QByteArray &message) {
    QMutexLocker locker(&m_mutex);
    m_sendQueue.enqueue(qMakePair(canId, message));
    m_cond.wakeAll();
}

void CanCommPCAN::startReceiveThread() {
    if (receiveThreadRunning) return;

    receiveThread = QThread::create([this]() { receiveLoop(); });
    receiveThreadRunning = true;
    receiveThread->start();

//    sendThread = QThread::create([this]() { sendLoop(); });
//    sendThread->start();

    qDebug() << "🚀 PCAN-FD 接收/发送线程已启动";
}

void CanCommPCAN::receiveLoop()
{
    qDebug() << "🟢 PCAN 接收线程启动";

    while (receiveThreadRunning) {
        if (paused.load()) {
            QThread::msleep(50);
            continue;
        }

        if (m_isFdMode) {
            // --- CAN-FD 模式 ---
            TPCANMsgFD msgFd;
            TPCANTimestampFD tsFd;
            TPCANStatus status = CAN_ReadFD(PCAN_USBBUS1, &msgFd, &tsFd);

            if (status == PCAN_ERROR_QRCVEMPTY) {
                QThread::msleep(2);
                continue;
            }

            if (status == PCAN_ERROR_OK) {
                QByteArray data(reinterpret_cast<const char*>(msgFd.DATA), msgFd.DLC);
                emit messageReceived(msgFd.ID, data);
            } else {
                if (status != PCAN_ERROR_QRCVEMPTY)
                    qDebug() << "⚠️ CAN_ReadFD 错误:" << QString("0x%1").arg(status, 0, 16);
            }
        } else {
            // --- 普通 CAN 模式 ---
            TPCANMsg msg;
            TPCANTimestamp ts;
            TPCANStatus status = CAN_Read(PCAN_USBBUS1, &msg, &ts);

            if (status == PCAN_ERROR_QRCVEMPTY) {
                QThread::msleep(2);
                continue;
            }

            if (status == PCAN_ERROR_OK) {
                QByteArray data(reinterpret_cast<const char*>(msg.DATA), msg.LEN);
                emit messageReceived(msg.ID, data);
            } else {
                if (status != PCAN_ERROR_QRCVEMPTY)
                    qDebug() << "⚠️ CAN_Read 错误:" << QString("0x%1").arg(status, 0, 16);
            }
        }
        QThread::msleep(5);
    }

    qDebug() << "🔴 PCAN 接收线程结束";
}

void CanCommPCAN::sendLoop() {
    while (m_running) {
        m_mutex.lock();
        if (m_sendQueue.isEmpty())
            m_cond.wait(&m_mutex);
        if (!m_running) {
            m_mutex.unlock();
            break;
        }

        auto pair = m_sendQueue.dequeue();
        m_mutex.unlock();

        sendMessage(pair.first, pair.second);
        QThread::msleep(10);
    }
}
