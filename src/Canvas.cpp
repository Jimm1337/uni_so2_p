#include "Canvas.hpp"
#include <algorithm>
#include "Projectile.hpp"
#include "Sprite.hpp"
#include "Text.hpp"
#include "fmt/core.h"

namespace so {

void Canvas::putLine(Vector2 start, Vector2 end, Color color) noexcept {
  const auto lock = std::unique_lock{ m_mutex };
  m_lines.emplace_back(start, end, color);
}

void Canvas::putFrame(Color color) noexcept {
  const auto lock = std::unique_lock{ m_mutex };
  m_frameColor    = color;
}

void Canvas::draw() const {
  const auto lock = std::unique_lock{ m_mutex };

  if (m_hidden) [[unlikely]] { return; }

  const auto rect = getRect();

  gpuMutex.lock();
  DrawRectangle(static_cast< int >(rect.x),
                static_cast< int >(rect.y),
                static_cast< int >(rect.width),
                static_cast< int >(rect.height),
                m_bgColor);
  DrawRectangleLinesEx(getRect(), LINE_THICKNESS, m_frameColor);
  gpuMutex.unlock();

  std::ranges::for_each(m_lines, [](const auto& line) {
    const auto [start, end, color]{ line };

    gpuMutex.lock();
    DrawLineEx(start, end, LINE_THICKNESS, color);
    gpuMutex.unlock();
  });

  std::ranges::for_each(m_objects, [](const Object* object) {
    if (object == nullptr) [[unlikely]] { return; }

    object->draw();
  });
}

} // namespace so
