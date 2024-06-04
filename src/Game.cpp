#include "Game.hpp"
#include <raylib.h>
#include <algorithm>

namespace so {

Game::Game():
  m_input{ "Keyboard" },
  m_canvas{ "Screen", { 0, 0 }, WINDOW_SIZE, GRAY },
  m_scoreText{ "Text__Score", "Score: 0", FONT_PATH, Vector2{ 10, 10 },
               FONT_SIZE,     FONT_COLOR, 1 },
  m_levelText{ "Text__Level",
               "Level: 1",
               FONT_PATH,
               Vector2{ WINDOW_SIZE.x - 200, WINDOW_SIZE.y - 40 },
               FONT_SIZE,
               FONT_COLOR,
               1 },
  m_obstacleLL{ "Obstacle__LL",
                Vector2{ WINDOW_SIZE.x / 4 - OBSTACLE_SIZE.x, OBSTACLE_Y },
                OBSTACLE_SIZE,
                OBSTACLE_CELL_SIZE,
                OBSTACLE_COLOR },
  m_obstacleLR{ "Obstacle__LR",
                Vector2{ WINDOW_SIZE.x / 4 + 2 * OBSTACLE_SIZE.x, OBSTACLE_Y },
                OBSTACLE_SIZE,
                OBSTACLE_CELL_SIZE,
                OBSTACLE_COLOR },
  m_obstacleRL{ "Obstacle__RL",
                Vector2{ WINDOW_SIZE.x * 3 / 4 - 2 * OBSTACLE_SIZE.x,
                         OBSTACLE_Y },
                OBSTACLE_SIZE,
                OBSTACLE_CELL_SIZE,
                OBSTACLE_COLOR },
  m_obstacleRR{ "Obstacle__RR",
                Vector2{ WINDOW_SIZE.x * 3 / 4 - OBSTACLE_SIZE.x, OBSTACLE_Y },
                OBSTACLE_SIZE,
                OBSTACLE_CELL_SIZE,
                OBSTACLE_COLOR },
  m_player{ initPlayer() },
  m_score{},
  m_lives{},
  m_level{},
  m_gameActive{ false },
  m_gameOver{ false } {
}

void Game::run() {
  hideAll();
  addBehaviorsToPlayer();
  addBehaviorsToEnemiesPassive();
  addBehaviorsToEnemiesShooter();
  addBehaviorsToEnemiesCommander();
  initCanvas();

  while (!WindowShouldClose()) {
    if (m_gameOver) [[unlikely]] {
      renderGameOverScreen();
      if (m_input.isKeyDown(KEY_ENTER)) [[unlikely]] { startGame(); }

    } else if (!m_gameActive) [[unlikely]] {
      renderStartScreen();
      if (m_input.isKeyDown(KEY_ENTER)) [[unlikely]] { startGame(); }

    } else [[likely]] {
      renderGameScreen();
    }
  }
}

Sprite Game::initPlayer() {
  return { "Player",
           TEXTURE_PLAYER,
           TEXTURE_DESTROYED,
           1,
           Vector2{ WINDOW_SIZE.x / 2, PLAYER_Y } };
}

Sprite Game::initEnemyPassive() {
  static auto id = 0ULL;

  const auto pos =
    Vector2{ static_cast< float >((1 + (2 * (id % ENEMIES_PER_ROW))) *
                                  TEXTURE_ENEMY_WIDTH),
             static_cast< float >(ENEMY_PASSIVE_Y * (1 + id % 2)) };

  ++id;

  return { "Enemy__Passive_" + std::to_string(id),
           TEXTURE_ENEMY_PASSIVE,
           TEXTURE_DESTROYED,
           1,
           pos };
}

Sprite Game::initEnemyShooter() {
  static auto id = 0;

  const auto pos =
    Vector2{ static_cast< float >((1 + (2 * (id % ENEMIES_PER_ROW))) *
                                  TEXTURE_ENEMY_WIDTH),
             static_cast< float >(ENEMY_SHOOTER_Y * (1 + id % 2)) };

  ++id;

  return { "Enemy__Shooter_" + std::to_string(id),
           TEXTURE_ENEMY_SHOOTER,
           TEXTURE_DESTROYED,
           1,
           pos };
}

Sprite Game::initEnemyCommander() {
  static auto id = 0;

  const auto pos =
    Vector2{ static_cast< float >((1 + (2 * (id % ENEMIES_PER_ROW))) *
                                  TEXTURE_ENEMY_WIDTH),
             static_cast< float >(ENEMY_COM_Y) };

  return { "Enemy__Commander_" + std::to_string(id),
           TEXTURE_ENEMY_COM,
           TEXTURE_DESTROYED,
           1,
           pos };
}

void Game::renderStartScreen() {
  BeginDrawing();

  m_canvas.draw();

  Object::gpuMutex.lock();
  DrawText(
    "Press ENTER to start", 100, WINDOW_SIZE.y / 2, FONT_SIZE_CTA, FONT_COLOR);
  Object::gpuMutex.unlock();

  EndDrawing();
}

void Game::renderGameOverScreen() {
  BeginDrawing();

  m_canvas.draw();

  Object::gpuMutex.lock();
  DrawText("Game Over",
           WINDOW_SIZE.x / 3 + 60,
           WINDOW_SIZE.y / 2,
           FONT_SIZE_CTA,
           FONT_COLOR);
  DrawText("Press ENTER to restart",
           WINDOW_SIZE.x / 3,
           WINDOW_SIZE.y / 2 + FONT_SIZE_CTA,
           FONT_SIZE,
           FONT_COLOR);
  Object::gpuMutex.unlock();

  EndDrawing();
}

void Game::renderGameScreen() {
  BeginDrawing();

  m_canvas.draw();

  EndDrawing();
}

void Game::addBehaviorsToPlayer() {
  m_player.addBehavior([this](Object& object) {
    auto& player = dynamic_cast< Sprite& >(object);

    if (m_input.isKeyDown(KEY_LEFT)) {
      m_player.setPosition(
        Vector2{ m_player.getPosition().x - 50 * GetFrameTime(),
                 m_player.getPosition().y });
    }

    if (m_input.isKeyDown(KEY_RIGHT)) {
      m_player.setPosition(
        Vector2{ m_player.getPosition().x + 50 * GetFrameTime(),
                 m_player.getPosition().y });
    }
  });
}

void Game::addBehaviorsToEnemiesPassive() {
}

void Game::addBehaviorsToEnemiesShooter() {
}

void Game::addBehaviorsToEnemiesCommander() {
}

void Game::addBehaviorsToProjectile(Projectile& projectile) {
}

void Game::hideAll() {
  std::ranges::for_each(m_enemiesCommander,
                        [](auto& pair) { pair.second.hide(); });

  std::ranges::for_each(m_enemiesShooter,
                        [](auto& pair) { pair.second.hide(); });

  std::ranges::for_each(m_enemiesPassive,
                        [](auto& pair) { pair.second.hide(); });

  m_player.hide();

  m_obstacleLL.hide();
  m_obstacleLR.hide();
  m_obstacleRL.hide();
  m_obstacleRR.hide();

  m_scoreText.hide();

  m_levelText.hide();
}

void Game::showAll() {
  std::ranges::for_each(m_enemiesCommander,
                        [](auto& pair) { pair.second.unHide(); });

  std::ranges::for_each(m_enemiesShooter,
                        [](auto& pair) { pair.second.unHide(); });

  std::ranges::for_each(m_enemiesPassive,
                        [](auto& pair) { pair.second.unHide(); });

  m_player.unHide();

  m_obstacleLL.unHide();
  m_obstacleLR.unHide();
  m_obstacleRL.unHide();
  m_obstacleRR.unHide();

  m_scoreText.unHide();

  m_levelText.unHide();
}

void Game::addAllToCanvas() {
  std::ranges::for_each(m_enemiesCommander, [this](auto& pair) {
    m_canvas.putObject(pair.second);
  });

  std::ranges::for_each(
    m_enemiesShooter, [this](auto& pair) { m_canvas.putObject(pair.second); });

  std::ranges::for_each(
    m_enemiesPassive, [this](auto& pair) { m_canvas.putObject(pair.second); });

  m_canvas.putObject(m_player);

  //  m_canvas.putObject(m_obstacleLL);
  //  m_canvas.putObject(m_obstacleLR);
  //  m_canvas.putObject(m_obstacleRL);
  //  m_canvas.putObject(m_obstacleRR);

  m_canvas.putObject(m_scoreText);

  m_canvas.putObject(m_levelText);
}

void Game::initCanvas() {
  m_canvas.putFrame(YELLOW);
  m_canvas.putLine(
    { 0, PLAYER_Y + 1 }, { WINDOW_SIZE.x, PLAYER_Y + 1 }, YELLOW);
  m_canvas.putLine(
    { 0, OBSTACLE_Y + 1 }, { WINDOW_SIZE.x, OBSTACLE_Y + 1 }, DARKGRAY);
  addAllToCanvas();
}

void Game::startGame() {
  m_gameActive = true;
  m_gameOver   = false;
  m_score      = 0;
  m_lives      = 3;
  m_level      = 1;

  m_enemiesPassive = [] {
    std::unordered_map< std::string, Sprite > map;

    static auto id = 0;

    for (auto i = 0; i < ENEMIES_PER_ROW * 2; ++i) {
      map.emplace("Enemy__Passive_" + std::to_string(id++), initEnemyPassive());
    }

    return map;
  }();

  m_enemiesShooter = [] {
    std::unordered_map< std::string, Sprite > map;

    static auto id = 0;

    for (auto i = 0; i < ENEMIES_PER_ROW * 2; ++i) {
      map.emplace("Enemy__Shooter_" + std::to_string(id++), initEnemyShooter());
    }

    return map;
  }();

  m_enemiesCommander = [] {
    std::unordered_map< std::string, Sprite > map;

    static auto id = 0;

    for (auto i = 0; i < ENEMIES_PER_ROW; ++i) {
      map.emplace("Enemy__Commander_" + std::to_string(id++),
                  initEnemyCommander());
    }

    return map;
  }();

  m_player = initPlayer();

  m_obstacleLL = { "Obstacle__LL",
                   Vector2{ WINDOW_SIZE.x / 4 - OBSTACLE_SIZE.x, OBSTACLE_Y },
                   OBSTACLE_SIZE,
                   OBSTACLE_CELL_SIZE,
                   OBSTACLE_COLOR };

  m_obstacleLR = { "Obstacle__LR",
                   Vector2{ WINDOW_SIZE.x / 4 + 2 * OBSTACLE_SIZE.x,
                            OBSTACLE_Y },
                   OBSTACLE_SIZE,
                   OBSTACLE_CELL_SIZE,
                   OBSTACLE_COLOR };

  m_obstacleRL = { "Obstacle__RL",
                   Vector2{ WINDOW_SIZE.x * 3 / 4 - 2 * OBSTACLE_SIZE.x,
                            OBSTACLE_Y },
                   OBSTACLE_SIZE,
                   OBSTACLE_CELL_SIZE,
                   OBSTACLE_COLOR };

  m_obstacleRR = { "Obstacle__RR",
                   Vector2{ WINDOW_SIZE.x * 3 / 4 - OBSTACLE_SIZE.x,
                            OBSTACLE_Y },
                   OBSTACLE_SIZE,
                   OBSTACLE_CELL_SIZE,
                   OBSTACLE_COLOR };

  m_scoreText = { "Text__Score", "Score: 0", FONT_PATH, Vector2{ 10, 10 },
                  FONT_SIZE,     FONT_COLOR, 1 };

  m_levelText = { "Text__Level",
                  "Level: 1",
                  FONT_PATH,
                  Vector2{ WINDOW_SIZE.x - 200, WINDOW_SIZE.y - 40 },
                  FONT_SIZE,
                  FONT_COLOR,
                  1 };

  m_projectiles.clear();

  showAll();
}

} // namespace so