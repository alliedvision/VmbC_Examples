/*=============================================================================
  Copyright (C) 2012 - 2017 Allied Vision Technologies.  All Rights Reserved.

  Redistribution of this file, in original or modified form, without
  prior written consent of Allied Vision Technologies is prohibited.

-------------------------------------------------------------------------------

  File:        MainWindow.cpp

  Description: Qt dialog class for the GUI of the MainWindow example of
               VimbaCPP.

-------------------------------------------------------------------------------

  THIS SOFTWARE IS PROVIDED BY THE AUTHOR "AS IS" AND ANY EXPRESS OR IMPLIED
  WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF TITLE,
  NON-INFRINGEMENT, MERCHANTABILITY AND FITNESS FOR A PARTICULAR  PURPOSE ARE
  DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT,
  INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
  (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
  LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
  AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR
  TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

=============================================================================*/

#include <sstream>

#include "ui_AsynchronousGrabGui.h"

#include "MainWindow.h"
#include "ModuleTreeModel.h"

#include <QTreeWidgetItem>

//TODO #include "VmbTransform.h"
//TODO #define NUM_COLORS 3
//TODO #define BIT_DEPTH 8

using VmbC::Examples::ApiCallResult;

MainWindow::MainWindow(QWidget* parent, Qt::WindowFlags flags)
    : QMainWindow(parent, flags),
    m_bIsStreaming(false),
    m_ui(new Gui())
{
    m_ui->setupUi(this);

    // TODO Connect GUI events with event handlers QObject::connect( ui.m_ButtonStartStop, SIGNAL( clicked() ), this, SLOT( OnBnClickedButtonStartstop() ) );

    // Start Vimba
    auto result = m_apiController.Startup();
    setWindowTitle(QString("Vmb C AsynchronousGrab API Version ") + QString::fromStdString(m_apiController.GetVersion()));
    Log("Starting Vimba", result.GetErrorCode());

    if (result)
    {
        // Connect new camera found event with event handler
        // TODO QObject::connect( m_ApiController.GetCameraObserver(), SIGNAL( CameraListChangedSignal(int) ), this, SLOT( OnCameraListChanged(int) ) );

        // Initially get all connected cameras
        InitializeCameraListBox();
        std::stringstream strMsg;
        strMsg << "Cameras found..." << m_cameras.size();
        Log(strMsg.str());
    }
}

MainWindow::~MainWindow()
{
    // if we are streaming stop streaming
    if (m_bIsStreaming)
    {
        OnBnClickedButtonStartstop();
    }

    // Before we close the application we stop Vimba
    m_apiController.Shutdown();
}

void MainWindow::OnBnClickedButtonStartstop()
{
#ifdef TODO
    int nRow = ui.m_ListBoxCameras->currentRow();

    if (-1 < nRow)
    {
        if (false == m_bIsStreaming)
        {
            // Start acquisition
            auto result = m_apiController.StartContinuousImageAcquisition(m_cameras[nRow]);
            // Set up Qt image
            if (VmbErrorSuccess == result)
            {
                m_image = QImage(m_apiController.GetWidth(),
                                 m_apiController.GetHeight(),
                                 QImage::Format_RGB888);

                QObject::connect(m_apiController.GetFrameObserver(), SIGNAL(FrameReceivedSignal(int)), this, SLOT(OnFrameReady(int)));
            }
            Log("Starting Acquisition", result.GetErrorCode());
            m_bIsStreaming = VmbErrorSuccess == result;
        }
        else
        {
            m_bIsStreaming = false;
            // Stop acquisition
            auto result = m_apiController.StopContinuousImageAcquisition();
            // Clear all frames that we have not picked up so far
            m_apiController.ClearFrameQueue();
            m_image = QImage();
            Log("Stopping Acquisition", result.GetErrorCode());
        }

        if (false == m_bIsStreaming)
        {
            ui.m_ButtonStartStop->setText(QString("Start Image Acquisition"));
        }
        else
        {
            ui.m_ButtonStartStop->setText(QString("Stop Image Acquisition"));
        }
    }
#endif
}

