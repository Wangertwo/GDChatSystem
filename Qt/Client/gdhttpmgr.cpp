#include "gdhttpmgr.h"

GDHttpMgr::GDHttpMgr(QObject *parent)
    : QObject{parent}
{
    connect(this, SIGNAL(sig_http_finish(ReqID,QString,ErrorCodes,Modules)), this, SLOT(slot_http_finish(ReqID,QString,ErrorCodes,Modules)));
}
GDHttpMgr::~GDHttpMgr() {

}
void GDHttpMgr::postHttpRequest(QUrl url, ReqID id, QJsonObject json, Modules mod)
{ // 发送http请求
    QByteArray data = QJsonDocument(json).toJson();
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    request.setHeader(QNetworkRequest::ContentLengthHeader, QByteArray::number(data.length()));

    auto self = shared_from_this();
    QNetworkReply* reply = _manager.post(request, data);
    QObject::connect(reply, &QNetworkReply::finished, [self, id, reply, mod](){
        // 回包错误
        if(reply->error() != QNetworkReply::NoError) {
            qDebug() << reply->errorString();
            emit self->sig_http_finish(id, "", ErrorCodes::ERR_NETWORK, mod);
            reply->deleteLater();
            return;
        }
        // 回包成功
        QString res = reply->readAll();
        emit self->sig_http_finish(id, res, ErrorCodes::SUCCESS, mod);
        reply->deleteLater();
        return;
    });
}

void GDHttpMgr::slot_http_finish(ReqID id, QString str, ErrorCodes err, Modules mod)
{ // 发送信号处理回包逻辑
    if(mod == Modules::REGISTERMOD) { // 待处理的模块（注册模块）
        emit sig_reg_mod_finish(id, str, err);
    }

    if(mod == Modules::RESETMOD) {
        emit sig_reset_mod_finish(id, str, err);
    }

    if(mod == Modules::LOGINUSER) {
        emit sig_login_mod_finish(id, str, err);
    }
}
