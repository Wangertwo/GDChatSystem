#ifndef GDCLICKEDBTN_H
#define GDCLICKEDBTN_H

#include <QPushButton>
#include "global.h"
class GDClickedBtn : public QPushButton
{
public:
    GDClickedBtn(QWidget * parent = nullptr);
    ~GDClickedBtn();
    void SetState(QString nomal, QString hover, QString press);
protected:
    virtual void enterEvent(QEnterEvent *event) override;
    virtual void leaveEvent(QEvent *event) override; // 鼠标离开
    virtual void mousePressEvent(QMouseEvent *event) override; // 鼠标按下
    virtual void mouseReleaseEvent(QMouseEvent *event) override; // 鼠标释放
private:
    QString _normal;
    QString _hover;
    QString _press;
};

#endif // GDCLICKEDBTN_H
