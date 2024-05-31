#include "Grid.hpp"
#include <algorithm>
#include <ranges>

namespace so {

std::vector< size_t > Grid::getRectIndices(Rectangle   rect,
                                           Grid::uVec2 cellSize,
                                           size_t      gridWidth) noexcept  {
  const auto xStart = static_cast< size_t >(rect.x / cellSize.x);
  const auto yStart = static_cast< size_t >(rect.y / cellSize.y);
  const auto xEnd =
    static_cast< size_t >((rect.x + rect.width) / cellSize.x) +
          static_cast< size_t >(rect.x + rect.width) % cellSize.x ==
        0 ?
      0 :
      1;
  const auto yEnd =
    static_cast< size_t >((rect.y + rect.height) / cellSize.y) +
          static_cast< size_t >(rect.y + rect.height) % cellSize.y ==
        0 ?
      0 :
      1;

  std::vector< size_t > indices{ (xEnd - xStart) * (yEnd - yStart) };
  for (int i = 0; i < yEnd - yStart; ++i) {
    for (int j = 0; j < xEnd - xStart; ++j) {
      indices[i * (xEnd - xStart) + j] =
        getCellIndex({ xStart + j, yStart + i }, gridWidth);
    }
  }

  return indices;
}

Grid::Grid(Grid&& other) noexcept:
  Object(std::move(other)),
  m_cellSize{ other.m_cellSize },
  m_gridSize(other.m_gridSize),
  m_locations(std::move(other.m_locations)),
  m_cells{ std::move(other.m_cells) }, m_gridClass{ other.m_gridClass } {
  other.m_cellSize = { 0, 0 };
  other.m_cells.clear();
  other.m_gridSize = { 0, 0 };
  other.m_locations.clear();
}

Grid& Grid::operator=(Grid&& other) noexcept {
  const auto lock = std::unique_lock{ m_mutex };

  if (this == &other) [[unlikely]] { return *this; }

  Object::operator=(std::move(other));

  m_cellSize  = other.m_cellSize;
  m_gridSize  = other.m_gridSize;
  m_locations = std::move(other.m_locations);
  m_cells     = std::move(other.m_cells);
        m_gridClass = other.m_gridClass;

  other.m_cellSize = { 0, 0 };
  other.m_cells.clear();
  other.m_gridSize = { 0, 0 };
  other.m_locations.clear();

  return *this;
}

void Grid::putObject(Object& object) {
  const auto lock = std::unique_lock{ m_mutex };

  const auto indices =
    getRectIndices(object.getRect(), m_cellSize, m_gridSize.x);

  if (indices.empty()) { return; }

  m_locations[&object] = indices[0];

  std::ranges::for_each(
    indices, [&](const auto index) { m_cells[index].occupied = true; });
}

void Grid::removeObject(Object& object) {
  const auto lock = std::unique_lock{ m_mutex };

  const auto indices =
    getRectIndices(object.getRect(), m_cellSize, m_gridSize.x);

  if (indices.empty()) { return; }

  m_locations.erase(&object);

  std::ranges::for_each(
    indices, [&](const auto index) { m_cells[index].occupied = false; });
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

  // DEBUG DRAWING
  std::ranges::for_each(m_cells, [&](const auto& cell) {
    if (cell.occupied) {
      DrawRectangle(cell.x, cell.y, m_cellSize.x, m_cellSize.y, RED);
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
