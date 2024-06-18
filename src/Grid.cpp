#include "Grid.hpp"
#include <algorithm>
#include <ranges>

namespace so {

std::vector< size_t > Grid::getRectIndices(Rectangle   rect,
                                           Grid::uVec2 cellSize,
                                           size_t      gridWidth) noexcept {
  const auto xStart = static_cast< size_t >(rect.x / cellSize.x);
  const auto yStart = static_cast< size_t >(rect.y / cellSize.y);
  const auto xEnd   = static_cast< size_t >((rect.x + rect.width) / cellSize.x)
    //          +
    //          static_cast< size_t >(rect.x + rect.width) % cellSize.x ==
    //        0 ?
    //      0 :
    //      1
    ;
  const auto yEnd = static_cast< size_t >((rect.y + rect.height) / cellSize.y)
    //          +
    //          static_cast< size_t >(rect.y + rect.height) % cellSize.y ==
    //        0 ?
    //      0 :
    //      1
    ;

  std::vector< size_t > indices{};
  indices.resize((xEnd - xStart) * (yEnd - yStart));
  for (int i = 0; i < yEnd - yStart; ++i) {
    for (int j = 0; j < xEnd - xStart; ++j) {
      indices[i * (xEnd - xStart) + j] =
        getCellIndex({ xStart + j, yStart + i }, gridWidth);
    }
  }

  return indices;
}

void Grid::putObject(Object& object) {
  const auto lock = std::unique_lock{ m_mutex };

  m_objects.emplace_back(&object);
}

void Grid::removeObject(Object& object) {
  const auto lock = std::unique_lock{ m_mutex };

  m_objects.remove(&object);
}

void Grid::putCell(uVec2 position) {
  const auto lock = std::unique_lock{ m_mutex };

  const auto idx = getCellIndex(position, m_gridSize.x);

  m_cells[idx].occupied = true;
}

void Grid::removeCell(uVec2 position) {
  const auto lock = std::unique_lock{ m_mutex };

  const auto idx = getCellIndex(position, m_gridSize.x);

  m_cells[idx].occupied = false;
}

void Grid::draw() const {
  const auto lock = std::unique_lock{ m_mutex };

  if (m_hidden) [[unlikely]] { return; }

  // DEBUG DRAWING
  std::ranges::for_each(m_cells, [&](const auto& cell) {
    if (cell.occupied) {
      gpuMutex.lock();
      DrawRectangle(cell.x, cell.y, m_cellSize.x, m_cellSize.y, RED);
      gpuMutex.unlock();
    }
  });
}

Rectangle Grid::getRect() const {
  const auto lock = std::unique_lock{ m_mutex };

  return { m_position.x,
           m_position.y,
           static_cast< float >(m_gridSize.x * m_cellSize.x),
           static_cast< float >(m_gridSize.y * m_cellSize.y) };
}

} // namespace so
