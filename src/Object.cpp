#include "Object.hpp"
#include <algorithm>
#include <ranges>

namespace so {

std::recursive_mutex Object::Object::gpuMutex;

Object::~Object() {
  const auto lock = std::unique_lock{ m_mutex };

  if (m_valid) { destroy(); }
}

void Object::create() {
  const auto lock = std::unique_lock{ m_mutex };

  if (m_valid) [[unlikely]] { return; }

  onCreate();

  m_updateThread = std::jthread(update, std::ref(*this));

  m_valid = true;
}

void Object::destroy() {
  const auto lock = std::unique_lock{ m_mutex };

  if (!m_valid) [[unlikely]] { return; }

  m_valid = false;

  if (m_updateThread.joinable()) [[likely]] { m_stop = true; }

  onDestroy();
}

void Object::draw() const {
}

Rectangle Object::getRect() const {
  const auto lock = std::unique_lock{ m_mutex };

  return { m_position.x, m_position.y, 0.0F, 0.0F };
}

void Object::update(Object& object) {
  while (!object.m_stop) {
    object.m_mutex.lock();

    std::ranges::for_each(object.m_behaviors, [&object](const auto& behavior) {
      behavior(object);
    });

    object.onUpdate();

    object.m_mutex.unlock();

    std::this_thread::yield();
  }
}

void Object::onCreate() {
}

void Object::onDestroy() {
}

void Object::onUpdate() {
}

} // namespace so
