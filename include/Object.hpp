#ifndef UNI_SO2_P_OBJECT_HPP
#define UNI_SO2_P_OBJECT_HPP

#include <raylib.h>
#include <functional>
#include <mutex>
#include <string>
#include <thread>
#include <vector>

namespace so {

class Object {
public:
  enum class TYPE : uint8_t {
    SPRITE,
    GRID,
    PROJECTILE,
    TEXT,
    INPUT,
    GENERIC [[maybe_unused]] = 255
  };

  Object(const Object& other) = delete;
  Object(Object&& other) = delete;
  Object& operator=(const Object& other) = delete;
  Object& operator=(Object&& other) = delete;
  virtual ~Object();

  template< typename PositionType, typename... BehaviorType >
  requires std::is_same_v< Vector2, std::remove_cvref_t< PositionType > > &&
             (std::is_invocable_v< BehaviorType, Object& > && ...)
  Object(std::string_view name,
         TYPE             type,
         PositionType&&   position,
         BehaviorType&&... behaviors):
    m_name{ name },
    m_position{ std::forward< PositionType >(position) },
    m_behaviors{ std::forward< BehaviorType >(behaviors)... },
    m_type{ type } {
    create();
  }

  void setPosition(const Vector2& position) noexcept {
    const auto lock = std::unique_lock{ m_mutex };
    m_position      = position;
  }

  [[nodiscard]] Vector2 getPosition() const noexcept {
    const auto lock = std::unique_lock{ m_mutex };
    return m_position;
  }

  void moveBy(const Vector2& offset) noexcept {
    const auto lock = std::unique_lock{ m_mutex };

    m_position.x += offset.x;
    m_position.y += offset.y;
  }

  [[nodiscard]] std::string getName() const noexcept {
    return m_name;
  }

  [[nodiscard]] TYPE getType() const noexcept {
    return m_type;
  }

  [[nodiscard]] bool isValid() const noexcept {
    const auto lock = std::unique_lock{ m_mutex };
    return m_valid;
  }

  void hide() noexcept {
    const auto lock = std::unique_lock{ m_mutex };
    m_hidden        = true;
  }

  void unHide() noexcept {
    const auto lock = std::unique_lock{ m_mutex };
    m_hidden        = false;
  }

  [[nodiscard]] bool isHidden() const noexcept {
    const auto lock = std::unique_lock{ m_mutex };
    return m_hidden;
  }

  template< typename BehaviorType >
  requires std::is_invocable_v< BehaviorType, Object& > &&
           std::is_same_v< void, std::invoke_result_t< BehaviorType, Object& > >
  void addBehavior(BehaviorType&& behavior) {
    const auto lock = std::unique_lock{ m_mutex };
    m_behaviors.emplace_back(std::forward< BehaviorType >(behavior));
  }

  void create();
  void destroy();

  virtual void                    draw() const;
  [[nodiscard]] virtual Rectangle getRect() const;

private:
  static void update(Object& object);

public:
  static std::recursive_mutex gpuMutex;

protected:
  mutable std::recursive_mutex                  m_mutex;
  std::vector< std::function< void(Object&) > > m_behaviors;
  std::string                                   m_name;
  std::jthread                                  m_updateThread;
  std::atomic_bool                              m_stop{ false };
  Vector2                                       m_position;
  TYPE                                          m_type;
  bool                                          m_valid{ false };
  bool                                          m_hidden{ false };

  virtual void onCreate();
  virtual void onDestroy();
  virtual void onUpdate();
};

} // namespace so

#endif // UNI_SO2_P_OBJECT_HPP
