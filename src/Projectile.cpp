#include "Projectile.hpp"

namespace so {

Projectile::Projectile(Projectile&& other) noexcept:
  Object(std::move(other)),
  m_width{ other.m_width },
  m_height{ other.m_height },
  m_speed{ other.m_speed },
  m_sourceName{ std::move(other.m_sourceName) },
  m_direction{ other.m_direction },
  m_collided{ other.m_collided },
  m_color{ other.m_color } {
  other.m_width      = 0;
  other.m_height     = 0;
  other.m_speed      = 0;
  other.m_sourceName = "";
  other.m_collided   = false;
  other.m_color      = BLANK;
}

Projectile& Projectile::operator=(Projectile&& other) noexcept {
  const auto lock = std::unique_lock{ m_mutex };

  if (this == &other) [[unlikely]] { return *this; }

  Object::operator=(std::move(other));

  m_width      = other.m_width;
  m_height     = other.m_height;
  m_speed      = other.m_speed;
  m_sourceName = std::move(other.m_sourceName);
  m_direction  = other.m_direction;
  m_collided   = other.m_collided;
  m_color      = other.m_color;

  other.m_width      = 0;
  other.m_height     = 0;
  other.m_speed      = 0;
  other.m_sourceName = "";
  other.m_collided   = false;
  other.m_color      = BLANK;

  return *this;
}

void Projectile::markCollided() {
  const auto lock = std::unique_lock{ m_mutex };

  m_collided = true;
  destroy();
}

void Projectile::draw() const {
  const auto lock = std::unique_lock{ m_mutex };

  if (m_hidden) [[unlikely]] { return; }

  if (!m_collided) [[likely]] {
    gpuMutex.lock();
    DrawRectangle(static_cast< int >(m_position.x),
                  static_cast< int >(m_position.y),
                  static_cast< int >(m_width),
                  static_cast< int >(m_height),
                  m_color);
    gpuMutex.unlock();
  }
}

Rectangle Projectile::getRect() const {
  const auto lock = std::unique_lock{ m_mutex };

  return { m_position.x, m_position.y, m_width, m_height };
}

void Projectile::onUpdate() {
  const auto lock = std::unique_lock{ m_mutex };

  if (m_collided) [[unlikely]] { return; }

  gpuMutex.lock();
  if (m_direction == Direction::UP) {
    m_position.y += m_speed * GetFrameTime();
  } else {
    m_position.y -= m_speed * GetFrameTime();
  }
  gpuMutex.unlock();
}

} // namespace so
