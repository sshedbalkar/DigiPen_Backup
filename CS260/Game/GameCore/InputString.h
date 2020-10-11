#ifndef INPUT_STRING_H
#define INPUT_STRING_H

#include <string>
#include <algorithm>
#include <vector>

using std::string;

const unsigned NUM_KEYS = 256;

extern std::vector<bool> KeyStates;

class InputString
{
public:
  InputString();
  void SetActive(bool active);

  void UpdateKeyboard(unsigned key, bool pressed);
  void Update();
  void Draw(int x, int y, unsigned color);

  bool IsActive() const;
  bool IsEntered() const;

  string GetString();

private:
  string m_oldStr;
  string m_str;

  bool m_active;
  bool m_confirm;

  std::vector<bool> m_keyStates;
};

#endif