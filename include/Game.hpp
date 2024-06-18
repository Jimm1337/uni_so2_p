#ifndef UNI_SO2_P_GAME_HPP
#define UNI_SO2_P_GAME_HPP

#include <array>
#include <memory>
#include <unordered_map>
#include <vector>
#include "Canvas.hpp"
#include "Projectile.hpp"
#include "Sprite.hpp"
#include "Text.hpp"

namespace so {

using namespace std::string_view_literals;

class Game final {
public:
  static constexpr auto WINDOW_NAME = "PROJEKT SO2"sv;
  static constexpr auto WINDOW_SIZE = Grid::uVec2{ 800, 600 };
  static constexpr auto WINDOW_FPS  = 30;

  Game(const Game& other)            = delete;
  Game(Game&& other) noexcept        = delete;
  Game& operator=(const Game& other) = delete;
  Game& operator=(Game&& other)      = delete;
  ~Game()                            = default;

  Game();

  void run();

private:
  Canvas                                     m_canvas;
  Text                                       m_scoreText;
  Text                                       m_levelText;
  Sprite                                     m_player;
  std::array< Sprite, 3 >                    m_livesSprites;
  std::list< Sprite >                        m_enemiesPassive;
  std::list< Sprite >                        m_enemiesShooter;
  std::list< Sprite >                        m_enemiesCommander;
  std::list< std::unique_ptr< Projectile > > m_playerProjectiles;
  std::list< std::unique_ptr< Projectile > > m_enemyProjectiles;
  int_fast32_t                               m_score;
  int_fast8_t                                m_lives;
  int_fast8_t                                m_level;
  bool                                       m_gameActive;
  bool                                       m_gameOver;

  static Sprite initPlayer();

  void renderStartScreen();
  void renderGameOverScreen();
  void renderGameScreen();

  void addBehaviorsToPlayer();
  void addBehaviorsToEnemiesPassive();
  void addBehaviorsToEnemiesShooter();
  void addBehaviorsToEnemiesCommander();
  void addBehaviorsToProjectilePlayer(Projectile& projectile);
  void addBehaviorsToProjectileEnemy(Projectile& projectile);

  void shootPlayer();
  void shootEnemy(Sprite& enemy);

  void hideAll();
  void showAll();

  void addAllToCanvas();
  void initCanvas();

  void startGame();

  void cleanup();

  static constexpr auto FONT_SIZE             = 24;
  static constexpr auto FONT_SIZE_CTA         = 48;
  static constexpr auto FONT_PATH             = RES_PATH "font/retro.ttf"sv;
  static constexpr auto FONT_COLOR            = WHITE;
  static constexpr auto TEXTURE_PLAYER        = RES_PATH "img/player.png"sv;
  static constexpr auto TEXTURE_ENEMY_PASSIVE = RES_PATH "img/passive.png"sv;
  static constexpr auto TEXTURE_ENEMY_SHOOTER = RES_PATH "img/shooter.png"sv;
  static constexpr auto TEXTURE_ENEMY_COM     = RES_PATH "img/commander.png"sv;
  static constexpr auto TEXTURE_ENEMY_WIDTH   = 40;
  static constexpr auto TEXTURE_ENEMY_HEIGHT  = 32;
  static constexpr auto TEXTURE_PLAYER_WIDTH  = 60;
  static constexpr auto TEXTURE_PLAYER_HEIGHT = 30;
  static constexpr auto TEXTURE_DESTROYED  = RES_PATH "img/placeholder.png"sv;
  static constexpr auto OBSTACLE_COLOR     = YELLOW;
  static constexpr auto OBSTACLE_Y         = WINDOW_SIZE.y * 3 / 4;
  static constexpr auto OBSTACLE_SIZE      = Grid::uVec2{ 80, 100 };
  static constexpr auto OBSTACLE_CELL_SIZE = Grid::uVec2{ 2, 2 };
  static constexpr auto SPEED_PER_LEVEL    = 100; // PIXELS/SECOND
  static constexpr auto PLAYER_Y           = WINDOW_SIZE.y - 100;
  static constexpr auto ENEMIES_MAX     = WINDOW_SIZE.x / TEXTURE_ENEMY_WIDTH;
  static constexpr auto ENEMIES_PER_ROW = ENEMIES_MAX / 2 - 2;
  static constexpr auto ENEMY_PASSIVE_Y = WINDOW_SIZE.y - 400;
  static constexpr auto ENEMY_SHOOTER_Y =
    ENEMY_PASSIVE_Y - 2 * TEXTURE_ENEMY_HEIGHT;
  static constexpr auto ENEMY_COM_Y =
    ENEMY_SHOOTER_Y - 2 * TEXTURE_ENEMY_HEIGHT;
};

} // namespace so

#endif // UNI_SO2_P_GAME_HPP
