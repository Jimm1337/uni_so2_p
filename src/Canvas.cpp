#include "Canvas.hpp"
#include <algorithm>
#include "Obstacle.hpp"
#include "Projectile.hpp"
#include "Sprite.hpp"
#include "Text.hpp"

namespace so {

Canvas::Canvas(Canvas&& other) noexcept:
  Grid(std::move(other)), m_bgColor{ other.m_bgColor } {
  other.m_bgColor = BLANK;
}

Canvas& Canvas::operator=(Canvas&& other) noexcept {
  const auto lock = std::unique_lock{ m_mutex };

  if (this == &other) [[unlikely]] { return *this; }

  Grid::operator=(std::move(other));

  m_bgColor = other.m_bgColor;

  other.m_bgColor = BLANK;

  return *this;
}

void Canvas::putLine(Vector2 start, Vector2 end, Color color) noexcept {
        const auto lock = std::unique_lock{ m_mutex };
  m_lines.emplace_back(start, end, color);
}

void Canvas::putFrame(Color color) noexcept {
        const auto lock = std::unique_lock{ m_mutex };
  m_frameColor = color;
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

  std::ranges::for_each(m_lines, [this](const auto& line) {
    const auto [start, end, color]{ line };

    gpuMutex.lock();
    DrawLineEx(start, end, LINE_THICKNESS, color);
    gpuMutex.unlock();
  });

  std::ranges::for_each(m_locations, [](const auto& pair) {
    if (pair.first == nullptr) [[unlikely]] { return; }

    const Object& object = *pair.first;

    switch (object.getType()) {
    case Object::TYPE::SPRITE: {
      const auto& sprite = dynamic_cast< const Sprite& >(object);
      sprite.draw();
      break;
    }
    case Object::TYPE::TEXT: {
      const auto& text = dynamic_cast< const Text& >(object);
      text.draw();
      break;
    }
    case Object::TYPE::PROJECTILE: {
      const auto& projectile = dynamic_cast< const Projectile& >(object);
      projectile.draw();
      break;
    }
    case Object::TYPE::GRID: {
      const auto& grid = dynamic_cast< const Grid& >(object);
      if (grid.getGridClass() == Grid::CLASS::OBSTACLE) {
        const auto& obstacle = dynamic_cast< const Obstacle& >(object);
        obstacle.draw();
      } else {
        grid.draw();
      }
      break;
    }
    [[unlikely]] default:
      break;
    }
  });
}

} // namespace so
