#ifndef GDRESETDIALOG_H
#define GDRESETDIALOG_H

#include <QDialog>
#include "global.h"
#include <QMessageBox>
namespace Ui {
class GDResetDialog;
}

class GDResetDialog : public QDialog
{
    Q_OBJECT

public:
    explicit GDResetDialog(QWidget *parent = nullptr);
    ~GDResetDialog();
    void addErrTips(TipErr te, QString tm);
    void delErrTips(TipErr te);
    void showErrTips(QString tm, bool ok);
    void initHandlers();
    void changeTipPage();
private:
    Ui::GDResetDialog *ui;
    QMap<TipErr, QString> _tip_errs;
    QMap<ReqID, std::function<void(QJsonObject&)>> _handlers;
    void checkUserValid();
    void checkEmailValid();
    void checkVarifyCodeValid();
    void checkNewPwdValid();
private slots:
    void slotReturnLoginClicked();
    void slotConfirmPBtnClicked();
    void slotObtainPBtnClicked();
    void slot_reset_mod_finish(ReqID id, QString str, ErrorCodes err);
signals:
    void emitReturnLoginSignal();
};

#endif // GDRESETDIALOG_H
