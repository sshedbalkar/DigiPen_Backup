// ---------------------------------------------------------------------------
// Project Name		:	Asteroid Game
// File Name		:	GameState_Play.cpp
// Author			:	Sun Tjen Fam
// Creation Date	:	2008/01/31
// Purpose			:	implementation of the 'play' game state
// History			:
// - 2008/01/31		:	- initial implementation
// ---------------------------------------------------------------------------

#include "main.h"

#include "Vector2D.h"
#include "Matrix2D.h"
#include "Math2D.h"

#pragma warning(disable:4244)
#pragma warning(disable:4996)
// ---------------------------------------------------------------------------

// Defines
#define GAME_OBJ_NUM_MAX      32			  //The total number of different objects (Shapes)
#define GAME_OBJ_INST_NUM_MAX 2048		  //The total number of different game object instances


#define SHIP_INITIAL_NUM      3			    // initial number of ship lives
#define SHIP_SIZE					    4.0f		  // ship size
#define SHIP_ACCEL_FORWARD		50.0f		  // ship forward acceleration (in m/s^2)
#define SHIP_ACCEL_BACKWARD		-100.0f		// ship backward acceleration (in m/s^2)
#define SHIP_ROT_SPEED				(2 * PI)	// ship rotation speed (degree/second)
#define SHIP_VEL_CAP          100.0f

#define VEL_CAP_SCALE         0.99f     // value to cap the velocity to mimic friction

#define BULLET_SCALE          1.5f      // how big the bullet is
#define BULLET_SPEED				  100.0f		// bullet speed (m/s)

#define MISSILE_SCALE         2.0f
#define MISSILE_SPEED         60.0f

#define MISSILE_TURN          (PI / MISSILE_SPEED)  // roughly 5 degrees (rad)

#define COLOR(a, r, g, b) ((a << 24) + (r << 16) + (g << 8) + b)

// ---------------------------------------------------------------------------
enum TYPE
{
	// list of game object types
	TYPE_SHIP = 0, 
	TYPE_BULLET,
	TYPE_ASTEROID,
	TYPE_MISSILE,

	TYPE_NUM
};

// ---------------------------------------------------------------------------
// object flag definition

#define FLAG_ACTIVE		0x00000001

// ---------------------------------------------------------------------------
// Struct/Class definitions

//Game object structure
struct GameObj
{
	unsigned long		type;	 // object type
	AEGfxTriList*		pMesh; // This will hold the triangles which will form the shape of the object
};

// ---------------------------------------------------------------------------

//Game object instance structure
struct GameObjInst
{
	GameObj*			pObject;	 // pointer to the 'original' shape
	unsigned long	flag;		   // bit flag or-ed together
	float				  scale;		 // scaling value of the object instance
	Vector2D			posCurr;	 // object current position
	Vector2D			velCurr;	 // object current velocity
	float				  dirCurr;	 // object current direction

  GameObjInst * pTarget;
  bool          hasTarget;

	Matrix2D			transform; // object transformation matrix: Each frame, calculate the object instance's transformation matrix and save it here
};

// ---------------------------------------------------------------------------
// Static variables

// list of original object
static GameObj				  sGameObjList[GAME_OBJ_NUM_MAX];				    // Each element in this array represents a unique game object (shape)
static unsigned long		sGameObjNum;								              // The number of defined game objects

// list of object instances
static GameObjInst			sGameObjInstList[GAME_OBJ_INST_NUM_MAX];	// Each element in this array represents a unique game object instance (sprite)
static unsigned long		sGameObjInstNum;							            // The number of used game object instances

// pointer ot the ship object
static GameObjInst*			spShip;										                // Pointer to the "Ship" game object instance

// number of ship available (lives 0 = game over)
static long					    sShipLives;									              // The number of lives left

// the score = number of asteroid destroyed
static unsigned long		sScore;										                // Current score

// ---------------------------------------------------------------------------

