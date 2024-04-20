#include "Sprite.hpp"

namespace so {

Sprite::Sprite(Sprite&& other) noexcept:
  Object(std::move(other)),
  m_texture{ other.m_texture },
  m_destroyedTexture{ other.m_destroyedTexture },
  m_spriteClass{ other.m_spriteClass } {
  other.m_texture          = { 0 };
  other.m_destroyedTexture = { 0 };
}

Sprite& Sprite::operator=(Sprite&& other) noexcept {
  const auto lock = std::unique_lock{ m_mutex };

  if (this == &other) [[unlikely]] { return *this; }

  Object::operator=(std::move(other));

  m_texture          = other.m_texture;
  m_destroyedTexture = other.m_destroyedTexture;
  m_spriteClass      = other.m_spriteClass;

  other.m_texture          = { 0 };
  other.m_destroyedTexture = { 0 };

  return *this;
}

void Sprite::draw() const {
  const auto lock = std::unique_lock{ m_mutex };

  if (m_visible) [[likely]] {
    if (m_destroyed) [[unlikely]] {
      DrawTexture(m_destroyedTexture,
                  static_cast< int >(m_position.x),
                  static_cast< int >(m_position.y),
                  WHITE);
    } else [[likely]] {
      DrawTexture(m_texture,
                  static_cast< int >(m_position.x),
                  static_cast< int >(m_position.y),
                  WHITE);
    }
  }
}

void Sprite::markDestroyed() {
  const auto lock = std::unique_lock{ m_mutex };

  if (m_destroyed) [[unlikely]] { return; }

  m_destroyed     = true;
  m_destroyedTime = std::chrono::high_resolution_clock::now();
}

void Sprite::onUpdate() {
  if (m_visible && m_destroyed) [[unlikely]] {
    const auto now = std::chrono::high_resolution_clock::now();

    if (std::chrono::duration_cast< std::chrono::milliseconds >(now -
                                                                m_destroyedTime)
          .count() >= DESTROYED_TIME) [[unlikely]] {
      m_visible = false;
    }
  }
}

} // namespace so
