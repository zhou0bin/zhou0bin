#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QThread>
#include <QDebug>
#include <QStringListModel>
#include <QTime>
#include <QTimer>
#include <QDateTime>
#include <QStandardItem>
#include <QVBoxLayout>
#include <QThreadPool>
#include <QEventLoop>
#include <QAtomicInt>
#include <QMutex>
#include <QtConcurrent/QtConcurrent>
#include <QStandardItemModel>
#include <QMap>
#include "workertask.h"
//#include "cancomm.h"  // 引入 CanComm 类
#include "cancomm_pcanfd.h"
#include "singleApi/cdc_control1_680.h"
#include "singleApi/cdc_15_443.h"
#include "singleApi/cdc_control2_683.h"
#include "expandablelistwidget.h"
#include "tablewidget.h"
#include "ProcessExecutor.h"
#include "ComboBoxDelegate.h"
#include "comboboxdele.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_receiveMessage(quint32 canId, const QByteArray &message);

    void on_sendButton_clicked();   // 发送按钮点击事件
    void on_receiveButton_clicked(); // 接收按钮点击事件
    void on_connectButton_clicked();

    void on_stopButton_clicked();

    void on_cleanButton_clicked();

    void on_startButton_clicked();

    void on_MEstopButton_clicked();

    void on_makeButton_clicked();

    void on_makeButton_2_clicked();

    void on_MEstopButton_5_clicked();

    void on_MEstopButton_6_clicked();

    void on_makeButton_3_clicked();

    void on_makeButton_4_clicked();

    void on_makeButton_5_clicked();

    void on_timerMake(int value);
    void on_timerMake0(int value);
    void on_makeButton_11_clicked();

    void on_makeButton_12_clicked();

    void on_makeButton_27_clicked();

    void on_makeButton_28_clicked();

    void on_makeButton_40_clicked();

    void on_makeButton_39_clicked();

    void on_makeButton_38_clicked();

    void on_makeButton_13_clicked();

    void on_makeButton_14_clicked();

    void on_horizontalSlider_sliderMoved(int position);

    void on_makeButton_34_clicked();

    void on_makeButton_6_clicked();

    void on_makeButton_29_clicked();

    void on_versionButtun_clicked();

    void slotAssemblyClicked(const QModelIndex &index);

    void on_startTemp_clicked();

    void on_stopTemp_clicked();

    void on_pcanConnect_clicked();

    void on_MEstopButton_10_clicked();

    void on_palyback_clicked();

    void appendShellOutput(const QString &text);
    void appendShellOutputLidar(const QString &text);
    void appendPicOutput(QString fileName, const int width, const int height);

    void on_resetTemp_clicked();

signals:
    void sendMessage(quint32 canId, const QByteArray &message);  // 发送消息的信号
    void receiveMessage(); // 接收到消息的信号
    void timerMake(int value);
//    void timerMake0(int value);
    void startAssembly(const QString &name);

private:
    Ui::MainWindow *ui;
    CanCommPCAN *canComm;     // CAN 通信对象
    QThread *canThread;   // CAN 连接的工作线程

    QStringListModel *listModel;  // 添加：模型用于显示数据
    QStringList messageList;      // 添加：存储数据内容

    QThread* sendThread;
    std::atomic_bool sendThreadRunning;

    QStandardItemModel *model;
    QList<QStandardItem*> stepItems;  // 保存每个步骤的父项

    bool lidartype;

    void initSteps(const QStringList &steps);
    void appendResultToStep(int stepIndex, bool success);

    void markStepResult(int stepIndex, bool success);
    void markStepResultSameLine(int stepIndex, bool success);
    void selectControl1_680(Cdc_control2_683 *sle_control1_680, Cdc_control1_680 *sle_control1_683, bool bControl1);

    void dataSend(Cdc_control1_680 cdc_control1_680_x);
    void dataSend683(Cdc_control2_683 cdc_control1_683_x);
    void dataSend443(Cdc_15_443 cdc_15_443);
    void myTaskFunction(int id);

    QByteArray buildCanFrameBySignal(int startBit, int bitLength, const QByteArray &data, int frameLen);
    void onCanFrameReceived(quint32 canId, const QByteArray &data);


    Cdc_control1_680 cdc_control1_680;
    Cdc_control1_680 cdc_control1_680_i[20];
    Cdc_control2_683 cdc_control1_683;
    Cdc_control2_683 cdc_control1_683_i[20];
    Cdc_15_443 cdc_15_443_i;

    QMutex dataMutex;
    QMutex dataMutex0;
    QMutex dataMutex1;

    QByteArray data470;

    QMap<quint32, QString> m_pendingUds;  // respId -> 名称

    QStandardItemModel *assemblyModel;
    QStandardItemModel *stepModel;
    QStandardItemModel *manualModel;

    QMap<QString, QList<ProcessStep>> processMap;

    void initTableView();
    void initProcess();
    void onStepFinished(int stepId, const QString &name, const QString &result);
    ProcessExecutor *executor;
    QString assemblyName;

    QTimer *espTimer;
    QTimer *dcuTimer;
    QTimer *dkmTimer;
};

#endif // MAINWINDOW_H
