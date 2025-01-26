#include "gdlogindialog.h"
#include "ui_gdlogindialog.h"
#include "gdhttpmgr.h"
#include "gdtcpmgr.h"
#include "applyfriend.h"
#include "findsuccessdlg.h"
#include "gdchatdialog.h"
#include <QTimer>
GDLoginDialog::GDLoginDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::GDLoginDialog)
{
    ui->setupUi(this);
    initHead();
    initHandlers();
    ui->passwordLEdit->setEchoMode(QLineEdit::Password);
    ui->password_visible->setState("unvisible", "unvisible_hover", "", "visible", "visible_hover", "");
    ui->forgetLabel->setState("normal", "normal_hover", "", "selected", "selected_hover", "");

    connect(ui->emailLEdit, &QLineEdit::editingFinished, this, [this]() {
        checkEmailVaild();
    });

    connect(ui->passwordLEdit, &QLineEdit::editingFinished, this, [this]() {
        checkPwdVaild();
    });

    connect(ui->password_visible, &GDClickedLabel::clicked, this, [this]() {
        auto state = ui->password_visible->getCurState();
        if(state == ClickLbState::NORMAL) {
            ui->passwordLEdit->setEchoMode(QLineEdit::Password);
        }else {
            ui->passwordLEdit->setEchoMode(QLineEdit::Normal);
        }
    });
    connect(ui->forgetLabel, &GDClickedLabel::clicked, this, &GDLoginDialog::slotForgetPwdClicked);
    connect(ui->registerPBtn, SIGNAL(clicked()), this, SLOT(slotRegisterClicked()));
    connect(ui->loginPBtn, &QPushButton::clicked, this, &GDLoginDialog::slotLoginClicked);

    connect(GDHttpMgr::GetInstance().get(), &GDHttpMgr::sig_login_mod_finish, this, &GDLoginDialog::slot_login_mod_finish);
    //连接tcp连接请求的信号和槽函数
    connect(this, &GDLoginDialog::sig_connect_tcp, GDTcpMgr::GetInstance().get(), &GDTcpMgr::slot_tcp_connect);
    //连接tcp管理者发出的连接成功信号
    connect(GDTcpMgr::GetInstance().get(), &GDTcpMgr::sig_con_success, this, &GDLoginDialog::slot_tcp_con_finish);
}
GDLoginDialog::~GDLoginDialog()
{
    qDebug() << "GDLoginDialog destroied !";
    delete ui;
}

bool GDLoginDialog::enableBtn(bool enableBtn)
{
    ui->loginPBtn->setEnabled(enableBtn);
    ui->registerPBtn->setEnabled(enableBtn);
    return true;
}

void GDLoginDialog::initHead()
{ // 初始化头像
    // 加载图片
    QPixmap originalPixmap(":/rec/rec/head_6.jpg");
        // 设置图片自动缩放
    qDebug()<< originalPixmap.size() << ui->headPixmap->size();
    originalPixmap = originalPixmap.scaled(ui->headPixmap->size(),
                                           Qt::KeepAspectRatio, Qt::SmoothTransformation);

    // 创建一个和原始图片相同大小的QPixmap，用于绘制圆角图片
    QPixmap roundedPixmap(originalPixmap.size());
    roundedPixmap.fill(Qt::transparent); // 用透明色填充

    QPainter painter(&roundedPixmap);
    painter.setRenderHint(QPainter::Antialiasing); // 设置抗锯齿，使圆角更平滑
    painter.setRenderHint(QPainter::SmoothPixmapTransform);

    // 使用QPainterPath设置圆角
    QPainterPath path;
    path.addRoundedRect(0, 0, originalPixmap.width(), originalPixmap.height(), 10, 10); // 最后两个参数分别是x和y方向的圆角半径
    painter.setClipPath(path);

    // 将原始图片绘制到roundedPixmap上
    painter.drawPixmap(0, 0, originalPixmap);

    // 设置绘制好的圆角图片到QLabel上
    ui->headPixmap->setPixmap(roundedPixmap);
}