void MainWindow::OnFrameReady(int status)
{
#ifdef TODO
    if (true == m_bIsStreaming)
    {
        // Pick up frame
        FramePtr pFrame = m_apiController.GetFrame();
        if (SP_ISNULL(pFrame))
        {
            Log("frame pointer is NULL, late frame ready message");
            return;
        }
        // See if it is not corrupt
        if (VmbFrameStatusComplete == status)
        {
            VmbUchar_t* pBuffer;
            VmbErrorType err = SP_ACCESS(pFrame)->GetImage(pBuffer);
            if (VmbErrorSuccess == err)
            {
                VmbUint32_t nSize;
                err = SP_ACCESS(pFrame)->GetImageSize(nSize);
                if (VmbErrorSuccess == err)
                {
                    VmbPixelFormatType ePixelFormat = m_apiController.GetPixelFormat();
                    if (!m_image.isNull())
                    {
                        // Copy it
                        // We need that because Qt might repaint the view after we have released the frame already
                        if (ui.m_ColorProcessingCheckBox->checkState() == Qt::Checked)
                        {
                            static const VmbFloat_t Matrix[] = { 8.0f, 0.1f, 0.1f, // this matrix just makes a quick color to mono conversion
                                                                    0.1f, 0.8f, 0.1f,
                                                                    0.0f, 0.0f, 1.0f };
                            if (VmbErrorSuccess != CopyToImage(pBuffer, ePixelFormat, m_image, Matrix))
                            {
                                ui.m_ColorProcessingCheckBox->setChecked(false);
                            }
                        }
                        else
                        {
                            CopyToImage(pBuffer, ePixelFormat, m_image);
                        }

                        // Display it
                        const QSize s = ui.m_LabelStream->size();
                        ui.m_LabelStream->setPixmap(QPixmap::fromImage(m_image).scaled(s, Qt::KeepAspectRatio));
                    }
                }
            }
        }
        else
        {
            // If we receive an incomplete image we do nothing but logging
            Log("Failure in receiving image", VmbErrorOther);
        }

        // And queue it to continue streaming
        m_apiController.QueueFrame(pFrame);
    }
#endif
}

void MainWindow::OnCameraListChanged(int reason)
{

#ifdef TODO
    bool bUpdateList = false;

    // We only react on new cameras being found and known cameras being unplugged
    if (AVT::VmbAPI::UpdateTriggerPluggedIn == reason)
    {
        Log("Camera list changed. A new camera was discovered by Vimba.");
        bUpdateList = true;
    }
    else if (AVT::VmbAPI::UpdateTriggerPluggedOut == reason)
    {
        Log("Camera list changed. A camera was disconnected from Vimba.");
        if (true == m_bIsStreaming)
        {
            OnBnClickedButtonStartstop();
        }
        bUpdateList = true;
    }

    if (true == bUpdateList)
    {
        UpdateCameraListBox();
    }

    ui.m_ButtonStartStop->setEnabled(0 < m_cameras.size() || m_bIsStreaming);
#endif
}

