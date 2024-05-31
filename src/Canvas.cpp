#include "Canvas.hpp"
#include <algorithm>
#include <ranges>
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

void Canvas::draw() const {
  const auto lock = std::unique_lock{ m_mutex };

  const auto rect = getRect();

  DrawRectangle(static_cast< int >(rect.x),
                static_cast< int >(rect.y),
                static_cast< int >(rect.width),
                static_cast< int >(rect.height),
                m_bgColor);

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
    [[unlikely]] default:
      break;
    }
  });
}

} // namespace so
