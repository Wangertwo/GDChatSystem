#include "gdcustomizeedit.h"

GDCustomizeEdit::GDCustomizeEdit(QWidget *parent):QLineEdit (parent),_max_len(0)
{
    connect(this, &QLineEdit::textChanged, this, &GDCustomizeEdit::limitTextLength);
}

void GDCustomizeEdit::SetMaxLength(int maxLen)
{
    _max_len = maxLen;
}
