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
 * \brief Definition of the ApiController helper class that demonstrates how
 *        to implement an asynchronous, continuous image acquisition with VmbC.
 * \author Fabian Klein
 */

#ifndef ASYNCHRONOUSGRAB_C_API_CONTROLLER_H
#define ASYNCHRONOUSGRAB_C_API_CONTROLLER_H

#include <QObject>

#include <Memory>
#include <string>
#include <vector>

#include "VimbaC/Include/VimbaC.h"

#include "ApiCallResult.h"
#include "ModuleData.h"

namespace VmbC
{
    namespace Examples
    {

        class ModuleTreeModel;

        class ApiController
        {
        public:
            ApiController();
            ~ApiController();

            /**
             * \brief Starts the Vimba API and loads all transport layers
             * \return the result of the API call
             */
            ApiCallResult Startup();

            /**
             * \brief Shuts down the API
             */
            void Shutdown();

            /**
             * \brief opens a camera and does some initializations.
             *
             * Opens the given camera
             * Sets the maximum possible Ethernet packet size
             * Adjusts the image format
             * Sets up the observer that will be notified on every incoming frame
             * Calls the API convenience function to start image acquisition
             * Closes the camera in case of failure
             *
             * \param[in] cameraHandle the handle for the Camera to open
             *
             * \return the result of the API calls
             */
            ApiCallResult StartContinuousImageAcquisition(VmbHandle_t  const cameraHandle);

            /**
             * \brief Closes the camera possibly stopping the acquisition, if necessary
             *
             * \return The result of the API calls
             */
            ApiCallResult StopContinuousImageAcquisition();

            /**
             * \brief Gets the width of a frame
             *
             *
             * \return The width as integer
             */
            int GetWidth() const;

            /**
             * \brief Gets the height of a frame
             *
             * \return The height as integer
             */
            int GetHeight() const;

            /**
             * \brief Gets the pixel format of a frame
             *
             * \return The pixel format as enum
             */
            VmbPixelFormat_t GetPixelFormat() const;

            /**
             * \brief Gets all cameras known to Vmb for a given interface
             *
             * \return A vector of camera info structs
             */
            std::vector<std::unique_ptr<CameraData>> GetCameraList(InterfaceData* iface);

            /**
             * \brief gets all systems known to vmb
             * \return a vector of system info structs
             */
            std::vector<std::unique_ptr<TlData>> GetSystemList();

            /**
             * \brief gets all interfaces for a given system known to vmb
             * \return a vector of interface info structs
             */
            std::vector<std::unique_ptr<InterfaceData>> GetInterfaceList(TlData* system);

            /**
             * \brief Gets the oldest frame that has not been picked up yet
             *
             * \return frame shared pointer
             */
            VmbFrame_t GetFrame();

            /**
             * \brief Queues a given frame to be filled by the API
             *
             * \param[in] pFrame The frame to queue
             *
             * \return the result of the API call
             */
            ApiCallResult QueueFrame(VmbFrame_t pFrame);

            /**
             * \brief Clears all remaining frames that have not been picked up
             */
            void ClearFrameQueue();

            /**
             * \return the camera observer as QObject pointer to connect their signals to the view's slots
             */
            QObject* GetCameraObserver();

            /**
             * \return the frame observer as QObject pointer to connect their signals to the view's slots
             */
            QObject* GetFrameObserver();

            /**
             * \brief Gets the version of the Vimba API
             *
             * \return the version as string
             */
            std::string GetVersion() const;
        private:
            

            // The current pixel format
            VmbInt64_t                  m_pixelFormat;
            // The current width
            VmbInt64_t                  m_imageWidth;
            // The current height
            VmbInt64_t                  m_imageHeight;
        };
    } // namespace Examples
} // namespace VmbC

#endif
