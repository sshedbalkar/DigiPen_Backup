#include "Game.h"
#include "Alpha_Engine\include\AEEngine.h"

Game * GAME(NULL);

Game::Game(void)
  : m_quit(false), m_restartLevel(false), m_paused(false), m_currentLevel(NULL), m_nextLevel(NULL)
{
  GAME = this;
}

int Game::GameLoop(void)
{
  AEGfxSetCamZoom(300.0f);

  AEInputInit();
  AEInputReset();

  if(m_currentLevel == NULL)
  {
    return EXIT_FAILURE;
  }

  while(IsRunning())
  {
    if(m_restartLevel)
    {
      m_nextLevel = m_currentLevel;
      m_restartLevel = false;
    }

    else
    {
      m_currentLevel->Load();
    }

    m_currentLevel->Init();

    while(m_currentLevel == m_nextLevel)
    {      
      AESysFrameStart();
      AEInputUpdate();

      if(!m_paused)
      {
        m_currentLevel->Update(static_cast<float>(gAEFrameTime));
      }

      m_currentLevel->Draw();
      
      if(m_restartLevel || m_quit)
      {
        m_currentLevel->Unload();
        return EXIT_SUCCESS;
      }

      AESysFrameEnd();
    }

    m_currentLevel->Free();      

    if(m_nextLevel != m_currentLevel)
    {
      m_currentLevel->Unload();
      m_currentLevel = m_nextLevel;
    }
  }

  AEInputExit();
  AESysExit();
  return EXIT_SUCCESS;
}

void Game::Quit()
{
  m_quit = true;
}

bool Game::IsRunning() const
{
  return m_quit == false;
}

bool Game::IsPaused(void) const
{
  return m_paused;
}

void Game::AddLevel(std::string levelName, Level * level, bool makeCurrent)
{
  if(level)
  {
    m_levels[levelName] = level;
    if(makeCurrent)
    {
      m_currentLevel = level;
      m_nextLevel = level;
    }
  }
}

void Game::SetCurrentLevel(std::string levelName)
{
  LevelMap::iterator it = m_levels.find(levelName);

  if(it != m_levels.end())
  {
    m_nextLevel = it->second;
  }
}