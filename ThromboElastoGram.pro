#-------------------------------------------------
#
# Project created by QtCreator 2016-09-01T18:56:08
#
#-------------------------------------------------

QT       += core gui sql serialport  printsupport webkit xml

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = ThromboElastoGram
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    fdatatbase.cpp \
    logindlg.cpp \
    plotinfo.cpp \
    scalepicker.cpp \
    scalezoomer.cpp \
    zoomer.cpp \
    plot.cpp \
    commconfigdlg.cpp \
    portlist.cpp \
    optimalacbiasform.cpp \
    optimaldcbiasform.cpp \
    parasetform.cpp \
    platsensorsetform.cpp \
    showlistcurveform.cpp \
    signaldata.cpp \
    newsampledlg.cpp \
    detailedsampleform.cpp \
    newpatientdlg.cpp \
    sampletypesdlg.cpp \
    auxiliaryinfoform.cpp \
    addsampletypedlg.cpp \
    systemsetupdlg.cpp \
    testparaform.cpp \
    printdialog.cpp \
    recordmanagedlg.cpp \
    selectmergedlg.cpp \
    newsensordlg.cpp \
    xmldomparser.cpp \
    xmlstreamwriter.cpp \
    hdbexchangedlg.cpp

HEADERS  += mainwindow.h \
    fdatatbase.h \
    logindlg.h \
    plotinfo.h \
    scalepicker.h \
    scalezoomer.h \
    utils.h \
    zoomer.h \
    plot.h \
    commconfigdlg.h \
    portlist.h \
    optimalacbiasform.h \
    optimaldcbiasform.h \
    parasetform.h \
    platsensorsetform.h \
    showlistcurveform.h \
    signaldata.h \
    newsampledlg.h \
    detailedsampleform.h \
    newpatientdlg.h \
    sampletypesdlg.h \
    auxiliaryinfoform.h \
    addsampletypedlg.h \
    systemsetupdlg.h \
    testparaform.h \
    printdialog.h \
    recordmanagedlg.h \
    selectmergedlg.h \
    newsensordlg.h \
    xmldomparser.h \
    xmlstreamwriter.h \
    hdbexchangedlg.h

FORMS    += mainwindow.ui \
    logindlg.ui \
    plotinfo.ui \
    commconfigdlg.ui \
    optimalacbiasform.ui \
    optimaldcbiasform.ui \
    parasetform.ui \
    platsensorsetform.ui \
    showlistcurveform.ui \
    newsampledlg.ui \
    detailedsampleform.ui \
    newpatientdlg.ui \
    sampletypesdlg.ui \
    auxiliaryinfoform.ui \
    addsampletypedlg.ui \
    systemsetupdlg.ui \
    testparaform.ui \
    recordmanagedlg.ui \
    selectmergedlg.ui \
    newsensordlg.ui \
    hdbexchangedlg.ui

CONFIG += c++11

LIBS +=  -lqwt
INCLUDEPATH += C:\Qt\Qt5.5.1\5.5\mingw492_32\include\QtQwt

RESOURCES += \
    appicon.qrc \
    toolbar.qrc \
    logindlg.qrc \
    hardsetimage.qrc \
    realtimecurveimage.qrc

RC_FILE = appicon.rc

DISTFILES +=
