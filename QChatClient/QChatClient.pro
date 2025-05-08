QT       += core gui
QT       += network
QT       += sql
QT       += widgets multimedia

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0
TEMPLATE = app

SOURCES += \
    client.cpp \
    database_manager.cpp \
    delay_send_dialog.cpp \
    email_login_window.cpp \
    feature_menu_widget.cpp \
    login_window.cpp \
    main.cpp \
    main_window.cpp \
    message_bubble_widget.cpp \
    message_search_widget.cpp \
    record_dialog.cpp \
    register_window.cpp

HEADERS += \
    client.h \
    database_manager.h \
    delay_send_dialog.h \
    email_login_window.h \
    feature_menu_widget.h \
    login_window.h \
    main_window.h \
    message_bubble_widget.h \
    message_search_widget.h \
    record_dialog.h \
    register_window.h \
    ui_record_dialog.h

FORMS += \
    main_window.ui \
    record_dialog.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
