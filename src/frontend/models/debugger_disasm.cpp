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

#include "debugger_disasm.h"

#include <core/disasm.h>

#include <QIcon>
#include <algorithm>

DebuggerDisasmModel::DebuggerDisasmModel(
    QObject* parent_object, chip8::VMInstance& vm_instance) noexcept
    : QAbstractTableModel(parent_object),
      vm_instance_(vm_instance),
      breakpoint_pixmap_(QIcon(QStringLiteral(":/assets/bp_indicator.png"))
                             .pixmap(QSize(12, 12))),
      current_address_pixmap_(
          QIcon(QStringLiteral(":/assets/current_pointer.png"))
              .pixmap(QSize(12, 12))) {}

auto DebuggerDisasmModel::GetRowFromAddress(
    const uint_fast16_t address) const noexcept -> int {
  return static_cast<int>((start_address_ - address) /
                          chip8::data_size::kInstructionLength);
}

auto DebuggerDisasmModel::GetAddressFromRow(
    const unsigned int row) const noexcept -> uint_fast16_t {
  return start_address_ + (chip8::data_size::kInstructionLength * row);
}

auto DebuggerDisasmModel::SetStartAddress(uint_fast16_t address) noexcept
    -> bool {
  if (address >= chip8::data_size::kInternalMemory) {
    return false;
  }

  beginResetModel();
  start_address_ = address;
  endResetModel();

  return true;
}

auto DebuggerDisasmModel::columnCount(const QModelIndex& parent) const noexcept
    -> int {
  return 5;
}

auto DebuggerDisasmModel::rowCount(const QModelIndex& parent) const noexcept
    -> int {
  return (chip8::data_size::kInternalMemory - start_address_) /
         chip8::data_size::kInstructionLength;
}

auto DebuggerDisasmModel::data(const QModelIndex& index,
                               int role) const noexcept -> QVariant {
  const auto address = GetAddressFromRow(index.row());

  switch (role) {
    case Qt::DisplayRole:
      switch (index.column()) {
        case Section::kAddress:
          return QStringLiteral("$%1")
              .arg(address, 4, 16, QLatin1Char('0'))
              .toUpper();

        case Section::kRawInstruction: {
          const auto hi = vm_instance_.impl_->memory_[address + 0];
          const auto lo = vm_instance_.impl_->memory_[address + 1];

          const auto result = (hi << 8) | lo;

          return QStringLiteral("%1")
              .arg(result, 4, 16, QLatin1Char('0'))
              .toUpper();
        }

        case Section::kDisassembly: {
          const auto hi = vm_instance_.impl_->memory_[address + 0];
          const auto lo = vm_instance_.impl_->memory_[address + 1];

          chip8::Instruction instruction((hi << 8) | lo);
          const auto result = chip8::debug::DisassembleInstruction(instruction);

          return QString::fromStdString(result);
        }

        case Section::kResult:
          return {};

        default:
          return {};
      }

    case Qt::DecorationRole:
      switch (index.column()) {
        case Section::kBreakpoint:
          if (vm_instance_.FindBreakpoint(address)) {
            return breakpoint_pixmap_;
          }

          if (vm_instance_.impl_->program_counter_ == address) {
            return current_address_pixmap_;
          }
          return {};

        default:
          return {};
      }

    default:
      return {};
  }
}

auto DebuggerDisasmModel::headerData(int section, Qt::Orientation orientation,
                                     int role) const noexcept -> QVariant {
  if ((role == Qt::DisplayRole) && (orientation == Qt::Horizontal)) {
    switch (section) {
      case Section::kAddress:
        return tr("Address");

      case Section::kRawInstruction:
        return tr("Raw");

      case Section::kDisassembly:
        return tr("Disassembly");

      case Section::kResult:
        return tr("Result");

      default:
        return {};
    }
  }
  return {};
}