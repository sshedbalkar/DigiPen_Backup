#ifndef LEVEL_H
#define LEVEL_H

class Level
{
public:
  virtual void Load(void)       = 0;
  virtual void Init(void)       = 0;
  virtual void Update(float dt) = 0;
  virtual void Draw(void)       = 0;
  virtual void Free(void)       = 0;
  virtual void Unload(void)     = 0;
};

#endif