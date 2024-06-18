#ifndef UNI_SO2_P_SPRITE_HPP
#define UNI_SO2_P_SPRITE_HPP

#include <raylib.h>
#include <chrono>
#include "Object.hpp"

namespace so {

class Sprite : public Object {
public:
  static constexpr auto DESTROYED_TIME = 250;

  enum class CLASS : uint8_t {
    GENERIC [[maybe_unused]] = 255
  };

  Sprite(const Sprite& other)            = delete;
  Sprite(Sprite&& other)                 = delete;
  Sprite& operator=(const Sprite& other) = delete;
  Sprite& operator=(Sprite&& other)      = delete;
  ~Sprite() override                     = default;

  template< typename PositionType, typename... BehaviorType >
  requires std::is_same_v< Vector2, std::remove_cvref_t< PositionType > > &&
             (std::is_invocable_v< BehaviorType, Sprite& > && ...)
  Sprite(std::string_view name,
         std::string_view texturePath,
         std::string_view destroyedTexturePath,
         float            scale,
         PositionType&&   position,
         bool             noUpdate    = false,
         BehaviorType&&... behaviors):
    Object(name,
           Object::TYPE::SPRITE,
           std::forward< PositionType >(position),
           std::forward< BehaviorType >(behaviors)...),
    m_texture{ [&] {
      const auto lock = std::unique_lock{ gpuMutex };
      const auto tex  = LoadTexture(texturePath.data());
      return tex;
    }() },
    m_destroyedTexture{ [&] {
      const auto lock = std::unique_lock{ gpuMutex };
      const auto tex  = LoadTexture(destroyedTexturePath.data());
      return tex;
    }() },
    m_scale{ scale },
    m_noUpdate{ noUpdate } {
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
  float                                                         m_scale;
  bool m_destroyed{ false };
  bool m_visible{ true };
  bool m_noUpdate{ false };
};

} // namespace so

#endif // UNI_SO2_P_SPRITE_HPP