// functions to create/destroy a game object instance
static GameObjInst*	 gameObjInstCreate (unsigned long type, float scale, Vector2D* pPos, Vector2D* pVel, float dir, bool hasTarget);
static void					 gameObjInstDestroy(GameObjInst* pInst);

// ---------------------------------------------------------------------------

static void acquireTarget(GameObjInst * pInst);

// "Load" function of this state
void GameStateAsteroidsLoad(void)
{
	// zero the game object array
	memset(sGameObjList, 0, sizeof(GameObj) * GAME_OBJ_NUM_MAX);
	// No game objects (shapes) at this point
	sGameObjNum = 0;

	// zero the game object instance array
	memset(sGameObjInstList, 0, sizeof(GameObjInst) * GAME_OBJ_INST_NUM_MAX);
	// No game object instances (sprites) at this point
	sGameObjInstNum = 0;

	// The ship object instance hasn't been created yet, so this "spShip" pointer is initialized to 0
	spShip = 0;

	// load/create the mesh data (game objects / Shapes)
	GameObj* pObj;

	// =====================
	// create the ship shape
	// =====================

	pObj = sGameObjList + sGameObjNum++;
	pObj->type	= TYPE_SHIP;

	AEGfxTriStart();

	AEGfxTriAdd(-0.5f,  0.5f, COLOR(255, 255, 255, 255), 0.0f, 0.0f, 
              -0.5f, -0.5f, COLOR(255, 255, 255, 255), 0.0f, 0.0f,
		           0.5f,  0.0f, COLOR(255, 0, 255, 0),     0.0f, 0.0f); 

	pObj->pMesh = AEGfxTriEnd();
	AE_ASSERT_MESG(pObj->pMesh, "Failed to create ship!");

	// =======================
	// create the bullet shape
	// =======================

  pObj = sGameObjList + sGameObjNum++;
	pObj->type	= TYPE_BULLET;

	AEGfxTriStart();
	AEGfxTriAdd(
		-0.5f,  0.5f, COLOR(255, 0, 200, 0), 0.0f, 0.0f, 
		-0.5f, -0.5f, COLOR(255, 0, 200, 0), 0.0f, 0.0f,
		 0.5f,  0.0f, COLOR(255, 0, 200, 0), 0.0f, 0.0f); 

	pObj->pMesh = AEGfxTriEnd();
	AE_ASSERT_MESG(pObj->pMesh, "Failed to create bullet!");

	// =========================
	// create the asteroid shape
	// =========================

  pObj = sGameObjList + sGameObjNum++;
	pObj->type	= TYPE_ASTEROID;

	AEGfxTriStart();
	  AEGfxTriAdd(
                 -0.5f, -0.5f, COLOR(255, 150, 50, 0), 0.0f, 0.0f,
                  0.5f, -0.5f, COLOR(255, 150, 50, 0), 0.0f, 0.0f,
                 -0.5f,  0.5f, COLOR(255, 150, 50, 0), 0.0f, 0.0f
                );

    AEGfxTriAdd( 
                  0.5f, -0.5f, COLOR(255, 150, 50, 0), 0.0f, 0.0f, 
                  0.5f,  0.5f, COLOR(255, 150, 50, 0), 0.0f, 0.0f,
                 -0.5f,  0.5f, COLOR(255, 150, 50, 0), 0.0f, 0.0f
                );

	pObj->pMesh = AEGfxTriEnd();
	AE_ASSERT_MESG(pObj->pMesh, "Failed to create bullet!");
	
	// ========================
	// create the missile shape
	// ========================

  pObj = sGameObjList + sGameObjNum++;
	pObj->type	= TYPE_MISSILE;

	AEGfxTriStart();

	  AEGfxTriAdd(-0.5f,  0.5f, COLOR(255, 255, 255, 255), 0.0f, 0.0f, 
                -0.5f, -0.5f, COLOR(255, 255, 255, 255), 0.0f, 0.0f,
		             0.5f,  0.0f, COLOR(255, 255, 0, 0), 0.0f, 0.0f); 

	pObj->pMesh = AEGfxTriEnd();
	AE_ASSERT_MESG(pObj->pMesh, "Failed to create missile!");
}

