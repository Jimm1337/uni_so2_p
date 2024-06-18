#include "Sprite.hpp"
#include <cstdio>
#include <iostream>
#include "fmt/core.h"

namespace so {

void Sprite::draw() const {
  const auto lock = std::unique_lock{ m_mutex };

  if (m_hidden) [[unlikely]] { return; }

  if (m_visible) [[likely]] {
    gpuMutex.lock();
    if (m_destroyed) [[unlikely]] {
      DrawTextureEx(m_destroyedTexture, m_position, 0.0F, m_scale, WHITE);
    } else [[likely]] {
      DrawTextureEx(m_texture, m_position, 0.0F, m_scale, WHITE);
    }
    gpuMutex.unlock();
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
  if (m_noUpdate) [[unlikely]] { return; }

  const auto now = std::chrono::high_resolution_clock::now();

  if (m_visible && m_destroyed) {
    if (std::chrono::duration_cast< std::chrono::milliseconds >(now -
                                                                m_destroyedTime)
          .count() >= DESTROYED_TIME) {
      m_visible = false;
      destroy();
    }
  }
}

void Sprite::onDestroy() {
  gpuMutex.lock();
  UnloadTexture(m_texture);
  UnloadTexture(m_destroyedTexture);
  gpuMutex.unlock();
}

} // namespace so
