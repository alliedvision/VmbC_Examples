/**
 * \date 2021-2023
 * \copyright Allied Vision Technologies. All Rights Reserved.
 *
 * \copyright Subject to the BSD 3-Clause License.
 *
 * \brief Implementation of ::VmbC::Examples::ModuleTreeModel
 */

#include "ModuleTreeModel.h"

#include <limits>
#include <unordered_map>

namespace VmbC
{
    namespace Examples
    {
        ModuleTreeModel::Item::Item()
            : m_parent(nullptr),
            m_indexInParent((std::numeric_limits<size_t>::max)())
        {
        }

        ModuleTreeModel::Item::Item(std::unique_ptr<ModuleData>&& module)
            : m_module(std::move(module)), m_parent(nullptr),
            m_indexInParent((std::numeric_limits<size_t>::max)())
        {
        }

        ModuleTreeModel::ModuleTreeModel(std::vector<std::unique_ptr<ModuleData>>&& moduleData)
        {
            m_data.reserve(moduleData.size());

            for (auto& md : moduleData)
            {
                m_data.emplace_back(std::move(md));
            }
            m_data.shrink_to_fit();

            // create mapping for ModuleData to Item containing it
            std::unordered_map<ModuleData*, Item*> mapping;
            mapping[nullptr] = &m_pseudoRoot;

            for (auto& item : m_data)
            {
                mapping[item.m_module.get()] = &item;
            }

            // initialize parent and child lists
            for (auto& item : m_data)
            {
                auto parentItem = mapping[item.m_module->GetParent()];

                item.m_parent = parentItem;
                item.m_indexInParent = parentItem->m_children.size();
                parentItem->m_children.push_back(&item);
            }
        }

        QModelIndex ModuleTreeModel::index(int r, int column, QModelIndex const& parent) const
        {
            if (column != 0 || r < 0)
            {
                return QModelIndex();
            }

            size_t const row = static_cast<size_t>(r);

            if (parent.isValid())
            {
                auto const ptr = parent.internalPointer();
                if (column != 0 || ptr == nullptr)
                {
                    return QModelIndex();
                }
                auto& children = static_cast<Item*>(ptr)->m_children;

                return (children.size() > row)
                    ? createIndex(r, column, children[row]) : QModelIndex();
            }
            else
            {
                return row >= m_pseudoRoot.m_children.size() ? QModelIndex() : createIndex(r, column, m_pseudoRoot.m_children[row]);
            }
        }

        QModelIndex ModuleTreeModel::parent(QModelIndex const& index) const
        {
            if (index.isValid())
            {
                Item const* const item = static_cast<Item*>(index.internalPointer());
                if (item == nullptr)
                {
                    return QModelIndex();
                }
                else
                {
                    auto const parentItem = item->m_parent;
                    return (parentItem == nullptr) ? QModelIndex() : createIndex(static_cast<int>(parentItem->m_indexInParent), 0, parentItem);
                }
            }
            else
            {
                return QModelIndex();
            }
        }

        int ModuleTreeModel::rowCount(QModelIndex const& parent) const
        {
            if (parent.isValid())
            {
                Item const* const item = static_cast<Item*>(parent.internalPointer());
                return (item == nullptr) ? 0 : static_cast<int>(item->m_children.size());
            }
            else
            {
                return static_cast<int>(m_pseudoRoot.m_children.size());
            }
        }

        int ModuleTreeModel::columnCount(QModelIndex const& parent) const
        {
            return 1;
        }

        namespace
        {
            struct DataRetrievalContext
            {
                Qt::ItemDataRole m_role;
                QVariant& m_result;
            };
        }

        QVariant ModuleTreeModel::data(QModelIndex const& index, int role) const
        {
            QVariant result;

            Item const* const item = static_cast<Item*>(index.internalPointer());

            if (item != nullptr)
            {
                DataRetrievalVisitor visitor { static_cast<Qt::ItemDataRole>(role), result };
                item->m_module->Accept(visitor);
            }

            return result;
        }

        Qt::ItemFlags ModuleTreeModel::flags(QModelIndex const& index) const
        {
            Qt::ItemFlags flags = Qt::ItemFlag::ItemIsEnabled;

            if (index.isValid())
            {
                auto ptr = index.internalPointer();
                if (ptr != nullptr)
                {
                    FlagUpdateVisitor visitor(flags);
                    static_cast<Item*>(ptr)->m_module->Accept(visitor);
                }
            }

            return flags;
        }

        ModuleData const* ModuleTreeModel::GetModule(QModelIndex const& modelIndex)
        {
            if (!modelIndex.isValid())
            {
                return nullptr;
            }
            auto ptr = modelIndex.internalPointer();
            return (ptr == nullptr) ? nullptr : static_cast<Item*>(ptr)->m_module.get();
        }

        void ModuleTreeModel::DataRetrievalVisitor::Visit(VmbCameraInfo_t const& data)
        {
            switch (m_role)
            {
            case Qt::ItemDataRole::DisplayRole:
                m_result = QString(data.modelName) + QString(" (") + QString(data.cameraName) + QString(")");
                break;
            }
        }

        void ModuleTreeModel::DataRetrievalVisitor::Visit(VmbInterfaceInfo_t const& data)
        {
            switch (m_role)
            {
            case Qt::ItemDataRole::DisplayRole:
                m_result = QString(data.interfaceName);
                break;
            }
        }

        void ModuleTreeModel::DataRetrievalVisitor::Visit(VmbTransportLayerInfo_t const& data)
        {
            switch (m_role)
            {
            case Qt::ItemDataRole::DisplayRole:
                m_result = QString(data.transportLayerName);
                break;
            case Qt::ItemDataRole::ToolTipRole:
                m_result = QString::fromStdString(std::string("transportLayerName: ") + data.transportLayerName);
                break;
            }
        }

        ModuleTreeModel::DataRetrievalVisitor::DataRetrievalVisitor(Qt::ItemDataRole role, QVariant& result)
            : m_role(role), m_result(result)
        {
        }

        void ModuleTreeModel::FlagUpdateVisitor::Visit(VmbCameraInfo_t const& data)
        {
            m_flags |= (Qt::ItemFlag::ItemNeverHasChildren | Qt::ItemFlag::ItemIsSelectable);
        }
} // namespace Examples
} // namespace VmbC
