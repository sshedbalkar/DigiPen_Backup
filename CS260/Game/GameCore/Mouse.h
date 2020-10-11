#ifndef MOUSE_H
#define MOUSE_H

struct Mouse
{
  Mouse();
  float worldX, worldY;
};

extern Mouse * MOUSE;

#endif