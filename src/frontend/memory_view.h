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

#include <QAbstractScrollArea>

/// This class defines a memory view widget. It allows a user to inspect, but
/// not modify, the contents of data in a hexadecimal/ASCII fashion.
class MemoryViewWidget : public QAbstractScrollArea {
  Q_OBJECT

 public:
  /// Constructs the memory view widget.
  ///
  /// \param parent_widget The parent object of which this class is a child of
  /// it.
  explicit MemoryViewWidget(QWidget* parent_widget = nullptr) noexcept;

  /// Sets the data to display.
  ///
  /// \param container A container containing any kind of data.
  template <typename Container, typename T = typename Container::value_type>
  void SetData(const Container& container) noexcept {
    SetData(container.data(), container.size());
  }

  /// Sets the data to display.
  ///
  /// \param data The data to display.
  /// \param size The size of the data, in bytes.
  void SetData(const void* data, unsigned int size) noexcept;

 protected:
  /// From Qt documentation:
  ///
  /// This event handler can be reimplemented in a subclass to receive paint
  /// events passed in \p event.
  ///
  /// A paint event is a request to repaint all or part of a widget. It can
  /// happen for one of the following reasons:
  ///
  /// repaint() or update() was invoked,
  /// the widget was obscured and has now been uncovered, or
  /// many other reasons.
  ///
  /// Many widgets can simply repaint their entire surface when asked to, but
  /// some slow widgets need to optimize by painting only the requested region:
  /// QPaintEvent::region(). This speed optimization does not change the
  /// result, as painting is clipped to that region during event processing.
  /// QListView and QTableView do this, for example.
  ///
  /// Qt also tries to speed up painting by merging multiple paint events into
  /// one. When update() is called several times or the window system sends
  /// several paint events, Qt merges these events into one event with a larger
  /// region (see QRegion::united()). The repaint() function does not permit
  /// this optimization, so we suggest using update() whenever possible.
  ///
  /// When the paint event occurs, the update region has normally been erased,
  /// so you are painting on the widget's background.
  ///
  /// The background can be set using setBackgroundRole() and setPalette().
  ///
  /// Since Qt 4.0, QWidget automatically double-buffers its painting, so there
  /// is no need to write double-buffering code in paintEvent() to avoid
  /// flicker.
  ///
  /// Note: Generally, you should refrain from calling update() or repaint()
  /// inside a paintEvent(). For example, calling update() or repaint() on
  /// children inside a paintEvent() results in undefined behavior; the child
  /// may or may not get a paint event.
  ///
  /// Warning: If you are using a custom paint engine without Qt's
  /// backingstore, \p Qt::WA_PaintOnScreen must be set. Otherwise,
  /// QWidget::paintEngine() will never be called; the backingstore will be
  /// used instead.
  void paintEvent(QPaintEvent* event) noexcept override;

  /// From Qt documentation:
  ///
  /// This event handler can be reimplemented in a subclass to receive widget
  /// resize events which are passed in the event parameter. When resizeEvent()
  /// is called, the widget already has its new geometry. The old size is
  /// accessible through QResizeEvent::oldSize().
  ///
  /// The widget will be erased and receive a paint event immediately after
  /// processing the resize event. No drawing need be (or should be) done
  /// inside this handler.
  void resizeEvent(QResizeEvent* event) noexcept override;

 private:
  auto GetHexAddressWidth() const noexcept -> unsigned int;

  auto GetHexCharWidth() const noexcept -> unsigned int;
  auto GetASCIIWidth() const noexcept -> unsigned int;

  /// Updates the font metrics.
  void UpdateFontMetrics() noexcept;

  void DrawAddressDivider(QPainter& painter, int offset_x) noexcept;
  void DrawHeaderDivider(QPainter& painter, int offset_x) noexcept;
  void DrawColumns(QPainter& painter, int offset_x) noexcept;

  /// Draws the data.
  ///
  /// \param painter The painter to use to draw the data.
  void DrawData(QPainter& painter, int offset_x) noexcept;

  /// The current data from which we are rendering its contents.
  const void* current_data_ = nullptr;

  /// The size of the current data.
  unsigned int current_data_size_ = 0;

  /// The distance appropriate for drawing a character after another, based on
  /// the current font.
  int char_width_;

  /// The height of the current font.
  int char_height_;

  /// The number of bytes that should be drawn on one line.
  unsigned int bytes_per_line_ = 16;

  int start_offset_;
  int end_offset_;
  int ascii_start_offset_;

 private slots:
  /// Called when the content of the memory view must be changed.
  void UpdateContent();
};