/* Start Header -------------------------------------------------------
Copyright (C) 2011 DigiPen Institute of Technology. Reproduction or disclosure of this file or its contents without the prior written consent of DigiPen Institute of Technology is prohibited.
File Name		: GameState_Asteroids.cpp
Purpose			: Game play implementation of the Asteroids game
Language		: C++/MS VS 2010 
Platform		: Microsoft Visual Studio 2010/i7 2630QM, 8GB DDR3 1600MHz, 3GB NVIDIA GT 555M/Windows 7 Home Premium
Project			: CS529_SantoshShedbalkar_Project3_2
Author			: Santosh Shedbalkar/santosh.s/60001111
Creation date	: 10/15/2011
- End Header --------------------------------------------------------*/
#include <time.h>
#include "main.h"
// ---------------------------------------------------------------------------
// Defines

#define GAME_OBJ_NUM_MAX			32			//The total number of different objects (Shapes)
#define GAME_OBJ_INST_NUM_MAX		2048		//The total number of different game object instances


#define SHIP_INITIAL_NUM			3			// initial number of ship lives
#define SHIP_SIZE					5.0f		// ship size
#define SHIP_ACCEL_FORWARD			50.0f		// ship forward acceleration (in m/s^2)
#define SHIP_ACCEL_BACKWARD			-100.0f		// ship backward acceleration (in m/s^2)
#define SHIP_ROT_SPEED				(2.0f * PI)	// ship rotation speed (degree/second)

#define BULLET_SPEED				100.0f		// bullet speed (m/s)
#define BULLET_INTERVAL				200			// time gap between successive bullets (ms)

#define TOTAL_ASTEROID_TYPES		5			// total number of different types of asteroid shapes

#define STAR_UPDATE_INTERVAL		3000		// update interval for the stars
#define SHIP_TRAIL_LIFE				1000		// life span of the ship trail particles
#define FIRE_BLAST_LIFE				250			// life span of the ship cannon
#define PARTICLES_INST_MAX			512			// Max number of particle instances
#define ASTEROIDS_INST_MAX			128			// Max number of asteroids
#define ASTEROID_KILL_SCORE			100			// score points for the first level for shooting 1 asteroid instance
#define ASTEROIDS_TO_KILL			10			// total number asteroids needed to shoot to clear the first level
#define MAX_ASTEROIDS_PER_LEVEL		10			// max instances of asteroids present for the first level
// ---------------------------------------------------------------------------
enum TYPE
{
	// list of game object types
	TYPE_SHIP = 0, 
	TYPE_BULLET,
	TYPE_STAR,
	TYPE_SHIP_TRAIL,
	TYPE_SHIP_TRAIL_2,
	TYPE_SHIP_TRAIL_3,
	TYPE_SHIP_OUTLINE_RED,
	TYPE_SHIP_OUTLINE_GREEN,
	TYPE_ASTEROID,

	TYPE_NUM
};
enum PARTICLE_TYPE
{
	ACCEL_TRAIL = 100,
	FIRE_BLAST,
	BLINK_GREEN,
	BLINK_RED
};
enum LEVEL_STAGE
{
	LEVEL_START = 200,
	LEVEL_PLAY,
	LEVEL_END
};
// ---------------------------------------------------------------------------
// object flag definition

#define FLAG_ACTIVE		0x00000001

// ---------------------------------------------------------------------------
// Struct/Class definitions

//Game object structure
struct GameObj
{
	unsigned long		type;		// object type
	AEGfxTriList*		pMesh;		// This will hold the triangles which will form the shape of the object
};

// ---------------------------------------------------------------------------

//Game object instance structure
struct GameObjInst
{
	GameObj*			pObject;	// pointer to the 'original' shape
	unsigned long		flag;		// bit flag or-ed together
	float				scale;		// scaling value of the object instance
	Vector2D			posCurr;	// object current position
	Vector2D			velCurr;	// object current velocity
	float				dirCurr;	// object current direction

	Matrix2D				transform;	// object transformation matrix: Each frame, calculate the object instance's transformation matrix and save it here
};

// Particle instance structure
struct Particle
{
	GameObjInst* pObj;
	int life;
	int type;
};

// Asteroid instance structure
struct Asteroid
{
	GameObjInst* pObj;
	bool alive;
};

// Data structure for the level start ship animation
struct OpeningAnim
{
	int stage;
	int stageTime;
	bool finished;
	void (*callBack)(void);
} animOpen;

// Data structure for the blink ship blink animation
struct BlinkAnim
{
	int stage;
	GameObjInst *pObj;
	int stageTime;
	void (*callBack)(void);
	int type;
} animBlink;

// ---------------------------------------------------------------------------
// Static variables

// list of original object
static GameObj				sGameObjList[GAME_OBJ_NUM_MAX];				// Each element in this array represents a unique game object (shape)
static unsigned long		sGameObjNum;								// The number of defined game objects

// list of object instances
static GameObjInst			sGameObjInstList[GAME_OBJ_INST_NUM_MAX];	// Each element in this array represents a unique game object instance (sprite)
static unsigned long		sGameObjInstNum;							// The number of used game object instances

// pointer ot the ship object
static GameObjInst*			spShip;										// Pointer to the "Ship" game object instance

// number of ship available (lives 0 = game over)
static long					sShipLives;									// The number of lives left

// the score = number of asteroid destroyed
static unsigned long		sScore;										// Current score
static bool					inputAccept;								// user input is allowed?
static bool					shipInvincible;								// is ship invincible?
static int					iCurrLevel;									// user current level
static int					iLevelStage;								// current game stage in the level
static int					iAsteroidKills;								// total asteroids killed by the user in the current level

// ---------------------------------------------------------------------------

// functions to create/destroy a game object instance
static GameObjInst*			gameObjInstCreate (unsigned long type, float scale, Vector2D* pPos, Vector2D* pVel, float dir);
static void					gameObjInstDestroy(GameObjInst* pInst);

