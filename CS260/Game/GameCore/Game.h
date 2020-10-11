#ifndef GAME_H
#define GAME_H

// Level
#include "Level.h"

// std
#include <map>
#include <string>

class Game
{
public:
  Game();

  int GameLoop(void);

  void Quit(void);

  bool IsRunning(void) const;
  bool IsPaused(void) const;

  void Pause(bool pause);

  void AddLevel(std::string levelName, Level * level, bool makeCurrent = true);
  void SetCurrentLevel(std::string levelName);

private:
  bool m_quit;
  bool m_restartLevel;
  bool m_paused;

  Level * m_currentLevel;
  Level * m_nextLevel;

  typedef std::map<std::string, Level*> LevelMap;
  LevelMap m_levels;
};

extern Game * GAME;

#endif