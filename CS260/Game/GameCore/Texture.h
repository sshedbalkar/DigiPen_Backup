#ifndef TEXTURE_H
#define TEXTURE_H

// Alpha Engine
#include "Alpha_Engine\include\AEEngine.h"

// std
#include <string>

class Texture
{
public:
  Texture(std::string fileName);
  ~Texture(void);

  bool Load(std::string fileName);

  operator AEGfxTriList*(void) const;
  operator AEGfxTexture*(void) const;

private:
  AEGfxTriList * m_mesh;
  AEGfxTexture * m_tex;
};

#endif