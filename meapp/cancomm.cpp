#include "cancomm.h"
#include <QDebug>
#include <QTimer>
#include <algorithm>
#include <QElapsedTimer>


char g_AppName[XL_MAX_APPNAME+1] = "qtCANApp";
XLportHandle g_xlPortHandle = XL_INVALID_PORTHANDLE;
XLapiIDriverConfigV1 g_xlDrvConfig;
XLdeviceDrvConfigListV1 g_xlDevConfig;
XLchannelDrvConfigListV1 g_xlChannelConfig;

XLaccess g_xlChannelIndex = XL_INVALID_CHANNEL_INDEX;
unsigned int g_BaudRate = 500000;
unsigned int g_canFdSupport = 0;
unsigned int g_canFdModeNoIso = ENABLE_CAN_FD_MODE_NO_ISO;

CanComm::CanComm(QObject *parent) : QObject(parent), receiveThread(nullptr) {

    QThread* worker = QThread::create([this] { sendLoop(); });
    worker->start();
}

CanComm::~CanComm() {
    closeConnection();
}

bool CanComm::openConnection() {
    XLstatus status = xlOpenDriver();
    if (status != XL_SUCCESS) {
        qWarning() << "Failed to open driver! Error:" << xlGetErrorString(status);
        return false;
    }
    qDebug() << "Driver opened successfully.";

    status = xlCreateDriverConfig(XL_IDRIVER_CONFIG_VERSION_1, (XLIDriverConfig*)&g_xlDrvConfig);
    if (status != XL_SUCCESS) {
        qWarning() << "Failed to create driver config! Error:" << xlGetErrorString(status);
        return false;
    }
    qDebug() << "Driver config created.";

    status = g_xlDrvConfig.fctGetDeviceConfig(g_xlDrvConfig.configHandle, &g_xlDevConfig);
    if (status != XL_SUCCESS) {
        qWarning() << "Failed to get device config! Error:" << xlGetErrorString(status);
        return false;
    }
    qDebug() << "Device config retrieved. Device count:" << g_xlDevConfig.count;

    status = g_xlDrvConfig.fctGetChannelConfig(g_xlDrvConfig.configHandle, &g_xlChannelConfig);
    if (status != XL_SUCCESS) {
        qWarning() << "Failed to get channel config! Error:" << xlGetErrorString(status);
        return false;
    }
    qDebug() << "Channel config retrieved. Channel count:" << g_xlChannelConfig.count;

    // 检查 CAN FD 支持
    g_canFdSupport = 0;
    for (unsigned int i = 0; i < g_xlDevConfig.count; ++i) {
        const auto& dev = g_xlDevConfig.item[i];
        for (unsigned int j = 0; j < dev.channelList.count; ++j) {
            const auto& ch = dev.channelList.item[j];
            if ((ch.channelCapabilities & XL_CHANNEL_FLAG_EX1_CANFD_ISO_SUPPORT) &&
                (dev.hwType != XL_HWTYPE_VIRTUAL)) {
                g_canFdSupport = 1;
            }
        }
    }
    qDebug() << "CAN FD support:" << (g_canFdSupport ? "Yes" : "No");

    if (g_canFdSupport) {
        status = xlCreatePort(&g_xlPortHandle, g_AppName, RX_QUEUE_SIZE_FD, XL_INTERFACE_VERSION_V4, XL_BUS_TYPE_CAN);
    } else {
        status = xlCreatePort(&g_xlPortHandle, g_AppName, RX_QUEUE_SIZE, XL_INTERFACE_VERSION_V3, XL_BUS_TYPE_CAN);
    }
    if (status != XL_SUCCESS) {
        qWarning() << "Failed to create port! Error:" << xlGetErrorString(status);
        return false;
    }
    qDebug() << "Port created successfully.";

    // 添加通道
    g_xlChannelIndex = XL_INVALID_CHANNEL_INDEX;
    for (unsigned int i = 0; i < g_xlDevConfig.count; ++i) {
        const auto& dev = g_xlDevConfig.item[i];
        for (unsigned int j = 0; j < dev.channelList.count; ++j) {
            const auto& ch = dev.channelList.item[j];
            if (ch.channelBusActiveCapabilities & XL_BUS_TYPE_CAN) {
                if (g_canFdSupport && !(ch.channelCapabilities & XL_CHANNEL_FLAG_EX1_CANFD_ISO_SUPPORT)) {
                    continue;
                }

                unsigned int initPermission = 0;
                status = xlAddChannelToPort(g_xlPortHandle, ch.channelIndex, 1, &initPermission, XL_BUS_TYPE_CAN);
                if (status == XL_SUCCESS && g_xlChannelIndex == XL_INVALID_CHANNEL_INDEX) {
                    g_xlChannelIndex = ch.channelIndex;
                }
            }
        }
    }
    qDebug() << "Channels added. Selected channel index:" << g_xlChannelIndex;

    xlFinalizePort(g_xlPortHandle);
    qDebug() << "Port finalized.";

    // 设置波特率
    if (g_canFdSupport) {
        XLcanFdConf fd;
        memset(&fd, 0, sizeof(fd));
        fd.arbitrationBitRate = 500000;
        fd.tseg1Abr = 63;
        fd.tseg2Abr = 16;
        fd.sjwAbr   = 16;
        fd.dataBitRate = 2000000;
        fd.tseg1Dbr = 15;
        fd.tseg2Dbr = 4;
        fd.sjwDbr   = 4;
        fd.options = 0;  // 加上这行，确保是 ISO 模式
        auto fdStatus = xlCanFdSetConfiguration(g_xlPortHandle, g_xlChannelIndex, &fd);
        if (fdStatus != XL_SUCCESS) {
            qWarning() << "Failed to configure CAN FD bitrate! Error:" << xlGetErrorString(fdStatus);
        } else {
            qDebug() << "CAN FD bitrate configured.";
        }
    } else {
        xlCanSetChannelBitrate(g_xlPortHandle, g_xlChannelIndex, g_BaudRate);
        qDebug() << "Standard CAN bitrate configured:" << g_BaudRate;
    }

    // 激活通道
    status = xlActivateChannel(g_xlPortHandle, g_xlChannelIndex, XL_BUS_TYPE_CAN, XL_ACTIVATE_RESET_CLOCK);
    if (status != XL_SUCCESS) {
        qWarning() << "Failed to activate channel! Error:" << xlGetErrorString(status);
        return false;
    }

    QElapsedTimer timer;
    timer.start();
    XLcanRxEvent evt;

    bool chipOK = false;
    while (timer.elapsed() < 1000) {  // 最多等 1 秒
        if (xlCanReceive(g_xlPortHandle, &evt) == XL_SUCCESS &&
            evt.tag == XL_CAN_EV_TAG_CHIP_STATE)
        {
            if (evt.tagData.canChipState.busStatus == XL_CHIPSTAT_ERROR_ACTIVE) {
                chipOK = true;
                break;
            }
        }
        QThread::msleep(10);
    }

    if (!chipOK) {
        qWarning() << "CAN hardware not ready: bus not OK";
        return false;
    }

    qDebug() << "Channel activated successfully.";

    return true;
}

