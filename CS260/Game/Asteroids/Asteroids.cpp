#include "Asteroids.h"
#include <algorithm>
#include <iterator>
#include <ctime>
#include <sstream>
#include <iostream>
#include "Globals.h"

#include "Game.h"

#include "common\ReturnCodes.h"
#include "utility\Utility.h"
//
bool GameObjSortPred(GameObject * lhs, GameObject * rhs)
{
  return lhs->zOrder < rhs->zOrder;
}

static void Print(std::string str, float x, float y, unsigned color)
{
  AEGfxPrint(static_cast<s32>(x), static_cast<s32>(y), color, const_cast<s8*>(str.c_str()));
}

//////////////////////////////////////////////////////////////////////

Asteroids::~Asteroids()
{
	delete m_network;
}
void Asteroids::Load(void)
{
  // Setup backdrop
  GameObject * backdrop = new GameObject("backdrop");
  Vector2DSet(backdrop->size, 800.0f, 600.0f);
  backdrop->texture = GetTexture("backdrop");
  backdrop->zOrder = 0.0f;
  AddGameObject(backdrop);
  
  pData = NULL;
  
  GameObject ** asteroids = new GameObject*[NUM_ASTEROIDS];
  for(size_t i = 0; i < NUM_ASTEROIDS; i++)
  {
    asteroids[i] = new GameObject;
  }
  
  AddAllPlayers();
  
  // Setup asteroids
  // Seed with the same number each time
  std::srand(Seed);

  for(int i = 0; i < NUM_ASTEROIDS; i++)
  {
    float x = frand(-400, 400.0f);
    float y = frand(-300, 300.0f);

    Vector2DSet(asteroids[i]->pos, x, y);
    float scale = frand(7.0f, 25.0f) * 0.10f;
    float rad = frand(-PI, PI);

    Vector2DSet(asteroids[i]->scale, scale, scale);
    Vector2DSet(asteroids[i]->size, ASTEROID_SIZE, ASTEROID_SIZE);
    Vector2DSet(asteroids[i]->vel, frand(-100, 100), frand(-100, 100));
    
    asteroids[i]->radAngle = rad;
    asteroids[i]->texture = GetTexture("asteroid");
    asteroids[i]->speed = frand(20.0f, 75.0f) * 0.01f;

    static float z = 2.0f;

    asteroids[i]->zOrder = z++;

    std::stringstream name;
    name << "asteroid_" << i;
    asteroids[i]->name = name.str();
    AddGameObject(asteroids[i]);
  }
}

void Asteroids::Init(void)
{
	ShipLives = 3;
	Points = 0;
	m_network = NETWORK;
}

void Asteroids::Update(float dt)
{
  std::string msg;
  
  if(m_network->RecvUDPMsg(msg) == RET_SUCCESS)
  {
    pData = (UDP_Packet*)(msg.c_str());
  }
  
  UpdateOtherPlayers();

  Update_Ship(player, dt);
  
  UpdateMe();

  ObjectList::iterator it = m_gameObjects.begin();
  
  while(it != m_gameObjects.end())
  { 
    (*it)->Update(dt);

    if((*it)->name.find("asteroid") != std::string::npos)
    {
      Update_Asteroid(*it, dt);
    }

    else if((*it)->name.find("bullet") != std::string::npos)
    {
      Update_Bullet(*it, dt);
    }

    else if((*it)->name.find("missile") != std::string::npos)
    {
      Update_Missile(*it, dt);
      GetTarget(*it, m_gameObjects);
    }
    
    else if((*it)->name.find("particle") != std::string::npos)
    {
      Update_Particle(*it, dt);
    }

    it++;
  }

  CheckCollisions();

  // cleanup dead objects
  for(unsigned int i = 0; i < m_gameObjects.size(); i++)
	{
    if(m_gameObjects[i]->deleteFlag)
		{
			m_gameObjects.erase(m_gameObjects.begin() + i);
		}
	}

  if(AEInputCheckTriggered(DIK_SPACE))
  {
    GameObject * bullet = CreateBullet(player->pos, player->radAngle);
    bullet->texture = GetTexture("bullet");
    bullet->zOrder = player->zOrder;
    
    AddGameObject(bullet);
  }

  if(AEInputCheckTriggered(DIK_M))
  {
    GameObject * missile = CreateMissile(player->pos);
    GetTarget(missile, m_gameObjects);
    missile->texture = GetTexture("missile");
    missile->zOrder = player->zOrder;
    missile->radAngle = player->radAngle;

    AddGameObject(missile);
  }
}

