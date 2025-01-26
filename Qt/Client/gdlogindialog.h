#ifndef GDLOGINDIALOG_H
#define GDLOGINDIALOG_H

#include <QDialog>
#include <QPainter>
#include <QPainterPath>
#include "global.h"
namespace Ui {
class GDLoginDialog;
}

class GDLoginDialog : public QDialog
{
    Q_OBJECT

public:
    explicit GDLoginDialog(QWidget *parent = nullptr);
    ~GDLoginDialog();
private:
    Ui::GDLoginDialog *ui;
    QMap<TipErr, QString> _tip_errs;
    QMap<ReqID, std::function<void(QJsonObject& jsonObj)>> _handlers;
    int _uid;
    QString _token;
    bool enableBtn(bool);
    void initHead();
    void initHandlers();
    void checkEmailVaild();
    void checkPwdVaild();
    void addTipError(TipErr te, QString tm);
    void delTipError(TipErr te);
    void showErrTips(QString tm, bool ok);
private slots:
    void slotRegisterClicked();
    void slotForgetPwdClicked();
    void slotLoginClicked();
    void slot_login_mod_finish(ReqID id, QString str, ErrorCodes err);
    void slot_tcp_con_finish(bool bsuccess);
    void slot_login_failed(int);
signals:
    void emitRegisterClicked();
    void emitForgetPwdClicked();
    void sig_connect_tcp(ServerInfo si);
};

#endif // GDLOGINDIALOG_H
