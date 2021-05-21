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
 * \brief Implementation file for the ApiController helper class that
 *        demonstrates how to implement an asynchronous, continuous image
 *        acquisition with VmbC.
 * \author Fabian Klein
 */

#include <sstream>
#include <iostream>

#include "ApiController.h"

 //TODO #include "Common/StreamSystemInfo.h"
 //TODO #include "Common/ErrorCodeToMessage.h"

namespace VmbC
{
    namespace Examples
    {

        enum { NUM_FRAMES = 3, };

        ApiController::ApiController()
            : m_imageWidth(0), m_imageHeight(0), m_pixelFormat(VmbPixelFormatLast)
        {
        }

        ApiController::~ApiController()
        {
        }

        ApiCallResult ApiController::Startup()
        {
            // TODO Start Vimba

            return ApiCallResult();
        }

        void ApiController::Shutdown()
        {
            // TODO Release Vimba
        }

#ifdef TODO
        inline VmbErrorType SetValueIntMod2(const CameraPtr& camera, const std::string& featureName, VmbInt64_t& storage)
        {
            VmbErrorType    res;
            FeaturePtr      pFeature;
            VmbInt64_t      minValue = 0;
            VmbInt64_t      maxValue = 0;
            VmbInt64_t      incrementValue = 0;

            res = SP_ACCESS(camera)->GetFeatureByName(featureName.c_str(), pFeature);
            if (VmbErrorSuccess != res)
            {
                return res;
            }

            res = SP_ACCESS(pFeature)->GetRange(minValue, maxValue);
            if (VmbErrorSuccess != res)
            {
                return res;
            }

            res = SP_ACCESS(pFeature)->GetIncrement(incrementValue);
            if (VmbErrorSuccess != res)
            {
                return res;
            }

            maxValue = maxValue - (maxValue % incrementValue);
            if (maxValue % 2 != 0)
            {
                maxValue -= incrementValue;
            }

            res = SP_ACCESS(pFeature)->SetValue(maxValue);
            if (VmbErrorSuccess != res)
            {
                return res;
            }

            storage = maxValue;
            return res;
        }
#endif

        ApiCallResult ApiController::StartContinuousImageAcquisition(VmbHandle_t const cameraHandle)
        {
            // TODO Open the desired camera by its handle

            return ApiCallResult();
        }

        ApiCallResult ApiController::StopContinuousImageAcquisition()
        {

            // TODO Stop streaming

            // TODO Close camera
            return ApiCallResult();
        }

        inline namespace TODO
        {
            VmbCameraInfo_t CreateCamera(const char* id, const char* name, const char* serial, const char* interfaceId, const char* modelName)
            {
                VmbCameraInfo_t cam;
                cam.cameraIdString = id;
                cam.cameraName = name;
                cam.interfaceIdString = interfaceId;
                cam.modelName = modelName;
                cam.permittedAccess = VmbAccessModeFull;
                cam.serialString = serial;
                return cam;
            }
        };

        int ApiController::GetWidth() const
        {
            return static_cast<int>(m_imageWidth);
        }

        int ApiController::GetHeight() const
        {
            return static_cast<int>(m_imageHeight);
        }

        VmbPixelFormat_t ApiController::GetPixelFormat() const
        {
            return static_cast<VmbPixelFormat_t>(m_pixelFormat);
        }


        struct TODOHandle
        {
        };
        TODOHandle sys1;
        TODOHandle sys2;

        TODOHandle iface1;
        TODOHandle iface2;
        TODOHandle iface3;
        TODOHandle iface4;

        TODOHandle cam1;
        TODOHandle cam2;
        TODOHandle cam3;
        TODOHandle cam4;
        TODOHandle cam5;
        TODOHandle cam6;

