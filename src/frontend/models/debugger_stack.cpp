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

#include "debugger_stack.h"

DebuggerStackModel::DebuggerStackModel(QObject* parent_object,
                                       chip8::VMInstance& vm_instance) noexcept
    : QAbstractListModel(parent_object), vm_instance_(vm_instance) {}

int DebuggerStackModel::rowCount(const QModelIndex&) const {
  return chip8::data_size::kStack;
}

int DebuggerStackModel::columnCount(const QModelIndex&) const { return 2; }

QVariant DebuggerStackModel::data(const QModelIndex& index, int role) const {
  if (role == Qt::DisplayRole) {
    const auto row = index.row();

    switch (index.column()) {
      case Columns::kEntry:
        return row;

      case Columns::kValue:
        return QString{"%1"}
            .arg(vm_instance_.impl_->stack_[row], 1, 16)
            .rightJustified(4, '0')
            .toUpper();
    }
  }
  return {};
}

QVariant DebuggerStackModel::headerData(int section,
                                        Qt::Orientation orientation,
                                        int role) const noexcept {
  if ((role == Qt::DisplayRole) && (orientation == Qt::Horizontal)) {
    switch (section) {
      case Columns::kEntry:
        return tr("Entry");

      case Columns::kValue:
        return tr("Value");

      default:
        return {};
    }
  }
  return {};
}
