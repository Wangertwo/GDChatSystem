#include "gdtimerbtn.h"

GDTimerBtn::GDTimerBtn(QWidget *parent)
    :QPushButton(parent), _counter(10)
{
    _timer = new QTimer(this);
    connect(_timer, &QTimer::timeout, [this]() {
        _counter--;
        if(_counter <= 0) {
            qDebug() << "timer ready";
            _timer->stop();
            _counter = 10;
            this->setEnabled(true);
            this->setText("获取");
            return;
        }
        this->setText(QString::number(_counter));
    });
}

GDTimerBtn::~GDTimerBtn()
{
    qDebug() << "TimerBtn destoried";
}

void GDTimerBtn::mouseReleaseEvent(QMouseEvent *e)
{
    if(e->button() == Qt::LeftButton) {
        qDebug() << "MyButton was released";
        this->setText(QString::number(_counter));
        this->setEnabled(false);
        _timer->start(1000); // 隔一秒发送一次
        emit clicked(); // 发送点击信号
    }
    return QPushButton::mouseReleaseEvent(e);
}
