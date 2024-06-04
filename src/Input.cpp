#include "Input.hpp"

namespace so {

Input::Input(Input&& other) noexcept:
  Object(std::move(other)), m_keysDown{ std::move(other.m_keysDown) } {
  other.m_keysDown.clear();
}

Input& Input::operator=(Input&& other) noexcept {
  const auto lock = std::unique_lock{ m_mutex };

  if (this == &other) [[unlikely]] { return *this; }

  Object::operator=(std::move(other));

  m_keysDown = std::move(other.m_keysDown);

  other.m_keysDown.clear();

  return *this;
}

void Input::onUpdate() {
  const auto lock = std::unique_lock{ m_mutex };

  kbdMutex.lock();
  m_keysDown[KEY_LEFT]  = IsKeyDown(KEY_LEFT);
  m_keysDown[KEY_RIGHT] = IsKeyDown(KEY_RIGHT);
  m_keysDown[KEY_SPACE] = IsKeyDown(KEY_SPACE);
  m_keysDown[KEY_ENTER] = IsKeyDown(KEY_ENTER);
  kbdMutex.unlock();
}

bool Input::isKeyDown(KeyboardKey key) const noexcept {
  const auto lock = std::unique_lock{ m_mutex };

  return m_keysDown.at(key);
}

} // namespace so
