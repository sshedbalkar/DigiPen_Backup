#ifndef GAME_OBJECT_H
#define GAME_OBJECT_H

// Graphics
#include "Texture.h"

// Math
#include "Vector2D.h"
#include "Matrix2D.h"

class GameObject
{
public:
  GameObject(void);
  GameObject(std::string Name);

  GameObject * target;

  std::string name;

  Vector2D pos;
  Vector2D vel;
  Vector2D scale;
  Vector2D size;

  float zOrder;
  float radAngle;
  float speed;
  float counter;
  float lifetime;

  bool deleteFlag;

  Matrix2D transform;

  Texture * texture;

  void Update(float dt);

  void Draw(void);
};

#endif