// ---------------------------------------------------------------------------
static Particle sParticles[PARTICLES_INST_MAX];
static Asteroid sAsteroids[ASTEROIDS_INST_MAX];
//char strBuff[1024];
static int iCurrTime = 0;
static int iLastBulletTime = 0;
static int iStarUpdateTime = 0;
AEGfxTriList *createAsteroidShape(void);
float randomFloat(float, float);
int findDirection(float, float, float, float, float, float);
void swapFloat(float &, float &);
void createAsteroid(void);
void collideShip(void);
void collideBullet(GameObjInst&);
void performCollision();
AEGfxTriList *createCirlceShape(unsigned int, unsigned int);
AEGfxTriList *createStarShape(unsigned int);
void createStarryBG(void);
void updateStars(void);
Particle* particleInstCreate(int, int, int, Vector2D*, Vector2D*, float);
void particleDestroy(Particle &);
void addShipTrail(void);
void updateparticles(void);
void updateAsteroids(void);
void wrapObjects(void);
void addFireBlast(void);
void collideAsteroids(void);
void killAsteroid(Asteroid &);
void destroyAsteroid(Asteroid &);
void updateAnimations(void);
AEGfxTriList *drawCircularOutline(unsigned int, float, unsigned int);
void createBlinkAnim(int);
void killAllAsteroids(void);
void levelWin(void);
void levelFail(void);
void levelStart(void);
void levelStartCallback(void);
void levelFailCallback(void);
void levelWinCallback(void);
void levelUpdate(void);
// ---------------------------------------------------------------------------

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

	// The ship object instance hasn't been ocreated yet, so this "spShip" pointer is initialized to 0
	spShip = 0;

	// zero the partcles array
	memset(sParticles, 0, sizeof(Particle) * PARTICLES_INST_MAX);

	// zero the asteroids array
	memset(sAsteroids, 0, sizeof(Asteroid) * ASTEROIDS_INST_MAX);

	// load/create the mesh data (game objects / Shapes)
	GameObj* pObj;

	// =====================
	// create the ship shape
	// =====================
	srand((unsigned)time(NULL));
	pObj		= sGameObjList + sGameObjNum++;
	pObj->type	= TYPE_SHIP;

	AEGfxTriStart();
	//
	AEGfxTriAdd(-0.4f, 0.5f, 0xFF777777, -0.1f, 0.0f, 0xFFAAAAAA, 0.5f, 0.0f, 0xFF222222);
	AEGfxTriAdd(-0.4f, -0.5f, 0xFF777777, 0.5f, 0.0f, 0xFF222222, -0.1f, 0.0f, 0xFFAAAAAA);

	AEGfxTriAdd(-0.5f, 0.3f, 0xFFBBBB99, -0.3f, 0.0f, 0xFFBBBB99, 0.3f, 0.0f, 0xFFBBBB99);
	AEGfxTriAdd(-0.3f, 0.0f, 0xFFBBBB99, -0.5f, -0.3f, 0xFFBBBB99, 0.3f, 0.0f, 0xFFBBBB99);
	//
	AEGfxTriAdd(-0.3f, -0.4f, 0xFF999999, -0.5f, -0.5f, 0xFF999999, -0.4f, -0.5f, 0xFF999999);
	AEGfxTriAdd(-0.5f, 0.5f, 0xFF999999, -0.3f, 0.4f, 0xFF999999, -0.4f, 0.5f, 0xFF999999);
	pObj->pMesh = AEGfxTriEnd();
	AE_ASSERT_MESG(pObj->pMesh, "failed to create object!!");

	// =======================
	// create the bullet shape
	// =======================
	pObj		= sGameObjList + sGameObjNum++;
	pObj->type	= TYPE_BULLET;

	AEGfxTriStart();
	AEGfxTriAdd(-0.18f, 0.0f, 0xFFFFFFFF, 0.1575f, -0.045f, 0xFFFFFFFF, 0.1575f, 0.045f, 0xFFFFFFFF);
	AEGfxTriAdd(-0.225f, 0.09f, 0xFFFFFFFF, -0.27f, 0.0f, 0xFFFFFFFF, 0.1575f, 0.045f, 0xFFFFFFFF);
	AEGfxTriAdd(-0.27f, 0.0f, 0xFFFFFFFF, -0.225f, -0.09f, 0xFFFFFFFF, 0.1575f, -0.045f, 0xFFFFFFFF);
	AEGfxTriAdd(0.1575f, 0.045f, 0xFFFFFFFF, 0.1575f, -0.045f, 0xFFFFFFFF, 0.225f, 0.0f, 0xFFFFFFFF);
	pObj->pMesh = AEGfxTriEnd();
	AE_ASSERT_MESG(pObj->pMesh, "failed to create object!!");

	// =======================
	// create the STAR shape
	// =======================
	pObj		= sGameObjList + sGameObjNum++;
	pObj->type	= TYPE_STAR;
	pObj->pMesh = createStarShape(0xFFFFFFFF);
	AE_ASSERT_MESG(pObj->pMesh, "failed to create object!!");
	// ----------------------------------------

	// =======================
	// create the SHIP TRAIL shapes
	// =======================
	pObj		= sGameObjList + sGameObjNum++;
	pObj->type	= TYPE_SHIP_TRAIL;
	AEGfxTriStart();
	AEGfxTriAdd(-0.3f, 0.0f, 0xFFFF3300, 0.3f, -0.3f, 0xFFFF3300, 0.3f, 0.3f, 0xFFFF3300);
	pObj->pMesh = AEGfxTriEnd();
	AE_ASSERT_MESG(pObj->pMesh, "failed to create object!!");

	pObj		= sGameObjList + sGameObjNum++;
	pObj->type	= TYPE_SHIP_TRAIL_2;
	AEGfxTriStart();
	AEGfxTriAdd(-0.3f, 0.0f, 0xFFFF8800, 0.3f, -0.3f, 0xFFFF8800, 0.3f, 0.3f, 0xFFFF8800);
	pObj->pMesh = AEGfxTriEnd();
	AE_ASSERT_MESG(pObj->pMesh, "failed to create object!!");

	pObj		= sGameObjList + sGameObjNum++;
	pObj->type	= TYPE_SHIP_TRAIL_3;
	AEGfxTriStart();
	AEGfxTriAdd(-0.3f, 0.0f, 0xFFFFCC00, 0.3f, -0.3f, 0xFFFFCC00, 0.3f, 0.3f, 0xFFFFCC00);
	pObj->pMesh = AEGfxTriEnd();
	AE_ASSERT_MESG(pObj->pMesh, "failed to create object!!");
	// ----------------------------------------

	// =======================
	// create the GREEN and RED SHIP OUTLINE shapes
	// =======================
	pObj		= sGameObjList + sGameObjNum++;
	pObj->type	= TYPE_SHIP_OUTLINE_RED;
	pObj->pMesh = drawCircularOutline(20, 0.05f, 0xFFFF2222);
	AE_ASSERT_MESG(pObj->pMesh, "failed to create object!!");

	pObj		= sGameObjList + sGameObjNum++;
	pObj->type	= TYPE_SHIP_OUTLINE_GREEN;
	pObj->pMesh = drawCircularOutline(20, 0.05f, 0xFF22FF22);
	AE_ASSERT_MESG(pObj->pMesh, "failed to create object!!");
	// ----------------------------------------

	// =========================
	// create the Asteroid shape
	// =========================
	for(int i = 0; i < TOTAL_ASTEROID_TYPES; ++i)
	{
		pObj		= sGameObjList + sGameObjNum++;
		pObj->type	= TYPE_ASTEROID + i;
		pObj->pMesh = createAsteroidShape();
		//pObj->pMesh = createCirlceShape(20, 0xFF991111);
		AE_ASSERT_MESG(pObj->pMesh, "failed to create object!!");
	}
	// ---------------------------------------
}

