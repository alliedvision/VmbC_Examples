/**
 * \date 2021-2023
 * \copyright Allied Vision Technologies. All Rights Reserved.
 *
 * \copyright Subject to the BSD 3-Clause License.
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
