#include "gdregisterdialog.h"
#include "ui_gdregisterdialog.h"
GDRegisterDialog::GDRegisterDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::GDRegisterDialog), _counter(5)
{
    ui->setupUi(this);
    ui->passwordLEdit->setEchoMode(QLineEdit::Password);
    ui->confirmLEdit->setEchoMode(QLineEdit::Password);
    ui->passwordLEdit->setCursor(Qt::PointingHandCursor);
    ui->confirmLEdit->setCursor(Qt::PointingHandCursor);
    ui->password_visible->setState("unvisible", "unvisible_hover", "", "visible", "visible_hover", "");
    ui->confirm_visible->setState("unvisible", "unvisible_hover", "", "visible", "visible_hover", "");
    ui->errTip->clear();
    connect(ui->userLEdit, &QLineEdit::editingFinished, this, [this]() {
        checkUserVaild();
    });
    connect(ui->passwordLEdit, &QLineEdit::editingFinished, this, [this]() {
        checkPwdValid();
    });
    connect(ui->emailLEdit, &QLineEdit::editingFinished, this, [this]() {
        checkEmailValid();
    });
    connect(ui->confirmLEdit, &QLineEdit::editingFinished, this, [this]() {
        checkConfirmValid();
    });
    connect(ui->varifyLEdit, &QLineEdit::editingFinished, this, [this]() {
        checkVarifyValid();
    });

    connect(ui->password_visible, &GDClickedLabel::clicked, this, [this]() {
        auto state = ui->password_visible->getCurState();
        if(state == ClickLbState::NORMAL) {
            ui->passwordLEdit->setEchoMode(QLineEdit::Password);
        }else {
            ui->passwordLEdit->setEchoMode(QLineEdit::Normal);
        }
    });

    connect(ui->confirm_visible, &GDClickedLabel::clicked, this, [this]() {
        auto state = ui->confirm_visible->getCurState();
        if(state == ClickLbState::NORMAL) {
            ui->confirmLEdit->setEchoMode(QLineEdit::Password);
        }else {
            ui->confirmLEdit->setEchoMode(QLineEdit::Normal);
        }
    });

    _timer = new QTimer(this);
    connect(_timer, &QTimer::timeout, [this]() {
        if(_counter == 0) {
            _timer->stop();
            emit regSwitchLogin();
            return;
        }
        _counter--;
        auto str = QString("注册成功，%1秒后返回登录页面").arg(_counter);
        ui->label_timer->setText(str);
    });

    connect(ui->returnLoginPBtn, &QPushButton::clicked, this, &GDRegisterDialog::slotSwitchLoginDialog, Qt::UniqueConnection);
    connect(ui->cancelPBtn, &QPushButton::clicked, this, &GDRegisterDialog::slotSwitchLoginDialog, Qt::UniqueConnection);
    connect(ui->obtainPBtn, SIGNAL(clicked()), this, SLOT(obtainPBtnClicked()));
    connect(ui->confirmPBtn, SIGNAL(clicked()), this, SLOT(confirmPBtnClicked()));

    connect(GDHttpMgr::GetInstance().get(), SIGNAL(sig_reg_mod_finish(ReqID,QString,ErrorCodes)), this, SLOT(slot_reg_mod_finish(ReqID,QString,ErrorCodes)));
    initIDHandlers();
}

GDRegisterDialog::~GDRegisterDialog()
{
    qDebug() << "GDRegisterDialog destroied !";
    delete ui;
}

void GDRegisterDialog::obtainPBtnClicked()
{ // 获取验证码
    QString email = ui->emailLEdit->text();
    QRegularExpression rx(R"(^[a-zA-Z0-9.!#$%&'*+/=?^_`{|}~-]+@[a-zA-Z0-9](?:[a-zA-Z0-9-]{0,61}[a-zA-Z0-9])?(?:\.[a-zA-Z0-9](?:[a-zA-Z0-9-]{0,61}[a-zA-Z0-9])?)*$)");
    bool match = rx.match(email).hasMatch(); // 正则表达式匹配
    if(match) {
        qDebug() << "Match email success!";
        // 发送http请求获取验证码
        QJsonObject json_obj;
        json_obj["email"] = email;
        GDHttpMgr::GetInstance()->postHttpRequest(QUrl(gate_url_prefix+"/get_varifycode"),
                                                 ReqID::ID_GET_VARIFY_CODE,
                                                 json_obj,
                                                 Modules::REGISTERMOD);

    }else {
        showErrorTip(tr("邮箱地址不正确"), false);
    }
}

void GDRegisterDialog::confirmPBtnClicked()
{ // 确认按钮
    if(!_tip_errs.empty()) {
        showErrorTip(_tip_errs.first(), false);
        return;
    }
    // 发送http请求注册用户
    QJsonObject json_obj;
    json_obj["user"] = ui->userLEdit->text();
    json_obj["password"] = xorString(ui->passwordLEdit->text());
    json_obj["email"] = ui->emailLEdit->text();
    json_obj["confirm"] = xorString(ui->confirmLEdit->text());
    json_obj["icon"] = ui->passwordLEdit->text();
    json_obj["varifycode"] = ui->varifyLEdit->text();
    // json_obj["desc"] = "我是sy";
    // json_obj["sex"] = 1;
    // json_obj["nick"] = ui->userLEdit->text();
    GDHttpMgr::GetInstance()->postHttpRequest(QUrl(gate_url_prefix+"/get_register"),
                                              ReqID::ID_REG_USER,
                                              json_obj,
                                              Modules::REGISTERMOD);

}

