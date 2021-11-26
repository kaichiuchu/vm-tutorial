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

#pragma once

#include <core/vm_instance.h>

#include <QAbstractTableModel>
#include <QPixmap>

/// This class provides a model to display CHIP-8 disassembly.
class DebuggerDisasmModel : public QAbstractTableModel {
  Q_OBJECT

 public:
  /// Constructs the model for the disassembly area on the debugger.
  ///
  /// \param parent_widget The parent object of which this class is a child of
  /// it.
  ///
  /// \param vm_instance The virtual machine instance.
  explicit DebuggerDisasmModel(QObject* parent_object,
                               chip8::VMInstance& vm_instance) noexcept;

  /// Determines the row within the model based on the address passed.
  ///
  /// \param address The memory address associated with some row.
  ///
  /// \returns The row, if any.
  auto GetRowFromAddress(uint_fast16_t address) const noexcept -> int;

  /// Sets the address to start disassembling from.
  ///
  /// \param address The address to disassemble from.
  ///
  /// \returns \p true if the address was valid, or \p false otherwise.
  auto SetStartAddress(uint_fast16_t address) noexcept -> bool;

  /// Determines the address associated with a row.
  ///
  /// \returns The address associated with a row.
  auto GetAddressFromRow(unsigned int row) const noexcept -> uint_fast16_t;

 private:
  /// From Qt documentation:
  ///
  /// Returns the number of columns for the children of the given \p parent.
  ///
  /// In most subclasses, the number of columns is independent of the \p parent.
  ///
  /// Note: When implementing a table based model, \ref
  /// QAbstractItemModel::columnCount() should return 0 when the parent is
  /// valid.
  ///
  /// Note: This function can be invoked via the meta-object system and from
  /// QML. See \ref Q_INVOKABLE.
  ///
  /// We override this method to return the number of columns we have.
  auto columnCount(const QModelIndex& parent = {}) const noexcept
      -> int override;

  /// From Qt documentation:
  ///
  /// Returns the number of rows under the given parent. When the parent is
  /// valid it means that rowCount is returning the number of children of
  /// parent.
  ///
  /// Note: When implementing a table based model, \ref
  /// QAbstractItemModel::rowCount() should return 0 when the parent is valid.
  ///
  /// Note: This function can be invoked via the meta-object system and from
  /// QML. See \ref Q_INVOKABLE.
  auto rowCount(const QModelIndex& parent = {}) const noexcept -> int override;

  /// From Qt documentation:
  ///
  /// Returns the data for the given role and section in the header with the
  /// specified orientation.
  ///
  /// For horizontal headers, the section number corresponds to the column
  /// number. Similarly, for vertical headers, the section number corresponds
  /// to the row number.
  ///
  /// Note: This function can be invoked via the meta-object system and from
  /// QML. See \ref Q_INVOKABLE.
  auto headerData(int section, Qt::Orientation orientation,
                  int role = Qt::DisplayRole) const noexcept
      -> QVariant override;

  /// From Qt documentation:
  ///
  /// Returns the data stored under the given role for the item referred to by
  /// the index.
  ///
  /// Note: If you do not have a value to return, return an invalid \ref
  /// QVariant instead of returning 0.
  ///
  /// Note: This function can be invoked via the meta-object system and from
  /// QML. See \ref Q_INVOKABLE.
  auto data(const QModelIndex& index, int role = Qt::DisplayRole) const noexcept
      -> QVariant override;

  /// Defines each of the headers and their locations.
  enum Section {
    kBreakpoint,
    kAddress,
    kRawInstruction,
    kDisassembly,
    kResult
  };

  /// The address to start disassembling from. This is necessary because CHIP-8
  /// instructions are sadly not byte-aligned.
  uint_fast16_t start_address_;

  /// The pixmap used to indicate that a breakpoint is set on an address.
  QPixmap breakpoint_pixmap_;

  /// The pixmap used to indicate what the current program counter is, or the
  /// current stack pointer.
  QPixmap current_address_pixmap_;

  /// The virtual machine instance.
  chip8::VMInstance& vm_instance_;
};