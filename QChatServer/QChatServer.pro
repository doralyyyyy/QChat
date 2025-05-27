QT       += core gui
QT       += network
QT       += sql
QT       += widgets multimedia multimediawidgets
QT       += printsupport charts

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0
TEMPLATE = app

RESOURCES += \
    resources.qrc

SOURCES += \
    avatar_cropper.cpp \
    camera.cpp \
    chat_page.cpp \
    chat_page2.cpp \
    close_confirm_dialog.cpp \
    database_manager.cpp \
    delay_send_dialog.cpp \
    emoji_picker.cpp \
    feature_menu_widget.cpp \
    feedback_dialog.cpp \
    file_confirm_dialog.cpp \
    friend_list_page.cpp \
    main.cpp \
    main_window.cpp \
    message.cpp \
    message_bubble_widget.cpp \
    message_context_menu_handler.cpp \
    message_search_widget.cpp \
    record_dialog.cpp \
    server.cpp \
    tray_manager.cpp \
    user_auth_database_manager.cpp

HEADERS += \
    avatar_cropper.h \
    camera.h \
    chat_page.h \
    chat_page2.h \
    close_confirm_dialog.h \
    database_manager.h \
    delay_send_dialog.h \
    emoji_picker.h \
    feature_menu_widget.h \
    feedback_dialog.h \
    file_confirm_dialog.h \
    friend_list_page.h \
    main_window.h \
    message.h \
    message_bubble_widget.h \
    message_context_menu_handler.h \
    message_search_widget.h \
    record_dialog.h \
    server.h \
    tray_manager.h \
    user_auth_database_manager.h

FORMS += \
    main_window.ui \
    record_dialog.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