// ---------------------------------------------------------------------------

// "Initialize" function of this state
void GameStateAsteroidsInit(void)
{
  Vector2D pos1, pos2, pos3, pos4, pos5;
  Vector2D vel1, vel2, vel3, vel4, vel5;

  Vector2DSet(pos1, 32, 0);
  Vector2DSet(pos2,  -50,  70);
  Vector2DSet(pos3,  90, 0);
  Vector2DSet(pos4,  -90, 17);
  Vector2DSet(pos5,  20, 15);

  Vector2DSet(vel1,  45, 15);
  Vector2DSet(vel2,  17,  -65);
  Vector2DSet(vel3,  -45, 13);
  Vector2DSet(vel4,  -10, 34);
  Vector2DSet(vel5,  -34, 13);

	// create the main ship
	spShip = gameObjInstCreate(TYPE_SHIP, SHIP_SIZE, 0, 0, 0.0f, false);
	AE_ASSERT(spShip);
	
	// CREATE THE INITIAL ASTEROIDS INSATNCES USING THE "GAMEOBJINSTCREATE" FUNCTION
  gameObjInstCreate(TYPE_ASTEROID, 8.0f,  &pos1, &vel1, 0.0f, false);
  gameObjInstCreate(TYPE_ASTEROID, 6.0f,  &pos2, &vel2, 0.0f, false);
  gameObjInstCreate(TYPE_ASTEROID, 10.0f, &pos3, &vel3, 0.0f, false);
  gameObjInstCreate(TYPE_ASTEROID, 10.0f, &pos4, &vel4, 0.0f, false);
  gameObjInstCreate(TYPE_ASTEROID, 10.0f, &pos5, &vel5, 0.0f, false);

	// reset the score and the number of ship
	sScore     = 0;
	sShipLives = SHIP_INITIAL_NUM;
}

// ---------------------------------------------------------------------------

