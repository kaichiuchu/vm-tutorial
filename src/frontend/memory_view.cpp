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

#include "memory_view.h"

#include <QPainter>
#include <QScrollBar>

MemoryViewWidget::MemoryViewWidget(QWidget* parent) noexcept
    : QAbstractScrollArea(parent) {
  connect(verticalScrollBar(), &QScrollBar::valueChanged, this,
          &MemoryViewWidget::UpdateContent);
  connect(horizontalScrollBar(), &QScrollBar::valueChanged, this,
          &MemoryViewWidget::UpdateContent);

  QFont fixedFont;
  fixedFont.setFamily(QStringLiteral("Consolas"));
  fixedFont.setFixedPitch(true);
  fixedFont.setStyleHint(QFont::TypeWriter);
  fixedFont.setPointSize(10);

  QAbstractScrollArea::setFont(fixedFont);

  UpdateFontMetrics();
}

void MemoryViewWidget::SetData(const void* data,
                               const unsigned int size) noexcept {
  current_data_ = data;
  current_data_size_ = size;

  UpdateContent();
}

void MemoryViewWidget::paintEvent(QPaintEvent* event) noexcept {
  if (!current_data_) {
    return;
  }

  QPainter painter(viewport());
  painter.setFont(font());
  painter.setPen(viewport()->palette().color(QPalette::WindowText));

  const auto offset_x = horizontalScrollBar()->value();

  DrawColumns(painter, offset_x);
  DrawAddressDivider(painter, offset_x);
  DrawHeaderDivider(painter, offset_x);
  DrawData(painter, offset_x);
}

void MemoryViewWidget::resizeEvent(QResizeEvent* event) noexcept {
  UpdateContent();
}

auto MemoryViewWidget::GetHexAddressWidth() const noexcept -> unsigned int {
  return 6 * char_width_;
}

auto MemoryViewWidget::GetHexCharWidth() const noexcept -> unsigned int {
  return 4 * char_width_;
}

auto MemoryViewWidget::GetASCIIWidth() const noexcept -> unsigned int {
  return ((char_width_ * 2) + 1) * char_width_;
}

void MemoryViewWidget::UpdateFontMetrics() noexcept {
  const auto font_metrics = fontMetrics();

  char_width_ = font_metrics.horizontalAdvance('0');
  char_height_ = font_metrics.height();
}

void MemoryViewWidget::DrawAddressDivider(QPainter& painter,
                                          const int offset_x) noexcept {
  const auto hex_address_width = GetHexAddressWidth();

  constexpr auto kPadding = 3;
  const auto x = (hex_address_width + kPadding) - offset_x;

  painter.drawLine(x, 0, x, height());
}

void MemoryViewWidget::DrawHeaderDivider(QPainter& painter,
                                         int offset_x) noexcept {
  constexpr auto kPadding = 3;
  const auto x = char_height_ + kPadding;

  painter.drawLine(0, x, width(), x);
}

void MemoryViewWidget::DrawColumns(QPainter& painter,
                                   const int offset_x) noexcept {
  auto x = (GetHexAddressWidth() + 8) - offset_x;

  for (auto col = 0; col < bytes_per_line_; ++col) {
    if ((col % 2) != 0) {
      painter.fillRect(x - 4, 0, GetHexCharWidth() - 4, height(),
                       viewport()->palette().color(QPalette::AlternateBase));
    }

    const auto col_text =
        QString{"%1"}.arg(col, 1, 16).rightJustified(2, '0').toUpper();

    painter.drawText(x, char_height_, col_text);

    x += GetHexCharWidth();
  }

  x -= 8;
  painter.drawLine(x, 0, x, height());

  x += 8;
  ascii_start_offset_ = x;

  for (auto col = 0; col < bytes_per_line_; ++col) {
    const auto ch =
        (col < 0xA) ? (QChar('0' + col)) : (QChar('A' + (col - 0xA)));
    painter.drawText(x, char_height_, ch);

    x += (2 * char_width_);
  }
}

void MemoryViewWidget::DrawData(QPainter& painter,
                                const int offset_x) noexcept {
  const auto hex_address_width = GetHexAddressWidth();

  // We need to start UNDER the lines we drew.
  auto y = char_height_ * 2;

  const auto row_count = (end_offset_ - start_offset_) / bytes_per_line_;

  auto offset = start_offset_;

  for (auto row = 0; row <= row_count; ++row, y += char_height_) {
    auto ascii_offset = ascii_start_offset_;
    const auto row_address = start_offset_ + (row * bytes_per_line_);

    const auto address_text =
        QString{"%1"}.arg(row_address, 1, 16).rightJustified(4, '0').toUpper();

    auto x = char_width_ - offset_x;

    painter.drawText(x, y, address_text);

    x = (char_width_ + hex_address_width) - offset_x;

    for (auto col = 0;
         ((col < bytes_per_line_) && (offset < current_data_size_));
         ++col, ++offset) {
      uint_fast8_t value;
      std::memcpy(&value,
                  static_cast<const uint_fast8_t*>(current_data_) + offset,
                  sizeof(value));

      value &= 0xFF;

      const auto value_text =
          QString{"%1"}.arg(value, 1, 16).rightJustified(2, '0').toUpper();

      painter.drawText(x + 1, y, value_text);

      if (!std::isprint(value)) {
        value = '.';
      }

      x += GetHexCharWidth();
      painter.drawText(ascii_offset, y, QChar(value));
      ascii_offset += (2 * char_width_);
    }
  }
}

void MemoryViewWidget::UpdateContent() {
  if (current_data_ == nullptr) {
    setEnabled(false);
    return;
  }

  setEnabled(true);

  const auto w = GetHexAddressWidth() + GetHexCharWidth() + GetASCIIWidth();
  horizontalScrollBar()->setRange(0, w - viewport()->width());
  horizontalScrollBar()->setPageStep(viewport()->width());

  const auto rows_visible = viewport()->height() / char_height_;
  const auto val = verticalScrollBar()->value();
  start_offset_ = (size_t)val * bytes_per_line_;
  end_offset_ = start_offset_ + rows_visible * bytes_per_line_ - 1;

  if (end_offset_ >= current_data_size_) {
    end_offset_ = current_data_size_ - 1;
  }

  const auto line_count =
      static_cast<int>(current_data_size_ / bytes_per_line_);
  verticalScrollBar()->setRange(0, line_count - (rows_visible - 1));
  verticalScrollBar()->setPageStep(rows_visible - 1);

  viewport()->update();
}