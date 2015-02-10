#-------------------------------------------------
#
# Project created by QtCreator 2014-08-10T22:50:21
#
#-------------------------------------------------

#QT       += widgets \
#            sql \
#            network
            #core gui
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets sql network

TARGET = Fias
TEMPLATE = app

LIBS += -L/home/bek03/Projects/qdbf/src/build-src-Desktop_Qt_5_3_0_GCC_64bit-Debug/lib -lQDbf

SOURCES += main.cpp\
        mainwindow.cpp \
    wgtconndb.cpp \
    wgtregions.cpp \
    modelregions.cpp \
    wgtfilesfias.cpp \
    wgtfilesdbf.cpp \
    wgtaddrobj.cpp \
    modeltwnstreet.cpp \
    modelhouse.cpp \
    modellcl.cpp \
    modellclstreet.cpp \
    datestyleditemdelegate.cpp \
    wgttestdelegate.cpp \
    modeltestdelegate.cpp \
    dateitemdelegate.cpp \
    testtableview.cpp \
    checkboxstyleditemdelegate.cpp

HEADERS  += mainwindow.h \
    wgtconndb.h \
    wgtregions.h \
    modelregions.h \
    wgtfilesfias.h \
    wgtfilesdbf.h \
    qdbftablemodel.h \
    qdbf_global.h \
    qdbftable.h \
    qdbfrecord.h \
    wgtaddrobj.h \
    modeltwnstreet.h \
    modelhouse.h \
    modellcl.h \
    modellclstreet.h \
    datestyleditemdelegate.h \
    wgttestdelegate.h \
    modeltestdelegate.h \
    dateitemdelegate.h \
    testtableview.h \
    checkboxstyleditemdelegate.h

RESOURCES += \
    res.qrc

FORMS += \
    wgtregions.ui \
    wgtfilesfias.ui \
    wgtfilesdbf.ui \
    wgtaddrobj.ui \
    wgttestdelegate.ui
