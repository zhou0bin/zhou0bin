#include "workertask.h"

WorkerTask::WorkerTask(int id, std::function<void(int)> taskFunc)
    : m_id(id), m_taskFunc(taskFunc)
{
    // 任务完成后自动释放对象
    setAutoDelete(true);
}

void WorkerTask::run()
{
    if (m_taskFunc) {
        // 调用用户传入的实际任务函数
        m_taskFunc(m_id);
    } else {
        qDebug() << "Task" << m_id << "has no function!";
    }
}

//#include <QCoreApplication>
//#include <QThreadPool>
//#include <QEventLoop>
//#include <QAtomicInt>
//#include "WorkerTask.h"
//#include <QThread>
//#include <QDebug>

//void myTaskFunction(int id) {
//    qDebug() << "Task" << id << "running in thread:" << QThread::currentThread();
//    QThread::sleep(1); // 模拟耗时操作
//    qDebug() << "Task" << id << "finished";
//}

//int main(int argc, char *argv[])
//{
//    QCoreApplication a(argc, argv);

//    const int taskCount = 20;
//    QThreadPool pool;
//    pool.setMaxThreadCount(taskCount);

//    QAtomicInt finishedCount(0);
//    QEventLoop loop;

//    // 提交20个任务
//    for (int i = 0; i < taskCount; ++i) {
//        pool.start(new WorkerTask(i, [&](int id){
//            myTaskFunction(id);

//            // 完成计数
//            finishedCount.fetchAndAddRelaxed(1);
//            if (finishedCount.loadRelaxed() >= taskCount) {
//                loop.quit(); // 所有任务完成
//            }
//        }));
//    }

//    loop.exec();
//    qDebug() << "All tasks finished";

//    return 0;
//}
