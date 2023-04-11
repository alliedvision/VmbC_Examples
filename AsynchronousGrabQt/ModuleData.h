/**
 * \date 2021-2023
 * \copyright Allied Vision Technologies. All Rights Reserved.
 *
 * \copyright Subject to the BSD 3-Clause License.
 *
 * \brief Definition of base class for modules
 */

#ifndef ASYNCHRONOUSGRAB_C_MODULE_DATA_H
#define ASYNCHRONOUSGRAB_C_MODULE_DATA_H

#include <string>

#include <VmbC/VmbC.h>

class MainWindow;

namespace VmbC
{
    namespace Examples
    {
        class ApiController;

        class ModuleData
        {
        public:
            struct Visitor
            {
                virtual ~Visitor() = default;

                virtual void Visit(VmbCameraInfo_t const& data);

                virtual void Visit(VmbInterfaceInfo_t const& data);

                virtual void Visit(VmbTransportLayerInfo_t const& data);
            };

            virtual ~ModuleData() = default;

            ModuleData* GetParent();

            virtual void Accept(Visitor& visitor) const = 0;
        protected:
            ModuleData();
            ModuleData* m_parent;

        };

        /**
         * \brief a class holding the info about a single module
         * \tparam T the info type, e.g. VmbCameraInfo_t or VmbInterfaceInfo_t
         */
        template<typename T>
        class ModuleDataImpl : public ModuleData
        {
        public:
            using InfoType = T;

            void Accept(Visitor& visitor) const override
            {
                visitor.Visit(m_info);
            }

            ModuleDataImpl(InfoType const& info)
                : m_info(info)
            {
            }

            InfoType const& GetInfo() const
            {
                return m_info;
            }

            void SetParent(ModuleData* parent)
            {
                m_parent = parent;
            }
        private:

            /**
             * \brief Vmb C Api struct holding the info about the module
             */
            InfoType m_info;

        };

        using CameraData = ModuleDataImpl<VmbCameraInfo_t>;
        using InterfaceData = ModuleDataImpl<VmbInterfaceInfo_t>;
        using TlData = ModuleDataImpl<VmbTransportLayerInfo_t>;

    } // namespace Examples
} // namespace VmbC

#endif
