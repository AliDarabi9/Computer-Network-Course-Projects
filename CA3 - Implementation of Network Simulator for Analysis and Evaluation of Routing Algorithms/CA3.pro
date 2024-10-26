QT = core

CONFIG += c++17 cmdline

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
        lsp.cpp \
        main.cpp \
        node.cpp \
        packet.cpp \
        pc.cpp \
        port.cpp \
        rip.cpp \
        router.cpp

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

HEADERS += \
    lsp.h \
    node.h \
    packet.h \
    pc.h \
    port.h \
    rip.h \
    router.h
