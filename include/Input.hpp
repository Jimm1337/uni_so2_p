#ifndef UNI_SO2_P_INPUT_HPP
#define UNI_SO2_P_INPUT_HPP

#include <unordered_map>
#include "Object.hpp"

namespace so {

class Input : public Object {
public:
  Input(const Input& other) = delete;
  Input(Input&& other) noexcept;
  Input& operator=(const Input& other) = delete;
  Input& operator=(Input&& other) noexcept;
  ~Input() override = default;

  template< typename... BehaviorType >
  requires(std::is_invocable_v< BehaviorType, Input& > && ...)
  Input(std::string_view name, BehaviorType&&... behaviors):
    Object(name,
           Object::TYPE::INPUT,
           Vector2{ 0, 0 },
           std::forward< BehaviorType >(behaviors)...),
    m_keysDown{
      { KEY_UP, false },    { KEY_DOWN, false },  { KEY_LEFT, false },
      { KEY_RIGHT, false }, { KEY_ENTER, false }, { KEY_SPACE, false }
    } {
  }

  [[nodiscard]] bool isKeyDown(KeyboardKey key) const noexcept;

private:
  void onUpdate() override;

protected:
  std::unordered_map< KeyboardKey, bool > m_keysDown;
};

} // namespace so

#endif // UNI_SO2_P_INPUT_HPP
