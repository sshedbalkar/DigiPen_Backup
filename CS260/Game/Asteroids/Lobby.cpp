#include "Lobby.h"
#include "Game.h"
#include "Globals.h"

#include <iostream>

static bool GameObjSortPred(GameObject * lhs, GameObject * rhs)
{
  return lhs->zOrder < rhs->zOrder;
}

static void Print(std::string str, float x, float y, unsigned color)
{
  AEGfxPrint(static_cast<s32>(x), static_cast<s32>(y), color, const_cast<s8*>(str.c_str()));
}

static int ReadConfigFile( const char *config, ConfigData *data );

void Lobby::Load(void)
{
  GameObject * backdrop = new GameObject("backdrop");

  Vector2DSet(backdrop->size, 800.0f, 600.0f);
  backdrop->texture = GetTexture("backdrop");

  AddGameObject(backdrop);
  m_network = NETWORK;
}

void Lobby::Init(void)
{
  inputString.SetActive(true);
}

void Lobby::Update(float dt)
{
  ObjectList::iterator it = m_gameObjects.begin();
  
  while(it != m_gameObjects.end())
  { 
    (*it)->Update(dt);
    it++;
  }

  inputString.Update();

  if(inputString.IsEntered())
  {
    ChatMessage msg;
    msg.msg = inputString.GetString();

    while(msg.msg.length() > 69)
    {
      msg.msg.pop_back();
    }
    
    m_network->SendTCPMsg(msg.msg);
  }

  GetChatMessages();
}

void Lobby::Draw(void)
{
  ObjectList::iterator it = m_gameObjects.begin();

  for(; it != m_gameObjects.end(); it++)
  {
    (*it)->Draw();
  }

  Print("Asteroids Lobby", 600.0f, 10.0f, COLOR(255, 0, 150, 255));

  for(size_t i = 0; i < m_chatMessages.size(); i++)
  {
    Print(m_chatMessages[i].msg, 0.0f, m_chatMessages[i].yPos, m_chatMessages[i].color);
  }

  inputString.Draw(0, 600, COLOR(255, 255, 255, 255));
}

void Lobby::Free(void)
{
}

void Lobby::Unload(void)
{
}

void Lobby::AddTexture(std::string name, Texture * texture)
{
  if(m_textures.find(name) == m_textures.end())
  {
    m_textures[name] = texture;
  }
}

Texture * Lobby::GetTexture(std::string name)
{
  if(m_textures.find(name) != m_textures.end())
  {
    return m_textures.at(name);
  }

  return NULL;
}

void Lobby::AddGameObject(GameObject * gameObject)
{
  m_gameObjects.push_back(gameObject);

  std::sort(m_gameObjects.begin(), m_gameObjects.end(), GameObjSortPred);
}

GameObject * Lobby::GetGameObject(std::string name)
{
  ObjectList::iterator it = m_gameObjects.begin();

  for(; it != m_gameObjects.end(); it++)
  {
    if((*it)->name == name)
    {
      return (*it);
    }
  }

  return NULL;
}

void Lobby::GetChatMessages()
{
  std::string msg;

  m_network->RecvTCPMsg(msg);

  if(msg != "")
  {
    for(size_t i = 0; i < m_chatMessages.size(); i++)
    {
      m_chatMessages[i].yPos -= 20.0f;
    }
    
    ChatMessage newMsg;
    
    unsigned char id = msg[0];
    switch(id)
    {                                  // A    R    G    B
      case PLAYER_1: newMsg.color = COLOR(255, 255, 0,   0); break;
      case PLAYER_2: newMsg.color = COLOR(255, 0,   0,   255); break;
      case PLAYER_3: newMsg.color = COLOR(255, 0,   255, 0); break;
      case PLAYER_4: newMsg.color = COLOR(255, 255, 255, 0); break;
    }
    
    msg = msg.substr(1, msg.length());
    newMsg.msg = msg;
    newMsg.yPos = 580.0f;
    
    m_chatMessages.push_back(newMsg);

    if(msg.find("ASTEROIDS") != std::string::npos)
    {
      msg.erase(0, msg.find_first_of(":") + 1);

      std::string seed = msg.substr(0, msg.find_first_of(":"));
      
      Seed = unsigned(seed[0]);
      msg.erase(0, msg.find_first_of(":") + 1);
      
      PlayerID = unsigned(msg[0]);
      msg.erase(0, msg.find_first_of(":") + 1);

      NumClients = unsigned(msg[0]);

      GAME->SetCurrentLevel("Asteroids");
    }
  }
}
