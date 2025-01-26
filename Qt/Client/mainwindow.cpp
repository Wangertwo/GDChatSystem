#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "gdtcpmgr.h"
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    _login_dialog = new GDLoginDialog(this);
    _login_dialog->setWindowFlags(Qt::CustomizeWindowHint|Qt::FramelessWindowHint);
    setCentralWidget(_login_dialog);

    connect(_login_dialog, SIGNAL(emitRegisterClicked()), this, SLOT(slotShowRegisterDialog()));
    connect(_login_dialog, SIGNAL(emitForgetPwdClicked()), this, SLOT(slotShowForgetPwdDialog()));
    connect(GDTcpMgr::GetInstance().get(),&GDTcpMgr::sig_swich_chatdlg, this, &MainWindow::slotShowChatDialog);

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::slotShowRegisterDialog()
{ // 切换到注册页面
    _register_dialog = new GDRegisterDialog(this);
    _register_dialog->setWindowFlags(Qt::CustomizeWindowHint|Qt::FramelessWindowHint);
    this->setCentralWidget(_register_dialog);
    
    connect(_register_dialog, &GDRegisterDialog::regSwitchLogin, this, &MainWindow::slotShowLoginDialog);
}

void MainWindow::slotShowLoginDialog()
{ // 切换到登入页面
    _login_dialog = new GDLoginDialog(this);
    _login_dialog->setWindowFlags(Qt::CustomizeWindowHint|Qt::FramelessWindowHint);
    this->setCentralWidget(_login_dialog);

    connect(_login_dialog, &GDLoginDialog::emitRegisterClicked, this, &MainWindow::slotShowRegisterDialog);
    connect(_login_dialog, &GDLoginDialog::emitForgetPwdClicked, this, &MainWindow::slotShowForgetPwdDialog);
}

void MainWindow::slotShowForgetPwdDialog()
{ // 切换到重置密码页面
    _reset_dialog = new GDResetDialog(this);
    _reset_dialog->setWindowFlags(Qt::CustomizeWindowHint|Qt::FramelessWindowHint);
    this->setCentralWidget(_reset_dialog);

    //返回登录页面
    connect(_reset_dialog, &GDResetDialog::emitReturnLoginSignal, this, &MainWindow::slotShowLoginDialog);
}

void MainWindow::slotShowChatDialog()
{
    _chat_dialog = new GDChatDialog(this);
    _chat_dialog->setWindowFlags(Qt::CustomizeWindowHint|Qt::FramelessWindowHint);
    this->setCentralWidget(_chat_dialog);

    this->setMinimumSize(QSize(1050,900));
    this->setMaximumSize(QWIDGETSIZE_MAX, QWIDGETSIZE_MAX);
}