// "Update" function of this state
void GameStateAsteroidsUpdate(void)
{
	// =========================
	// update according to input
	// =========================

	// This input handling moves the ship without any velocity nor acceleration
	// It should be changed when implementing the Asteroids project
	if (AEInputCheckCurr(DIK_UP))
  {
    Vector2D vel;
    Vector2DSet(vel, cos(spShip->dirCurr), sin(spShip->dirCurr));
    Vector2DAdd(spShip->velCurr, spShip->velCurr, vel);
  }

	if (AEInputCheckCurr(DIK_DOWN))
	{
    Vector2D vel;
    Vector2DSet(vel, -cos(spShip->dirCurr), -sin(spShip->dirCurr));
    Vector2DAdd(spShip->velCurr, spShip->velCurr, vel);
  }

	if (AEInputCheckCurr(DIK_LEFT))
	{
		spShip->dirCurr += SHIP_ROT_SPEED * (float)(gAEFrameTime);
		spShip->dirCurr =  AEWrap(spShip->dirCurr, -PI, PI);
	}

	if (AEInputCheckCurr(DIK_RIGHT))
	{
		spShip->dirCurr -= SHIP_ROT_SPEED * (float)(gAEFrameTime);
		spShip->dirCurr =  AEWrap(spShip->dirCurr, -PI, PI);
	}

  // Shoot a bullet if space is triggered (Create a new object instance)
	//if (AEInputCheckTriggered(DIK_SPACE))

  if (AEInputCheckTriggered(DIK_SPACE))
  { 
    gameObjInstCreate(TYPE_BULLET, BULLET_SCALE, &spShip->posCurr, &spShip->velCurr, spShip->dirCurr, false);
  }
  
  // Shoot a homing missle if 'm' is triggered (Create a new object instance)
	//if (AEInputCheckTriggered(DIK_M))

  if (AEInputCheckTriggered(DIK_M))
  { 
    gameObjInstCreate(TYPE_MISSILE, MISSILE_SCALE, &spShip->posCurr, &spShip->velCurr, spShip->dirCurr, false);
  }

  // ==================================================
	// update physics of all active game object instances
	//	-- Positions are updated here
	// ==================================================

  for (unsigned long i = 0; i < GAME_OBJ_INST_NUM_MAX; i++)
	{
		GameObjInst * pInst = sGameObjInstList + i;

		// skip non-active object
		if ((pInst->flag & FLAG_ACTIVE) == 0)
			continue;
		
		// check if the object is a ship
		if (pInst->pObject->type == TYPE_ASTEROID)
		{
      Vector2D v;

      Vector2DScale(v, pInst->velCurr, .20f * gAEFrameTime);

      Vector2DAdd(pInst->posCurr, pInst->posCurr, v);
    }

    if (pInst->pObject->type == TYPE_BULLET)
		{
      Vector2D v;

      Vector2DSet(v, cos(pInst->dirCurr), sin(pInst->dirCurr));
      Vector2DScale(v, v, BULLET_SPEED * gAEFrameTime);

      Vector2DAdd(pInst->posCurr, pInst->posCurr, v);
    }

    if (pInst->pObject->type == TYPE_MISSILE)
		{
      Vector2D v;

      Vector2DSet(v, cos(pInst->dirCurr), sin(pInst->dirCurr));
      Vector2DScale(v, v, MISSILE_SPEED * gAEFrameTime);
      
      Vector2DAdd(pInst->posCurr, pInst->posCurr, v);
    }
  }

	// ===================================
	// update active game object instances
	// Example:
	//		-- Wrap specific object instances around the world (Needed for the assignment)
	//		-- Removing the bullets as they go out of bounds (Needed for the assignment)
	//		-- If you have a homing missile for example, compute its new orientation (Homing missiles are not required for the Asteroids project)
	//		-- Update a particle effect (Not required for the Asteroids project)
	// ===================================
	for (unsigned long i = 0; i < GAME_OBJ_INST_NUM_MAX; i++)
	{
		GameObjInst* pInst = sGameObjInstList + i;

		// skip non-active object
		if ((pInst->flag & FLAG_ACTIVE) == 0)
			continue;
		
		// check if the object is a ship
		if (pInst->pObject->type == TYPE_SHIP)
		{
      Vector2D v;

      Vector2DScale(spShip->velCurr, spShip->velCurr, VEL_CAP_SCALE);
      Vector2DScale(v, spShip->velCurr, gAEFrameTime);

      Vector2DAdd(spShip->posCurr, spShip->posCurr, v);

			// warp the ship from one end of the screen to the other
			pInst->posCurr.x = AEWrap(pInst->posCurr.x, gAEWinMinX - SHIP_SIZE, gAEWinMaxX + SHIP_SIZE);
			pInst->posCurr.y = AEWrap(pInst->posCurr.y, gAEWinMinY - SHIP_SIZE, gAEWinMaxY + SHIP_SIZE);
    }

		// Wrap asteroids here
    if(pInst->pObject->type == TYPE_ASTEROID)
    {
      pInst->posCurr.x = AEWrap(pInst->posCurr.x, gAEWinMinX - pInst->scale, gAEWinMaxX + pInst->scale);
	    pInst->posCurr.y = AEWrap(pInst->posCurr.y, gAEWinMinY - pInst->scale, gAEWinMaxY + pInst->scale);
    }

    if(pInst->pObject->type == TYPE_BULLET || pInst->pObject->type == TYPE_MISSILE)
    {
      if(pInst->posCurr.x < -80 || pInst->posCurr.y < -60 ||
         pInst->posCurr.x >  80 || pInst->posCurr.y > 60)
      {
        gameObjInstDestroy(pInst);
      }
    }

    // Update missile (Check if previous target is still alive, adjust angle, find new target etc..)

    if(pInst->pObject->type == TYPE_MISSILE)
		{
      if(pInst->hasTarget == false)
      {
        acquireTarget(pInst);
      }

      else
      {
        Vector2D distAsteroid;
        Vector2DSub(distAsteroid, pInst->pTarget->posCurr, pInst->posCurr);
        
        float angle = atan2(distAsteroid.y, distAsteroid.x) - pInst->dirCurr;
        
        if(angle < 0.0f)
        {
          angle += 2.0f * PI;
        }

        if(angle > 0.0f && angle < PI)
        {
          pInst->dirCurr += MISSILE_TURN;
        }

        else
        {
          pInst->dirCurr -= MISSILE_TURN;
        }
      }
    }
	}

  
  // ====================
	// check for collision
	// ====================

  for (unsigned long i = 0; i < GAME_OBJ_INST_NUM_MAX; i++)
	{
		GameObjInst * pInst = sGameObjInstList + i;

		// skip non-active object
		if ((pInst->flag & FLAG_ACTIVE) == 0)
			continue;

    if(pInst->pObject->type == TYPE_ASTEROID)
    {
      for (unsigned long j = 0; j < GAME_OBJ_INST_NUM_MAX; j++)
	    {
		    GameObjInst* pInst2 = sGameObjInstList + j;

		    // skip non-active object
        if ((pInst2->flag & FLAG_ACTIVE) == 0 || pInst2->pObject->type == TYPE_ASTEROID)
			    continue;

        if(pInst2->pObject->type == TYPE_SHIP)
        {
          if(StaticCircleToStaticCircle(pInst->posCurr, pInst->scale / 2.0f, pInst2->posCurr, pInst->scale / 2.0f))
          {
            // handle asteroid hitting player
            sShipLives--;
            
            Vector2DSet(spShip->posCurr, 0, 0);
            Vector2DSet(spShip->velCurr, 0, 0);
          }
        }

        if(pInst2->pObject->type == TYPE_BULLET)
        {
          if(StaticPointToStaticCircle(pInst2->posCurr, pInst->posCurr, pInst->scale / 2.0f))
          {
            // handle asteroid hitting bullet
            sScore += 50;
            gameObjInstDestroy(pInst);
            gameObjInstDestroy(pInst2);
          }
        }

        if(pInst2->pObject->type == TYPE_MISSILE)
        {
          if(StaticPointToStaticCircle(pInst2->posCurr, pInst->posCurr, pInst->scale / 2.0f))
          {
            // handle asteroid hitting missile
            sScore += 25;
            gameObjInstDestroy(pInst);
            gameObjInstDestroy(pInst2);
          }
        }
      }
    }
  }
  
  // ====================
	// Update transforms
	// ====================
	for (unsigned long i = 0; i < GAME_OBJ_INST_NUM_MAX; i++)
	{
		GameObjInst* pInst = sGameObjInstList + i;
		Matrix2D trans, rot, scale;

		// skip non-active object
		if ((pInst->flag & FLAG_ACTIVE) == 0)
			continue;

    Matrix2DScale(scale, pInst->scale, pInst->scale);
    Matrix2DRotRad(rot, pInst->dirCurr);
    Matrix2DTranslate(trans, pInst->posCurr.x, pInst->posCurr.y);
    
    Matrix2DConcat(rot, rot, scale);
    Matrix2DConcat(pInst->transform, trans, rot);
	}
}

