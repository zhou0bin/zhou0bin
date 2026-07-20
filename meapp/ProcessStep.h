#pragma once

#include <QString>
#include <QList>
#include <QJsonObject>

struct CanCmd
{
    QString action;     // can sleep shell

    QString bus;
    QString id;

    int ms = 0;

    QString shell;

    QJsonObject signalMap;
};

struct ProcessStep
{
    int id;
    QString name;
    QString type;

    QList<CanCmd> cmds;
    QJsonObject expect;

    QString result = "WAIT";   // WAIT / PASS / FAIL
};

struct ProcessAssembly
{
    int id = 0;
    QString name;
    QString type;
    QList<ProcessStep> steps;
};
