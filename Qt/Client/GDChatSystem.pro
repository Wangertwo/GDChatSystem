QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

UI_DIR=./UI

CONFIG += c++17
RC_ICONS = icon.ico
# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    adduseritem.cpp \
    applyfriend.cpp \
    applyfrienditem.cpp \
    applyfriendlist.cpp \
    applyfriendpage.cpp \
    authenfriend.cpp \
    bubbleframe.cpp \
    chatitembase.cpp \
    chatpage.cpp \
    chatuserwid.cpp \
    chatview.cpp \
    clickedoncelabel.cpp \
    contactuserlist.cpp \
    conuseritem.cpp \
    findfaildlg.cpp \
    findsuccessdlg.cpp \
    friendinfopage.cpp \
    friendlabel.cpp \
    gdchatdialog.cpp \
    gdchatuserlist.cpp \
    gdclickedbtn.cpp \
    gdclickedlabel.cpp \
    gdcustomizeedit.cpp \
    gdhttpmgr.cpp \
    gdlogindialog.cpp \
    gdregisterdialog.cpp \
    gdresetdialog.cpp \
    gdtcpmgr.cpp \
    gdtimerbtn.cpp \
    gdusermgr.cpp \
    global.cpp \
    grouptipitem.cpp \
    invaliditem.cpp \
    lineitem.cpp \
    listitembase.cpp \
    loadingdlg.cpp \
    main.cpp \
    mainwindow.cpp \
    messagetextedit.cpp \
    picturebubble.cpp \
    searchlist.cpp \
    statelabel.cpp \
    statewidget.cpp \
    textbubble.cpp \
    userdata.cpp

HEADERS += \
    Singleton.h \
    adduseritem.h \
    applyfriend.h \
    applyfrienditem.h \
    applyfriendlist.h \
    applyfriendpage.h \
    authenfriend.h \
    bubbleframe.h \
    chatitembase.h \
    chatpage.h \
    chatuserwid.h \
    chatview.h \
    clickedoncelabel.h \
    contactuserlist.h \
    conuseritem.h \
    findfaildlg.h \
    findsuccessdlg.h \
    friendinfopage.h \
    friendlabel.h \
    gdchatdialog.h \
    gdchatuserlist.h \
    gdclickedbtn.h \
    gdclickedlabel.h \
    gdcustomizeedit.h \
    gdhttpmgr.h \
    gdlogindialog.h \
    gdregisterdialog.h \
    gdresetdialog.h \
    gdtcpmgr.h \
    gdtimerbtn.h \
    gdusermgr.h \
    global.h \
    grouptipitem.h \
    invaliditem.h \
    lineitem.h \
    listitembase.h \
    loadingdlg.h \
    mainwindow.h \
    messagetextedit.h \
    picturebubble.h \
    searchlist.h \
    statelabel.h \
    statewidget.h \
    textbubble.h \
    userdata.h

FORMS += \
    adduseritem.ui \
    applyfriend.ui \
    applyfrienditem.ui \
    applyfriendpage.ui \
    authenfriend.ui \
    chatpage.ui \
    chatuserwid.ui \
    conuseritem.ui \
    findfaildlg.ui \
    findsuccessdlg.ui \
    friendinfopage.ui \
    friendlabel.ui \
    gdchatdialog.ui \
    gdlogindialog.ui \
    gdregisterdialog.ui \
    gdresetdialog.ui \
    grouptipitem.ui \
    lineitem.ui \
    loadingdlg.ui \
    mainwindow.ui \
    statewidget.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    resource.qrc

DISTFILES += \
    config.ini

# 将config.ini配置文件拷贝到debug的子目录下
win32:CONFIG(debug){
    DESTDIR = debug
    TargetConfig = $${PWD}/config.ini
    TargetConfig = $$replace(TargetConfig, /, \\)
    OutputDir = $${OUT_PWD}/$${DESTDIR}
    OutputDir = $$replace(OutputDir, /, \\)
    QMAKE_POST_LINK += copy /Y \"$$TargetConfig\" \"$$OutputDir\" &

    # # 首先，定义static文件夹的路径
    # StaticDir = $${PWD}/static
    # # 将路径中的"/"替换为"\"
    # StaticDir = $$replace(StaticDir, /, \\)
    # #message($${StaticDir})
    # # 使用xcopy命令拷贝文件夹，/E表示拷贝子目录及其内容，包括空目录。/I表示如果目标不存在则创建目录。/Y表示覆盖现有文件而不提示。
    # QMAKE_POST_LINK += xcopy /Y /E /I \"$$StaticDir\" \"$$OutputDir\\static\\\"
}

# CONFIG(debug, debug | release) {
#     #指定要拷贝的文件目录为工程目录下release目录下的所有dll、lib文件，例如工程目录在D:\QT\Test
#     #PWD就为D:/QT/Test，DllFile = D:/QT/Test/release/*.dll
#     TargetConfig = $${PWD}/config.ini
#     #将输入目录中的"/"替换为"\"
#     TargetConfig = $$replace(TargetConfig, /, \\)
#     #将输出目录中的"/"替换为"\"
#     OutputDir =  $${OUT_PWD}/$${DESTDIR}
#     OutputDir = $$replace(OutputDir, /, \\)
#     //执行copy命令
#     QMAKE_POST_LINK += copy /Y \"$$TargetConfig\" \"$$OutputDir\" &
#     # 首先，定义static文件夹的路径
#     StaticDir = $${PWD}/static
#     # 将路径中的"/"替换为"\"
#     StaticDir = $$replace(StaticDir, /, \\)
#     #message($${StaticDir})
#     # 使用xcopy命令拷贝文件夹，/E表示拷贝子目录及其内容，包括空目录。/I表示如果目标不存在则创建目录。/Y表示覆盖现有文件而不提示。
#     QMAKE_POST_LINK += xcopy /Y /E /I \"$$StaticDir\" \"$$OutputDir\\static\\\"
# }else{
#       #release
#     message("release mode")
#     #指定要拷贝的文件目录为工程目录下release目录下的所有dll、lib文件，例如工程目录在D:\QT\Test
#     #PWD就为D:/QT/Test，DllFile = D:/QT/Test/release/*.dll
#     TargetConfig = $${PWD}/config.ini
#     #将输入目录中的"/"替换为"\"
#     TargetConfig = $$replace(TargetConfig, /, \\)
#     #将输出目录中的"/"替换为"\"
#     OutputDir =  $${OUT_PWD}/$${DESTDIR}
#     OutputDir = $$replace(OutputDir, /, \\)
#     //执行copy命令
#     QMAKE_POST_LINK += copy /Y \"$$TargetConfig\" \"$$OutputDir\"
#     # 首先，定义static文件夹的路径
#     StaticDir = $${PWD}/static
#     # 将路径中的"/"替换为"\"
#     StaticDir = $$replace(StaticDir, /, \\)
#     #message($${StaticDir})
#     # 使用xcopy命令拷贝文件夹，/E表示拷贝子目录及其内容，包括空目录。/I表示如果目标不存在则创建目录。/Y表示覆盖现有文件而不提示。
#      QMAKE_POST_LINK += xcopy /Y /E /I \"$$StaticDir\" \"$$OutputDir\\static\\\"
# }
win32-msvc*:QMAKE_CXXFLAGS += /wd"4819" /utf-8
