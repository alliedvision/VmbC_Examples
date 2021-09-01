/**
 * \date 2021
 * \copyright Allied Vision Technologies.  All Rights Reserved.
 *
 * \copyright Redistribution of this file, in original or modified form, without
 *            prior written consent of Allied Vision Technologies is prohibited.
 *
 * \warning THIS SOFTWARE IS PROVIDED BY THE AUTHOR "AS IS" AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF TITLE,
 * NON-INFRINGEMENT, MERCHANTABILITY AND FITNESS FOR A PARTICULAR  PURPOSE ARE
 * DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
 * AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR
 * TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
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
