/**
 * \date 2021-2023
 * \copyright Allied Vision Technologies. All Rights Reserved.
 *
 * \copyright Subject to the BSD 3-Clause License.
 *
 * \brief The main window of the AsynchronousGrab example of VmbC.
 */

#ifndef ASYNCHRONOUSGRAB_C_MAIN_WINDOW_H
#define ASYNCHRONOUSGRAB_C_MAIN_WINDOW_H

#include <memory>
#include <mutex>

#include <QMainWindow>
#include <QPixmap>

#include <VmbC/VmbC.h>

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

/**
 * \brief The GUI. Displays the available cameras, the image received and an
 *        event log.
 */
class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    MainWindow(QWidget* parent = 0, Qt::WindowFlags flags = Qt::Widget);

    ~MainWindow();

    /**
     * \brief Asynchonously schedule rendering of image
     */
    void RenderImage(QPixmap image);
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

    /**
     * \brief the next image to be rendered
     */
    QPixmap m_queuedImage;

    /**
     * \brief mutex for synchonizing access to m_queuedImage
     */
    std::mutex m_imageSynchronizer;

    /**
     * \brief Object for managing the acquisition; this includes the transfer
     *        of converted images to this object
     */
    VmbC::Examples::AcquisitionManager m_acquisitionManager;

    /**
     * \brief the model used for the QTableView to display log messages.
     */
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

    /**
     * \brief initialized the QTableView used for logging
     */
    void SetupLogView();

    /**
     * \brief start the acquisition for a given camera
     */
    void StartAcquisition(VmbCameraInfo_t const& cameraInfo);

    /**
     * \brief stop the acquistion
     */
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

    /**
     * \brief Slot for the size changes of the label used for rendering the images
     */
    void ImageLabelSizeChanged(QSize newSize);

    /**
     * \brief Slot for replacing the pixmap of the label used for rendering.
     *
     * Thread affinity with this object required
     */
    void RenderImage();
signals:
    /**
     * \brief signal emitted from a background thread to notify the gui about
     *        a new image being available for rendering
     */
    void ImageReady();
};

#endif // ASYNCHRONOUSGRAB_C_MAIN_WINDOW_H