void CanComm::closeConnection() {
    receiveThreadRunning.store(false);
    if (receiveThread && receiveThread->isRunning()) {
        receiveThread->quit();
        receiveThread->wait();
    }
    if (g_xlPortHandle != XL_INVALID_PORTHANDLE) {
        xlClosePort(g_xlPortHandle);
        g_xlPortHandle = XL_INVALID_PORTHANDLE;
    }
}

uint8_t getCanFdDlc(int length) {
    if (length <= 8) return length;
    else if (length <= 12) return 9;
    else if (length <= 16) return 10;
    else if (length <= 20) return 11;
    else if (length <= 24) return 12;
    else if (length <= 32) return 13;
    else if (length <= 48) return 14;
    else return 15;
}

bool CanComm::sendMessage(quint32 canId, const QByteArray &message) {
    if (g_canFdSupport) {
        XLcanTxEvent txEvt;
        memset(&txEvt, 0, sizeof(txEvt));
        txEvt.tag = XL_CAN_EV_TAG_TX_MSG;
        txEvt.tagData.canMsg.canId = canId;

        // 自动根据数据长度设置 DLC（合法范围）
        txEvt.tagData.canMsg.dlc = getCanFdDlc(message.size());

        // >8 字节才设置为 CAN FD
        txEvt.tagData.canMsg.msgFlags = (message.size() > 8) ?
            (XL_CAN_TXMSG_FLAG_EDL | XL_CAN_TXMSG_FLAG_BRS) : 0;

        // 拷贝数据，最多 64 字节
        memcpy(txEvt.tagData.canMsg.data, message.data(),
               std::min<size_t>(message.size(), sizeof(txEvt.tagData.canMsg.data)));

        unsigned int sent = 0;
        int state = xlCanTransmitEx(g_xlPortHandle, g_xlChannelIndex, 1, &sent, &txEvt);
        if (state == XL_SUCCESS) {
            qDebug() << "[CANFD] Send success. CAN ID:" << QString::number(canId, 16).toUpper()
                     << "Data:" << message.toHex(' ').toUpper();
        } else {
            qWarning() << "[CANFD] Send failed! Code:" << state << xlGetErrorString(state);
        }
        return state == XL_SUCCESS;
    } else {
        static XLevent evt;
        memset(&evt, 0, sizeof(evt));
        evt.tag = XL_TRANSMIT_MSG;
        evt.tagData.msg.id = canId;
        evt.tagData.msg.dlc = message.size();
        memcpy(evt.tagData.msg.data, message.data(), message.size());

        unsigned int count = 1;
        int state = xlCanTransmit(g_xlPortHandle, g_xlChannelIndex, &count, &evt);
        if (state == XL_SUCCESS) {
            qDebug() << "[CAN] Send success. CAN ID:" << QString::number(canId, 16).toUpper()
                     << "Data:" << message.toHex(' ').toUpper();
        } else {
            qWarning() << "[CAN] Send failed! Code:" << state << xlGetErrorString(state);
        }
        return state == XL_SUCCESS;
    }
}

