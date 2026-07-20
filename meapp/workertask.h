#ifndef WORKERTASK_H
#define WORKERTASK_H


#include <QRunnable>
#include <QDebug>
#include <functional>

class WorkerTask : public QRunnable
{
public:
    // 构造函数：传入任务ID和实际任务函数
    WorkerTask(int id, std::function<void(int)> taskFunc);

    // 重载 run()，线程池会调用
    void run() override;

private:
    int m_id; // 任务ID
    std::function<void(int)> m_taskFunc; // 实际任务函数
};

#endif // WORKERTASK_H
