#include "InputString.h"
#include "Alpha_Engine\include\AEEngine.h"

std::vector<bool> KeyStates(NUM_KEYS);

InputString::InputString()
  : m_confirm(false), m_active(false)
{
}

void InputString::SetActive(bool active)
{
  m_active = active;
}

bool InputString::IsActive() const
{
  return m_active;
}

bool InputString::IsEntered() const
{
  return m_active && m_confirm;
}

string InputString::GetString()
{
  if(m_confirm)
  {
    m_confirm = false;
    return m_oldStr;
  }

  return std::string("");
}

void InputString::UpdateKeyboard(unsigned key, bool pressed)
{
  KeyStates[key] = pressed;
}

void InputString::Update()
{
  if(!m_active)
    return;

  for(unsigned i = 0; i < NUM_KEYS; i++)
  {
    if(KeyStates[i])
    {
      if(i == VK_BACK && !m_str.empty())
        m_str.pop_back();

      else if(i == VK_OEM_PERIOD)
        m_str.push_back('.');
      
      else if(i == VK_OEM_2)
        m_str.push_back('/');  

      else if(i == VK_SHIFT)
       continue;

      else
        m_str.push_back(i);

      KeyStates[i] = false;
    }
  }
  
  size_t pos = m_str.find(VK_BACK);
  if(pos != std::string::npos)
  {
    m_str.erase(pos);
  }

  pos = m_str.find(VK_RETURN);
  if(pos != std::string::npos)
  {
    m_str.erase(pos);
    m_oldStr = m_str;
    m_str.clear();
    m_confirm = true;
  }
}

void InputString::Draw(int x, int y, unsigned color)
{
  AEGfxPrint((s32)x, (s32)y, color, (s8*)m_str.c_str()); 
}