VmbErrorType MainWindow::CopyToImage(VmbUchar_t* pInBuffer, VmbPixelFormat_t ePixelFormat, QImage& pOutImage, const float* Matrix /*= NULL */)
{

#ifdef TODO
    const int           nHeight = m_apiController.GetHeight();
    const int           nWidth = m_apiController.GetWidth();

    VmbImage            SourceImage, DestImage;
    VmbError_t          Result;
    SourceImage.Size = sizeof(SourceImage);
    DestImage.Size = sizeof(DestImage);

    Result = VmbSetImageInfoFromPixelFormat(ePixelFormat, nWidth, nHeight, &SourceImage);
    if (VmbErrorSuccess != Result)
    {
        Log("Could not set source image info", static_cast<VmbErrorType>(Result));
        return static_cast<VmbErrorType>(Result);
    }
    QString             OutputFormat;
    const int           bytes_per_line = pOutImage.bytesPerLine();
    switch (pOutImage.format())
    {
    default:
        Log("unknown output format", VmbErrorBadParameter);
        return VmbErrorBadParameter;
    case QImage::Format_RGB888:
        if (nWidth * 3 != bytes_per_line)
        {
            Log("image transform does not support stride", VmbErrorWrongType);
            return VmbErrorWrongType;
        }
        OutputFormat = "RGB24";
        break;
    }
    Result = VmbSetImageInfoFromString(OutputFormat.toStdString().c_str(), OutputFormat.length(), nWidth, nHeight, &DestImage);
    if (VmbErrorSuccess != Result)
    {
        Log("could not set output image info", static_cast<VmbErrorType>(Result));
        return static_cast<VmbErrorType>(Result);
    }
    SourceImage.Data = pInBuffer;
    DestImage.Data = pOutImage.bits();
    // do color processing?
    if (NULL != Matrix)
    {
        VmbTransformInfo TransformParameter;
        Result = VmbSetColorCorrectionMatrix3x3(Matrix, &TransformParameter);
        if (VmbErrorSuccess == Result)
        {
            Result = VmbImageTransform(&SourceImage, &DestImage, &TransformParameter, 1);
        }
        else
        {
            Log("could not set matrix t o transform info ", static_cast<VmbErrorType>(Result));
            return static_cast<VmbErrorType>(Result);
        }
    }
    else
    {
        Result = VmbImageTransform(&SourceImage, &DestImage, NULL, 0);
    }
    if (VmbErrorSuccess != Result)
    {
        Log("could not transform image", static_cast<VmbErrorType>(Result));
        return static_cast<VmbErrorType>(Result);
    }
    return static_cast<VmbErrorType>(Result);
#endif
    return VmbErrorSuccess;
}

void MainWindow::InitializeCameraListBox()
{

    using VmbC::Examples::ModuleTreeModel;
    using VmbC::Examples::ModuleData;
    using VmbC::Examples::CameraData;
    using VmbC::Examples::InterfaceData;
    using VmbC::Examples::TlData;

    std::vector<std::unique_ptr<ModuleData>> moduleData;

    // read module info and populate moduleData
    try
    {
        auto systems = m_apiController.GetSystemList();
        for (auto& system : systems)
        {
            try
            {
                auto interfaces = m_apiController.GetInterfaceList(system.get());

                for (auto& iface : interfaces)
                {
                    try
                    {
                        auto cameras = m_apiController.GetCameraList(iface.get());

                        for (auto& cam : cameras)
                        {
                            moduleData.emplace_back(std::move(cam));
                        }
                    }
                    catch (std::exception const& ex)
                    {
                        // TODO
                    }
                    moduleData.emplace_back(std::move(iface));
                }
            }
            catch (std::exception const& ex)
            {
                // TODO
            }
            moduleData.emplace_back(std::move(system));
        }
    }
    catch (std::exception const& ex)
    {
        // TODO
    }

    ModuleTreeModel* model = new ModuleTreeModel(std::move(moduleData));

    m_ui->m_cameraSelectionTree->setModel(model);

#ifdef TODO
    // Get all cameras currently connected to Vimba
    CameraPtrVector cameras = m_apiController.GetCameraList();

    // Simply forget about all cameras known so far
    ui.m_ListBoxCameras->clear();
    m_cameras.clear();

    // And query the camera details again
    for (CameraPtrVector::const_iterator iter = cameras.begin();
         cameras.end() != iter;
         ++iter)
    {
        std::string strCameraName;
        std::string strCameraID;
        if (VmbErrorSuccess != (*iter)->GetName(strCameraName))
        {
            strCameraName = "[NoName]";
        }
        // If for any reason we cannot get the ID of a camera we skip it
        if (VmbErrorSuccess == (*iter)->GetID(strCameraID))
        {
            ui.m_ListBoxCameras->addItem(QString::fromStdString(strCameraName + " " + strCameraID));
            m_cameras.push_back(strCameraID);
        }
    }

    ui.m_ButtonStartStop->setEnabled(0 < m_cameras.size() || m_bIsStreaming);

#endif
}

void MainWindow::Log(std::string strMsg, VmbError_t eErr)
{
#ifdef TODO
    strMsg += "..." + m_apiController.ErrorCodeToMessage(eErr);
    ui.m_ListLog->insertItem(0, QString::fromStdString(strMsg));
#endif
}

void MainWindow::Log(std::string strMsg)
{

#ifdef TODO
    ui.m_ListLog->insertItem(0, QString::fromStdString(strMsg));
#endif
}
