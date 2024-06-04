#ifndef UNI_SO2_P_CANVAS_HPP
#define UNI_SO2_P_CANVAS_HPP

#include "Grid.hpp"
#include <tuple>
#include <vector>

namespace so {

class Canvas : public Grid {
  static constexpr auto LINE_THICKNESS = 4;

public:
  Canvas(const Canvas& other) = delete;
  Canvas(Canvas&& other) noexcept;
  Canvas& operator=(const Canvas& other) = delete;
  Canvas& operator=(Canvas&& other) noexcept;
  ~Canvas() override = default;

  template< typename... BehaviorType >
  requires(std::is_invocable_v< BehaviorType, Object& > && ...)
  Canvas(std::string_view name,
         Vector2          position,
         Grid::uVec2      size,
         Color            background,
         BehaviorType&&... behaviors):
    Grid(name,
         position,
         size,
         uVec2{1, 1},
         Grid::CLASS::CANVAS,
         std::forward< BehaviorType >(behaviors)...),
    m_bgColor{ background }, m_frameColor{ BLANK } {
  }

  void putLine(Vector2 start, Vector2 end, Color color) noexcept;

  void putFrame(Color color) noexcept;

  void draw() const override;

protected:
  Color m_bgColor;
  Color m_frameColor;
  std::vector<std::tuple<Vector2, Vector2, Color>> m_lines;
};

} // namespace so

#endif // UNI_SO2_P_CANVAS_HPP
