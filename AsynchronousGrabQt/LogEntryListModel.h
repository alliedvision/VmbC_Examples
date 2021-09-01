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
