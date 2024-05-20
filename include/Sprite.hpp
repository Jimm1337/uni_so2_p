#ifndef UNI_SO2_P_SPRITE_HPP
#define UNI_SO2_P_SPRITE_HPP

#include <chrono>
#include "Object.hpp"

namespace so {

class Sprite : public Object {
public:
  static constexpr auto DESTROYED_TIME = 1000;

  enum class CLASS : uint8_t {
    PLAYER,
    ENEMY_PASSIVE,
    ENEMY_SHOOTER,
    ENEMY_COMMANDER,
    ENEMY_MYSTERY,
    GENERIC [[maybe_unused]] = 255
  };

  Sprite(const Sprite& other) = delete;
  Sprite(Sprite&& other) noexcept;
  Sprite& operator=(const Sprite& other) = delete;
  Sprite& operator=(Sprite&& other) noexcept;
  ~Sprite() override = default;

  template< typename PositionType, typename... BehaviorType >
  requires std::is_base_of_v< Vector2, PositionType > &&
             (std::is_invocable_v< BehaviorType, Sprite& > && ...)
  Sprite(std::string_view name,
         std::string_view texturePath,
         std::string_view destroyedTexturePath,
         float            scale,
         CLASS            spriteClass,
         PositionType&&   position,
         BehaviorType&&... behaviors):
    Object(name,
           Object::TYPE::SPRITE,
           std::forward< PositionType >(position),
           std::forward< BehaviorType >(behaviors)...),
    m_texture{ LoadTexture(texturePath.data()) },
    m_destroyedTexture{ LoadTexture(destroyedTexturePath.data()) },
    m_spriteClass{ spriteClass },
    m_scale{ scale } {
  }

  [[nodiscard]] CLASS getSpriteClass() const noexcept {
    return m_spriteClass;
  }

  [[nodiscard]] bool isDestroyed() const noexcept {
    const auto lock = std::unique_lock{ m_mutex };

    return m_destroyed;
  }

  [[nodiscard]] bool isVisible() const noexcept {
    const auto lock = std::unique_lock{ m_mutex };

    return m_visible;
  }



  void markDestroyed();

  void                    draw() const override;
  [[nodiscard]] Rectangle getRect() const override;

private:
  void onUpdate() override;
  void onDestroy() override;

protected:
  Texture2D m_texture;
  Texture2D m_destroyedTexture;
  std::chrono::time_point< std::chrono::high_resolution_clock > m_destroyedTime;
  CLASS                                                         m_spriteClass;
  float                                                         m_scale;
  bool m_destroyed{ false };
  bool m_visible{ true };
};

} // namespace so

#endif // UNI_SO2_P_SPRITE_HPP
