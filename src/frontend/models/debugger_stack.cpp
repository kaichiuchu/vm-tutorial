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

#include <QIcon>

DebuggerStackModel::DebuggerStackModel(QObject* parent_object,
                                       chip8::VMInstance& vm_instance) noexcept
    : QAbstractListModel(parent_object),
      vm_instance_(vm_instance),
      current_stack_pixmap_(
          QIcon(QStringLiteral(":/assets/current_pointer.png"))
              .pixmap(QSize(12, 12))) {}

auto DebuggerStackModel::rowCount(const QModelIndex&) const noexcept -> int {
  return chip8::data_size::kStack;
}

auto DebuggerStackModel::columnCount(const QModelIndex&) const noexcept -> int {
  return 2;
}

auto DebuggerStackModel::data(const QModelIndex& index, int role) const noexcept
    -> QVariant {
  switch (role) {
    case Qt::DisplayRole: {
      const auto row = index.row();

      switch (index.column()) {
        case Columns::kEntry:
          return row;

        case Columns::kValue:
          return QString{"%1"}
              .arg(vm_instance_.impl_->stack_[row], 1, 16)
              .rightJustified(4, '0')
              .toUpper();

        default:
          return {};
      }
    }

    case Qt::DecorationRole:
      if ((index.column() == Columns::kEntry) &&
          (index.row() == vm_instance_.impl_->stack_pointer_)) {
        return current_stack_pixmap_;
      }
      return {};

    default:
      return {};
  }
  return {};
}

auto DebuggerStackModel::headerData(int section, Qt::Orientation orientation,
                                    int role) const noexcept -> QVariant {
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
