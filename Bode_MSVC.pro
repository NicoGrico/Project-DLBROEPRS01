QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets printsupport

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    main.cpp \
    mainwindow.cpp \
    analysiswindow.cpp \
    qcustomplot.cpp \
    transferfunction.cpp

HEADERS += \
    mainwindow.h \
    analysiswindow.h \
    exprtk.hpp \
    qcustomplot.h \
    transferfunction.h

FORMS += \
    analysiswindow.ui \
    form.ui \
    mainwindow.ui \
    analysiswindow.ui


#fix error
win32:QMAKE_CXXFLAGS += /bigobj
unix:QMAKE_CXXFLAGS += -Wa,-mbig-obj

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
