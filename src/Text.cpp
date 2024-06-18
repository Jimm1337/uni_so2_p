#include "Text.hpp"

namespace so {

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
