#ifndef ASTEROIDS_H
#define ASTEROIDS_H

#include "Alpha_Engine\include\AEEngine.h"
#include "Globals.h"

// Level interface
#include "Level.h"

// GameObjects
#include "GameObject.h"
#include "Math2D.h"

#include "networking\NetworkManager.h"

// std
#include <vector>
#include <map>
#include <string>
//
#define CONFIG_FILE		"../config.txt"
#define COLOR(a, r, g, b) ((a << 24) + (r << 16) + (g << 8) + b)

const float ASTEROID_SIZE = 64.0f;

const float SHIP_ROT_SPEED = float(1.5f * PI);
const float SHIP_VEL_CAP = 0.99f;

void Update_Ship(GameObject * ship, float dt);
void Update_Asteroid(GameObject * asteroid, float dt);
void Update_Bullet(GameObject * bullet, float dt);
void Update_Missile(GameObject * missile, float dt);
void Update_Particle(GameObject * particle, float dt);

const unsigned NUM_ASTEROIDS = 20;

const float BULLET_SPEED = 500.0f;
const float MISSILE_SPEED = 250.0f;
const float MISSILE_TURN = PI / 50.0f;

float frand(float min, float max);

GameObject * CreateBullet(Vector2D pos, float radAngle);
GameObject * CreateMissile(Vector2D pos);

void GetTarget(GameObject*, std::vector<GameObject*> objectList);

class Asteroids : public Level
{
public:
	virtual ~Asteroids();
	//
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

	void CheckCollisions();
	void CreateExplosion(Vector2D pos, float z);

	int ShipLives;
	int Points;

  float respawnCounter;

  GameObject * player;
  UDP_Packet * pData;
  
  void UpdateOtherPlayers();

  void AddAllPlayers();
  void UpdateMe();

	//
private:
	typedef std::vector<GameObject*> ObjectList;
	ObjectList	m_gameObjects;

	typedef std::map<std::string, Texture*> TextureMap;
	TextureMap	m_textures;

	NetworkManager*	m_network;
};

#endif