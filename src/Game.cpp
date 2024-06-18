#include "Game.hpp"
#include <raylib.h>
#include <algorithm>
#include <iostream>
#include <random>
#include "fmt/core.h"

namespace so {

Game::Game():
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
  m_player{ initPlayer() },
  m_score{},
  m_lives{},
  m_level{},
  m_gameActive{ false },
  m_gameOver{ false },
  m_livesSprites{ Sprite{ "Life__1",
                          TEXTURE_PLAYER,
                          TEXTURE_DESTROYED,
                          0.5,
                          Vector2{ 30, WINDOW_SIZE.y - 40 },
                          true },
                  Sprite{ "Life__2",
                          TEXTURE_PLAYER,
                          TEXTURE_DESTROYED,
                          0.5,
                          Vector2{ 80, WINDOW_SIZE.y - 40 },
                          true },
                  Sprite{ "Life__3",
                          TEXTURE_PLAYER,
                          TEXTURE_DESTROYED,
                          0.5,
                          Vector2{ 130, WINDOW_SIZE.y - 40 },
                          true } } {
}

void Game::run() {
  for (auto id = 0; id < ENEMIES_PER_ROW; ++id) {
    const auto pos =
      Vector2{ static_cast< float >(2 * TEXTURE_ENEMY_WIDTH * (1 + id)),
               static_cast< float >(ENEMY_PASSIVE_Y) };

    m_enemiesPassive.emplace_back("Enemy__Passive_" + std::to_string(id),
                                  TEXTURE_ENEMY_PASSIVE,
                                  TEXTURE_DESTROYED,
                                  1,
                                  pos);
  }

  for (auto id = 0; id < ENEMIES_PER_ROW; ++id) {
    const auto pos = Vector2{
      static_cast< float >(2 * TEXTURE_ENEMY_WIDTH * (1 + id)),
      static_cast< float >(ENEMY_PASSIVE_Y + TEXTURE_ENEMY_HEIGHT * 2)
    };

    m_enemiesPassive.emplace_back(
      "Enemy__Passive_" + std::to_string(id + ENEMIES_PER_ROW),
      TEXTURE_ENEMY_PASSIVE,
      TEXTURE_DESTROYED,
      1,
      pos);
  }

  for (auto id = 0; id < ENEMIES_PER_ROW; ++id) {
    const auto pos =
      Vector2{ static_cast< float >(2 * TEXTURE_ENEMY_WIDTH * (1 + id)),
               static_cast< float >(ENEMY_SHOOTER_Y) };

    m_enemiesShooter.emplace_back("Enemy__Shooter_" + std::to_string(id),
                                  TEXTURE_ENEMY_SHOOTER,
                                  TEXTURE_DESTROYED,
                                  1,
                                  pos);
  }

  for (auto id = 0; id < ENEMIES_PER_ROW; ++id) {
    const auto pos =
      Vector2{ static_cast< float >(2 * TEXTURE_ENEMY_WIDTH * (1 + id)),
               static_cast< float >(ENEMY_COM_Y) };

    m_enemiesCommander.emplace_back("Enemy__Commander_" + std::to_string(id),
                                    TEXTURE_ENEMY_COM,
                                    TEXTURE_DESTROYED,
                                    1,
                                    pos);
  }

  hideAll();
  addBehaviorsToPlayer();
  addBehaviorsToEnemiesPassive();
  addBehaviorsToEnemiesShooter();
  addBehaviorsToEnemiesCommander();
  initCanvas();

  while (!WindowShouldClose()) {
    if (m_gameOver) [[unlikely]] {
      renderGameOverScreen();

    } else if (!m_gameActive) [[unlikely]] {
      renderStartScreen();
      if (IsKeyDown(KEY_ENTER)) [[unlikely]] { startGame(); }

    } else [[likely]] {
      renderGameScreen();
    }

    cleanup();
    std::this_thread::yield();
    std::this_thread::sleep_for(std::chrono::milliseconds{ 1 });
  }
}

Sprite Game::initPlayer() {
  return { "Player",
           TEXTURE_PLAYER,
           TEXTURE_DESTROYED,
           1,
           Vector2{ WINDOW_SIZE.x / 2, PLAYER_Y } };
}

void Game::renderStartScreen() {
  BeginDrawing();

  m_canvas.draw();

  Object::gpuMutex.lock();
  DrawText(
    "Press ENTER to start", 90, WINDOW_SIZE.y / 2, FONT_SIZE_CTA, FONT_COLOR);
  Object::gpuMutex.unlock();

  EndDrawing();
}

void Game::renderGameOverScreen() {
  BeginDrawing();

  ClearBackground(BLACK);

  m_canvas.draw();

  Object::gpuMutex.lock();
  DrawText("Game Over",
           WINDOW_SIZE.x / 3,
           WINDOW_SIZE.y / 2 - FONT_SIZE_CTA,
           FONT_SIZE_CTA,
           FONT_COLOR);
  Object::gpuMutex.unlock();

  EndDrawing();
}

void Game::renderGameScreen() {
  BeginDrawing();

  ClearBackground(BLACK);

  m_canvas.draw();

  EndDrawing();
}

void Game::addBehaviorsToPlayer() {
  m_player.addBehavior([](Object& object) {
    if (object.getPosition().x > 4.0 && IsKeyDown(KEY_A)) {
      object.setPosition(Vector2{ object.getPosition().x - .3F * GetFrameTime(),
                                  object.getPosition().y });
    }

    if (object.getPosition().x < WINDOW_SIZE.x - TEXTURE_PLAYER_WIDTH - 4.0 &&
        IsKeyDown(KEY_D)) {
      object.setPosition(Vector2{ object.getPosition().x + .3F * GetFrameTime(),
                                  object.getPosition().y });
    }
  });

  m_player.addBehavior([this](Object&) {
    static auto lastShot = std::chrono::high_resolution_clock::now();

    if (!m_gameActive) { return; }

    if (IsKeyPressed(KEY_SPACE) &&
        std::chrono::duration_cast< std::chrono::milliseconds >(
          std::chrono::high_resolution_clock::now() - lastShot)
            .count() > 500) {
      shootPlayer();
      lastShot = std::chrono::high_resolution_clock::now();
    }
  });
}

void Game::addBehaviorsToEnemiesPassive() {
}

void Game::addBehaviorsToEnemiesShooter() {
  std::ranges::for_each(m_enemiesShooter, [this](auto& enemy) {
    enemy.addBehavior([this](Object& object) {
      if (!m_gameActive) { return; }

      static auto lastShot = std::chrono::high_resolution_clock::now();

      if (std::chrono::duration_cast< std::chrono::milliseconds >(
            std::chrono::high_resolution_clock::now() - lastShot)
            .count() > 500) {
        shootEnemy(dynamic_cast< Sprite& >(object));
        lastShot = std::chrono::high_resolution_clock::now();
      }
    });
  });
}

void Game::addBehaviorsToEnemiesCommander() {
}

void Game::addBehaviorsToProjectilePlayer(Projectile& projectile) {
  projectile.addBehavior([this](Object& object) {
    if (object.getPosition().y < 0.) {
      m_canvas.removeObject(object);
      object.destroy();
    }
  });

  projectile.addBehavior([this](Object& object) {
    for (auto& enemy : m_enemiesPassive) {
      if (!enemy.isDestroyed() &&
          CheckCollisionRecs(object.getRect(), enemy.getRect())) {
        m_canvas.removeObject(object);
        object.destroy();
        enemy.markDestroyed();
        m_score += 10;
        m_scoreText.setText(fmt::format("Score: {}", m_score));
      }
    }

    for (auto& enemy : m_enemiesShooter) {
      if (!enemy.isDestroyed() &&
          CheckCollisionRecs(object.getRect(), enemy.getRect())) {
        m_canvas.removeObject(object);
        object.destroy();
        enemy.markDestroyed();
        m_score += 20;
        m_scoreText.setText(fmt::format("Score: {}", m_score));
      }
    }

    for (auto& enemy : m_enemiesCommander) {
      if (!enemy.isDestroyed() &&
          CheckCollisionRecs(object.getRect(), enemy.getRect())) {
        m_canvas.removeObject(object);
        object.destroy();
        enemy.markDestroyed();
        m_score += 30;
        m_scoreText.setText(fmt::format("Score: {}", m_score));
      }
    }
  });
}

void Game::addBehaviorsToProjectileEnemy(so::Projectile& projectile) {
  projectile.addBehavior([this](Object& object) {
    if (object.getPosition().y > PLAYER_Y + TEXTURE_PLAYER_HEIGHT) {
      m_canvas.removeObject(object);
      object.destroy();
    }
  });

  projectile.addBehavior([this](Object& object) {
    if (CheckCollisionRecs(object.getRect(), m_player.getRect())) {
      m_canvas.removeObject(object);
      object.destroy();
      m_livesSprites[m_lives - 1].markDestroyed();
      --m_lives;
      m_player.setPosition(Vector2{ WINDOW_SIZE.x / 2, PLAYER_Y });
      if (m_lives == 0) {
        m_gameOver   = true;
        m_gameActive = false;
        hideAll();
        m_scoreText.unHide();
        m_levelText.unHide();
      }
    }
  });
}

void Game::shootPlayer() {
  static auto id = 0;

  const auto      pos     = m_player.getPosition();
  const Rectangle newProj = {
    pos.x + (TEXTURE_PLAYER_WIDTH / 2 - 2), pos.y - 10, 4, 10
  };

  m_playerProjectiles.emplace_back(
    std::make_unique< Projectile >("PlayerProjectile__" + std::to_string(id++),
                                   "Player",
                                   newProj,
                                   .5f,
                                   Projectile::Direction::UP,
                                   WHITE));

  auto& proj = *m_playerProjectiles.back();
  addBehaviorsToProjectilePlayer(proj);

  m_canvas.putObject(proj);
}

void Game::shootEnemy(Sprite& enemy) {
  static auto id = 0;

  static std::random_device                   rd;
  static std::mt19937                         gen(rd());
  static std::uniform_int_distribution< int > dis(0, 3);

  if (dis(gen) != 0) [[unlikely]] { return; }

  const auto      pos     = enemy.getPosition();
  const Rectangle newProj = {
    pos.x + (TEXTURE_ENEMY_WIDTH / 2 - 2), pos.y + TEXTURE_ENEMY_HEIGHT, 4, 10
  };

  m_enemyProjectiles.emplace_back(
    std::make_unique< Projectile >("EnemyProjectile__" + std::to_string(id++),
                                   enemy.getName(),
                                   newProj,
                                   .3f,
                                   Projectile::Direction::DOWN,
                                   ORANGE));

  auto& proj = *m_enemyProjectiles.back();
  addBehaviorsToProjectileEnemy(proj);

  m_canvas.putObject(proj);
}

void Game::hideAll() {
  std::ranges::for_each(m_enemiesCommander, [](auto& enemy) { enemy.hide(); });

  std::ranges::for_each(m_enemiesShooter, [](auto& enemy) { enemy.hide(); });

  std::ranges::for_each(m_enemiesPassive, [](auto& enemy) { enemy.hide(); });

  std::ranges::for_each(m_playerProjectiles,
                        [](auto& projectile) { projectile->hide(); });

  std::ranges::for_each(m_enemyProjectiles,
                        [](auto& projectile) { projectile->hide(); });

  m_player.hide();

  m_scoreText.hide();

  m_levelText.hide();

  std::ranges::for_each(m_livesSprites, [](auto& sprite) { sprite.hide(); });
}

void Game::showAll() {
  std::ranges::for_each(m_enemiesCommander,
                        [](auto& enemy) { enemy.unHide(); });

  std::ranges::for_each(m_enemiesShooter, [](auto& enemy) { enemy.unHide(); });

  std::ranges::for_each(m_enemiesPassive, [](auto& enemy) { enemy.unHide(); });

  m_player.unHide();

  m_scoreText.unHide();

  m_levelText.unHide();

  std::ranges::for_each(m_livesSprites, [](auto& sprite) { sprite.unHide(); });
}

void Game::addAllToCanvas() {
  std::ranges::for_each(m_enemiesCommander,
                        [this](auto& enemy) { m_canvas.putObject(enemy); });

  std::ranges::for_each(m_enemiesShooter,
                        [this](auto& enemy) { m_canvas.putObject(enemy); });

  std::ranges::for_each(m_enemiesPassive,
                        [this](auto& enemy) { m_canvas.putObject(enemy); });

  m_canvas.putObject(m_player);

  m_canvas.putObject(m_scoreText);

  m_canvas.putObject(m_levelText);

  std::ranges::for_each(m_livesSprites,
                        [this](auto& sprite) { m_canvas.putObject(sprite); });
}

void Game::initCanvas() {
  m_canvas.putFrame(YELLOW);
  m_canvas.putLine(
    { 0, PLAYER_Y + 32 }, { WINDOW_SIZE.x, PLAYER_Y + 32 }, YELLOW);
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

  showAll();
}

void Game::cleanup() {
  std::erase_if(m_playerProjectiles,
                [](const auto& projectile) { return !projectile->isValid(); });
  std::erase_if(m_enemyProjectiles,
                [](const auto& projectile) { return !projectile->isValid(); });
}

} // namespace so