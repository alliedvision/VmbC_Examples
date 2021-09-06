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