void GDRegisterDialog::checkUserVaild()
{
    if(ui->userLEdit->text() == "") {
        addTipError(TipErr::TIP_USER_ERR, tr("用户名不能为空"));
        return;
    }
    delTipError(TipErr::TIP_USER_ERR);
    return;
}

void GDRegisterDialog::checkPwdValid()
{
    QString pwd = ui->passwordLEdit->text();
    if(pwd.length() < 6 || pwd.length() > 15) {
        addTipError(TipErr::TIP_PWD_ERR, tr("密码长度不合法（6~15）"));
        return;
    }
    // 创建一个正则表达式对象，按照上述密码要求
    // 这个正则表达式解释：
    // ^[a-zA-Z0-9!@#$%^&*]{6,15}$ 密码长度至少6，可以是字母、数字和特定的特殊字符
    QRegularExpression regExp("^[a-zA-Z0-9!@#$%^&*]{6,15}$");
    bool match = regExp.match(pwd).hasMatch();
    if(!match){
        //提示字符非法
        addTipError(TipErr::TIP_PWD_ERR, tr("不能包含非法字符"));
        return;
    }
    delTipError(TipErr::TIP_PWD_ERR);
    return;
}

void GDRegisterDialog::checkConfirmValid()
{
    QString conPwd = ui->confirmLEdit->text();
    QString pwd = ui->passwordLEdit->text();
    if(pwd != conPwd) {
        addTipError(TipErr::TIP_PWD_CONFIRM, tr("密码和确认密码不匹配"));
        return;
    }
    delTipError(TipErr::TIP_PWD_CONFIRM);
    return;
}

void GDRegisterDialog::checkEmailValid()
{
    //验证邮箱的地址正则表达式
    auto email = ui->emailLEdit->text();
    // 邮箱地址的正则表达式
    QRegularExpression regex(R"((\w+)(\.|_)?(\w*)@(\w+)(\.(\w+))+)");
    bool match = regex.match(email).hasMatch(); // 执行正则表达式匹配
    if(!match){
        //提示邮箱不正确
        addTipError(TipErr::TIP_EMAIL_ERR, tr("邮箱地址不正确"));
        return;
    }
    delTipError(TipErr::TIP_EMAIL_ERR);
    return;
}

void GDRegisterDialog::checkVarifyValid()
{
    QString varifyCode = ui->varifyLEdit->text();
    if(varifyCode.isEmpty()) {
        addTipError(TipErr::TIP_VARIFY_ERR, tr("验证码不能为空"));
        return;
    }
    delTipError(TipErr::TIP_VARIFY_ERR);
    return;
}

void GDRegisterDialog::showErrorTip(QString str, bool ok)
{ // 展示提示信息
    if(ok) {
        ui->errTip->setProperty("state", "normal");
    }else {
        ui->errTip->setProperty("state", "error");
    }
    ui->errTip->setText(str);
    repolish(ui->errTip);
}

void GDRegisterDialog::initIDHandlers()
{ // 初始化ID回调者

    // 注册获取验证码回包逻辑
    _handlers.insert(ReqID::ID_GET_VARIFY_CODE, [this](QJsonObject& jsonObj) {
        int error = jsonObj["error"].toInt();
        if(error != ErrorCodes::SUCCESS) {
            showErrorTip(tr("参数错误"), false);
            return;
        }
        QString email = jsonObj["email"].toString();
        qDebug() << "Email is: " << email;
        showErrorTip(tr("验证码已发送到邮箱，请查收"), true);
    });

    // 注册获取用户回包逻辑
    _handlers.insert(ReqID::ID_REG_USER, [this](QJsonObject& jsonObj) {
        int error = jsonObj["error"].toInt();
        if(error != ErrorCodes::SUCCESS) {
            showErrorTip(tr("参数错误"), false);
            return;
        }
        for(QJsonObject::Iterator it = jsonObj.begin(); it != jsonObj.end(); it++) {
            qDebug() << it.key() << " is: " << it.value();
        }
        showErrorTip(tr("用户注册成功!"), true);
        changTipPage();
    });
}

void GDRegisterDialog::addTipError(TipErr te, QString tm)
{ // 缓存错误信息
    _tip_errs[te] = tm;
    showErrorTip(tm, false);
}

void GDRegisterDialog::delTipError(TipErr te)
{ // 删除错误信息
    _tip_errs.remove(te);
    if(_tip_errs.empty()) {
        ui->errTip->clear();
        return;
    }
    showErrorTip(_tip_errs.first(), false);
}

void GDRegisterDialog::changTipPage()
{ // 切换页面
    _timer->stop();
    ui->stackedWidget->setCurrentWidget(ui->page_2);
    _timer->start(1000);
}

void GDRegisterDialog::slot_reg_mod_finish(ReqID id, QString str, ErrorCodes err)
{
    if(err != ErrorCodes::SUCCESS) {
        showErrorTip(tr("网络传输错误"), false);
        return;
    }

    QJsonDocument jsonDoc = QJsonDocument::fromJson(str.toUtf8());

    if(jsonDoc.isNull()) {
        showErrorTip(tr("Json解析错误"), false);
        return;
    }

    if(!jsonDoc.isObject()) {
        showErrorTip(tr("Json解析错误"), false);
        return;
    }

    QJsonObject jsonObj = jsonDoc.object(); // 回包解析出的json对象
    _handlers[id](jsonObj); // 根据id处理逻辑
}

void GDRegisterDialog::slotSwitchLoginDialog()
{
    _timer->stop();
    emit regSwitchLogin();
}