// ---------------------------------------------------------------------------

// "Initialize" function of this state
void GameStateAsteroidsInit(void)
{
	createStarryBG();
	// create the main ship
	spShip = gameObjInstCreate(TYPE_SHIP, SHIP_SIZE, 0, 0, PI/2.0f);
	AE_ASSERT(spShip);
	// CREATE THE INITIAL ASTEROIDS INSATNCES USING THE "GAMEOBJINSTCREATE" FUNCTION
	for(unsigned int i = 0; i < MAX_ASTEROIDS_PER_LEVEL; ++i)
	{
		createAsteroid();
	}
	// reset the score and the number of ship
	sScore      = 0;
	sShipLives    = SHIP_INITIAL_NUM;
	iCurrLevel = 1;
	//
	
	animBlink.stage = 0;
	animBlink.pObj = 0;

	// start the level
	levelStart();
}

// creates the starry sky BG
void createStarryBG(void)
{
	GameObjInst *pInst;
	Vector2D pos;
	for(int i = 0; i < 50; ++i)
	{
		pos.x = randomFloat(gAEWinMinX, gAEWinMaxX);
		pos.y = randomFloat(gAEWinMinY, gAEWinMaxY);
		pInst = gameObjInstCreate(TYPE_STAR, randomFloat(0.5, 2.5), &pos, 0, 0);
		AE_ASSERT(pInst);
	}
	
}

// creates an asteroid instance and places it off-stage with proper velocity vector
void createAsteroid(void)
{
	GameObjInst *pInst;
	Vector2D pos, vel;
	float scale = randomFloat(6.0f, 15.0f), rot = randomFloat(-PI, PI);
	//
	if(randomFloat(0, 1) < 0.5)
	{
		if(randomFloat(0, 1) < 0.5)
		{
			pos.x = gAEWinMinX - scale;
			pos.y = randomFloat(gAEWinMinY + 10, gAEWinMaxY - 10);
			//
			vel.x = randomFloat(5, 15);
			vel.y = randomFloat(-15, 15);
		}
		else
		{
			pos.x = gAEWinMaxX + scale;
			pos.y = randomFloat(gAEWinMinY + 10, gAEWinMaxY - 10);
			//
			vel.x = randomFloat(-15, -5);
			vel.y = randomFloat(-15, 15);
		}
	}
	else
	{
		if(randomFloat(0, 1) < 0.5)
		{
			pos.x = randomFloat(gAEWinMinX + 10, gAEWinMaxX - 10);
			pos.y = gAEWinMaxY + scale;
			//
			vel.x = randomFloat(-15, 15);
			vel.y = randomFloat(-5, -15);
		}
		else
		{
			pos.x = randomFloat(gAEWinMinX + 10, gAEWinMaxX - 10);
			pos.y = gAEWinMinY - scale;
			//
			vel.x = randomFloat(-15, 15);
			vel.y = randomFloat(5, 15);
		}
	}
	for(int i = 0; i < ASTEROIDS_INST_MAX; ++i)
	{
		Asteroid *aster = sAsteroids + i;
		if(aster->pObj)
		{
			continue;
		}
		pInst = gameObjInstCreate(TYPE_ASTEROID + (int)(randomFloat(0, TOTAL_ASTEROID_TYPES)), scale, &pos, &vel, rot);
		AE_ASSERT(pInst);
		aster->pObj = pInst;
		aster->alive = true;
		i = ASTEROIDS_INST_MAX;
	}
}

// Destroys an on-stage asteroid instance
void destroyAsteroid(Asteroid &aster)
{
	if(aster.pObj)
	{
		gameObjInstDestroy(aster.pObj);
	}
	aster.pObj = 0;
	aster.alive = false;
}
// ---------------------------------------------------------------------------

