QT       += core gui
QT       += network
QT       += sql
QT       += widgets multimedia multimediawidgets
QT       += printsupport charts
QT       += quick quickcontrols2 quick3d

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0
TEMPLATE = app

SOURCES += \
    avatar_cropper.cpp \
    background_setting_dialog.cpp \
    camera.cpp \
    chat_page.cpp \
    chat_page2.cpp \
    client.cpp \
    close_confirm_dialog.cpp \
    database_manager.cpp \
    delay_send_dialog.cpp \
    email_login_window.cpp \
    emoji_picker.cpp \
    feature_menu_widget.cpp \
    feedback_dialog.cpp \
    file_confirm_dialog.cpp \
    friend_list_page.cpp \
    login_window.cpp \
    main.cpp \
    main_window.cpp \
    match_page.cpp \
    message.cpp \
    message_bubble_widget.cpp \
    message_context_menu_handler.cpp \
    message_search_widget.cpp \
    record_dialog.cpp \
    register_window.cpp \
    setting_page.cpp \
    tray_manager.cpp

HEADERS += \
    avatar_cropper.h \
    background_setting_dialog.h \
    camera.h \
    chat_page.h \
    chat_page2.h \
    client.h \
    close_confirm_dialog.h \
    database_manager.h \
    delay_send_dialog.h \
    email_login_window.h \
    emoji_picker.h \
    feature_menu_widget.h \
    feedback_dialog.h \
    file_confirm_dialog.h \
    friend_list_page.h \
    login_window.h \
    main_window.h \
    match_page.h \
    message.h \
    message_bubble_widget.h \
    message_context_menu_handler.h \
    message_search_widget.h \
    record_dialog.h \
    register_window.h \
    setting_page.h \
    tray_manager.h

RESOURCES +=  \
    resources.qrc

FORMS += \
    main_window.ui \
    record_dialog.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
