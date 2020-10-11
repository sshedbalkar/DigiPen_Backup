#include "Texture.h"

Texture::Texture(std::string fileName)
  : m_tex(NULL), m_mesh(NULL)
{
  Load(fileName);
}

Texture::~Texture(void)
{
  if(m_mesh)
    AEGfxTriFree(m_mesh);

  if(m_tex && m_tex->mTexture)
    AEGfxTextureUnload(m_tex);
}

bool Texture::Load(std::string fileName)
{
  if(m_tex)
  {
    AEGfxTextureUnload(m_tex);
  }

  if(m_mesh)
  {
    AEGfxTriFree(m_mesh);
  }
  
  AEGfxTriStart();
  AEGfxTriAdd(
               -0.5f, -0.5f, 0xFFFFFFFF, 0.0f, 1.0f,
                0.5f, -0.5f, 0xFFFFFFFF, 1.0f, 1.0f,
               -0.5f,  0.5f, 0xFFFFFFFF, 0.0f, 0.0f
             );

  AEGfxTriAdd( 
                0.5f, -0.5f, 0xFFFFFFFF, 1.0f, 1.0f, 
                0.5f,  0.5f, 0xFFFFFFFF, 1.0f, 0.0f,
               -0.5f,  0.5f, 0xFFFFFFFF, 0.0f, 0.0f
             );

  m_mesh = AEGfxTriEnd();
  
  m_tex = AEGfxTextureLoad(const_cast<char*>(fileName.c_str()));
  
  return m_mesh != NULL && m_tex != NULL;
}

Texture::operator AEGfxTriList*(void) const
{
  return m_mesh;
}

Texture::operator AEGfxTexture*(void) const
{
  return m_tex;
}
