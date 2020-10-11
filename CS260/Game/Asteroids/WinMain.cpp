#include "Alpha_Engine\include\AEEngine.h"
#include "Asteroids.h"
#include "Lobby.h"
#include "Game.h"
#include "Mouse.h"
#include "InputString.h"
#include "Globals.h"
#include <iostream>

#pragma comment(lib, "Alpha_Engine.lib")
#pragma comment(lib, "GameCore.lib")

#pragma warning(disable:4996)

LRESULT CALLBACK MessageHandler(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
  HDC dc;
  PAINTSTRUCT ps;

  switch(msg)
  {
    case WM_PAINT:
    {
      dc = BeginPaint(hwnd, &ps);
      EndPaint(hwnd, &ps);
      break;
    }
    
    case WM_MOUSEMOVE:
    {
      POINT mousePos;
      GetCursorPos(&mousePos);
      ScreenToClient(AESysGetWindowHandle(), &mousePos);
      mousePos.x -= 400;
      mousePos.y -= 300;

      MOUSE->worldX = static_cast<float>(mousePos.x);
      MOUSE->worldY = static_cast<float>(mousePos.y);

      break;
    }

    case WM_KEYDOWN:
    {
      KeyStates[wParam] = true;
      break;
    }

    case WM_KEYUP:
    {
      KeyStates[wParam] = false;
      break;
    }

    case WM_MOVE:
    {
      InvalidateRect(hwnd, NULL, FALSE);
      break;
    }

    case WM_ACTIVATE:
    {
      *(AESysGetAppActive()) = (LOWORD(wParam) == WA_INACTIVE) ? 0 : 1;
      break;
    }

    case WM_DESTROY:
    {
      GAME->Quit();
      PostQuitMessage(0);
      break;
    }

    default:
    {
      return DefWindowProc(hwnd, msg, wParam, lParam);
    }
  }
  return 0;
}

INT WINAPI WinMain(HINSTANCE hInst, HINSTANCE pInst, LPSTR, INT show)
{
  int useConsoleWindow(1);
  int width(800), height(600);
  
  Mouse mouse;

  DWORD windowStyle(WS_OVERLAPPEDWINDOW);
  
  RECT winRect;
  SetRect(&winRect, 0, 0, width, height);

  AdjustWindowRect(&winRect, windowStyle, FALSE); 

  AESysInit(hInst, show, winRect.right - winRect.left, winRect.bottom - winRect.top, useConsoleWindow, MessageHandler);
  AEGfxInit();
  AEGfxReset();
  AESysSetWindowTitle("Asteroids");
  
  bool fullscreen(false);

  freopen("CON", "w", stdout);

  if(fullscreen)
  {
    HWND window = AESysGetWindowHandle();
    
    SetWindowLong(window, GWL_STYLE, WS_EX_TOPMOST|WS_POPUP);
    ShowWindow(window, SW_SHOWMAXIMIZED);
  }

  Game game;
  Asteroids asteroids;
  Lobby lobby;

  // Load textures
  Texture backdrop("backdrop.png");
  Texture ship_red("ship_red.png");
  Texture ship_blue("ship_blue.png");
  Texture ship_green("ship_green.png");
  Texture ship_yellow("ship_yellow.png");
  Texture asteroid("asteroid.png");
  Texture bullet("bullet.png");
  Texture missile("missile.png");
  Texture particle_smoke("particle_smoke.png");
  Texture particle_fire("particle_fire.png");

  // Add textures to the level
  asteroids.AddTexture("backdrop",       &backdrop);
  asteroids.AddTexture("ship_red",       &ship_red);
  asteroids.AddTexture("ship_blue",      &ship_blue);
  asteroids.AddTexture("ship_green",     &ship_green);
  asteroids.AddTexture("ship_yellow",    &ship_yellow);
  asteroids.AddTexture("asteroid",       &asteroid);
  asteroids.AddTexture("bullet",         &bullet);
  asteroids.AddTexture("missile",        &missile);
  asteroids.AddTexture("particle_smoke", &particle_smoke);
  asteroids.AddTexture("particle_fire",  &particle_fire);

  lobby.AddTexture("backdrop", &backdrop);

  // Add the Asteroids level to the game
  game.AddLevel("Lobby", &lobby, true);
  game.AddLevel("Asteroids", &asteroids, false);

  NetworkManager * networkManager = new NetworkManager();
	networkManager->Init( CONFIG_FILE );
  NETWORK = networkManager;

  return game.GameLoop();
}