// "Update" function of this state
void GameStateAsteroidsUpdate(void)
{
	Vector2D velNew;

	Vector2DSet(velNew, spShip->velCurr.x, spShip->velCurr.y);

	// update the game time
	iCurrTime = clock() * 1000 / CLOCKS_PER_SEC;

	// =========================
	// update according to input
	// =========================

	if(inputAccept)
	{
		// add forward acceleration and fire trail when UP is pressed
		if (AEInputCheckCurr(DIK_UP))
		{
			float scale = (float) (gAEFrameTime) * SHIP_ACCEL_FORWARD;
			Vector2D accel;
			Vector2DSet(accel, cosf(spShip->dirCurr) * scale, sinf(spShip->dirCurr) * scale);
			Vector2DAdd(velNew, velNew, accel);
			Vector2DScale(velNew, velNew, 0.997f);

			addShipTrail();
		}
	
		// add backward trail when BACK is pressed
		if (AEInputCheckCurr(DIK_DOWN))
		{
			float scale = (float) (gAEFrameTime) * SHIP_ACCEL_BACKWARD;
			Vector2D accel;
			Vector2DSet(accel, cosf(spShip->dirCurr) * scale, sinf(spShip->dirCurr) * scale);
			Vector2DAdd(velNew, velNew, accel);
			Vector2DScale(velNew, velNew, 0.997f);
		}

		// rotate the ship counter clockwise when LEFT is pressed
		if (AEInputCheckCurr(DIK_LEFT))
		{
			spShip->dirCurr += SHIP_ROT_SPEED * (float)(gAEFrameTime);
			spShip->dirCurr =  AEWrap(spShip->dirCurr, -PI, PI);
		}

		// rotate the ship clockwise when RIGHT is pressed
		if (AEInputCheckCurr(DIK_RIGHT))
		{
			spShip->dirCurr -= SHIP_ROT_SPEED * (float)(gAEFrameTime);
			spShip->dirCurr =  AEWrap(spShip->dirCurr, -PI, PI);
		}

		// fire a bullet and add particle effect when SPACE is pressed
		//if (AEInputCheckTriggered(DIK_SPACE))
		if (AEInputCheckCurr(DIK_SPACE))
		{
			int diff = iCurrTime - iLastBulletTime;
			if(diff > BULLET_INTERVAL)
			{
				iLastBulletTime = iCurrTime;
				Vector2D pos, vel;
				GameObjInst *pInst ;
				//
				Vector2DSet(pos, spShip->posCurr.x, spShip->posCurr.y);
				Vector2DFromAngle(vel, spShip->dirCurr);
				Vector2DScale(vel, vel, BULLET_SPEED);
				pInst = gameObjInstCreate(TYPE_BULLET, SHIP_SIZE, &pos, &vel, spShip->dirCurr);
				AE_ASSERT(pInst);

				addFireBlast();
			}
		}
	}

	// update ship velocity
	Vector2DSet(spShip->velCurr, velNew.x, velNew.y);

	// ==================================================
	// update physics of all active game object instances
	//	-- Positions are updated here
	// ==================================================

	// update all game objects' positions
	for (unsigned long i = 0; i < GAME_OBJ_INST_NUM_MAX; i++)
	{
		GameObjInst* pInst = sGameObjInstList + i;

		// skip non-active object
		if ((pInst->flag & FLAG_ACTIVE) == 0)
			continue;
		Vector2DScaleAdd(pInst->posCurr, pInst->velCurr, pInst->posCurr, (float) gAEFrameTime);
	}

	// update all the onscreen animations
	updateAnimations();

	// ===================================
	// update active game object instances
	// Example:
	//		-- Wrap specific object instances around the world
	//		-- Removing the bullets as they go out of bounds
	//		-- If you have a homing missile for example, compute its new orientation (Homing missiles are not required for the Asteroids project)
	//		-- Update particle effects (Not required for the Asteroids project)
	// ===================================
	
	// wrap objects
	wrapObjects();

	// ====================
	// check for collision
	// ====================
	
	/*
	for each object instance: oi1
		if oi1 is not active
			skip

		if oi1 is an asteroid
			for each object instance oi2
				if(oi2 is not active or oi2 is an asteroid)
					skip

				if(oi2 is the ship)
					Check for collision between ship and asteroids (Rectangle - Rectangle)
					Update game behavior accordingly
					Update "Object instances array"
				else
				if(oi2 is a bullet)
					Check for collision between ship and asteroids (Rectangle - Point)
					Update game behavior accordingly
					Update "Object instances array"
	*/

	// do collision test
	performCollision();

	// update the level data
	levelUpdate();

	// ==============================================
	// calculate the matrix for all objects instances
	// ==============================================

	for (unsigned long i = 0; i < GAME_OBJ_INST_NUM_MAX; i++)
	{
		GameObjInst* pInst = sGameObjInstList + i;
		Matrix2D		 trans, rot, scale, *transform;

		// skip non-active object
		if ((pInst->flag & FLAG_ACTIVE) == 0)
			continue;

		// Compute the scaling matrix
		// Compute the rotation matrix 
		// Compute the translation matrix
		// Concatenate the 3 matrix in the correct order in the object instance's "transform" matrix

		transform = &(pInst->transform);
		Matrix2DIdentity(*transform);
		Matrix2DScale(scale, pInst->scale, pInst->scale);
		Matrix2DRotRad(rot, pInst->dirCurr);
		Matrix2DTranslate(trans, pInst->posCurr.x, pInst->posCurr.y);
		//
		Matrix2DConcat(*transform, scale, *transform);
		Matrix2DConcat(*transform, rot, *transform);
		Matrix2DConcat(*transform, trans, *transform);
	}
}

// wraps selected objects around the screen
void wrapObjects()
{
	for (unsigned long i = 0; i < GAME_OBJ_INST_NUM_MAX; i++)
	{
		GameObjInst* pInst = sGameObjInstList + i;
		float scale;

		// skip non-active object
		if ((pInst->flag & FLAG_ACTIVE) == 0)
			continue;
		scale = pInst->scale;
		// check if the object is a ship
		int type = pInst->pObject->type;
		if (type != TYPE_BULLET && (type < TYPE_SHIP_TRAIL || type > TYPE_SHIP_TRAIL_3))
		{
			// warp the ship from one end of the screen to the other
			pInst->posCurr.x = AEWrap(pInst->posCurr.x, gAEWinMinX - scale, gAEWinMaxX + scale);
			pInst->posCurr.y = AEWrap(pInst->posCurr.y, gAEWinMinY - scale, gAEWinMaxY + scale);
		}
		else
		{
			if(pInst->posCurr.x < gAEWinMinX - scale || pInst->posCurr.x > gAEWinMaxX + scale || 
			   pInst->posCurr.y < gAEWinMinY - scale || pInst->posCurr.y > gAEWinMaxY + scale)
			{
				gameObjInstDestroy(pInst);
			}
		}
	}
}

