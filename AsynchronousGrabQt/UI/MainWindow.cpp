/*=============================================================================
  Copyright (C) 2012-2023 Allied Vision Technologies. All Rights Reserved.
  Subject to the BSD 3-Clause License.
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
        m_apiController.reset(new ApiController(*this));
    }
    catch (VmbException const& ex)
    {
        Log(ex);
    }

    if (m_apiController)
    {
        SetupUi(*(m_apiController.get()));
        SetupCameraTree();
        m_acquisitionManager.SetOutputSize(m_ui->m_renderLabel->size());
        QObject::connect(m_ui->m_renderLabel, &ImageLabel::sizeChanged, this, &MainWindow::ImageLabelSizeChanged);
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

void MainWindow::ImageLabelSizeChanged(QSize newSize)
{
    m_acquisitionManager.SetOutputSize(newSize);
}

void MainWindow::RenderImage()
{
    QPixmap pixmap;
    {
        std::lock_guard<std::mutex> lock(m_imageSynchronizer);

        if (m_renderingRequired)
        {
            m_renderingRequired = false;
            std::swap(pixmap, m_queuedImage);
        }
        else
        {
            return;
        }
    }

    m_ui->m_renderLabel->setPixmap(pixmap);
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
    QObject::disconnect(m_ui->m_renderLabel, &ImageLabel::sizeChanged, this, &MainWindow::ImageLabelSizeChanged);
    m_acquisitionManager.StopAcquisition();
}

void MainWindow::RenderImage(QPixmap image)
{
    bool notify = false;

    {
        std::lock_guard<std::mutex> lock(m_imageSynchronizer);

        m_queuedImage = std::move(image);

        if (!m_renderingRequired)
        {
            notify = true;
            m_renderingRequired = true;
        }
    }

    if (notify)
    {
        emit ImageReady();
    }
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
                                                                                  return sysPtr->GetInfo().transportLayerHandle == iface->GetInfo().transportLayerHandle;
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
                                                                                  return ifacePtr->GetInfo().interfaceHandle == cam->GetInfo().interfaceHandle;
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

    m_ui->m_cameraSelectionTree->header()->setStretchLastSection(false);
    m_ui->m_cameraSelectionTree->header()->setSectionResizeMode(QHeaderView::ResizeToContents);

}

void MainWindow::Log(VmbC::Examples::VmbException const& exception)
{
    (*m_log) << LogEntry(exception.what(), exception.GetExitCode());
}

void MainWindow::Log(std::string const& strMsg)
{
    (*m_log) << LogEntry(strMsg);
}
