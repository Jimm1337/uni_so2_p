#include "Object.hpp"
#include <algorithm>
#include <ranges>

namespace so {

Object::Object(const Object& other):
  m_name(other.m_name),
  m_position(other.m_position),
  m_behaviors(other.m_behaviors),
  m_type(other.m_type) {
  create();
}

Object::Object(Object&& other) noexcept:
  m_name(std::move(other.m_name)),
  m_position(other.m_position),
  m_behaviors(std::move(other.m_behaviors)),
  m_type(other.m_type),
  m_updateThread(std::move(other.m_updateThread)),
  m_valid(other.m_valid) {
  other.m_valid = false;
}

Object& Object::operator=(const Object& other) {
  const auto lock = std::unique_lock{ m_mutex };

  if (this == &other) [[unlikely]] { return *this; }

  m_name      = other.m_name;
  m_position  = other.m_position;
  m_behaviors = other.m_behaviors;
  m_type      = other.m_type;
  m_valid     = false;

  create();

  return *this;
}

Object& Object::operator=(Object&& other) noexcept {
  const auto lock = std::unique_lock{ m_mutex };

  if (this == &other) [[unlikely]] { return *this; }

  m_name         = std::move(other.m_name);
  m_position     = other.m_position;
  m_behaviors    = std::move(other.m_behaviors);
  m_type         = other.m_type;
  m_updateThread = std::move(other.m_updateThread);
  m_valid        = other.m_valid;

  other.m_valid = false;

  return *this;
}

Object::~Object() {
  const auto lock = std::unique_lock{ m_mutex };

  if (m_valid) { destroy(); }
}

void Object::create() {
  const auto lock = std::unique_lock{ m_mutex };

  if (m_valid) [[unlikely]] { return; }

  onCreate();

  m_updateThread = std::jthread(&Object::update, std::ref(*this));

  m_valid = true;
}

void Object::destroy() {
  const auto lock = std::unique_lock{ m_mutex };

  if (!m_valid) [[unlikely]] { return; }

  m_valid = false;

  if (m_updateThread.joinable()) [[likely]] {
    m_updateThread.request_stop();
    m_updateThread.join();
  }

  m_behaviors.clear();

  onDestroy();
}

void Object::draw() const {
}

Rectangle Object::getRect() const {
  const auto lock = std::unique_lock{ m_mutex };

  return {m_position.x, m_position.y, 0.0F, 0.0F};
}

void Object::update(const std::stop_token& token, Object& object) {
  while (!token.stop_requested()) {
    const auto lock = std::unique_lock{ object.m_mutex };

    std::ranges::for_each(object.m_behaviors, [&object](const auto& behavior) {
      behavior(object);
    });

    object.onUpdate();
  }
}

void Object::onCreate() {
}

void Object::onDestroy() {
}

void Object::onUpdate() {
}

} // namespace so
