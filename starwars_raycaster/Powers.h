#ifndef POWERS_H
#define POWERS_H
#include "olcPixelGameEngine.h"
#include "Player.h"
#include "RC_Objects.h"
#include "RC_Map.h"

enum Hands
{
	DEFAULT,        //0
	GRIP,           //1
	PUSH,           //2
	PULL,           //3
	CHOKE,          //4
	LIGHTNING       //5
};
class Powers
{
public:
	Powers();
	~Powers();

	void reticledraw(olc::PixelGameEngine* pge,bool reticle);
	void drawHands(olc::PixelGameEngine* pge);
	void setHand(int index);
	bool isinsight(RC_Object& object, Player& player, float fov, float& angle2player);
	void TKpower(olc::PixelGameEngine* pge, RC_Object& object, Player& player, RC_Map& map, float deltatime);
	void tkRotation(RC_Object& object, Player& player,RC_Map& map, float deltatime);
	void tkMove(RC_Object& object, Player& player, RC_Map& map);
	bool getinsight();
	void setinsight(bool sight);
	void distancecontrols(olc::PixelGameEngine* pge, RC_Object& object, Player& player, RC_Map& map, float deltatime);



private:
	bool insight = false;
	std::vector<olc::Sprite*> reticles;
	std::vector<olc::Sprite*> handsprites;
	float distanceoffset = 0.0f;
	Hands hands = Hands::DEFAULT;
};

#endif // !POWERS_H


