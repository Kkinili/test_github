QT       += core gui sql network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    appointmenteditview.cpp \
    appointmentview.cpp \
    departmenteditview.cpp \
    departmentview.cpp \
    doctoreditview.cpp \
    doctorview.cpp \
    drugeditview.cpp \
    drugview.cpp \
    idatabase.cpp \
    loginview.cpp \
    main.cpp \
    masterview.cpp \
    networksync.cpp \
    patienteditview.cpp \
    patientview.cpp \
    recordeditview.cpp \
    recordview.cpp \
    statisticsview.cpp \
    statisticsworker.cpp \
    welcomeview.cpp

HEADERS += \
    appointmenteditview.h \
    appointmentview.h \
    departmenteditview.h \
    departmentview.h \
    doctoreditview.h \
    doctorview.h \
    drugeditview.h \
    drugview.h \
    idatabase.h \
    loginview.h \
    masterview.h \
    networksync.h \
    patienteditview.h \
    patientview.h \
    recordeditview.h \
    recordview.h \
    statisticsview.h \
    statisticsworker.h \
    welcomeview.h

FORMS += \
    appointmenteditview.ui \
    appointmentview.ui \
    departmenteditview.ui \
    departmentview.ui \
    doctoreditview.ui \
    doctorview.ui \
    drugeditview.ui \
    drugview.ui \
    loginview.ui \
    masterview.ui \
    patienteditview.ui \
    patientview.ui \
    recordeditview.ui \
    recordview.ui \
    statisticsview.ui \
    welcomeview.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    lab4.qrc