// performs collision test between ship, bullets and asteroids
void performCollision(void)
{
	collideAsteroids();
	collideShip();
	for (unsigned long i = 0; i < GAME_OBJ_INST_NUM_MAX; ++i)
	{
		GameObjInst* pObj1 = sGameObjInstList + i;
		if((pObj1->flag & FLAG_ACTIVE) == 0)
		{
			continue;
		}
		if(pObj1->pObject->type == TYPE_BULLET)
		{
			collideBullet(*pObj1);
		}
	}
}

// performs collision with the ship
void collideShip()
{
	if(!shipInvincible)
	{
		Asteroid *aster;
		GameObjInst *obj1;
		//
		Vector2D newPos;
		Vector2D intersectionPoint;
		//
		float t;
		Vector2D r;
		bool colliding = false;
		//
		for(int i = 0; i < ASTEROIDS_INST_MAX; ++i)
		{
			aster = sAsteroids + i;
			if(aster->alive)
			{
				obj1 = aster->pObj;
				//
				Vector2DScaleAdd(newPos, spShip->velCurr, spShip->posCurr, (float)gAEFrameTime);
				t = ReflectAnimatedCircleOnStaticCircle(spShip->posCurr, newPos, (spShip->scale)/2.0f, obj1->posCurr, (obj1->scale)/2.0f, intersectionPoint, r);
				//
				if(t > 0.0f)
				{
					colliding = true;
					i = ASTEROIDS_INST_MAX;
				}
				else
				{
					Vector2DScaleAdd(newPos, obj1->velCurr, obj1->posCurr, (float)gAEFrameTime);
					t = ReflectAnimatedCircleOnStaticCircle(obj1->posCurr, newPos, (obj1->scale)/2.0f, spShip->posCurr, (spShip->scale)/2.0f, intersectionPoint, r);
					if(t > 0.0f)
					{
						colliding = true;
						i = ASTEROIDS_INST_MAX;
					}
				}
			}
		}
		if(colliding)
		{
			levelFail();
		}
	}
}

// performs collision with the asteroids
void collideAsteroids()
{
	Asteroid *aster1, *aster2;
	GameObjInst *obj1, *obj2, *iobj;
	//
	Vector2D newPos;
	Vector2D intersectionPoint;
	//
	float smallestT1, smallestT2, t;
	Vector2D closestIntersectionPoint1, r1, closestIntersectionPoint2, r2;
	//
	for(int i = 0; i < ASTEROIDS_INST_MAX; ++i)
	{
		aster1 = sAsteroids + i;
		if(aster1->alive)
		{
			smallestT1 = smallestT2 = -1.0f;
			iobj = 0;
			obj1 = aster1->pObj;
			//
			for(int j = i + 1; j < ASTEROIDS_INST_MAX; ++j)
			{
				aster2 = sAsteroids + j;
				if(aster2->alive && aster2 != aster1)
				{
					obj2 = aster2->pObj;
					//
					Vector2DScaleAdd(newPos, obj1->velCurr, obj1->posCurr, (float)gAEFrameTime);
					t = ReflectAnimatedCircleOnStaticCircle(obj1->posCurr, newPos, (obj1->scale)/2.0f, obj2->posCurr, (obj2->scale)/2.0f, intersectionPoint, r1);
					//
					if(t > 0.0f && (t < smallestT1 || smallestT1 < 0.0f))
					{
						closestIntersectionPoint1 = intersectionPoint;
						smallestT1 = t;
					}
					//
					Vector2DScaleAdd(newPos, obj2->velCurr, obj2->posCurr, (float)gAEFrameTime);
					t = ReflectAnimatedCircleOnStaticCircle(obj2->posCurr, newPos, (obj2->scale)/2.0f, obj1->posCurr, (obj1->scale)/2.0f, intersectionPoint, r2);
					//
					if(t > 0.0f && (t < smallestT2 || smallestT2 < 0.0f))
					{
						closestIntersectionPoint2 = intersectionPoint;
						smallestT2 = t;
						iobj = obj2;
					}
				}
			}
			if(smallestT1 > 0.0)
			{
				Vector2DAdd(obj1->posCurr, closestIntersectionPoint1, r1);
				Vector2DNormalize(r1, r1);
				Vector2DScale(obj1->velCurr, r1,  Vector2DLength(obj1->velCurr));
			}
			if(smallestT2 > 0.0)
			{
				Vector2DAdd(iobj->posCurr, closestIntersectionPoint2, r2);
				Vector2DNormalize(r2, r2);
				Vector2DScale(iobj->velCurr, r2,  Vector2DLength(iobj->velCurr));
			}
		}
	}
}

// performs collision with the bullet
void collideBullet(GameObjInst& bullet)
{
	Asteroid *aster, *colAster = 0;
	GameObjInst *obj1;
	//
	Vector2D newPos;
	Vector2D intersectionPoint;
	//
	float smallestT = -1.0f, t;
	Vector2D closestIntersectionPoint, r;
	//
	for(int i = 0; i < ASTEROIDS_INST_MAX; ++i)
	{
		aster = sAsteroids + i;
		if(aster->alive)
		{
			obj1 = aster->pObj;
			//
			Vector2DScaleAdd(newPos, bullet.velCurr, bullet.posCurr, (float)gAEFrameTime);
			t = ReflectAnimatedCircleOnStaticCircle(bullet.posCurr, newPos, (bullet.scale)/2.0f, obj1->posCurr, (obj1->scale)/2.0f, intersectionPoint, r);
			//
			if(t > 0.0f && (t < smallestT || smallestT < 0.0f))
			{
				closestIntersectionPoint = intersectionPoint;
				smallestT = t;
				colAster = aster;
			}
		}
	}
	if(smallestT > 0.0f)
	{
		gameObjInstDestroy(&bullet);
		killAsteroid(*colAster);

		sScore += ASTEROID_KILL_SCORE * iCurrLevel;
		++iAsteroidKills;
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
		//AEGfxSetTransform(&(AEMtx33(pInst->transform.m)));
		AEGfxSetTransform((AEMtx33*)&pInst->transform);
		AEGfxTriDraw(pInst->pObject->pMesh);
	}

	sprintf_s(strBuffer, "Score: %d", sScore);
	AEGfxPrint(10, 10, -1, strBuffer);

	sprintf_s(strBuffer, "Ship Left: %d", sShipLives >= 0 ? sShipLives : 0);
	AEGfxPrint(600, 10, -1, strBuffer);

	// display the game over message
	if (sShipLives < 0)
		AEGfxPrint(280, 260, 0xFFFFFFFF, "       GAME OVER       ");

	sprintf_s(strBuffer, "Level: %d", iCurrLevel);
	AEGfxPrint(10, 30, -1, strBuffer);
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
		for (unsigned long i = 0; i < sGameObjNum; i++)
	{
		GameObj* pInst = sGameObjList + i;
		AEGfxTriFree(pInst->pMesh);
	}
}