void Asteroids::UpdateMe()
{
  PlayerData data;
  
  data.isConnected = true;
  data.firedBullet = false;
  data.playerHasWon = false;
  data.positionX = player->pos.x;
  data.positionY = player->pos.y;
  data.rotation = player->radAngle;
  
  m_network->SendUDPMsg((char*)&data, sizeof(data), m_network->GetUPDServerAddr());
}

void Asteroids::Draw(void)
{
  ObjectList::iterator it = m_gameObjects.begin();

  for(; it != m_gameObjects.end(); it++)
  {
    (*it)->Draw();
  }

  std::stringstream info;
  info << " Points: " << Points;
  Print(info.str(), 0, 0, 0xFFFFFFFF);
}

void Asteroids::Free(void)
{
}

void Asteroids::Unload(void)
{
  ObjectList::iterator it = m_gameObjects.begin();

  for(; it != m_gameObjects.end(); it++)
  {
    delete (*it);
  }

  m_gameObjects.clear();
}

void Asteroids::AddGameObject(GameObject * gameObject)
{
  m_gameObjects.push_back(gameObject);

  std::sort(m_gameObjects.begin(), m_gameObjects.end(), GameObjSortPred);
}

GameObject * Asteroids::GetGameObject(std::string name)
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

void Asteroids::AddTexture(std::string name, Texture * texture)
{
  if(m_textures.find(name) == m_textures.end())
  {
    m_textures[name] = texture;
  }
}

Texture * Asteroids::GetTexture(std::string name)
{
  if(m_textures.find(name) != m_textures.end())
  {
    return m_textures.at(name);
  }

  return NULL;
}

void Asteroids::CheckCollisions()
{
  for (size_t i = 0; i < m_gameObjects.size(); i++)
	{
		GameObject * pInst = m_gameObjects[i];

    if(pInst->deleteFlag)
          continue;

    if(pInst->name.find("asteroid") != std::string::npos)
    {
      for (size_t j = 0; j < m_gameObjects.size(); j++)
	    {
        GameObject * pInst2 = m_gameObjects[j];

        if(pInst2->deleteFlag)
          continue;

        if(pInst2->name.find("player") != std::string::npos)
        {
          if(StaticCircleToStaticCircle(pInst->pos,  (pInst->size.x * pInst->scale.x) / 2.0f, 
                                        pInst2->pos, (pInst2->size.x * pInst2->scale.x) / 2.0f))
          {
            //decrement lives, reset ship blah blah
            //CreateExplosion(pInst->pos, pInst->zOrder);
          }
        }

        else if(pInst2->name.find("bullet") != std::string::npos)
        {
          if(StaticCircleToStaticCircle(pInst->pos,  (pInst->size.x * pInst->scale.x) / 2.0f, 
                                        pInst2->pos, (pInst2->size.x * pInst2->scale.x) / 2.0f))
          {
            // handle asteroid hitting bullet, increase score etc
            pInst->deleteFlag = true;
            pInst2->deleteFlag = true;

            CreateExplosion(pInst->pos, pInst->zOrder);

            Points += 100;

            break;
          }
        }

        else if(pInst2->name.find("missile") != std::string::npos)
        {
          if(StaticCircleToStaticCircle(pInst->pos,  (pInst->size.x * pInst->scale.x) / 2.0f, 
                                        pInst2->pos, (pInst2->size.x * pInst2->scale.x) / 2.0f))
          {
            // handle asteroid hitting bullet, increase score etc
            pInst->deleteFlag = true;
            pInst2->deleteFlag = true;

            CreateExplosion(pInst->pos, pInst->zOrder);

            Points += 150;
          }
        }
      }
    }
  }
}

void Asteroids::CreateExplosion(Vector2D pos, float z)
{
  for(int i = 0; i < 50; i++)
  {
    static int id = 0;
    std::stringstream name;
    name << "particle_" << id++;

    GameObject * particle = new GameObject(name.str());

    particle->pos.x = pos.x;
    particle->pos.y = pos.y;

    particle->vel.x = frand(-2.0f, 2.0f);
    particle->vel.y = frand(-2.0f, 2.0f);

    particle->speed = frand(10.0f, 20.0f);
    
    particle->size.x = 32.0f;
    particle->size.y = 32.0f;

    particle->scale.x = 1.0f;
    particle->scale.y = 1.0f;

    particle->lifetime = frand(20.0f, 50.0f);
    particle->zOrder = z;

    if((i % 2) == 0)
    {
      particle->texture = GetTexture("particle_fire");
    }

    else
    {
      particle->texture = GetTexture("particle_smoke");
    }

    AddGameObject(particle);
  }
}

