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
 * \brief Definition of a model for filling a TreeView with model data
 */

#ifndef ASYNCHRONOUSGRAB_C_MODULE_TREE_MODEL_H
#define ASYNCHRONOUSGRAB_C_MODULE_TREE_MODEL_H

#include <string>
#include <memory>
#include <vector>

#include <QAbstractItemModel>
#include <QVariant>

#include "ModuleData.h"

namespace VmbC
{
    namespace Examples
    {

        class ModuleTreeModel : public QAbstractItemModel
        {
        public:
            ModuleTreeModel(std::vector<std::unique_ptr<ModuleData>>&& moduleData);

            QModelIndex index(int row, int column, QModelIndex const& parent) const override;
            QModelIndex parent(QModelIndex const& index) const override;
            int rowCount(QModelIndex const& parent) const override;
            int columnCount(QModelIndex const& parent) const override;
            QVariant data(QModelIndex const& index, int role) const override;
            Qt::ItemFlags flags(QModelIndex const& index) const override;

            /**
             * \brief get the module data at the given model index
             * \return a pointer to the module data object or null, if the index is invalid
             */
            static ModuleData const* GetModule(QModelIndex const& modelIndex);
        private:
            struct DataRetrievalVisitor : ModuleData::Visitor
            {
                Qt::ItemDataRole m_role;
                QVariant& m_result;

                DataRetrievalVisitor(Qt::ItemDataRole role, QVariant& result);

                void Visit(VmbCameraInfo_t const& data) override;

                void Visit(VmbInterfaceInfo_t const& data) override;

                void Visit(VmbTransportLayerInfo_t const& data) override;
            };

            struct FlagUpdateVisitor : ModuleData::Visitor
            {
                Qt::ItemFlags& m_flags;

                FlagUpdateVisitor(Qt::ItemFlags& flags)
                    : m_flags(flags)
                {
                }

                void Visit(VmbCameraInfo_t const& data) override;

            };

            /**
             * \brief one node in the tree
             */
            struct Item
            {
                Item();

                Item(std::unique_ptr<ModuleData>&& module);

                Item(Item&&) = default;

                Item& operator=(Item&&) = default;

                std::unique_ptr<ModuleData> m_module;

                Item* m_parent;
                size_t m_indexInParent;
                std::vector<Item*> m_children;
            };

            /**
             * \brief item used as parent for the actual roots of the trees
             */
            Item m_pseudoRoot;

            /**
             * \brief a list of all items of the module  
             */
            std::vector<Item> m_data;
        };
    } // namespace Examples
} // namespace VmbC

#endif
