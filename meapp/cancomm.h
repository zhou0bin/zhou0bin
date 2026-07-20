#ifndef CAN_COMM_H
#define CAN_COMM_H

#include <QObject>
#include <QByteArray>
#include <QThread>
#include <QMutex>
#include <QAtomicInteger>
#include <windows.h>
#include <QWaitCondition>
#include <qqueue.h>

#include "vxlapi.h"
#include "global_xl.h"

class CanComm : public QObject {
    Q_OBJECT

public:
    explicit CanComm(QObject *parent = nullptr);
    ~CanComm();

    bool openConnection();
    void closeConnection();
    bool sendMessage(quint32 canId, const QByteArray &message);
    void startReceiveThread();  // 启动接收线程

    void setPaused(bool p) { paused.store(p); }

    void enqueueMessage(quint32 canId, const QByteArray &message);

signals:
    void messageReceived(quint32 canId, QByteArray data);

private:
    QThread* receiveThread;
    QAtomicInteger<bool> receiveThreadRunning;

    std::atomic_bool paused{false};  // 用原子变量确保线程安全
    void receiveLoop();  // 接收循环线程体

    QQueue<QPair<quint32, QByteArray>> m_sendQueue;
    QMutex m_mutex;
    QWaitCondition m_cond;
    QThread m_sendThread;
    bool m_running = true;

    void sendLoop();
};

#endif // CAN_COMM_H