// ---------------------------------------------------------------------------

GameObjInst* gameObjInstCreate(unsigned long type, float scale, Vector2D* pPos, Vector2D* pVel, float dir)
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
// ---------------------------------------------------------------------------

// creates the STAR shape
AEGfxTriList *createStarShape(unsigned int color)
{
	AEGfxTriStart();
	AEGfxTriAdd(0.0f, 0.5f, color, -0.15f, 0.15f, color, 0.15f, 0.15f, color);
	AEGfxTriAdd(-0.5f, 0.075f, color, -0.15f, 0.0f, color, -0.15f, 0.15f, color);
	AEGfxTriAdd(-0.15f, 0.0f, color, 0.0f, -0.5f, color, 0.15f, 0.0f, color);
	AEGfxTriAdd(0.15f, 0.0f, color, 0.5f, 0.075f, color, 0.15f, 0.15f, color);
	AEGfxTriAdd(-0.15f, 0.15f, color, 0.15f, 0.0f, color, 0.15f, 0.15f, color);
	AEGfxTriAdd(-0.15f, 0.15f, color, -0.15f, 0.0f, color, 0.15f, 0.0f, color);
	//
	return AEGfxTriEnd();
}

// craetes the circular shape
AEGfxTriList *createCirlceShape(unsigned int sides, unsigned int color)
{
	float ang = -PI;
	float lastX = -0.5, lastY = 0.0f, newX, newY;
	//
	float precision = 2 * PI / sides;
	//
	AEGfxTriStart();
	while(ang < PI)
	{
		ang += precision;
		newX = 0.5f * cosf(ang);
		newY = 0.5f * sinf(ang);
		AEGfxTriAdd(lastX, lastY, color, newX, newY, color, 0, 0, color);
		//
		lastX = newX;
		lastY = newY;
	}
	return AEGfxTriEnd();
}

// creates random asteroid shape.
AEGfxTriList *createAsteroidShape(void)
{
	float x0, y0, x1, y1, x2, y2;
	unsigned int color0, color1, color2;
	int dir;

	AEGfxTriStart();
	for(int i = 0; i < 50; ++i)
	{
		x0 = randomFloat(-0.5, 0.5);
		y0 = randomFloat(-0.5, 0.5);
		x1 = randomFloat(-0.5, 0.5);
		y1 = randomFloat(-0.5, 0.5);
		x2 = randomFloat(-0.5, 0.5);
		y2 = randomFloat(-0.5, 0.5);
		//
		color0 = (unsigned int) randomFloat((float) 0x00000000, (float) 0xFFFFFFFF);
		color1 = (unsigned int) randomFloat((float) 0x00000000, (float) 0xFFFFFFFF);
		color2 = (unsigned int) randomFloat((float) 0x00000000, (float) 0xFFFFFFFF);
		//
		if((dir = findDirection(x0, y0, x1, y1, x2, y2)) != 1)
		{
			if(dir == 0)
			{
				continue;
			}
			swapFloat(x1, x2);
			swapFloat(y1, y2);
		}
		AEGfxTriAdd(x0, y0, color0, x1, y1, color1, x2, y2, color2);
	}
	
	return AEGfxTriEnd();
}

// swaps 2 float variables
void swapFloat(float &a, float &b)
{
	float t = a;
	a = b;
	b = t;
}

// checks the winding of 3 points
int findDirection(float x0, float y0, float x1, float y1, float x2, float y2)
{
	float area = (x1 - x0)*(y2 - y0) - (y1 - y0)*(x2 - x0);
	return (area > 0) ? 1 : ((area < 0) ? -1 : 0);
}

// generates a random float number in a range
float randomFloat(float min, float max)
{
	float r = (float)rand() / (float)RAND_MAX;
	return min + r * (max - min);
}

// updates the starry BG
void updateStars(void)
{
	if(iCurrTime - iStarUpdateTime > STAR_UPDATE_INTERVAL)
	{
		iStarUpdateTime = iCurrTime;

		for (unsigned long i = 0; i < GAME_OBJ_INST_NUM_MAX; i++)
		{
			GameObjInst* pInst = sGameObjInstList + i;

			// skip non-active object
			if ((pInst->flag & FLAG_ACTIVE) == 0)
				continue;

			if(pInst->pObject->type == TYPE_STAR)
			{
				if(randomFloat(0.0f, 1.0f) < 0.1)
				{
					pInst->scale = randomFloat(0.2f, 2.0f);
				}
			}
		}
	}
}

// destroys a particle object
void particleDestroy(Particle &part)
{
	gameObjInstDestroy(part.pObj);
	part.pObj = 0;
}

// creates a particle instance
Particle* particleInstCreate(int type1, int type2, int life, Vector2D* pPos, Vector2D* pVel, float dir)
{
	Particle *part = 0;
	//
	for(int i = 0; i < PARTICLES_INST_MAX; ++i)
	{
		part = sParticles + i;
		if(part->pObj == 0 || part->pObj->flag == 0)
		{
			part->pObj = gameObjInstCreate(type1, 1, pPos, pVel, dir);
			AE_ASSERT(part->pObj);
			part->life = life;
			part->type = type2;
			i = PARTICLES_INST_MAX;
		}
	}
	// cannot find empty slot => return 0
	return part;
}

