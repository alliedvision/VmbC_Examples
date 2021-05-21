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
 * \brief The main window of the AsynchronousGrab example of
 *              VmbC.
 * \author Fabian Klein
 */

#ifndef ASYNCHRONOUSGRAB_C_MAIN_WINDOW_H
#define ASYNCHRONOUSGRAB_C_MAIN_WINDOW_H

#include <QMainWindow>

#include "VimbaC/Include/VimbaC.h"

#include "ApiController.h"

using VmbC::Examples::ApiController;

QT_BEGIN_NAMESPACE

class QListView;
class QTreeView;

namespace Ui
{
    class AsynchronousGrabGui;
}

QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget* parent = 0, Qt::WindowFlags flags = 0);
    ~MainWindow();

private:
    using Gui = Ui::AsynchronousGrabGui;

    /**
     * \brief variable for holding the content from UI/res/AsynchronousGrabGui.ui
     */
    Gui* m_ui;

    /**
     * \brief Our controller that wraps API access
     */
    ApiController m_apiController;

    /**
     * \brief A list of known cameras
     */
    std::vector<std::string> m_cameras;

    /**
     * \brief Are we streaming?
     */
    bool m_bIsStreaming;

    /**
     * \brief Our Qt image to display
     */
    QImage m_image;

    /**
     * \brief Queries and lists all known camera
     */
    void InitializeCameraListBox();

    /**
     * \brief Prints out a given logging string, error code and the descriptive representation of that error code
     *
     * \param[in] strMsg A given message to be printed out
     * \param[in] eErr The API status code
     */
    void Log(std::string strMsg, VmbError_t eErr);

    /**
     * \brief Prints out a given logging string
     *
     * \param[in] strMsg A given message to be printed out
     */
    void Log(std::string strMsg);

    /**
     * \brief Copies the content of a byte buffer to a Qt image with respect to the image's alignment
     *
     * \param[in] pInbuffer The byte buffer as received from the cam
     * \param[in] ePixelFormat The pixel format of the frame
     * \param[out] OutImage The filled Qt image
     */
    VmbErrorType CopyToImage(VmbUchar_t* pInBuffer, VmbPixelFormat_t ePixelFormat, QImage& pOutImage, const float* Matrix = NULL);

private slots:
    /**
     * \brief The event handler for starting / stopping acquisition
     */
    void OnBnClickedButtonStartstop();

    /**
     * \brief This event handler (Qt slot) is triggered through a Qt signal posted by the frame observer
     *
     * \param[in] status The frame receive status (complete, incomplete, ...)
     */
    void OnFrameReady(int status);

    /**
     * This event handler (Qt slot) is triggered through a Qt signal posted by the camera observer
     *
     * \param[in] reason The reason why the callback of the observer was triggered (plug-in, plug-out, ...)
     */
    void OnCameraListChanged(int reason);
};

#endif // ASYNCHRONOUSGRAB_C_MAIN_WINDOW_H
