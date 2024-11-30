#ifndef POWERS_H
#define POWERS_H
#include "olcPixelGameEngine.h"
#include "Player.h"
#include "RC_Objects.h"
#include "RC_Map.h"


class Powers
{
public:
	Powers();
	~Powers();

	void reticledraw(olc::PixelGameEngine* pge,bool reticle);
	bool isinsight(RC_Object& object, Player& player, float fov, float& angle2player);
	void TKpower(RC_Object& object, Player& player, RC_Map& map);
	void tkRotation(RC_Object& object, Player& player,RC_Map& map);
	void tkMove(RC_Object& object, Player& player, RC_Map& map);
	bool getinsight();
	void setinsight(bool sight);
	
private:
	bool insight = false;
	std::vector<olc::Sprite*> reticles;
	float distanceoffset = 0.0f;
	
};

#endif // !POWERS_H