// adds ship trail particle effect
void addShipTrail(void)
{
	float dir = randomFloat(-PI, PI), yt;
	Matrix2D mat1, mat2, mrot, mtrans, mscale;
	int life;
	//
	Matrix2DIdentity(mat1);
	Matrix2DIdentity(mat2);
	Matrix2DRotRad(mrot, spShip->dirCurr);
	Matrix2DTranslate(mtrans, spShip->posCurr.x, spShip->posCurr.y);
	Matrix2DScale(mscale, spShip->scale, spShip->scale);

	Matrix2DConcat(mat1, mscale, mat1);
	Matrix2DConcat(mat1, mrot, mat1);
	Matrix2DConcat(mat1, mtrans, mat1);

	Matrix2DConcat(mat2, mscale, mat2);
	Matrix2DConcat(mat2, mrot, mat2);
	//
	for(int i = 0; i < 1; ++i)
	{
		Vector2D pos, vel;
		//
		pos.x = -0.5;
		pos.y = randomFloat(-0.3f, 0.3f);
		yt = abs(pos.y);
		//
		vel.x = randomFloat(-3.0f, -9.0f);
		vel.y = randomFloat(-6.0f, 6.0f);
		//
		Matrix2DMultVec(pos, mat1, pos);
		Matrix2DMultVec(vel, mat2, vel);
		//
		life = iCurrTime + (int)randomFloat(SHIP_TRAIL_LIFE * 0.85f, SHIP_TRAIL_LIFE * 1.15f);
		//
		if(yt < 0.13)
		{
			particleInstCreate(TYPE_SHIP_TRAIL, ACCEL_TRAIL, life, &pos, &vel, dir);
		}
		else if(yt < 0.23)
		{
			particleInstCreate(TYPE_SHIP_TRAIL_2, ACCEL_TRAIL, life, &pos, &vel, dir);
		}
		else
		{
			particleInstCreate(TYPE_SHIP_TRAIL_3, ACCEL_TRAIL, life, &pos, &vel, dir);
		}
	}
}

// adds fire blast particle effect
void addFireBlast(void)
{
	float rnd = randomFloat(0.0f, 1.0f), dir = randomFloat(-PI, PI);
	Matrix2D mat1, mat2, mrot, mtrans, mscale;
	int life, count = (int)randomFloat(2.0f, 7.0f);
	//
	Matrix2DIdentity(mat1);
	Matrix2DIdentity(mat2);
	Matrix2DRotRad(mrot, spShip->dirCurr);
	Matrix2DTranslate(mtrans, spShip->posCurr.x, spShip->posCurr.y);
	Matrix2DScale(mscale, spShip->scale, spShip->scale);

	Matrix2DConcat(mat1, mscale, mat1);
	Matrix2DConcat(mat1, mrot, mat1);
	Matrix2DConcat(mat1, mtrans, mat1);

	Matrix2DConcat(mat2, mscale, mat2);
	Matrix2DConcat(mat2, mrot, mat2);
	//
	for(int i = count; i > 0; --i)
	{
		Vector2D pos, vel;
		//
		pos.x = 0.5;
		pos.y = 0;
		//
		vel.x = randomFloat(3, 9);
		vel.y = randomFloat(-3, 3);
		//
		Matrix2DMultVec(pos, mat1, pos);
		Matrix2DMultVec(vel, mat2, vel);
		//
		life = iCurrTime + (int)randomFloat(FIRE_BLAST_LIFE * 0.85, FIRE_BLAST_LIFE * 1.15);
		//
		if(rnd < 0.5)
		{
			particleInstCreate(TYPE_SHIP_TRAIL, FIRE_BLAST, life, &pos, &vel, dir);
		}
		else if(rnd < 0.8)
		{
			particleInstCreate(TYPE_SHIP_TRAIL_2, FIRE_BLAST, life, &pos, &vel, dir);
		}
		else
		{
			particleInstCreate(TYPE_SHIP_TRAIL_3, FIRE_BLAST, life, &pos, &vel, dir);
		}
	}
}

// updates all particle effects
void updateparticles()
{
	Particle *part;
	float step = PI/36.0f;
	for(unsigned int i = 0; i < PARTICLES_INST_MAX; ++i)
	{
		part = sParticles + i;
		if(!(part->pObj) || ((part->pObj->flag & FLAG_ACTIVE) == 0))
		{
			continue;
		}
		switch(part->type)
		{
			case ACCEL_TRAIL:
				if(part->life - iCurrTime < 0)
				{
					particleDestroy(*part);
				}
				else
				{
					Vector2D *pos = &(part->pObj->posCurr);
					float scale = part->pObj->scale;
					if(pos->x < (gAEWinMinX - scale) || pos->x > (gAEWinMaxX + scale) || 
					   pos->y < (gAEWinMinY - scale) || pos->y > (gAEWinMaxY + scale))
					{
						//output_To_File("destroying particle 4\n");
						particleDestroy(*part);
					}
					else
					{
						part->pObj->dirCurr += step;
					}
				}
				break;
			case FIRE_BLAST:
				if(part->life - iCurrTime < 0)
				{
					particleDestroy(*part);
				}
				else
				{
					Vector2D *pos = &(part->pObj->posCurr);
					float scale = part->pObj->scale;
					if(pos->x < (gAEWinMinX - scale) || pos->x > (gAEWinMaxX + scale) || 
					   pos->y < (gAEWinMinY - scale) || pos->y > (gAEWinMaxY + scale))
					{
						particleDestroy(*part);
					}
					else
					{
						part->pObj->dirCurr += step;
					}
				}
				break;
		}
		
	}
}

