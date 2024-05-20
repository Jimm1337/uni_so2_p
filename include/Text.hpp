#ifndef UNI_SO2_P_TEXT_HPP
#define UNI_SO2_P_TEXT_HPP

#include "Object.hpp"

namespace so {

class Text : public Object {
public:
  Text(const Text& other) = delete;
  Text(Text&& other) noexcept;
  Text& operator=(const Text& other) = delete;
  Text& operator=(Text&& other) noexcept;
  ~Text() override = default;

  template< typename... BehaviorType >
  requires(std::is_invocable_v< BehaviorType, Text& > && ...)
  Text(std::string_view name,
       std::string_view text,
       std::string_view fontPath,
       Vector2          position,
       float            fontSize,
       Color            color,
       float            spacing,
       BehaviorType&&... behaviors):
    Object(name,
           Object::TYPE::TEXT,
           position,
           std::forward< BehaviorType >(behaviors)...),
    m_text{ text },
    m_fontSize{ fontSize },
    m_color{ color },
    m_spacing{ spacing },
    m_font{ LoadFont(fontPath.data()) } {
  }

  void setText(std::string_view text) noexcept {
    const auto lock = std::unique_lock{ m_mutex };
    m_text          = text;
  }

  [[nodiscard]] std::string getText() const noexcept {
    const auto lock = std::unique_lock{ m_mutex };
    return m_text;
  }

  void setFontSize(float fontSize) noexcept {
    const auto lock = std::unique_lock{ m_mutex };
    m_fontSize      = fontSize;
  }

  [[nodiscard]] float getFontSize() const noexcept {
    const auto lock = std::unique_lock{ m_mutex };
    return m_fontSize;
  }

  void setColor(Color color) noexcept {
    const auto lock = std::unique_lock{ m_mutex };
    m_color         = color;
  }

  [[nodiscard]] Color getColor() const noexcept {
    const auto lock = std::unique_lock{ m_mutex };
    return m_color;
  }

  void setVisible(bool visible) noexcept {
    const auto lock = std::unique_lock{ m_mutex };
    m_visible       = visible;
  }

  [[nodiscard]] bool isVisible() const noexcept {
    const auto lock = std::unique_lock{ m_mutex };
    return m_visible;
  }

  void                    draw() const override;
  [[nodiscard]] Rectangle getRect() const override;

private:
  void onDestroy() override;

protected:
  std::string m_text;
  float       m_fontSize;
  Color       m_color;
  Font        m_font;
  float       m_spacing;
  bool        m_visible{ true };
};

} // namespace so

#endif // UNI_SO2_P_TEXT_HPP