void GDLoginDialog::initHandlers()
{
    _handlers.insert(ReqID::ID_LOGIN_USER, [this](QJsonObject& jsonObj) {
        int error = jsonObj["error"].toInt();
        if(error != ErrorCodes::SUCCESS) {
            showErrTips(tr("参数错误"), false);
            enableBtn(true);
            return;
        }
        qDebug() << "user login success";
        ServerInfo si;
        //发送信号通知tcpMgr发送长链接
        si.Uid = jsonObj["uid"].toInt();
        si.Host = jsonObj["host"].toString();
        si.Port = jsonObj["port"].toString();
        si.Token = jsonObj["token"].toString();
        // user数据寄存
        _uid = si.Uid;
        _token = si.Token;
        for(auto it = jsonObj.begin(); it != jsonObj.end(); it++) {
            qDebug() << it.key() << " is " << it.value();
        }
        showErrTips(tr("用户登录成功"), true);
        emit sig_connect_tcp(si);
    });
}

void GDLoginDialog::checkEmailVaild()
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

void GDLoginDialog::checkPwdVaild()
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

void GDLoginDialog::addTipError(TipErr te, QString tm)
{
    _tip_errs[te] = tm;
    showErrTips(tm, false);
}

void GDLoginDialog::delTipError(TipErr te)
{
    _tip_errs.remove(te);
    if(_tip_errs.empty()) {
        ui->errTip->clear();
        return;
    }
    showErrTips(_tip_errs.first(), false);
}

void GDLoginDialog::showErrTips(QString tm, bool ok)
{
    if(ok) {
        ui->errTip->setProperty("state", "normal");
    }else {
        ui->errTip->setProperty("state", "error");
    }
    ui->errTip->setText(tm);
    repolish(ui->errTip);
}

void GDLoginDialog::slotRegisterClicked()
{ // send click_signal to mainwindow

    emit emitRegisterClicked();
}

void GDLoginDialog::slotForgetPwdClicked()
{ // send click_signal to mainwindow
    qDebug() << "emit forgetPwd signal";
    emit emitForgetPwdClicked();
}

void GDLoginDialog::slotLoginClicked()
{ // 登录按钮
    if(!_tip_errs.empty()) {
        showErrTips(_tip_errs.first(), false);
        return;
    }
    enableBtn(false);
    QJsonObject json_obj;
    json_obj["email"] = ui->emailLEdit->text();
    json_obj["pwd"] = xorString(ui->passwordLEdit->text());
    GDHttpMgr::GetInstance()->postHttpRequest(gate_url_prefix+"/user_login",
                                              ReqID::ID_LOGIN_USER,
                                              json_obj,
                                              Modules::LOGINUSER);
    enableBtn(true);
}

void GDLoginDialog::slot_login_mod_finish(ReqID id, QString str, ErrorCodes err)
{ // 处理回包
    if(err != ErrorCodes::SUCCESS) {
        showErrTips(tr("网络传输错误"), false);
        return;
    }
    QJsonDocument jsonDoc = QJsonDocument::fromJson(str.toUtf8());
    if(jsonDoc.isNull()) {
        showErrTips(tr("json解析错误"), false);
        return;
    }
    if(!jsonDoc.isObject()) {
        showErrTips(tr("json解析错误"), false);
        return;
    }

    QJsonObject jsonObj = jsonDoc.object();
    // 处理回包逻辑
    _handlers[id](jsonObj);
    return;
}

void GDLoginDialog::slot_tcp_con_finish(bool bsuccess)
{
    if(bsuccess){
        showErrTips(tr("聊天服务连接成功，正在登录..."),true);
        QJsonObject jsonObj;
        jsonObj["uid"] = _uid;
        jsonObj["token"] = _token;
        QJsonDocument doc(jsonObj);
        QByteArray jsonData = doc.toJson(QJsonDocument::Indented);

        //发送tcp请求给chat server
        emit GDTcpMgr::GetInstance()->sig_send_data(ReqID::ID_CHAT_LOGIN, jsonData);

    }else{
        showErrTips(tr("网络异常"),false);
        enableBtn(true);
    }
}

void GDLoginDialog::slot_login_failed(int err)
{
    QString result = QString("登录失败, err is %1")
                         .arg(err);
    showErrTips(result,false);
    enableBtn(true);
}
