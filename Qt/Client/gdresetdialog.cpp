#include "gdresetdialog.h"
#include "ui_gdresetdialog.h"
#include "gdhttpmgr.h"
GDResetDialog::GDResetDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::GDResetDialog)
{
    ui->setupUi(this);
    ui->newPwdLEdit->setEchoMode(QLineEdit::Password);
    ui->password_visible->setState("unvisible", "unvisible_hover", "", "visible", "visible_hover", "");
    connect(ui->returnPBtn, SIGNAL(clicked()), this, SLOT(slotReturnLoginClicked()));
    ui->errTip->clear();
    connect(ui->userLEdit, &QLineEdit::editingFinished, this, [this]() {
        checkUserValid();
    });
    connect(ui->emailLEdit, &QLineEdit::editingFinished, this, [this]() {
        checkEmailValid();
    });
    connect(ui->varifyCodeLEdit, &QLineEdit::editingFinished, this, [this]() {
        checkVarifyCodeValid();
    });
    connect(ui->newPwdLEdit, &QLineEdit::editingFinished, this, [this]() {
        checkNewPwdValid();
    });

    connect(ui->password_visible, &GDClickedLabel::clicked, this, [this]() {
        auto state = ui->password_visible->getCurState();
        if(state == ClickLbState::NORMAL) {
            ui->newPwdLEdit->setEchoMode(QLineEdit::Password);
        }else {
            ui->newPwdLEdit->setEchoMode(QLineEdit::Normal);
        }
    });
    connect(ui->confirmPBtn, &QPushButton::clicked, this, &GDResetDialog::slotConfirmPBtnClicked);
    connect(ui->obtainPBtn, &QPushButton::clicked, this, &GDResetDialog::slotObtainPBtnClicked);
    connect(GDHttpMgr::GetInstance().get(), &GDHttpMgr::sig_reset_mod_finish, this, &GDResetDialog::slot_reset_mod_finish);
    initHandlers();
}

GDResetDialog::~GDResetDialog()
{
    qDebug() << "GDResetDialog destoried";
    delete ui;
}

void GDResetDialog::addErrTips(TipErr te, QString tm)
{
    _tip_errs[te] = tm;
    showErrTips(tm, false);
}

void GDResetDialog::delErrTips(TipErr te)
{
    if(_tip_errs.contains(te)) {
        _tip_errs.remove(te);
    }
    if(_tip_errs.empty()) {
        ui->errTip->clear();
        return;
    }
    showErrTips(_tip_errs.first(), false);
}

void GDResetDialog::showErrTips(QString tm, bool ok)
{
    if(ok) {
        ui->errTip->setProperty("state", "normal");
    }else {
        ui->errTip->setProperty("state", "error");
    }
    ui->errTip->setText(tm);
    repolish(ui->errTip);
}

void GDResetDialog::initHandlers()
{
    _handlers.insert(ReqID::ID_RESET_PWD, [this](QJsonObject& jsonObj) {
        int error = jsonObj["error"].toInt();
        if(error != ErrorCodes::SUCCESS) {
            showErrTips("参数错误", false);
            return;
        }
        for(auto it = jsonObj.begin(); it != jsonObj.end(); it++) {
            qDebug() << it.key() << " is " << it.value();
        }
        showErrTips("密码重置成功", true);
        changeTipPage();
        return;
    });

    _handlers.insert(ReqID::ID_GET_VARIFY_CODE, [this](QJsonObject& jsonObj) {
        int error = jsonObj["error"].toInt();
        if(error != ErrorCodes::SUCCESS) {
            showErrTips("邮箱发送失败", false);
            return;
        }
        QString email = jsonObj["email"].toString();
        qDebug() << "Email is " << email;
        showErrTips(tr("验证码已发送到邮箱，请查收"), true);
    });
}

void GDResetDialog::changeTipPage()
{
    QMessageBox::information(this, "成功", "密码重置成功，返回登录页面", QMessageBox::Ok, QMessageBox::NoButton);
    emit emitReturnLoginSignal();
}

