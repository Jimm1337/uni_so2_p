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
    PHYSICS,
    GENERIC [[maybe_unused]] = 255
  };

  Object(const Object& other);
  Object(Object&& other) noexcept;
  Object& operator=(const Object& other);
  Object& operator=(Object&& other) noexcept;
  virtual ~Object();

  template< typename PositionType, typename... BehaviorType >
  requires std::is_base_of_v< Vector2, PositionType > &&
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
  static void update(const std::stop_token& token, Object& object);

protected:
  mutable std::recursive_mutex                  m_mutex;
  std::vector< std::function< void(Object&) > > m_behaviors;
  std::string                                   m_name;
  std::jthread                                  m_updateThread;
  Vector2                                       m_position;
  TYPE                                          m_type;
  bool                                          m_valid{ false };

  virtual void onCreate();
  virtual void onDestroy();
  virtual void onUpdate();
};

} // namespace so

#endif // UNI_SO2_P_OBJECT_HPP
