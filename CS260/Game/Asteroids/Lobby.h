#ifndef LOBBY_H
#define LOBBY_H

// Level interface
#include "Level.h"

#include "Alpha_Engine\include\AEEngine.h"

#include "Texture.h"
#include "GameObject.h"

#include "InputString.h"

// Networking
#include "networking\NetworkManager.h"

// std
#include <vector>
#include <map>
#include <string>

#ifndef COLOR
  #define COLOR(a, r, g, b) ((a << 24) + (r << 16) + (g << 8) + b)
#endif

struct ChatMessage
{
  unsigned color;
  float yPos;
  std::string msg;
};

struct ConfigData
{
	std::string		ServerIP;
	unsigned short	ServerPort;
	unsigned short	ClientPort;
};

#ifndef CONFIG_FILE
  #define CONFIG_FILE	"../config.txt"
#endif

class Lobby : public Level
{
public:
  virtual void Load(void);
  virtual void Init(void);
  virtual void Update(float dt);
  virtual void Draw(void);
  virtual void Free(void);
  virtual void Unload(void);

  void AddGameObject(GameObject * gameObject);
	GameObject * GetGameObject(std::string name);

  void AddTexture(std::string name, Texture * texture);
  Texture * GetTexture(std::string name);

  InputString inputString;

  void GetChatMessages();

private:
  typedef std::map<std::string, Texture*> TextureMap;
  TextureMap m_textures;

  typedef std::vector<GameObject*> ObjectList;
	ObjectList	m_gameObjects;

  std::vector<ChatMessage> m_chatMessages;

  NetworkManager*	m_network;
};

#endif