void Print(std::string str, float x, float y)
{
  AEGfxPrint(static_cast<s32>(x), static_cast<s32>(y), 0xFFFFFFFF, const_cast<s8*>(str.c_str()));
}

float frand(float min, float max)
{
  return float(min + (rand() % (int)(max - min + 1)));
}

void Update_Ship(GameObject * ship, float dt)
{
  if(AEInputCheckCurr(DIK_UP))
  {
    Vector2D vel;
    Vector2DSet(vel, cos(ship->radAngle), sin(ship->radAngle));
    Vector2DAdd(ship->vel, ship->vel, vel);
  }

	if(AEInputCheckCurr(DIK_DOWN))
	{
    Vector2D vel;
    Vector2DSet(vel, -cos( ship->radAngle), -sin(ship->radAngle));
    Vector2DAdd(ship->vel, ship->vel, vel);
  }

	if(AEInputCheckCurr(DIK_LEFT))
	{
		ship->radAngle += SHIP_ROT_SPEED * (float)(gAEFrameTime);
		ship->radAngle = AEWrap( ship->radAngle, -PI, PI);
	}

	if(AEInputCheckCurr(DIK_RIGHT))
	{
		ship->radAngle -= SHIP_ROT_SPEED * (float)(gAEFrameTime);
		ship->radAngle =  AEWrap(ship->radAngle, -PI, PI);
	}

  Vector2D v;

  Vector2DScale(ship->vel, ship->vel, SHIP_VEL_CAP);
  Vector2DScale(v, ship->vel, ship->speed * dt);

  Vector2DAdd(ship->pos, ship->pos, v);

	// wrap the ship from one end of the screen to the other
	ship->pos.x = AEWrap(ship->pos.x, -400.0f - ship->size.x, 400.0f + ship->size.x);
	ship->pos.y = AEWrap(ship->pos.y, -300.0f - ship->size.y, 300.0f + ship->size.y);
}

void Update_Asteroid(GameObject * asteroid, float dt)
{
  Vector2D v;
  Vector2DScale(v, asteroid->vel, asteroid->speed * dt);
  Vector2DAdd(asteroid->pos, asteroid->pos, v);

  asteroid->pos.x = AEWrap(asteroid->pos.x, -400.0f - asteroid->size.x, 400.0f + asteroid->size.x);
	asteroid->pos.y = AEWrap(asteroid->pos.y, -300.0f - asteroid->size.y, 300.0f + asteroid->size.y);
}

void Update_Bullet(GameObject * bullet, float dt)
{
  Vector2D v;
  Vector2DSet(v, cos(bullet->radAngle), sin(bullet->radAngle));
  Vector2DScale(v, v, bullet->speed * dt);
  Vector2DAdd(bullet->pos, bullet->pos, v);

  if(bullet->pos.x < -400 || bullet->pos.x > 400 ||
     bullet->pos.y < -300 || bullet->pos.y > 300)
  {
    bullet->deleteFlag = true;
  }
}

void Update_Missile(GameObject * missile, float dt)
{
  Vector2D v;

  Vector2DSet(v, cos(missile->radAngle), sin(missile->radAngle));
  Vector2DScale(v, v, missile->speed * dt);
      
  Vector2DAdd(missile->pos, missile->pos, v);

  if(missile->target != NULL)
  {
    Vector2D distAsteroid;
    Vector2DSub(distAsteroid, missile->target->pos, missile->pos);
        
    float angle = atan2(distAsteroid.y, distAsteroid.x) - missile->radAngle;
        
    if(angle < 0.0f)
    {
      angle += 2.0f * PI;
    }

    if(angle > 0.0f && angle < PI)
    {
      missile->radAngle += MISSILE_TURN;
    }

    else
    {
      missile->radAngle -= MISSILE_TURN;
    }
  }

  if(missile->pos.x < -400 || missile->pos.x > 400 ||
     missile->pos.y < -300 || missile->pos.y > 300)
  {
    missile->deleteFlag = true;
  }
}

