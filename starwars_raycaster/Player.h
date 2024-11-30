#ifndef PLAYER_H
#define PLAYER_H
#include "olcPixelGameEngine.h"
#include "Defs.h"
#include "RC_Misc.h"
#include "RC_Map.h"

class Player
{
public:
	Player();
	~Player() = default;

	void processInput(olc::PixelGameEngine* pge, RC_Map& cMap, float fSpeedUp, float fElapsedTime);
	float getRotateDiff();
	void setRotateDiff(float oldR, float newR);

	olc::vf2d getMoveDiff();
	void setMoveDiff(olc::vf2d oldpos, olc::vf2d newpos);
public:
	float fPlayerX;
	float fOffsetX;
	float fOffsetY;
	float fPlayerY;
	float fPlayerH;
	float fPlayerA_deg;
	float fLookUp;
	float fPlayerFoV_deg;
	float fPlayerFoV_rad;
	float fRadius;
	bool backtowall;
	bool canrotate;
	float rotate_speed;
private:
	olc::vf2d movedifference;
	float rotatedifference;
};
#endif // !PLAYER_H


