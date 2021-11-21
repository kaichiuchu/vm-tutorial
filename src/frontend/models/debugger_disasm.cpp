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

DebuggerDisasmModel::DebuggerDisasmModel(
    QObject* parent_object, chip8::VMInstance& vm_instance) noexcept
    : QAbstractTableModel(parent_object), vm_instance_(vm_instance) {}

auto DebuggerDisasmModel::GetRowFromAddress(
    const uint_fast16_t address) const noexcept -> int {
  return static_cast<int>(address / 2);
}

auto DebuggerDisasmModel::columnCount(const QModelIndex& parent) const noexcept
    -> int {
  return 5;
}

auto DebuggerDisasmModel::rowCount(const QModelIndex& parent) const noexcept
    -> int {
  return chip8::data_size::kInternalMemory /
         chip8::data_size::kInstructionLength;
}

auto DebuggerDisasmModel::data(const QModelIndex& index,
                               int role) const noexcept -> QVariant {
  if (role == Qt::DisplayRole) {
    switch (index.column()) {
      case Section::kBreakpoint:
        return {};

      case Section::kAddress:
        return QString{"$%1"}
            .arg(2 * index.row(), 1, 16)
            .rightJustified(2, '0')
            .toUpper();

      case Section::kRawInstruction: {
        const auto address = (2 * index.row());

        const auto hi = vm_instance_.impl_->memory_[address + 0];
        const auto lo = vm_instance_.impl_->memory_[address + 1];

        const auto result = (hi << 8) | lo;

        return QString{"$%1"}
            .arg(result, 1, 16)
            .rightJustified(2, '0')
            .toUpper();
      }

      case Section::kDisassembly: {
        const auto address = (2 * index.row());

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
  }
  return {};
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