void CanComm::startReceiveThread() {
    if (receiveThreadRunning.load()) return;

    receiveThreadRunning.store(true);
    receiveThread = QThread::create([this] { receiveLoop(); });
    receiveThread->start();
}

void CanComm::receiveLoop() {
    XLcanRxEvent evt;
    while (receiveThreadRunning.load()) {
        if (xlCanReceive(g_xlPortHandle, &evt) == XL_SUCCESS) {

            if (paused.load()) {
                QThread::msleep(5);  // 可选：防止 CPU 忙等
                continue;
            }

            switch (evt.tag) {
            case XL_CAN_EV_TAG_RX_OK: {
                const auto& msg = evt.tagData.canRxOkMsg;
                QByteArray data(reinterpret_cast<const char*>(msg.data), msg.dlc);

                emit messageReceived(msg.canId, data);
//                qDebug() << "[RX_OK] CAN ID:" << QString::number(msg.canId, 16)
//                         << "DLC:" << msg.dlc
//                         << "Data:" << data.toHex(' ').toUpper();
                break;
            }
            case XL_CAN_EV_TAG_TX_OK: {
                const auto& msg = evt.tagData.canTxOkMsg;
                qDebug() << "[TX_OK] CAN ID:" << QString::number(msg.canId, 16);
                break;
            }
            case XL_CAN_EV_TAG_RX_ERROR: {
                qWarning() << "[RX_ERROR]";
                break;
            }
            case XL_CAN_EV_TAG_TX_ERROR: {
                qWarning() << "[TX_ERROR]";
                break;
            }
            case XL_CAN_EV_TAG_CHIP_STATE: {
                const auto& cs = evt.tagData.canChipState;
                qWarning() << "[CHIP_STATE] busStatus:" << cs.busStatus
                           << "txErrorCounter:" << cs.txErrorCounter
                           << "rxErrorCounter:" << cs.rxErrorCounter;
                break;
            }
            default:
                qDebug() << "[OTHER EVENT] tag:" << evt.tag;
                break;
            }
        }
        QThread::msleep(5);  // 防止 CPU 占用过高
    }
}

void CanComm::enqueueMessage(quint32 canId, const QByteArray &message) {
    QMutexLocker locker(&m_mutex);
    m_sendQueue.enqueue(qMakePair(canId, message));
    m_cond.wakeOne(); // 通知线程有新数据
}

void CanComm::sendLoop() {
    while (m_running) {
        QPair<quint32, QByteArray> msg;
        {
            QMutexLocker locker(&m_mutex);
            if (m_sendQueue.isEmpty()) {
                m_cond.wait(&m_mutex);  // 队列空就等待
                continue;
            }
            msg = m_sendQueue.dequeue();
        }
        sendMessage(msg.first, msg.second); // 调用你现有的 sendMessage()
        QThread::msleep(50); // 可选，控制速率，避免打爆总线
    }
}

