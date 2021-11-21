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

#include "models/app_settings.h"

MemoryViewWidget::MemoryViewWidget(QWidget* parent) noexcept
    : QAbstractScrollArea(parent) {
  ConnectSignalsToSlots();
  SetupFromAppSettings();
}

void MemoryViewWidget::SetData(const void* data,
                               const unsigned int size) noexcept {
  current_data_ = data;
  current_data_size_ = size;

  UpdateContent();
}

void MemoryViewWidget::SetFont(const QFont& font) noexcept {
  QAbstractScrollArea::setFont(font);
  UpdateFontMetrics();
}

void MemoryViewWidget::paintEvent(QPaintEvent* event) noexcept {
  if (!current_data_) {
    return;
  }

  QPainter painter(viewport());
  painter.setFont(font());
  painter.setPen(viewport()->palette().color(QPalette::WindowText));

  DrawColumns(painter);
  DrawAddressDivider(painter);
  DrawHeaderDivider(painter);
  DrawData(painter);
}

void MemoryViewWidget::resizeEvent(QResizeEvent* event) noexcept {
  UpdateContent();
}

void MemoryViewWidget::ConnectSignalsToSlots() noexcept {
  connect(verticalScrollBar(), &QScrollBar::valueChanged, this,
          &MemoryViewWidget::UpdateContent);
}

void MemoryViewWidget::SetupFromAppSettings() noexcept {
  SetFont(AppSettingsModel().GetDebuggerFont());
}

void MemoryViewWidget::UpdateFontMetrics() noexcept {
  const auto font_metrics = fontMetrics();

  font_metrics_.data_width_ = font_metrics.horizontalAdvance("FF");
  font_metrics_.address_width_ = font_metrics.horizontalAdvance("FFFF");

  font_metrics_.char_width_ = font_metrics.horizontalAdvance('0');
  font_metrics_.char_height_ = font_metrics.height();

  font_metrics_.ascii_start_x_ =
      (font_metrics_.data_width_ * bytes_per_line_ * 2) + 36;
}

void MemoryViewWidget::DrawAddressDivider(QPainter& painter) noexcept {
  painter.drawLine(font_metrics_.address_width_, 0,
                   font_metrics_.address_width_, height());
}

void MemoryViewWidget::DrawHeaderDivider(QPainter& painter) noexcept {
  const auto kPaddedHeight = font_metrics_.char_height_ + 3;

  painter.drawLine(0, kPaddedHeight, width(), kPaddedHeight);
}

void MemoryViewWidget::DrawColumns(QPainter& painter) noexcept {
  auto x = ((font_metrics_.data_width_ * 2) + 8);
  auto ascii_x_ = font_metrics_.ascii_start_x_;

  for (auto col = 0; col < bytes_per_line_; ++col,
            x += (font_metrics_.data_width_ * 2),
            ascii_x_ += (font_metrics_.char_width_ * 2)) {
    if ((col % 2) != 0) {
      painter.fillRect(x, 0, font_metrics_.data_width_, height(),
                       viewport()->palette().color(QPalette::AlternateBase));
    }

    const auto col_text =
        QString{"%1"}.arg(col, 1, 16).rightJustified(2, '0').toUpper();

    painter.drawText(x, font_metrics_.char_height_, col_text);

    const auto ch =
        (col < 0xA) ? (QChar('0' + col)) : (QChar('A' + (col - 0xA)));
    painter.drawText(ascii_x_, font_metrics_.char_height_, ch);
  }
}

void MemoryViewWidget::DrawData(QPainter& painter) noexcept {
  // We need to start UNDER the lines we drew.
  auto y = font_metrics_.char_height_ * 2;

  const auto row_count = (end_offset_ - start_offset_) / bytes_per_line_;

  auto offset = start_offset_;

  for (auto row = 0; row <= row_count; ++row, y += font_metrics_.char_height_) {
    auto ascii_offset = font_metrics_.ascii_start_x_;
    auto x = 0;

    const auto row_address = start_offset_ + (row * bytes_per_line_);

    const auto address_text =
        QString{"%1"}.arg(row_address, 1, 16).rightJustified(4, '0').toUpper();

    painter.drawText(x, y, address_text);

    x = (font_metrics_.data_width_ * 2) + 8;

    for (auto col = 0;
         ((col < bytes_per_line_) && (offset < current_data_size_));
         ++col, ++offset) {
      uint_fast8_t value;
      std::memcpy(&value,
                  static_cast<const uint_fast8_t*>(current_data_) + offset,
                  sizeof(uint_fast8_t));

      value &= 0xFF;

      const auto value_text =
          QString{"%1"}.arg(value, 1, 16).rightJustified(2, '0').toUpper();

      painter.drawText(x, y, value_text);
      x += (font_metrics_.data_width_ * 2);

      if (!std::isprint(value)) {
        value = '.';
      }

      painter.drawText(ascii_offset, y, QChar(value));
      ascii_offset += (font_metrics_.char_width_ * 2);
    }
  }
}

void MemoryViewWidget::UpdateContent() {
  const auto rows_visible = viewport()->height() / font_metrics_.char_height_;
  const auto val = verticalScrollBar()->value();
  start_offset_ = val * bytes_per_line_;
  end_offset_ = start_offset_ + (rows_visible * bytes_per_line_ + 2);

  if (end_offset_ >= current_data_size_) {
    end_offset_ = current_data_size_ - 1;
  }

  const auto line_count =
      static_cast<int>(current_data_size_ / bytes_per_line_);
  verticalScrollBar()->setRange(0, line_count - rows_visible + 1);
  verticalScrollBar()->setPageStep(rows_visible);

  viewport()->update();
}
