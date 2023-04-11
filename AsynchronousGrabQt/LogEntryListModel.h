/**
 * \date 2021-2023
 * \copyright Allied Vision Technologies. All Rights Reserved.
 *
 * \copyright Subject to the BSD 3-Clause License.
 *
 * \brief Definition of a model for filling a TreeView with model data
 */

#ifndef ASYNCHRONOUSGRAB_C_LIST_ENTRY_LIST_MODEL_H
#define ASYNCHRONOUSGRAB_C_LIST_ENTRY_LIST_MODEL_H

#include <vector>

#include <QAbstractTableModel>
#include <QVariant>

#include "LogEntry.h"

namespace VmbC
{
    namespace Examples
    {

        class LogEntryListModel : public QAbstractTableModel
        {
        public:
            static constexpr int ErrorCodeColumn = 0;
            static constexpr int MessageColumn = 1;

            LogEntryListModel(QObject* parent = nullptr);

            int columnCount(QModelIndex const& parent) const override;
            int rowCount(QModelIndex const& parent) const override;
            QVariant data(QModelIndex const& index, int role) const override;
            QVariant headerData(int section, Qt::Orientation, int role) const override;

            LogEntryListModel& operator<<(LogEntry&& entry);
        private:

            /**
             * \brief a list of all log entries
             */
            std::vector<LogEntry> m_data;
        };
    } // namespace Examples
} // namespace VmbC

#endif
