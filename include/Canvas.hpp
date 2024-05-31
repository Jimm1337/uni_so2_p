#ifndef UNI_SO2_P_CANVAS_HPP
#define UNI_SO2_P_CANVAS_HPP

#include "Grid.hpp"

namespace so {

class Canvas : public Grid {
public:
  Canvas(const Canvas& other) = delete;
  Canvas(Canvas&& other) noexcept;
  Canvas& operator=(const Canvas& other) = delete;
  Canvas& operator=(Canvas&& other) noexcept;
  ~Canvas() override = default;

  template< typename... BehaviorType >
  requires(std::is_invocable_v< BehaviorType, Canvas& > && ...)
  Canvas(std::string_view name,
         Vector2          position,
         Grid::uVec2      size,
         Color            background,
         BehaviorType&&... behaviors):
    Grid(name,
         position,
         size,
         1,
         Grid::CLASS::CANVAS,
         std::forward< BehaviorType >(behaviors)...),
    m_bgColor{ background } {
  }

  void draw() const override;

protected:
  Color m_bgColor;
};

} // namespace so

#endif // UNI_SO2_P_CANVAS_HPP
