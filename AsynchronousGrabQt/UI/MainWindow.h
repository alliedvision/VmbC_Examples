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

#include <memory>
#include <mutex>


#include <QMainWindow>

#include "VimbaC/Include/VimbaC.h"

#include "ApiController.h"
#include "AcquisitionManager.h"
#include "support/NotNull.h"

using VmbC::Examples::ApiController;

QT_BEGIN_NAMESPACE

class QListView;
class QItemSelection;
class QTreeView;

namespace Ui
{
    class AsynchronousGrabGui;
}

QT_END_NAMESPACE

namespace VmbC
{
    namespace Examples
    {
        class ApiController;
        class Image;
        class LogEntryListModel;
        class VmbException;
    }
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    MainWindow(QWidget* parent = 0, Qt::WindowFlags flags = Qt::Widget);

    ~MainWindow();

    /**
     * \brief Asynchonously schedule rendering of image 
     * \param a smart pointer containing the image to render; an existing image for reuse may be swapped into the smart pointer
     */
    void RenderImage(std::unique_ptr<VmbC::Examples::Image>& image);
private:
    using Gui = Ui::AsynchronousGrabGui;

    /**
     * \brief variable for holding the content from UI/res/AsynchronousGrabGui.ui
     */
    VmbC::Examples::NotNull<Gui> m_ui;

    /**
     * \brief Our controller that wraps API access
     */
    std::unique_ptr<ApiController> m_apiController;

    bool m_renderingRequired{ false };

    std::unique_ptr<VmbC::Examples::Image> m_onscreenImage;
    std::unique_ptr<VmbC::Examples::Image> m_renderingImage;
    std::unique_ptr<VmbC::Examples::Image> m_queuedImage;

    /**
     * \brief mutex for synchonizing access to m_queuedImage
     */
    std::mutex m_imageSynchronizer;

    VmbC::Examples::AcquisitionManager m_acquisitionManager;

    VmbC::Examples::NotNull<VmbC::Examples::LogEntryListModel> m_log;

    /**
     * \brief Queries and lists all known camera
     */
    void SetupCameraTree();

    /**
     * \brief Log an exception thrown because of a VmbC libary function call
     *
     * \param[in] exception the exception thrown
     */
    void Log(VmbC::Examples::VmbException const& exception);

    /**
     * \brief Prints out a given logging string
     *
     * \param[in] strMsg A given message to be printed out
     */
    void Log(std::string const& strMsg);

    /**
     * \brief setup api with info retrieved from controller
     */
    void SetupUi(VmbC::Examples::ApiController& controller);

    void SetupLogView();

    void StartAcquisition(VmbCameraInfo_t const& cameraInfo);

    void StopAcquisition();

private slots:

    /**
     * \brief Slot for selection changes in the camera tree
     */
    void CameraSelected(QItemSelection const& newSelection);

    /**
     * \brief Slot for clicks of the start / stop acquisition button
     */
    void StartStopClicked();

    void RenderImage();
signals:
    void ImageReady();
};

#endif // ASYNCHRONOUSGRAB_C_MAIN_WINDOW_H
