#include "Text.hpp"

namespace so {

Text::Text(Text&& other) noexcept:
  Object(std::move(other)),
  m_text{ std::move(other.m_text) },
  m_fontSize{ other.m_fontSize },
  m_color{ other.m_color },
  m_spacing{ other.m_spacing },
  m_font{ other.m_font } {
  other.m_text     = "";
  other.m_fontSize = 0;
  other.m_color    = BLANK;
  other.m_spacing  = 0;
  other.m_font     = { 0 };
}

Text& Text::operator=(Text&& other) noexcept {
  const auto lock = std::unique_lock{ m_mutex };

  if (this == &other) [[unlikely]] { return *this; }

  Object::operator=(std::move(other));

  m_text     = std::move(other.m_text);
  m_fontSize = other.m_fontSize;
  m_color    = other.m_color;
  m_spacing  = other.m_spacing;
  m_font     = other.m_font;

  other.m_text     = "";
  other.m_fontSize = 0;
  other.m_color    = BLANK;
  other.m_spacing  = 0;
  other.m_font     = { 0 };

  return *this;
}

void Text::draw() const {
  const auto lock = std::unique_lock{ m_mutex };

  if (m_hidden) [[unlikely]] { return; }

  if (m_visible) [[likely]] {
    gpuMutex.lock();
    DrawTextEx(
      m_font, m_text.c_str(), m_position, m_fontSize, m_spacing, m_color);
    gpuMutex.unlock();
  }
}

Rectangle Text::getRect() const {
  const auto lock = std::unique_lock{ m_mutex };

  gpuMutex.lock();
  const auto size =
    MeasureTextEx(m_font, m_text.c_str(), m_fontSize, m_spacing);
  gpuMutex.unlock();

  return { m_position.x, m_position.y, size.x, size.y };
}

void Text::onDestroy() {
  const auto lock = std::unique_lock{ m_mutex };

  gpuMutex.lock();
  UnloadFont(m_font);
  gpuMutex.unlock();
}

} // namespace so