        std::vector<std::unique_ptr<CameraData>> ApiController::GetCameraList(InterfaceData* iface)
        {
            std::vector<std::unique_ptr<CameraData>> result;
            auto& interfaceInfo = iface->GetInfo();
            
            size_t count;
            std::unique_ptr<VmbCameraInfo_t[]> cameraInfo;

#ifndef TODO
            count = 3;
#endif
            cameraInfo = std::make_unique<VmbCameraInfo_t[]>(count);

#ifndef TODO

            static std::string const if1 { "Interface 1" };
            static std::string const if2 { "Interface 2" };
            static std::string const if3 { "Interface 3" };


            if (if1 == interfaceInfo.interfaceName)
            {
                count = 2;
                cameraInfo[0].cameraName = "Camera 1";
                cameraInfo[1].cameraName = "Camera 2";
            }
            else if (if2 == interfaceInfo.interfaceName)
            {
                count = 1;
                cameraInfo[0].cameraName = "Camera 3";
            }
            else if (if3 == interfaceInfo.interfaceName)
            {
                count = 3;
                cameraInfo[0].cameraName = "Camera 4";
                cameraInfo[1].cameraName = "Camera 5";
                cameraInfo[2].cameraName = "Camera 6";
            }
            else
            {
                count = 0;
            }
#endif

            result.reserve(count);

            for (size_t i = 0; i != count; ++i)
            {
                auto cam = std::make_unique<CameraData>();
                cam->Initialize(cameraInfo[i], iface);
                result.emplace_back(std::move(cam));
            }

            return result;
        }

        std::vector<std::unique_ptr<TlData>> ApiController::GetSystemList()
        {
            std::vector<std::unique_ptr<TlData>> result;


            size_t count;
#ifndef TODO
            count = 2;
#endif

            auto tlInfos = std::make_unique<VmbTransportLayerInfo_t[]>(count);

#ifndef TODO
            VmbTransportLayerInfo_t tlInfo;

            tlInfo.transportLayerName = "System 1";
            tlInfo.transportLayerHandle = &sys1;
            tlInfos[0] = tlInfo;

            tlInfo.transportLayerName = "System 2";
            tlInfo.transportLayerHandle = &sys2;
            tlInfos[1] = tlInfo;
#endif

            for (size_t i = 0; i != count; ++i)
            {
                auto sys = std::make_unique<TlData>();
                sys->Initialize(tlInfos[i]);
                result.emplace_back(std::move(sys));
            }

            return result;
        }

        std::vector<std::unique_ptr<InterfaceData>> ApiController::GetInterfaceList(TlData* system)
        {
            std::vector<std::unique_ptr<InterfaceData>> result;
            auto& systemInfo = system->GetInfo();

            size_t count;

#ifndef TODO
            count = 2;
#endif

            auto interfaceInfos = std::make_unique<VmbInterfaceInfo_t[]>(count);

#ifndef TODO
            
            if (systemInfo.transportLayerHandle == &sys1)
            {
                auto& i1 = interfaceInfos[0];
                auto& i2 = interfaceInfos[1];
                i1.interfaceName = "Interface 1";
                i2.interfaceName = "Interface 2";
            }
            else if (systemInfo.transportLayerHandle == &sys2)
            {
                auto& i1 = interfaceInfos[0];
                auto& i2 = interfaceInfos[1];
                i1.interfaceName = "Interface 3";
                i2.interfaceName = "Interface 4";
            }
            else
            {
                count = 0;
            }

#endif

            result.reserve(count);

            for (size_t i = 0; i != count; ++i)
            {
                auto iface = std::make_unique<InterfaceData>();
                iface->Initialize(interfaceInfos[i], system);
                result.emplace_back(std::move(iface));
            }

            return result;
        }

        VmbFrame_t ApiController::GetFrame()
        {
            // TODO
            return {};
        }

        void ApiController::ClearFrameQueue()
        {
            // TODO
        }

        ApiCallResult ApiController::QueueFrame(VmbFrame_t pFrame)
        {
            // TODO
            return ApiCallResult();
        }

        QObject* ApiController::GetCameraObserver()
        {
            // TODO
            return nullptr;
        }

        QObject* ApiController::GetFrameObserver()
        {
            // TODO
            return nullptr;
        }

        std::string ApiController::GetVersion() const
        {
            std::ostringstream os;

            VmbVersionInfo_t versionInfo;
            auto const error = VmbVersionQuery(&versionInfo, sizeof(versionInfo));

            if (error == VmbErrorSuccess)
            {
                os
                    << versionInfo.major << '.'
                    << versionInfo.minor << '.'
                    << versionInfo.patch;
            }

            return os.str();
        }

    } // namespace Examples
} // namespace VmbC
