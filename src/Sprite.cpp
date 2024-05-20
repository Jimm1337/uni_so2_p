#include "Sprite.hpp"

namespace so {

Sprite::Sprite(Sprite&& other) noexcept:
  Object(std::move(other)),
  m_texture{ other.m_texture },
  m_destroyedTexture{ other.m_destroyedTexture },
  m_spriteClass{ other.m_spriteClass },
  m_scale{ other.m_scale } {
  other.m_texture          = { 0 };
  other.m_destroyedTexture = { 0 };
  other.m_scale            = 0.0F;
}

Sprite& Sprite::operator=(Sprite&& other) noexcept {
  const auto lock = std::unique_lock{ m_mutex };

  if (this == &other) [[unlikely]] { return *this; }

  Object::operator=(std::move(other));

  m_texture          = other.m_texture;
  m_destroyedTexture = other.m_destroyedTexture;
  m_spriteClass      = other.m_spriteClass;
  m_scale            = other.m_scale;

  other.m_texture          = { 0 };
  other.m_destroyedTexture = { 0 };
  other.m_scale            = 0.0F;

  return *this;
}

void Sprite::draw() const {
  const auto lock = std::unique_lock{ m_mutex };

  if (m_visible) [[likely]] {
    if (m_destroyed) [[unlikely]] {
      DrawTextureEx(m_destroyedTexture, m_position, 0.0F, m_scale, WHITE);
    } else [[likely]] {
      DrawTextureEx(m_texture, m_position, 0.0F, m_scale, WHITE);
    }
  }
}

Rectangle Sprite::getRect() const {
  const auto lock = std::unique_lock{ m_mutex };

  return { m_position.x,
           m_position.y,
           static_cast< float >(m_texture.width),
           static_cast< float >(m_texture.height) };
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
      destroy();
    }
  }
}

void Sprite::onDestroy() {
  UnloadTexture(m_texture);
  UnloadTexture(m_destroyedTexture);
}

} // namespace so
