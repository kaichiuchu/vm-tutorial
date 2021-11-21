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
/// not modify, the contents of data in a hexadecimal and ASCII fashion.
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

  /// Sets the font to use when drawing the data.
  ///
  /// \param font The font to use when draawing the data.
  void SetFont(const QFont& font) noexcept;

 protected:
  /// From Qt documentation:
  ///
  /// This event handler can be reimplemented in a subclass to receive paint
  /// events passed in \p event.
  ///
  /// A paint event is a request to repaint all or part of a widget. It can
  /// happen for one of the following reasons:
  ///
  /// - \ref QWidget::repaint() or \ref QWidget::update() was invoked,
  /// - the widget was obscured and has now been uncovered, or
  ///   many other reasons.
  ///
  /// Many widgets can simply repaint their entire surface when asked to, but
  /// some slow widgets need to optimize by painting only the requested region:
  /// \ref QPaintEvent::region(). This speed optimization does not change the
  /// result, as painting is clipped to that region during event processing.
  /// \ref QListView and \ref QTableView do this, for example.
  ///
  /// Qt also tries to speed up painting by merging multiple paint events into
  /// one. When \ref QWidget::update() is called several times or the window
  /// system sends several paint events, Qt merges these events into one event
  /// with a larger region (see \ref QRegion::united()). The \ref
  /// QWidget::repaint() function does not permit this optimization, so we
  /// suggest using \ref QWidget::update() whenever possible.
  ///
  /// When the paint event occurs, the update region has normally been erased,
  /// so you are painting on the widget's background.
  ///
  /// The background can be set using \ref QWidget::setBackgroundRole() and \ref
  /// QWidget::setPalette().
  ///
  /// Since Qt 4.0, QWidget automatically double-buffers its painting, so there
  /// is no need to write double-buffering code in paintEvent() to avoid
  /// flicker.
  ///
  /// \b Note: Generally, you should refrain from calling \ref QWidget::update()
  /// or \ref QWidget::repaint() \b inside a paintEvent(). For example, calling
  /// \ref QWidget::update() or \ref QWidget::repaint() on children inside a
  /// paintEvent() results in undefined behavior; the child may or may not get a
  /// paint event.
  ///
  /// \b Warning: If you are using a custom paint engine without Qt's
  /// backingstore, \ref Qt::WA_PaintOnScreen must be set. Otherwise,
  /// QWidget::paintEngine() will never be called; the backingstore will be
  /// used instead.
  ///
  /// We override this event here to draw the widget.
  void paintEvent(QPaintEvent* event) noexcept override;

  /// From Qt documentation:
  ///
  /// This event handler can be reimplemented in a subclass to receive widget
  /// resize events which are passed in the \p event parameter. When
  /// resizeEvent() is called, the widget already has its new geometry. The old
  /// size is accessible through \ref QResizeEvent::oldSize().
  ///
  /// The widget will be erased and receive a paint event immediately after
  /// processing the resize event. No drawing need be (or should be) done
  /// inside this handler.
  ///
  /// We override this event here to adjust the content of the memory view
  /// widget, insofar as to what extent the widget is drawn.
  void resizeEvent(QResizeEvent* event) noexcept override;

 private:
  /// Connects signals from various widgets to slots.
  void ConnectSignalsToSlots() noexcept;

  /// Configures the memory view widget based on the current application
  /// settings.
  void SetupFromAppSettings() noexcept;

  /// Determines the width of a full hex address in pixels, based on the current
  /// font.
  ///
  /// \returns The width of a full hex address in pixels, based on the current
  /// font.
  auto GetHexAddressWidth() const noexcept -> unsigned int;

  /// Determines the width of a hex character in pixels, based on the current
  /// font.
  ///
  /// \returns The width of a hex character in pixels, based on the current
  /// font.
  auto GetHexCharWidth() const noexcept -> unsigned int;

  /// Updates the font metrics, using the current font.
  void UpdateFontMetrics() noexcept;

  /// Draws the divider separating the addresses from the columns and data.
  ///
  /// \param painter The painter to use to draw on the widget.
  void DrawAddressDivider(QPainter& painter) noexcept;

  /// Draws the divider separating the columns from the data.
  ///
  /// \param painter The painter to use to draw on the widget.
  void DrawHeaderDivider(QPainter& painter) noexcept;

  /// Draws the columns defining the last two characters of an address, and the
  /// ASCII header.
  ///
  /// \param painter The painter to use to draw on the widget.
  void DrawColumns(QPainter& painter) noexcept;

  /// Draws the data.
  ///
  /// \param painter The painter to use to draw the data.
  void DrawData(QPainter& painter) noexcept;

  /// The current data from which we are rendering its contents.
  const void* current_data_ = nullptr;

  /// The number of elements within the current data.
  unsigned int current_data_size_ = 0;

  /// This struct contains information regarding pixel distances from a
  /// character or multiple characters. This is adjusted based on the current
  /// font.
  struct {
    /// The distance appropriate for drawing a character after another.
    unsigned int char_width_;

    /// The height of a character.
    unsigned int char_height_;

    /// The distance appropriate for drawing a data character (e.g. FF) after
    /// another.
    unsigned int data_width_;

    /// The distance appropriate for drawing an address (e.g. FFFF) after
    /// another.
    unsigned int address_width_;

    /// The X position of the ASCII area.
    unsigned int ascii_start_x_;
  } font_metrics_;

  unsigned int start_offset_;
  unsigned int end_offset_;

  /// The number of bytes that should be drawn on one line.
  const unsigned int bytes_per_line_ = 16;

 private slots:
  /// Called when the content of the memory view must be changed.
  void UpdateContent();
};