void GDResetDialog::checkUserValid()
{
    QString user = ui->userLEdit->text();
    if(user == "") {
        addErrTips(TipErr::TIP_USER_ERR, tr("用户名不能为空"));
        return;
    }
    delErrTips(TipErr::TIP_USER_ERR);
    return;
}

void GDResetDialog::checkEmailValid()
{
    //验证邮箱的地址正则表达式
    auto email = ui->emailLEdit->text();
    // 邮箱地址的正则表达式
    QRegularExpression regex(R"((\w+)(\.|_)?(\w*)@(\w+)(\.(\w+))+)");
    bool match = regex.match(email).hasMatch(); // 执行正则表达式匹配
    if(!match){
        //提示邮箱不正确
        addErrTips(TipErr::TIP_EMAIL_ERR, tr("邮箱地址不正确"));
        return;
    }
    delErrTips(TipErr::TIP_EMAIL_ERR);
    return;
}

void GDResetDialog::checkVarifyCodeValid()
{
    auto vc = ui->varifyCodeLEdit->text();
    if(vc.isEmpty()) {
        addErrTips(TipErr::TIP_VARIFY_ERR, tr("验证码不能为空"));
        return;
    }
    delErrTips(TipErr::TIP_VARIFY_ERR);
    return;
}

void GDResetDialog::checkNewPwdValid()
{
    QString pwd = ui->newPwdLEdit->text();
    if(pwd.length() < 6 || pwd.length() > 15) {
        addErrTips(TipErr::TIP_PWD_ERR, tr("密码长度不合法（6~15）"));
        return;
    }
    // 创建一个正则表达式对象，按照上述密码要求
    // 这个正则表达式解释：
    // ^[a-zA-Z0-9!@#$%^&*]{6,15}$ 密码长度至少6，可以是字母、数字和特定的特殊字符
    QRegularExpression regExp("^[a-zA-Z0-9!@#$%^&*]{6,15}$");
    bool match = regExp.match(pwd).hasMatch();
    if(!match){
        //提示字符非法
        addErrTips(TipErr::TIP_PWD_ERR, tr("不能包含非法字符"));
        return;
    }
    delErrTips(TipErr::TIP_PWD_ERR);
    return;
}

void GDResetDialog::slotReturnLoginClicked()
{ // emit signal
    emit emitReturnLoginSignal();
}

void GDResetDialog::slotConfirmPBtnClicked()
{ // 重置密码
    if(!_tip_errs.empty()) {
        showErrTips(_tip_errs.first(), false);
        return;
    }
    // 发送http请求重置密码
    QJsonObject json_obj;
    json_obj["user"] = ui->userLEdit->text();
    json_obj["email"] = ui->emailLEdit->text();
    json_obj["varifyCode"] = ui->varifyCodeLEdit->text();
    json_obj["newPwd"] = xorString(ui->newPwdLEdit->text());
    GDHttpMgr::GetInstance()->postHttpRequest(gate_url_prefix+"/reset_pwd",
                                              ReqID::ID_RESET_PWD,
                                              json_obj,
                                              Modules::RESETMOD);
}

void GDResetDialog::slotObtainPBtnClicked()
{ // 获取验证码
    QJsonObject json_obj;
    json_obj["email"] = ui->emailLEdit->text();
    if(_tip_errs.find(TipErr::TIP_EMAIL_ERR) != _tip_errs.end()) {
        showErrTips("验证码不能为空", false);
        return;
    }
    GDHttpMgr::GetInstance()->postHttpRequest(gate_url_prefix+"/get_varifycode",
                                              ReqID::ID_GET_VARIFY_CODE,
                                              json_obj,
                                              Modules::RESETMOD);
}

void GDResetDialog::slot_reset_mod_finish(ReqID id, QString str, ErrorCodes err)
{
    if(err != ErrorCodes::SUCCESS) {
        showErrTips("网络传输错误", false);
        return;
    }

    QJsonDocument jsonDoc = QJsonDocument::fromJson(str.toUtf8());
    if(jsonDoc.isNull()) {
        showErrTips("json解析错误", false);
        return;
    }
    if(!jsonDoc.isObject()) {
        showErrTips("json解析错误", false);
        return;
    }
    QJsonObject jsonObj = jsonDoc.object();
    _handlers[id](jsonObj);
}
