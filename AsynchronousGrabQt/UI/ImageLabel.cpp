/*=============================================================================
  Copyright (C) 2012-2023 Allied Vision Technologies. All Rights Reserved.
  Subject to the BSD 3-Clause License.
=============================================================================*/

#include <QResizeEvent>

#include "UI/ImageLabel.h"

ImageLabel::ImageLabel(QWidget* parent, Qt::WindowFlags flags)
    : QLabel(parent, flags)
{
}

void ImageLabel::resizeEvent(QResizeEvent* event)
{
    QLabel::resizeEvent(event);
    emit sizeChanged(event->size());
}
