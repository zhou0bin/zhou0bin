#pragma once

#include <QObject>
#include <QMap>
#include <QStringList>
#include <qdebug.h>
#include <QElapsedTimer>
#include <QCoreApplication>
#include "ProcessStep.h"
#include "cancomm_pcanfd.h"
#include "singleApi/cdc_control1_680.h"
#include "singleApi/cdc_15_443.h"
#include "singleApi/cdc_control2_683.h"
#include "SignalMapper443.h"
#include "SignalMapper680.h"
#include "SignalMapper683.h"

class ProcessExecutor : public QObject
{
    Q_OBJECT

public:
    explicit ProcessExecutor(QObject *parent = nullptr);

    void load(QString filePath);
    void runAssembly(const QString &name);

    CanCommPCAN *canCommPro;
signals:
    void stepStarted(int stepId, const QString &name);
    void stepFinished(int stepId, const QString &name, const QString &result);
    void log(const QString &msg);
    void shellOutput(const QString &text);
    void shellOutputLidar(const QString &text);
    void picOutput(QString fileName, const int width, const int height);

private:
    void runStep(const ProcessStep &step);

    bool executeCan(const CanCmd &cmd);        // ✅ 改成 bool
    bool executeShell(const CanCmd &cmd);     // ✅ 改成 bool
    bool checkExpect(const ProcessStep &step);

public slots:
    void stop();   // ✅ 必须 public（否则UI不能调用）
    void on_receiveMessage0(quint32 canId, const QByteArray &message);

private:
    QMap<QString, QList<ProcessStep>> processMap;

    bool m_stopFlag = false;
    bool m_waitVersionReady = false;
    QString m_waitVersion;
    int waitCanid;
    QSet<uint32_t> m_pendingCanIds;
};
