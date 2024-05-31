#include "Obstacle.hpp"
#include <algorithm>
#include <cmath>
#include <ranges>

namespace so {

size_t Obstacle::positionToCellIdx(Vector2 position) const noexcept {
  const auto lock = std::unique_lock{ m_mutex };

  const uVec2 idx = { static_cast< size_t >(
                        std::floor((position.x - m_position.x) / m_cellSize.x)),
                      static_cast< size_t >(std::floor(
                        (position.y - m_position.y) / m_cellSize.y)) };

  return getCellIndex(idx, m_gridSize.x);
}

Vector2 Obstacle::cellIdxToPosition(size_t idx) const noexcept {
  const auto lock = std::unique_lock{ m_mutex };

  const uVec2 cellIdx = { idx % m_gridSize.x, idx / m_gridSize.x };

  return { m_position.x + cellIdx.x * m_cellSize.x,
           m_position.y + cellIdx.y * m_cellSize.y };
}

Obstacle::Obstacle(Obstacle&& other) noexcept:
  Grid(std::move(other)),
  m_destructionGen(other.m_destructionGen),
  m_cellColor(other.m_cellColor) {
}

Obstacle& Obstacle::operator=(Obstacle&& other) noexcept {
  const auto lock = std::unique_lock{ m_mutex };

  if (this == &other) [[unlikely]] { return *this; }

  Grid::operator=(std::move(other));

  m_destructionGen = other.m_destructionGen;
  m_cellColor      = other.m_cellColor;

  return *this;
}

void Obstacle::draw() const {
  const auto lock = std::unique_lock{ m_mutex };

  std::ranges::for_each(m_cells, [this](const auto& cell) {
    const auto [x, y, set]{ cell };

    if (!set) { return; }

    const auto position = cellIdxToPosition(y * m_gridSize.x + x);

    DrawRectangle(static_cast< int >(position.x),
                  static_cast< int >(position.y),
                  static_cast< int >(m_cellSize.x),
                  static_cast< int >(m_cellSize.y),
                  m_cellColor);
  });
}

bool Obstacle::hasCellAt(Vector2 position) const noexcept {
  const auto lock = std::unique_lock{ m_mutex };

  const auto idx = positionToCellIdx(position);

  if (idx >= m_cells.size()) { return false; }

  return m_cells[idx].occupied;
}

void Obstacle::impactAt(Vector2 position, bool upwards) noexcept {
  const auto lock = std::unique_lock{ m_mutex };

  auto idx = positionToCellIdx(position);

  if (idx >= m_cells.size()) { return; }

  m_cells[idx].occupied = false;

  std::uniform_int_distribution< size_t > destructionSizeDist{ 2, 4 };
  const auto destructionSize = destructionSizeDist(m_destructionGen);

  if (upwards) {
    // 1 2 3
    // 0 N 4
    std::uniform_int_distribution< uint_fast16_t > destructionDist{ 0, 4 };

    for (size_t i = 0; i < destructionSize; ++i) {
      const auto destructionCell = destructionDist(m_destructionGen);

      uVec2 cell = { idx % m_gridSize.x, idx / m_gridSize.x };

      switch (destructionCell) {
      case 0:
        if (cell.y > 0) { --cell.y; }
        break;
      case 1:
        if (cell.x > 0) { --cell.x; }
        if (cell.y > 0) { --cell.y; }
        break;
      case 2:
        if (cell.y > 0) { --cell.y; }
        break;
      case 3:
        if (cell.x < m_gridSize.x - 1) { ++cell.x; }
        if (cell.y > 0) { --cell.y; }
        break;
      case 4:
        if (cell.x < m_gridSize.x - 1) { ++cell.x; }
        break;
      [[unlikely]] default:
        break;
      }

      m_cells[getCellIndex(cell, m_gridSize.x)].occupied = false;
    }
  } else {
    // 0 N 4
    // 1 2 3
    std::uniform_int_distribution< uint_fast16_t > destructionDist{ 0, 4 };

    for (size_t i = 0; i < destructionSize; ++i) {
      const auto destructionCell = destructionDist(m_destructionGen);

      uVec2 cell = { idx % m_gridSize.x, idx / m_gridSize.x };

      switch (destructionCell) {
      case 0:
        if (cell.x > 0) { --cell.x; }
        break;
      case 1:
        if (cell.x > 0) { --cell.x; }
        if (cell.y < m_gridSize.y - 1) { ++cell.y; }
        break;
      case 2:
        if (cell.y < m_gridSize.y - 1) { ++cell.y; }
        break;
      case 3:
        if (cell.x < m_gridSize.x - 1) { ++cell.x; }
        if (cell.y < m_gridSize.y - 1) { ++cell.y; }
        break;
      case 4:
        if (cell.x < m_gridSize.x - 1) { ++cell.x; }
        break;
      [[unlikely]] default:
        break;
      }

      m_cells[getCellIndex(cell, m_gridSize.x)].occupied = false;
    }
  }
}

bool Obstacle::isDestroyed() const noexcept {
  const auto lock = std::unique_lock{ m_mutex };

  return std::ranges::all_of(m_cells, [](const auto& cell) {
    return !cell.occupied;
  });
}

} // namespace so
