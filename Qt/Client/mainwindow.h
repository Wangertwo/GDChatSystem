#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "gdregisterdialog.h"
#include "gdlogindialog.h"
#include "gdresetdialog.h"
#include "gdchatdialog.h"
QT_BEGIN_NAMESPACE

namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    GDLoginDialog *_login_dialog;
    GDRegisterDialog *_register_dialog;
    GDResetDialog *_reset_dialog;
    GDChatDialog *_chat_dialog;
private slots:
    void slotShowRegisterDialog();
    void slotShowLoginDialog();
    void slotShowForgetPwdDialog();
    void slotShowChatDialog();
};
#endif // MAINWINDOW_H
