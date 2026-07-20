#ifndef TABLEROWDATA_H
#define TABLEROWDATA_H

#include <QString>

struct TableRowData {
    QString valvePosition;   // 电磁阀位置
    int sprayDuration;       // 连喷时长
    int cycleCount;          // 点喷循环次数
    int openDuration;        // 点喷开时长
    int closeDuration;       // 点喷关时长

    QString wiperWork;       // 附带雨刮工作

    int cleanCount;          // 去污次数
    int cleanFrequency;      // 去污频率
    int cleanDelay;          // 去污延时

    int waterCount;          // 去水痕次数
    int waterFrequency;      // 去水痕频率
    int waterDelay;          // 去水痕延时
};

#endif // TABLEROWDATA_H
