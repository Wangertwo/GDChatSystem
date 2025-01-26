#ifndef LISTITEMBASE_H
#define LISTITEMBASE_H
#include <QWidget>
#include "global.h"
class ListItemBase:public QWidget
{
    Q_OBJECT
public:
    explicit ListItemBase(QWidget *parent = nullptr);
    ~ListItemBase();
    void SetItemType(ListItemType itemType);
    ListItemType GetItemType();
private:
    ListItemType _itemType;
protected:
    void paintEvent(QPaintEvent* event);
public slots:
signals:
};

#endif // LISTITEMBASE_H
