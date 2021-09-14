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
 * \brief Implementation of LogEntryListModel.
 */

#include "LogEntryListModel.h"

namespace VmbC
{
    namespace Examples
    {
        LogEntryListModel::LogEntryListModel(QObject* parent)
            : QAbstractTableModel(parent)
        {
        }

        int LogEntryListModel::columnCount(QModelIndex const& parent) const
        {
            return 2;
        }

        int LogEntryListModel::rowCount(QModelIndex const& parent) const
        {
            return static_cast<int>(m_data.size());
        }

        namespace
        {
            QVariant ErrorCodeText(VmbError_t errorCode)
            {
                if (errorCode == VmbErrorSuccess)
                {
                    return {};
                }
                return QString::number(errorCode);
            }
        }

        QVariant LogEntryListModel::data(QModelIndex const& index, int role) const
        {
            if (role == Qt::ItemDataRole::DisplayRole)
            {
                auto& entry = m_data.at(static_cast<size_t>(index.row()));
                switch (index.column())
                {
                case ErrorCodeColumn:
                    return ErrorCodeText(entry.GetErrorCode());
                case MessageColumn:
                    return QString::fromStdString(entry.GetMessage());
                }
            }

            return QVariant();
        }

        QVariant LogEntryListModel::headerData(int section, Qt::Orientation, int role) const
        {
            if (role == Qt::DisplayRole)
            {
                switch (section)
                {
                case ErrorCodeColumn:
                    return QString("Error Code");
                case MessageColumn:
                    return QString("Message");
                }
            }
            return QVariant();
        }

        LogEntryListModel& LogEntryListModel::operator<<(LogEntry&& entry)
        {
            int size = static_cast<int>(m_data.size());
            beginInsertRows(QModelIndex(), size, size);
            try
            {
                m_data.emplace_back(std::move(entry));
                insertRows(size, 1);
            }
            catch (...)
            {
            }
            endInsertRows();
            return *this;
        }

    } // namespace Examples
} // namespace VmbC
