/**
 * \date 2021-2023
 * \copyright Allied Vision Technologies. All Rights Reserved.
 *
 * \copyright Subject to the BSD 3-Clause License.
 *
 * \brief QLabel subclass that provides a signal for getting size updates
 */

#ifndef ASYNCHRONOUSGRAB_C_IMAGE_LABEL_H
#define ASYNCHRONOUSGRAB_C_IMAGE_LABEL_H

#include <QLabel>
#include <QSize>

/**
 * \brief Widget for displaying a the images received from a camera.
 *        Provides a signal for listening to size updates
 */
class ImageLabel : public QLabel
{
    Q_OBJECT
public:
    ImageLabel(QWidget* parent = 0, Qt::WindowFlags flags = Qt::Widget);
protected:
    /**
     * \brief adds sizeChanged signal emission to QLabel::resizeEvent
     */
    void resizeEvent(QResizeEvent* event) override;
signals:
    /**
     * \brief signal triggered during the resize event
     * \param value the new size after the resize event
     */
    void sizeChanged(QSize value);

};

#endif
