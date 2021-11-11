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

#include <QAbstractListModel>

/// This class provides a model to display the current CHIP-8 stack data.
class DebuggerStackModel : public QAbstractListModel {
  Q_OBJECT

 public:
  /// Constructs the model for the stack area on the debugger.
  ///
  /// \param parent_widget The parent object of which this class is a child of
  /// it.
  ///
  /// \param vm_instance The virtual machine instance.
  explicit DebuggerStackModel(QObject* parent_object,
                              chip8::VMInstance& vm_instance) noexcept;

  /// From Qt documentation:
  ///
  /// Returns the number of rows under the given \p parent. When the parent is
  /// valid it means that rowCount is returning the number of children of
  /// parent.
  ///
  /// Note: When implementing a table based model, rowCount() should return 0
  /// when the parent is valid.
  ///
  /// Note: This function can be invoked via the meta-object system and from
  /// QML. See Q_INVOKABLE.
  int rowCount(const QModelIndex& parent = {}) const override;

  /// From Qt documentation:
  ///
  /// Returns the number of columns for the children of the given \p parent.
  ///
  /// In most subclasses, the number of columns is independent of the
  /// \p parent.
  ///
  /// Note: When implementing a table based model, \ref columnCount() should
  /// return 0 when the parent is valid.
  ///
  /// Note: This function can be invoked via the meta-object system and from
  /// QML. See Q_INVOKABLE.
  int columnCount(const QModelIndex& parent = {}) const override;

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
  /// QML. See Q_INVOKABLE.
  QVariant headerData(int section, Qt::Orientation orientation,
                      int role = Qt::DisplayRole) const noexcept override;

  /// From Qt documentation:
  ///
  /// Returns the data stored under the given role for the item referred to
  /// by the index.
  ///
  /// Note: If you do not have a value to return, return an invalid
  /// \ref QVariant instead of returning 0.
  ///
  /// Note: This function can be invoked via the meta-object system and from
  /// QML. See Q_INVOKABLE.
  QVariant data(const QModelIndex& index, int role) const override;

 private:
  /// Defines the column locations within a tree widget.
  enum Columns { kEntry, kValue };

  /// The virtual machine instance.
  chip8::VMInstance& vm_instance_;
};