// ---------------------------------------------------------------------------

void GameStateAsteroidsDraw(void)
{
	char strBuffer[1024];

	// draw all object instances in the list
	for (unsigned long i = 0; i < GAME_OBJ_INST_NUM_MAX; i++)
	{
		GameObjInst* pInst = sGameObjInstList + i;

		// skip non-active object
		if ((pInst->flag & FLAG_ACTIVE) == 0)
			continue;
		

		// Set the current object instance's transform matrix using "AEGfxSetTransform"
		// Draw the shape used by the current object instance using "AEGfxTriDraw"
    AEGfxSetTransform((AEMtx33*)&pInst->transform);
    AEGfxTriDraw(pInst->pObject->pMesh);
  }

	sprintf(strBuffer, "Score: %d", sScore);
	AEGfxPrint(10, 10, -1, strBuffer);

	sprintf(strBuffer, "Ship Left: %d", sShipLives >= 0 ? sShipLives : 0);
	AEGfxPrint(600, 10, -1, strBuffer);

	// display the game over message
	if (sShipLives < 0)
  {
		AEGfxPrint(280, 260, 0xFFFFFFFF, "       GAME OVER       \n PRESS R TO RESTART");
    if(AEInputCheckTriggered(DIK_R))
    {
      gGameStateNext = GS_RESTART;
    }
  }
}

