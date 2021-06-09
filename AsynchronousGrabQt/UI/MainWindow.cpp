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

#include <algorithm>

#include <QItemSelection>
#include <QPixmap>

#include "ui_AsynchronousGrabGui.h"

#include "Image.h"
#include "LogEntryListModel.h"
#include "MainWindow.h"
#include "ModuleTreeModel.h"
#include "VmbException.h"

#include <QTreeWidgetItem>

using VmbC::Examples::VmbException;
using VmbC::Examples::LogEntry;
using VmbC::Examples::LogEntryListModel;

namespace Text
{
    QString StartAcquisition()
    {
        return "Start Acquisition";
    }

    QString StopAcquisition()
    {
        return "Stop Acquisition";
    }

    QString WindowTitleStartupError()
    {
        return "Vmb C AsynchronousGrab API Version";
    }

    QString WindowTitle(std::string const& vmbCVersion)
    {
        return QString::fromStdString("Vmb C AsynchronousGrab API Version " + vmbCVersion);
    }
}

namespace
{
    struct SelectableCheckVisitor : VmbC::Examples::ModuleData::Visitor
    {
        bool m_selectable { false };

        void Visit(VmbCameraInfo_t const&) override
        {
            m_selectable = true;
        }
    };

    struct CameraInfoRetrievalVisitor : VmbC::Examples::ModuleData::Visitor
    {
        VmbCameraInfo_t const* m_info { nullptr };

        void Visit(VmbCameraInfo_t const& info) override
        {
            m_info = &info;
        }
    };
}

MainWindow::MainWindow(QWidget* parent, Qt::WindowFlags flags)
    : QMainWindow(parent, flags),
    m_ui(new Gui()),
    m_acquisitionManager(*this),
    m_log(new LogEntryListModel())
{
    m_ui->setupUi(this);
    SetupLogView();

    try
    {
        m_apiController = std::make_unique<ApiController>(*this);
    }
    catch (VmbException const& ex)
    {
        Log(ex);
    }

    if (m_apiController)
    {
        SetupUi(*(m_apiController.get()));
        SetupCameraTree();
    }
    else
    {
        setWindowTitle(Text::WindowTitleStartupError());
    }
}

void MainWindow::StartStopClicked()
{
    if (m_acquisitionManager.IsAcquisitionActive())
    {
        StopAcquisition();
    }
    else
    {
        auto selectionModel = m_ui->m_cameraSelectionTree->selectionModel();
        if (selectionModel->hasSelection())
        {
            auto selection = selectionModel->selectedRows();
            if (!selection.isEmpty())
            {
                auto modelData = VmbC::Examples::ModuleTreeModel::GetModule(selection.at(0));
                if (modelData != nullptr)
                {
                    CameraInfoRetrievalVisitor visitor;
                    modelData->Accept(visitor);
                    
                    if (visitor.m_info != nullptr)
                    {
                        StartAcquisition(*(visitor.m_info));
                    }
                }
            }
        }
    }

}

void MainWindow::RenderImage()
{
    {
        std::lock_guard<std::mutex> lock(m_imageSynchronizer);

        if (m_queuedImage && m_renderingRequired)
        {
            m_renderingRequired = false;
            std::swap(m_queuedImage, m_renderingImage);
        }
        else
        {
            return;
        }
    }

    QImage qImage(m_renderingImage->GetData(), m_renderingImage->GetWidth(), m_renderingImage->GetBytesPerLine(), QImage::Format::Format_RGB888);
    m_ui->m_renderLabel->setPixmap(QPixmap::fromImage(qImage));

    std::swap(m_onscreenImage, m_renderingImage);
}

void MainWindow::SetupUi(VmbC::Examples::ApiController& controller)
{
    setWindowTitle(Text::WindowTitle(m_apiController->GetVersion()));
    
    QObject::connect(m_ui->m_acquisitionStartStopButton, &QPushButton::clicked, this, &MainWindow::StartStopClicked);
    QObject::connect(this, &MainWindow::ImageReady, this, static_cast<void (MainWindow::*)()>(&MainWindow::RenderImage), Qt::ConnectionType::QueuedConnection);
}

void MainWindow::SetupLogView()
{
    auto old = m_ui->m_eventLog->model();;
    m_ui->m_eventLog->setModel(m_log);
    delete old;
}

void MainWindow::StartAcquisition(VmbCameraInfo_t const& cameraInfo)
{
    bool success = false;

    try
    {
        m_acquisitionManager.StartAcquisition(cameraInfo);
        success = true;
    }
    catch (VmbException const& ex)
    {
        Log(ex);
    }

    if (success)
    {
        Log("Acquisition Started");
        // update button text
        m_ui->m_acquisitionStartStopButton->setText(Text::StopAcquisition());
    }
}

