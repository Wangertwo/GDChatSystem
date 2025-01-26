#ifndef GDTCPMGR_H
#define GDTCPMGR_H
#include <QObject>
#include <QTcpSocket>
#include "singleton.h"
#include "global.h"
#include <functional>
#include "userdata.h"
#include <QJsonArray>

class GDTcpMgr:public QObject, public Singleton<GDTcpMgr>,
                 public std::enable_shared_from_this<GDTcpMgr>
{
    Q_OBJECT
    friend class Singleton<GDTcpMgr>;
public:
    ~GDTcpMgr();
private:
    GDTcpMgr();
    void initHandlers();
    void handleMsg(ReqID id, int len, QByteArray data);
    QTcpSocket _socket;
    QString _host;
    uint16_t _port;
    QByteArray _buffer;
    bool _b_recv_pending;
    quint16 _message_id;
    quint16 _message_len;
    QMap<ReqID, std::function<void(ReqID id, int len, QByteArray data)>> _handlers;
public slots:
    void slot_tcp_connect(ServerInfo);
    void slot_send_data(ReqID reqId, QByteArray data);
signals:
    void sig_con_success(bool bsuccess);
    void sig_send_data(ReqID reqId, QByteArray data);
    void sig_swich_chatdlg();
    void sig_login_failed(int);
    void sig_user_search(std::shared_ptr<SearchInfo>);
    void sig_load_apply_list(QJsonArray json_array);
    void sig_friend_apply(std::shared_ptr<AddFriendApply>);
    void sig_add_auth_friend(std::shared_ptr<AuthInfo>);
    void sig_auth_rsp(std::shared_ptr<AuthRsp>);
    void sig_text_chat_msg(std::shared_ptr<TextChatMsg> msg);
};

#endif // GDTCPMGR_H
