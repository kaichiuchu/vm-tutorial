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

int DebuggerRegistersModel::rowCount(const QModelIndex&) const {
  /// The additional rows are for the program counter and the stack pointer.
  constexpr auto kAdditionalRows = 2;
  return chip8::data_size::kV + kAdditionalRows;
}

int DebuggerRegistersModel::columnCount(const QModelIndex&) const { return 2; }

QVariant DebuggerRegistersModel::data(const QModelIndex& index,
                                      int role) const {
  if (role == Qt::DisplayRole) {
    const auto row = index.row();

    switch (index.column()) {
      case Columns::kName:
        switch (row) {
          case Rows::kV0... Rows::kVF:
            return QString{"V%1"}.arg(row, 1, 16).toUpper();

          case Rows::kSP:
            return QString{"SP"};

          case Rows::kPC:
            return QString{"PC"};
        }

      case Columns::kValue:
        switch (row) {
          case Rows::kV0... Rows::kVF:
            return QString{"%1"}
                .arg(vm_instance_.impl_->V_[row], 1, 16)
                .rightJustified(2, '0')
                .toUpper();

          case Rows::kSP:
            return QString::number(vm_instance_.impl_->stack_pointer_);

          case Rows::kPC:
            return QString{"%1"}
                .arg(vm_instance_.impl_->program_counter_, 1, 16)
                .rightJustified(4, '0')
                .toUpper();
        }
    }
  }
  return {};
}

QVariant DebuggerRegistersModel::headerData(int section,
                                            Qt::Orientation orientation,
                                            int role) const noexcept {
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
