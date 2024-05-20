#ifndef UNI_SO2_P_PROJECTILE_HPP
#define UNI_SO2_P_PROJECTILE_HPP

#include "Object.hpp"

namespace so {

class Projectile : public Object {
public:
  enum class Direction : uint8_t {
    UP,
    DOWN
  };

  Projectile(const Projectile& other) = delete;
  Projectile(Projectile&& other) noexcept;
  Projectile& operator=(const Projectile& other) = delete;
  Projectile& operator=(Projectile&& other) noexcept;
  ~Projectile() override = default;

  template< typename... BehaviorType >
  requires(std::is_invocable_v< BehaviorType, Projectile& > && ...)
  Projectile(std::string_view name,
             std::string_view sourceName,
             Rectangle        positionAndSize,
             float            speed,
             Direction        direction,
             Color            color,
             BehaviorType&&... behaviors):
    Object(name,
           Object::TYPE::PROJECTILE,
           Vector2{ positionAndSize.x, positionAndSize.y },
           std::forward< BehaviorType >(behaviors)...),
    m_width{ positionAndSize.width },
    m_height{ positionAndSize.height },
    m_speed{ speed },
    m_sourceName{ sourceName },
    m_direction{ direction },
    m_color{ color } {
  }

  [[nodiscard]] bool isCollided() const noexcept {
    const auto lock = std::unique_lock{ m_mutex };

    return m_collided;
  }

  void markCollided();

  void                    draw() const override;
  [[nodiscard]] Rectangle getRect() const override;

private:
  void onUpdate() override;

protected:
  float       m_width;
  float       m_height;
  float       m_speed; // pixels per second
  std::string m_sourceName;
  Direction   m_direction;
  Color       m_color;
  bool        m_collided{ false };
};

} // namespace so

#endif // UNI_SO2_P_PROJECTILE_HPP
