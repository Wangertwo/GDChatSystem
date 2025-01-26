#ifndef GDTIMERBTN_H
#define GDTIMERBTN_H
#include <QPushButton>
#include <QTimer>
#include <QMouseEvent>
class GDTimerBtn: public QPushButton
{
public:
    GDTimerBtn(QWidget *parent = nullptr);
    ~GDTimerBtn();
    void mouseReleaseEvent(QMouseEvent *e) override;

private:
    QTimer* _timer;
    int _counter;
};

#endif // GDTIMERBTN_H
