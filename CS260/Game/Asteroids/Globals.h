#ifndef GLOBALS_H
#define GLOBALS_H

#include "networking\NetworkManager.h"

const int MAX_PLAYERS = 4;

enum
{
  PLAYER_1,
  PLAYER_2,
  PLAYER_3,
  PLAYER_4
};

struct PlayerData
{
  bool isConnected;
  bool playerHasWon;
  bool firedBullet;
  float positionX;
  float positionY;
  float rotation;
};

struct UDP_Packet
{
  PlayerData arr[MAX_PLAYERS];
};

extern unsigned PlayerID;
extern NetworkManager * NETWORK;
extern unsigned Seed;
extern unsigned NumClients;

#endif