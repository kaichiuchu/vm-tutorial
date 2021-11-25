// vm-tutorial - Virtual machine tutorial targeting CHIP-8
//
// Written in 2021 by Michael Rodriguez aka kaichiuchu <mike@kaichiuchu.dev>
//
// To the extent possible under law, the author(s) have dedicated all copyright
// and related and neighboring rights to this software to the public domain
// worldwide. This software is distributed without any warranty.
//
// You should have received a copy of the CC0 Public Domain Dedication along
// with this software. If not, see
// <http://creativecommons.org/publicdomain/zero/1.0/>.

#include "debugger_registers.h"

DebuggerRegistersModel::DebuggerRegistersModel(
    QObject* parent_object, chip8::VMInstance& vm_instance) noexcept
    : QAbstractListModel(parent_object), vm_instance_(vm_instance) {}

auto DebuggerRegistersModel::rowCount(const QModelIndex&) const noexcept
    -> int {
  /// The additional rows are for the program counter, stack pointer, delay and
  /// sound timers.
  constexpr auto kAdditionalRows = 4;
  return chip8::data_size::kV + kAdditionalRows;
}

auto DebuggerRegistersModel::columnCount(const QModelIndex&) const noexcept
    -> int {
  return 2;
}

auto DebuggerRegistersModel::data(const QModelIndex& index,
                                  int role) const noexcept -> QVariant {
  const auto row = index.row();

  switch (role) {
    case Qt::DisplayRole:
      switch (index.column()) {
        case Columns::kName:
          switch (row) {
            case Rows::kV0... Rows::kVF:
              return QStringLiteral("V%1").arg(row, 1, 16).toUpper();

            case Rows::kSP:
              return QStringLiteral("SP");

            case Rows::kPC:
              return QStringLiteral("PC");

            case Rows::kDT:
              return QStringLiteral("DT");

            case Rows::kST:
              return QStringLiteral("ST");

            default:
              return {};
          }

        case Columns::kValue:
          switch (row) {
            case Rows::kV0... Rows::kVF:
              return QStringLiteral("$%1")
                  .arg(vm_instance_.impl_->V_[row], 2, 16, QLatin1Char('0'))
                  .toUpper();

            case Rows::kSP:
              return QString::number(vm_instance_.impl_->stack_pointer_);

            case Rows::kPC:
              return QStringLiteral("$%1")
                  .arg(vm_instance_.impl_->program_counter_, 4, 16,
                       QLatin1Char('0'))
                  .toUpper();

            case Rows::kDT:
              return QStringLiteral("$%1")
                  .arg(vm_instance_.impl_->delay_timer_, 2, 16,
                       QLatin1Char('0'))
                  .toUpper();

            case Rows::kST:
              return QStringLiteral("$%1")
                  .arg(vm_instance_.impl_->sound_timer_, 2, 16,
                       QLatin1Char('0'))
                  .toUpper();

            default:
              return {};
          }

        default:
          return {};
      }

    default:
      return {};
  }
}

auto DebuggerRegistersModel::headerData(int section,
                                        Qt::Orientation orientation,
                                        int role) const noexcept -> QVariant {
  if ((role == Qt::DisplayRole) && (orientation == Qt::Horizontal)) {
    switch (section) {
      case Columns::kName:
        return tr("Name");

      case Columns::kValue:
        return tr("Value");
    }
  }
  return {};
}
