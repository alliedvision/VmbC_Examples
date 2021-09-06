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
 */

#ifndef ASYNCHRONOUSGRAB_C_API_CONTROLLER_H
#define ASYNCHRONOUSGRAB_C_API_CONTROLLER_H

#include <QObject>

#include <memory>
#include <string>
#include <vector>
#include <sstream>

#include <VmbC/VmbC.h>

#include "ModuleData.h"
#include "VmbLibraryLifetime.h"

class MainWindow;

namespace VmbC
{
    namespace Examples
    {

        class ModuleTreeModel;

        class ApiController
        {
        public:
            ApiController(MainWindow& mainWindow);

            /**
             * \brief Gets all cameras known to Vmb for a given interface
             *
             * \return A vector of camera info structs
             */
            std::vector<std::unique_ptr<CameraData>> GetCameraList();

            /**
             * \brief gets all systems known to vmb
             * \return a vector of system info structs
             */
            std::vector<std::unique_ptr<TlData>> GetSystemList();

            /**
             * \brief gets all interfaces for a given system known to vmb
             * \return a vector of interface info structs
             */
            std::vector<std::unique_ptr<InterfaceData>> GetInterfaceList();

            /**
             * \brief Gets the version of the Vmb API
             *
             * \return the version as string
             */
            std::string GetVersion() const;
        private:
            VmbLibraryLifetime m_libraryLife;
            std::vector<TlData> m_tls;
            std::vector<InterfaceData> m_interfaces;
            std::vector<CameraData> m_cameras;
        };
    } // namespace Examples
} // namespace VmbC

#endif