// updates all asteroid instances
void updateAsteroids()
{
	float ang = (0.25f * PI / 180.0f);
	Asteroid *aster;
	int countAster = 0;
	//
	if(iLevelStage == LEVEL_PLAY)
	{
		for(int i = 0; i < ASTEROIDS_INST_MAX; ++i)
		{
			aster = sAsteroids + i;
			if(aster->pObj)
			{
				if(randomFloat(0.0f, 1.0f) < 0.05)
				{
					aster->pObj->dirCurr *= -1;
				}
				aster->pObj->dirCurr += ang;
				if(!aster->alive)
				{
					aster->pObj->velCurr.x = aster->pObj->velCurr.y = 0;
					aster->pObj->scale *= 0.99f;
					if(aster->pObj->scale < 0.2f)
					{
						destroyAsteroid(*aster);
					}
				}
				else
				{
					++countAster;
				}
			}
		}
		while(countAster < (int)(MAX_ASTEROIDS_PER_LEVEL * (iCurrLevel * 0.5)))
		{
			createAsteroid();
			++countAster;
		}
	}
}

// kills an asteroid
void killAsteroid(Asteroid &aster)
{
	aster.alive = false;
	Vector2DZero(aster.pObj->velCurr);
}

// updates all animations
void updateAnimations()
{
	updateStars();
	updateparticles();
	updateAsteroids();
	//
	if(animOpen.finished == 0)
	{
		int diff;
		switch(animOpen.stage)
		{
			case 0:
				animOpen.stage = 1;
				animOpen.stageTime = iCurrTime;
				break;
			case 1:
			case 2:
				diff = iCurrTime - animOpen.stageTime;
				if(diff > 600)
				{
					++(animOpen.stage);
					animOpen.stageTime = iCurrTime;
				}
				else if(diff > 450)
				{
					//
				}
				else if(diff > 250)
				{
					addShipTrail();
				}
				break;
			case 3:
				animOpen.finished = 1;
				if(animOpen.callBack)
				{
					animOpen.callBack();
				}
				
		}
	}

	if(animBlink.stage < 5)
	{
		GameObjInst *inst;
		Vector2D pos;
		switch(animBlink.stage)
		{
			case 1:
				++animBlink.stage;
				animBlink.pObj = 0;
				animBlink.stageTime = iCurrTime;
				break;
			case 2:
			case 3:
			case 4:
				if(iCurrTime - animBlink.stageTime > 750)
				{
					animBlink.stageTime = iCurrTime;
					++animBlink.stage;
					if((animBlink.stage > 4) && animBlink.callBack)
					{
						animBlink.callBack();
					}
				}
				else if(iCurrTime - animBlink.stageTime > 500)
				{
					if(animBlink.pObj)
					{
						gameObjInstDestroy(animBlink.pObj);
					}
					animBlink.pObj = 0;
				}
				else if(iCurrTime - animBlink.stageTime > 250)
				{
					if(!(animBlink.pObj))
					{
						Vector2DSet(pos, spShip->posCurr.x, spShip->posCurr.y);
						inst = gameObjInstCreate((animBlink.type == BLINK_RED) ? TYPE_SHIP_OUTLINE_RED : TYPE_SHIP_OUTLINE_GREEN, SHIP_SIZE * 2, &pos, 0, 0);
						AE_ASSERT(inst);
						//
						animBlink.pObj = inst;
					}
				}
				break;
		}
	}
}

// draws a circular outline shape
AEGfxTriList *drawCircularOutline(unsigned int sides, float thickness, unsigned int color)
{
	float ang = -PI, step = 2.0f*PI/sides;
	float lastx1 = -0.5, lasty1 = 0.0f, lastx2, lasty2, newx1, newy1, newx2, newy2;
	float rad = abs(thickness);
	//
	rad = (rad > 0.25f) ? 0.25f : 0.5f - rad;
	lastx2 = -rad;
	lasty2 = 0.0f;
	//
	AEGfxTriStart();
	while(ang < PI)
	{
		ang += step;
		newx1 = 0.5f * cosf(ang);
		newy1 = 0.5f * sinf(ang);
		newx2 = rad * cosf(ang);
		newy2 = rad * sinf(ang);
		AEGfxTriAdd(lastx1, lasty1, color, newx2, newy2, color, lastx2, lasty2, color);
		AEGfxTriAdd(lastx1, lasty1, color, newx1, newy1, color, newx2, newy2, color);
		//
		lastx1 = newx1; lasty1 = newy1;
		lastx2 = newx2; lasty2 = newy2;
	}
	return AEGfxTriEnd();
}

// creates blinking effect around the ship
void createBlinkAnim(int type)
{
	animBlink.stage = 1;
	animBlink.type = type;
}

// kills all asteroids
void killAllAsteroids()
{
	Asteroid *aster;
	//
	for(int i = 0; i < ASTEROIDS_INST_MAX; ++i)
	{
		aster = sAsteroids + i;
		if(aster->alive)
		{
			killAsteroid(*aster);
		}
	}
}

// handler for when user completes a level
void levelWin()
{
	iLevelStage = LEVEL_END;
	iAsteroidKills = 0;
	inputAccept = false;
	Vector2DZero(spShip->velCurr);
	++iCurrLevel;
	animBlink.callBack = levelWinCallback;
	killAllAsteroids();
	createBlinkAnim(BLINK_GREEN);
}

// function to handle when the user loses a life
void levelFail()
{
	iLevelStage = LEVEL_END;
	inputAccept = false;
	Vector2DZero(spShip->velCurr);
	animBlink.callBack = levelFailCallback;
	killAllAsteroids();
	createBlinkAnim(BLINK_RED);
}

// restart a level
void levelStart()
{
	iLevelStage = LEVEL_START;
	iAsteroidKills = 0;
	inputAccept = false;
	shipInvincible = true;
	spShip->dirCurr = PI / 2.0f;
	Vector2DZero(spShip->posCurr);
	animOpen.stage = 0;
	animOpen.finished = 0;
	animOpen.callBack = levelStartCallback;
}

// callback function vfrom the level start animation
void levelStartCallback()
{
	shipInvincible = false;
	inputAccept = true;
	iLevelStage = LEVEL_PLAY;
}

// callback function from the level fail animation
void levelFailCallback(void)
{
	if(--sShipLives > -1)
	{
		levelStart();
	}
}

// callback function function from the level win animation 
void levelWinCallback()
{
	levelStart();
}

// updates level data during game play
void levelUpdate()
{
	if(iAsteroidKills >= (ASTEROIDS_TO_KILL * iCurrLevel))
	{
		levelWin();
	}
}