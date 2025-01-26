#ifndef GDHTTPMGR_H
#define GDHTTPMGR_H

#include <QObject>
#include "Singleton.h"

class GDHttpMgr : public QObject, public Singleton<GDHttpMgr>, public std::enable_shared_from_this<GDHttpMgr>
{
    Q_OBJECT
    friend class Singleton<GDHttpMgr>;
private:
    explicit GDHttpMgr(QObject *parent = nullptr);
    QNetworkAccessManager _manager;
public:
    ~GDHttpMgr();
    void postHttpRequest(QUrl url, ReqID id, QJsonObject json, Modules mod);
private slots:
    void slot_http_finish(ReqID id, QString str, ErrorCodes err, Modules mod);

signals:
    void sig_http_finish(ReqID id, QString str, ErrorCodes err, Modules mod);
    void sig_reg_mod_finish(ReqID id, QString str, ErrorCodes err);
    void sig_reset_mod_finish(ReqID id, QString str, ErrorCodes err);
    void sig_login_mod_finish(ReqID id, QString str, ErrorCodes err);
};

#endif // GDHTTPMGR_H
