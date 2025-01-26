#ifndef GDREGISTERDIALOG_H
#define GDREGISTERDIALOG_H

#include <QDialog>
#include "gdhttpmgr.h"
#include <QTimer>
namespace Ui {
class GDRegisterDialog;
}

class GDRegisterDialog : public QDialog
{
    Q_OBJECT

public:
    explicit GDRegisterDialog(QWidget *parent = nullptr);
    ~GDRegisterDialog();
    void showErrorTip(QString str, bool ok);
    void initIDHandlers();
    void addTipError(TipErr te, QString tm);
    void delTipError(TipErr te);
    void changTipPage();
private:
    Ui::GDRegisterDialog *ui;
    QMap<ReqID, std::function<void(QJsonObject&)>> _handlers;
    QMap<TipErr, QString> _tip_errs;
    QTimer *_timer;
    int _counter;

private slots:
    void obtainPBtnClicked();
    void confirmPBtnClicked();
    void checkUserVaild();
    void checkPwdValid();
    void checkConfirmValid();
    void checkEmailValid();
    void checkVarifyValid();
    void slot_reg_mod_finish(ReqID id, QString str, ErrorCodes err);
    void slotSwitchLoginDialog();
signals:
    void regSwitchLogin();
};

#endif // GDREGISTERDIALOG_H
