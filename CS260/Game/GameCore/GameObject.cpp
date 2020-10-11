#include "GameObject.h"

GameObject::GameObject(void)
  : texture(NULL), deleteFlag(false), radAngle(0.0f), scale(1.0f, 1.0f), speed(0.0f), size(0.0f, 0.0f), zOrder(0.0f), target(NULL), counter(0.0f)
{
}

GameObject::GameObject(std::string Name)
  : name(Name), texture(NULL), deleteFlag(false), radAngle(0.0f), scale(1.0f, 1.0f), speed(0.0f), size(0.0f, 0.0f), zOrder(0.0f), target(NULL), counter(0.0f)
{
}

void GameObject::Update(float dt)
{
  Matrix2D t, r, s, ss;

  Matrix2DScale(ss, size.x, size.y);
  Matrix2DScale(s, scale.x, scale.y);
  Matrix2DRotRad(r, radAngle);
  Matrix2DTranslate(t, pos.x, pos.y);
    
  Matrix2DConcat(s, s, ss);
  Matrix2DConcat(r, r, s);
  Matrix2DConcat(transform, t, r);
}

void GameObject::Draw(void)
{
  if(texture)
  {
    AEGfxTextureSet(*texture);
    AEGfxSetTransform((AEMtx33*)(&transform));
    AEGfxTriDraw(*texture);

    Matrix2D identity;
    Matrix2DIdentity(identity);

    AEGfxSetTransform((AEMtx33*)(&identity));
  }
}