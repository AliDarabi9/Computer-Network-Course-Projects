QT = core \
    widgets
QT += multimedia
QT += widgets
CONFIG += c++17 cmdline

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
        audioplay.cpp \
        audiorec.cpp \
        main.cpp \
        receive.cpp \
        send.cpp

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

HEADERS += \
    audioplay.h \
    audiorec.h \
    receive.h \
    send.h

#INCLUDEPATH += C:\Users\alida\OneDrive\Desktop\boost_1_84_0\boost_1_84_0
#LIBS += -L"C:\Users\alida\OneDrive\Desktop\boost_1_84_0\boost_1_84_0\stage\mingw-x64\lib -lboost_system-mgw11-mt-d-x64-1_84"
#LIBS += -lws2_32


INCLUDEPATH += "D:\UT\Term 6\CN\CA1\boost_1_84_0"
LIBS += -L"D:\UT\Term 6\CN\CA1\boost_1_84_0\stage\x64\lib -lboost_system-mgw11-mt-d-x64-1_84"
LIBS += -lws2_32
