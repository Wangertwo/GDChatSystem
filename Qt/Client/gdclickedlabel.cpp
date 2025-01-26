#include "gdclickedlabel.h"

GDClickedLabel::GDClickedLabel(QWidget *parent)
    :QLabel(parent), _curState(ClickLbState::NORMAL)
{
    this->setCursor(Qt::PointingHandCursor);
}

GDClickedLabel::~GDClickedLabel()
{

}

void GDClickedLabel::setState(QString normal, QString normal_hover, QString normal_press, QString selected, QString select_hover, QString select_press)
{
    _normal = normal;
    _normal_hover = normal_hover;
    _normal_press = normal_press;
    _selected = selected;
    _select_hover = select_hover;
    _select_press = select_press;
    setProperty("state", normal);
    repolish(this);
}

void GDClickedLabel::mousePressEvent(QMouseEvent *ev)
{
    if (ev->button() == Qt::LeftButton) {
        if(_curState == ClickLbState::NORMAL){
            // qDebug()<<"PressEvent , change to selected press: "<< _selected_press;
            _curState = ClickLbState::SELECTED;
            setProperty("state",_select_press);
            repolish(this);
            update();

        }else{
            // qDebug()<<"PressEvent , change to normal press: "<< _normal_press;
            _curState = ClickLbState::NORMAL;
            setProperty("state",_normal_press);
            repolish(this);
            update();
        }
        return;
    }
    // 调用基类的mousePressEvent以保证正常的事件处理
    QLabel::mousePressEvent(ev);
}

void GDClickedLabel::mouseReleaseEvent(QMouseEvent *ev)
{
    if (ev->button() == Qt::LeftButton) {
        if(_curState == ClickLbState::NORMAL){
            // qDebug()<<"ReleaseEvent , change to normal hover: "<< _normal_hover;
            setProperty("state",_normal_hover);
            repolish(this);
            update();
        }else{
            //  qDebug()<<"ReleaseEvent , change to select hover: "<< _selected_hover;
            setProperty("state",_select_hover);
            repolish(this);
            update();
        }
        emit clicked(this->text(), _curState);
        return;
    }
    // 调用基类的mousePressEvent以保证正常的事件处理
    QLabel::mousePressEvent(ev);
}

void GDClickedLabel::enterEvent(QEnterEvent *event)
{
    if(_curState == ClickLbState::NORMAL) {
        setProperty("state", _normal_hover);
        repolish(this);
        update();
    }else {
        setProperty("state", _select_hover);
        repolish(this);
        update();
    }
    return QLabel::enterEvent(event);
}

void GDClickedLabel::leaveEvent(QEvent *event)
{
    if(_curState == ClickLbState::NORMAL){
        setProperty("state",_normal);
        repolish(this);
        update();
    }else{
        setProperty("state",_selected);
        repolish(this);
        update();
    }
    return QLabel::leaveEvent(event);
}

ClickLbState GDClickedLabel::getCurState()
{
    return _curState;
}

bool GDClickedLabel::setCurState(ClickLbState state)
{
    _curState = state;
    if (_curState == ClickLbState::NORMAL) {
        setProperty("state", _normal);
        repolish(this);
    }
    else if (_curState == ClickLbState::SELECTED) {
        setProperty("state", _selected);
        repolish(this);
    }

    return true;
}

void GDClickedLabel::ResetNormalState()
{
    _curState = ClickLbState::NORMAL;
    setProperty("state", _normal);
    repolish(this);
}

