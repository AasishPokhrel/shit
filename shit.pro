QT += core gui widgets

RC_FILE = shit.rc

CONFIG += c++17

SOURCES += \
    main.cpp \
    mainwindow.cpp

qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

HEADERS += \
    mainwindow.h

RESOURCES += \
    shit.qrc
