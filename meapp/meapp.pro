QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

# 添加 Vector 驱动库的头文件和库文件路径
#INCLUDEPATH += D:/vector/bin
#               D:/PCAN-Basic/Include

# 配置库文件路径和链接 vxlapi
#LIBS += -L"D:/vector/bin" -lvxlapi
#        -L"D:/PCAN-Basic/x86/BB_LIB" -lPCANBasic

#INCLUDEPATH += /usr/include
LIBS += -lpcanbasic

#INCLUDEPATH += ../PCAN-Basic/Include
#LIBS += -L"../PCAN-Basic/x64/VC_LIB" -lPCANBasic
# 在编译后将 vxlapi.dll 复制到可执行文件目录
#DESTDIR = ./bin
#win32: QMAKE_POST_LINK += copy /Y "D:/vector/bin/vxlapi.dll" $$DESTDIR

# 添加源代码文件
SOURCES += \
    ProcessExecutor.cpp \
    SignalMapper443.cpp \
    SignalMapper680.cpp \
    SignalMapper683.cpp \
    cancomm_pcanfd.cpp \
    expandablelistwidget.cpp \
    main.cpp \
    mainwindow.cpp \
    singleApi/byte.c \
    singleApi/cdc_15_443.c \
    singleApi/cdc_control1_680.c \
    singleApi/cdc_control2_683.c \
    singleApi/protocol_data.c \
    tablewidget.cpp \
    workertask.cpp

# 添加头文件
HEADERS += \
#    ../PCAN-Basic/Include/PCANBasic.h \
    ComboBoxDelegate.h \
    ProcessExecutor.h \
    ProcessStep.h \
    SignalMapper443.h \
    SignalMapper680.h \
    SignalMapper683.h \
    TableRowData.h \
    cancomm_pcanfd.h \
    comboboxdele.h \
    expandablelistwidget.h \
    global_xl.h \
    mainwindow.h \
#    D:/vector/bin/vxlapi.h \
    singleApi/byte.h \
    singleApi/cdc_15_443.h \
    singleApi/cdc_control1_680.h \
    singleApi/cdc_control2_683.h \
    singleApi/protocol_data.h \
    tablewidget.h \
    workertask.h

# 添加 UI 文件
FORMS += \
    mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

