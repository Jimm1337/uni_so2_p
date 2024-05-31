#ifndef UNI_SO2_P_GRID_HPP
#define UNI_SO2_P_GRID_HPP

#include "Object.hpp"

namespace so {

class Grid : public Object {
public:
  struct Cell {
    size_t x;
    size_t y;
    bool   occupied;
  };

  struct uVec2 {
    size_t x;
    size_t y;
  };

  enum class CLASS : uint8_t {
    CANVAS,
    OBSTACLE,
    GENERIC [[maybe_unused]] = 255
  };

protected:
  [[nodiscard]] static constexpr size_t getCellIndex(
    uVec2 idx, size_t gridWidth) noexcept {
    return idx.x + idx.y * gridWidth;
  }

  [[nodiscard]] static std::vector< size_t > getRectIndices(
    Rectangle rect, uVec2 cellSize, size_t gridWidth) noexcept;

public:
  Grid(const Grid& other) = delete;
  Grid(Grid&& other) noexcept;
  Grid& operator=(const Grid& other) = delete;
  Grid& operator=(Grid&& other) noexcept;
  ~Grid() override = default;

  template< typename... BehaviorType >
  requires(std::is_invocable_v< BehaviorType, Grid& > && ...)
  Grid(std::string_view name,
       Vector2          position,
       uVec2            gridSize,
       uVec2            cellSize,
       CLASS            gridClass = CLASS::GENERIC,
       BehaviorType&&... behaviors):
    Object(name,
           Object::TYPE::GRID,
           position,
           std::forward< BehaviorType >(behaviors)...),
    m_cells{ [gridSize, cellSize] {
      std::vector< Cell > cells{ gridSize.x * gridSize.y };
      for (size_t y = 0; y < gridSize.y; ++y) {
        for (size_t x = 0; x < gridSize.x; ++x) {
          cells[getCellIndex({ x, y }, gridSize.x)] = {
            x * cellSize.x,
            y * cellSize.y,
            false,
          };
        }
      }
    }() },
    m_cellSize{ cellSize },
    m_gridSize{ gridSize },
    m_gridClass{ gridClass } {
  }

  void putObject(Object& object);
  void removeObject(Object& object);

  void putCell(uVec2 position);
  void removeCell(uVec2 position);

  void                    draw() const override;
  [[nodiscard]] Rectangle getRect() const override;

protected:
  std::vector< Cell >                   m_cells;
  std::unordered_map< Object*, size_t > m_locations;
  uVec2                                 m_cellSize;
  uVec2                                 m_gridSize;
  CLASS                                 m_gridClass;
};

} // namespace so

#endif // UNI_SO2_P_GRID_HPP