// ---------------------------------------------------------------------------

void GameStateAsteroidsFree(void)
{
	// kill all object instances in the array using "gameObjInstDestroy"
  for (unsigned long i = 0; i < GAME_OBJ_INST_NUM_MAX; i++)
	{
		GameObjInst* pInst = sGameObjInstList + i;
    gameObjInstDestroy(pInst);
  }
}

// ---------------------------------------------------------------------------

void GameStateAsteroidsUnload(void)
{
	// free all mesh data (shapes) of each object using "AEGfxTriFree"
  for (unsigned long i = 0; i < GAME_OBJ_INST_NUM_MAX; i++)
	{
		GameObjInst* pInst = sGameObjInstList + i;
    if (pInst->flag != 0)
		{
      AEGfxTriFree(pInst->pObject->pMesh);
    }
  }
}

// ---------------------------------------------------------------------------

GameObjInst* gameObjInstCreate(unsigned long type, float scale, Vector2D* pPos, Vector2D* pVel, float dir, bool hasTarget)
{
	Vector2D zero = { 0.0f, 0.0f };

	AE_ASSERT_PARM(type < sGameObjNum);
	
	// loop through the object instance list to find a non-used object instance
	for (unsigned long i = 0; i < GAME_OBJ_INST_NUM_MAX; i++)
	{
		GameObjInst* pInst = sGameObjInstList + i;

		// check if current instance is not used
		if (pInst->flag == 0)
		{
			// it is not used => use it to create the new instance
			pInst->pObject	= sGameObjList + type;
			pInst->flag		= FLAG_ACTIVE;
			pInst->scale	= scale;
			pInst->posCurr	= pPos ? *pPos : zero;
			pInst->velCurr	= pVel ? *pVel : zero;
			pInst->dirCurr	= dir;
      pInst->pTarget  = NULL;
      pInst->hasTarget = false;

			// return the newly created instance
			return pInst;
		}
	}

	// cannot find empty slot => return 0
	return 0;
}

// ---------------------------------------------------------------------------

void gameObjInstDestroy(GameObjInst* pInst)
{
	// if instance is destroyed before, just return
	if (pInst->flag == 0)
		return;

	// zero out the flag
	pInst->flag = 0;
}

static float Vector2DDet(Vector2D & v1, Vector2D & v2)
{
  return (v1.x * v2.y) + (v1.y * v2.x);
}

// ---------------------------------------------------------------------------

static void acquireTarget(GameObjInst * pInst)
{
  float minAngleDiff = -1.0f;
  float curAngleDiff =  0.0f;

  for(unsigned long i = 0; i < GAME_OBJ_INST_NUM_MAX; i++)
	{
		GameObjInst * obj = sGameObjInstList + i;

		if ((obj->flag & FLAG_ACTIVE) == 0)
    {
		  continue;
    }
    
    // find asteroids and get the nearest one
    if(obj->pObject->type == TYPE_ASTEROID)
    {
      Vector2D distAsteroid;
      Vector2DSub(distAsteroid, obj->posCurr, pInst->posCurr);
      
      float angle = atan2(distAsteroid.y, distAsteroid.x);
      
      if(angle < 0.0f)
      {
        angle += 2.0f * PI;
      }

      curAngleDiff = (float)abs(pInst->dirCurr - angle);

      if(curAngleDiff < minAngleDiff || minAngleDiff < 0.0f)
      {
        pInst->hasTarget = true;
        pInst->pTarget = obj;
        minAngleDiff = curAngleDiff;
      }
    }
  }
}

// ---------------------------------------------------------------------------