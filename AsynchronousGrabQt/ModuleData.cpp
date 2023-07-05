/**
 * \date 2021-2023
 * \copyright Allied Vision Technologies. All Rights Reserved.
 *
 * \copyright Subject to the BSD 3-Clause License.
 *
 * \brief Implementation of ModuleData.
 */

#include "ModuleData.h"

namespace VmbC
{
    namespace Examples
    {
        ModuleData* ModuleData::GetParent()
        {
            return m_parent;
        }

        ModuleData::ModuleData()
            : m_parent(nullptr)
        {
        }

        void ModuleData::Visitor::Visit(VmbCameraInfo_t const& data)
        {
        }

        void ModuleData::Visitor::Visit(VmbInterfaceInfo_t const& data)
        {
        }

        void ModuleData::Visitor::Visit(VmbTransportLayerInfo_t const& data)
        {
        }
    } // namespace Examples
} // namespace VmbC
