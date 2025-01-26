#ifndef GDCLICKEDLABEL_H
#define GDCLICKEDLABEL_H
#include <QLabel>
#include <QMouseEvent>
#include <QEnterEvent>
#include <QEvent>
#include "global.h"
class GDClickedLabel: public QLabel
{
    Q_OBJECT
public:
    GDClickedLabel(QWidget *parent=nullptr);
    ~GDClickedLabel();
    void setState(QString normal, QString normal_hover, QString normal_press,
            QString selected, QString select_hover, QString select_press);
    void mousePressEvent(QMouseEvent *ev) override;
    virtual void mouseReleaseEvent(QMouseEvent *ev) override;
    virtual void enterEvent(QEnterEvent *event) override;
    virtual void leaveEvent(QEvent *event) override;
    ClickLbState getCurState();
    bool setCurState(ClickLbState state);
    void ResetNormalState();
private:
    QString _normal;
    QString _normal_hover;
    QString _normal_press;
    QString _selected;
    QString _select_hover;
    QString _select_press;
    ClickLbState _curState;
signals:
    void clicked(QString, ClickLbState);
};

#endif // GDCLICKEDLABEL_H
