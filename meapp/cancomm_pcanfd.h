#ifndef CAN_COMM_PCANFD_H
#define CAN_COMM_PCANFD_H

#include <QObject>
#include <QByteArray>
#include <QThread>
#include <QMutex>
#include <QWaitCondition>
#include <QQueue>
#include <QAtomicInteger>
#include <atomic>
//#include <windows.h>
#include <PCANBasic.h>

class CanCommPCAN : public QObject {
    Q_OBJECT

public:
    explicit CanCommPCAN(QObject *parent = nullptr);
    ~CanCommPCAN();

    bool openConnection(bool useFd);
    void closeConnection();
    bool sendMessage(quint32 canId, const QByteArray &message);
    bool sendMessageCan(quint32 canId, const QByteArray &data);
    bool sendMessage2(quint32 canId, const QByteArray &data);
    void startReceiveThread();

    void setPaused(bool p) { paused.store(p); }
    void enqueueMessage(quint32 canId, const QByteArray &message);

signals:
    void messageReceived(quint32 canId, QByteArray data);

private:
    void receiveLoop();
    void sendLoop();

    QThread *receiveThread = nullptr;
    QThread *sendThread = nullptr;

    QAtomicInteger<bool> receiveThreadRunning{false};
    std::atomic_bool m_running{false};
    std::atomic_bool paused{false};

    QMutex m_mutex;
    QWaitCondition m_cond;
    QQueue<QPair<quint32, QByteArray>> m_sendQueue;
    bool m_isFdMode;
};

#endif // CAN_COMM_PCANFD_H