void MainWindow::StopAcquisition()
{
    m_acquisitionManager.StopAcquisition();

    Log("Acquisition Stopped");

    auto& button = *(m_ui->m_acquisitionStartStopButton);

    button.setText(Text::StartAcquisition());
    button.setEnabled(m_ui->m_cameraSelectionTree->selectionModel()->hasSelection());
}

void MainWindow::CameraSelected(QItemSelection const& newSelection)
{
    if (!m_acquisitionManager.IsAcquisitionActive())
    {
        m_ui->m_acquisitionStartStopButton->setText(Text::StartAcquisition());

        bool disableBtn = newSelection.empty() ;
        if (!disableBtn)
        {
            auto moduleData = VmbC::Examples::ModuleTreeModel::GetModule(newSelection.at(0).topLeft());

            SelectableCheckVisitor visitor;
            moduleData->Accept(visitor);
            disableBtn = !visitor.m_selectable;
        }

        m_ui->m_acquisitionStartStopButton->setDisabled(disableBtn);
    }
}

MainWindow::~MainWindow()
{
    m_acquisitionManager.StopAcquisition();
}

void MainWindow::RenderImage(std::unique_ptr<VmbC::Examples::Image>& image)
{
    if (image)
    {
        std::lock_guard<std::mutex> lock(m_imageSynchronizer);

        std::swap(m_queuedImage, image);
        m_renderingRequired = true;
    }

    emit ImageReady();
}

void MainWindow::SetupCameraTree()
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
        auto systems = m_apiController->GetSystemList();
        auto interfaces = m_apiController->GetInterfaceList();
        auto cameras = m_apiController->GetCameraList();

        auto const systemsBegin = systems.begin();
        auto const systemsEnd = systems.end();

        auto ifEnd = std::stable_partition(interfaces.begin(), interfaces.end(),
                                           [this, systemsBegin, systemsEnd](std::unique_ptr<InterfaceData> const& ifPtr)
                                           {
                                               auto iface = ifPtr.get();
                                               auto parentIter = std::find_if(systemsBegin, systemsEnd, [iface](std::unique_ptr<TlData> const& sysPtr)
                                                                              {
                                                                                  // TODO
                                                                                  return true;
                                                                              });
                                               if (parentIter == systemsEnd)
                                               {
                                                   Log(std::string("parent module not found for interface ") + iface->GetInfo().interfaceName + " ignoring interface");
                                                   return false;
                                               }
                                               else
                                               {
                                                   iface->SetParent(parentIter->get());
                                                   return true;
                                               }
                                           });

        auto ifBegin = interfaces.begin();

        auto camerasEnd = std::stable_partition(cameras.begin(), cameras.end(),
                                           [this, ifBegin, ifEnd](std::unique_ptr<CameraData> const& camPtr)
                                           {
                                               auto cam = camPtr.get();
                                               auto parentIter = std::find_if(ifBegin, ifEnd, [cam](std::unique_ptr<InterfaceData> const& ifacePtr)
                                                                              {
                                                                                  // TODO: use handle?
                                                                                  return std::strcmp(ifacePtr->GetInfo().interfaceIdString, cam->GetInfo().interfaceIdString) == 0;
                                                                              });
                                               if (parentIter == ifEnd)
                                               {
                                                   Log(std::string("parent module not found for camera ") + cam->GetInfo().cameraName + " ignoring camera");
                                                   return false;
                                               }
                                               else
                                               {
                                                   cam->SetParent(parentIter->get());
                                                   return true;
                                               }
                                           });

        moduleData.resize(systems.size()
                          + std::distance(ifBegin, ifEnd)
                          + std::distance(cameras.begin(), camerasEnd)
        );

        // move data of all modules reachable from a tl to moduleData
        auto pos = std::move(systems.begin(), systems.end(), moduleData.begin());
        pos = std::move(ifBegin, ifEnd, pos);
        std::move(cameras.begin(), camerasEnd, pos);
    }
    catch (VmbException const& ex)
    {
        Log(ex);
    }

    ModuleTreeModel* model = new ModuleTreeModel(std::move(moduleData));

    m_ui->m_cameraSelectionTree->setModel(model);
    m_ui->m_cameraSelectionTree->expandAll();

    auto selectionModel = m_ui->m_cameraSelectionTree->selectionModel();
    QObject::connect(selectionModel, &QItemSelectionModel::selectionChanged, this, &MainWindow::CameraSelected);

    m_ui->m_cameraSelectionTree->setDisabled(false);
}

void MainWindow::Log(VmbC::Examples::VmbException const& exception)
{
    (*m_log) << LogEntry(exception.what(), exception.GetExitCode());
}

void MainWindow::Log(std::string const& strMsg)
{
    (*m_log) << LogEntry(strMsg);
}
