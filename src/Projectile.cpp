#include "Projectile.hpp"
#include "fmt/core.h"
#include <thread>

namespace so {

void Projectile::markCollided() {
  const auto lock = std::unique_lock{ m_mutex };

  m_collided = true;
  destroy();
}

void Projectile::draw() const {
  const auto lock = std::unique_lock{ m_mutex };

  if (m_hidden) [[unlikely]] { return; }

  if (!m_collided) [[likely]] {
    const auto lockg = std::unique_lock{ gpuMutex };
    DrawRectangle(static_cast< int >(m_position.x),
                  static_cast< int >(m_position.y),
                  static_cast< int >(m_width),
                  static_cast< int >(m_height),
                  m_color);
  }
}

Rectangle Projectile::getRect() const {
  const auto lock = std::unique_lock{ m_mutex };

  return { m_position.x, m_position.y, m_width, m_height };
}

void Projectile::onUpdate() {
  if (m_collided) [[unlikely]] { return; }

  if (m_direction == Direction::DOWN) {
    m_position.y += m_speed * GetFrameTime();
  } else {
    m_position.y -= m_speed * GetFrameTime();
  }
}

} // namespace so