void Update_Particle(GameObject * particle, float dt)
{
  Vector2D v;
  Vector2DScale(v, particle->vel, particle->speed * dt);
  Vector2DAdd(particle->pos, particle->pos, v);

  if(particle->pos.x < -400 || particle->pos.x > 400 ||
     particle->pos.y < -300 || particle->pos.y > 300)
  {
    particle->deleteFlag = true;
  }

  particle->counter++;

  if(particle->counter > particle->lifetime)
  {
    particle->deleteFlag = true;
  }
}

GameObject * CreateBullet(Vector2D pos, float radAngle)
{
  static int id = 0;

  std::stringstream name;
  name << "bullet_" << id++;

  GameObject * bullet = new GameObject(name.str());

  bullet->pos = pos;

  bullet->radAngle = radAngle;
  bullet->speed = BULLET_SPEED;

  bullet->size.x = 5;
  bullet->size.y = 5;

  return bullet;
 }

GameObject * CreateMissile(Vector2D pos)
{
  static int id = 0;

  std::stringstream name;
  name << "missile_" << id++;

  GameObject * missile = new GameObject(name.str());

  missile->pos = pos;

  missile->speed = MISSILE_SPEED;

  missile->size.x = 16;
  missile->size.y = 7;

  return missile;
 }

void GetTarget(GameObject * missile, std::vector<GameObject*> objectList)
{
  float minAngleDiff = -1.0f;
  float curAngleDiff =  0.0f;

  for(unsigned long i = 0; i < objectList.size(); i++)
	{
		GameObject * obj = objectList[i];
    
    // find asteroids and get the nearest one
    if(obj->name.find("asteroid") != std::string::npos)
    {
      Vector2D distAsteroid;
      Vector2DSub(distAsteroid, obj->pos, obj->pos);
      
      float angle = atan2(distAsteroid.y, distAsteroid.x);
      
      if(angle < 0.0f)
      {
        angle += 2.0f * PI;
      }

      curAngleDiff = (float)abs(obj->radAngle - angle);

      if(curAngleDiff < minAngleDiff || minAngleDiff < 0.0f)
      {
        missile->target = obj;
        minAngleDiff = curAngleDiff;
      }
    }
  }
}

void Asteroids::UpdateOtherPlayers()
{
  if(pData)
  {
    for(unsigned i = 0; i < NumClients; i++)
    {
      if(i == PlayerID)
        continue;

      if(pData->arr[i].isConnected )
      {
        std::stringstream name;
        name << "player_" << i;
        GameObject * otherPlayer = GetGameObject(name.str());
        
        if(otherPlayer)
        {
          otherPlayer->pos = Vector2D(pData->arr[i].positionX, pData->arr[i].positionY);
          otherPlayer->radAngle = pData->arr[i].rotation;
        }
      }
    }
  }
}

void Asteroids::AddAllPlayers()
{
  for(unsigned i = 0; i < NumClients; i++)
  {
    std::stringstream name;
    name << "player_" << i;

    GameObject * newPlayer = new GameObject(name.str());

    if(PLAYER_1 == i)
    { 
      Vector2DSet(newPlayer->pos, -336.0f,  236.0f); 
      newPlayer->texture = GetTexture("ship_red");   
      newPlayer->radAngle = 0.0f;

      if(PlayerID == i)
      {
        player = newPlayer;
      }
    }

    else if(PLAYER_2 == i) 
    { 
      Vector2DSet(newPlayer->pos,  336.0f,  236.0f); 
      newPlayer->texture = GetTexture("ship_blue");
      newPlayer->radAngle = PI;
      
      if(PlayerID == i)
      {
        player = newPlayer;
      }
    }

    else if(PLAYER_3 == i) 
    { 
      Vector2DSet(newPlayer->pos,  336.0f, -236.0f); 
      newPlayer->texture = GetTexture("ship_green");
      newPlayer->radAngle = PI;
      
      if(PlayerID == i)
      {
        player = newPlayer;
      }
    }

    else if(PLAYER_4 == i) 
    {
      Vector2DSet(newPlayer->pos, -336.0f, -236.0f); 
      newPlayer->texture = GetTexture("ship_yellow");
      newPlayer->radAngle = 0.0f;
      
      if(PlayerID == i)
      {
        player = newPlayer;
      }
    }
    
    newPlayer->size = Vector2D(64.0f, 64.0f);
    newPlayer->zOrder = NUM_ASTEROIDS / 2.0f;
    newPlayer->speed = 4.0f;

    AddGameObject(newPlayer);
  }
}