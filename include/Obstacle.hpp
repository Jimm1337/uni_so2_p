#ifndef UNI_SO2_P_OBSTACLE_HPP
#define UNI_SO2_P_OBSTACLE_HPP

#include <random>
#include "Grid.hpp"

namespace so {

class Obstacle : public Grid {
protected:
  [[nodiscard]] size_t positionToCellIdx(Vector2 position) const noexcept;
  [[nodiscard]] Vector2 cellIdxToPosition(size_t idx) const noexcept;

public:
  Obstacle(const Obstacle& other) = delete;
  Obstacle(Obstacle&& other) noexcept;
  Obstacle& operator=(const Obstacle& other) = delete;
  Obstacle& operator=(Obstacle&& other) noexcept;
  ~Obstacle() override = default;

  template< typename... BehaviorType >
  requires(std::is_invocable_v< BehaviorType, Obstacle& > && ...)
  Obstacle(std::string_view name,
           Vector2          position,
           uVec2            size,
           uVec2            cellSize,
           Color            cellColor,
           BehaviorType&&... behaviors):
    Grid(name,
         position,
         size,
         cellSize,
         CLASS::OBSTACLE,
         std::forward< BehaviorType >(behaviors)...),
    m_destructionGen{ std::random_device{}() },
    m_cellColor{ cellColor } {
  }

  void draw() const override;

  [[nodiscard]] bool hasCellAt(Vector2 position) const noexcept;

  void impactAt(Vector2 position, bool upwards) noexcept;

  [[nodiscard]] bool isDestroyed() const noexcept;

protected:
  std::mt19937 m_destructionGen;
  Color        m_cellColor;
};

} // namespace so

#endif // UNI_SO2_P_OBSTACLE_HPP
