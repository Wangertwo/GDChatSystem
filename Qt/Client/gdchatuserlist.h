#ifndef GDCHATUSERLIST_H
#define GDCHATUSERLIST_H

#include <QListWidget>
#include <QWheelEvent>
#include <QEvent>
#include <QScrollBar>
#include <QDebug>
class GDChatUserList : public QListWidget
{
    Q_OBJECT
public:
    GDChatUserList(QWidget *parent = nullptr);
protected:
    bool eventFilter(QObject *watched, QEvent *event) override;
private:
    bool _load_pending;
signals:
    void sig_loading_chat_user();
};

#endif // GDCHATUSERLIST_H
