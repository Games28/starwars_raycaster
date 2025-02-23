#include "Powers.h"
#include <iostream>

Powers::Powers()
{
	olc::Sprite* r1 = new olc::Sprite("powers/newicon.png");
	reticles.push_back(r1);
	olc::Sprite* r2 = new olc::Sprite("powers/newiconglow.png");
	reticles.push_back(r2);
	olc::Sprite* hand0 = new olc::Sprite("powers/defaulthandrighttest.png");
	handsprites.push_back(hand0);
	olc::Sprite* hand1 = new olc::Sprite("powers/griphandrighttest.png");
	handsprites.push_back(hand1);
}

Powers::~Powers()
{
}

void Powers::reticledraw(olc::PixelGameEngine* pge,bool reticle)
{
	if (reticle)
	{
		pge->DrawSprite(pge->ScreenWidth() / 2, (pge->ScreenHeight() / 2) - 100, reticles[1]);
	}
	else
	{
		pge->DrawSprite(pge->ScreenWidth() / 2, (pge->ScreenHeight() / 2) - 100, reticles[0]);
	}
}

void Powers::drawHands(olc::PixelGameEngine* pge)
{
	olc::vi2d pos{ pge->ScreenWidth() - 150, pge->ScreenHeight() / 2};
	olc::Sprite* s = handsprites[hands];
	for (int y = 0; y < s->height; y++)
	{
		for (int x = 0; x < s->width; x++)
		{
			int posX = pos.x + x;
			int posY = pos.y + y;

			olc::Pixel p = s->GetPixel(x, y);
			if(p != olc::BLANK)
				pge->Draw(posX, posY, p);
		}
	}
}

void Powers::setHand(int index)
{
	switch (index)
	{
	case 0:
	{
		hands = Hands::DEFAULT;
	}break;
	case 1:
	{
		hands = Hands::GRIP;
	}break;
	case 2:
	{
		hands = Hands::PUSH;
	}break;
	case 3:
	{
		hands = Hands::PULL;
	}break;
	case 4:
	{
		hands = Hands::CHOKE;
	}break;
	case 5:
	{
		hands = Hands::LIGHTNING;
	}break;
	}
}

bool Powers::isinsight(RC_Object& object, Player& player, float fov, float& angle2player)
{
	auto Deg2Rad = [=](float angle) { return angle / 180.0f * 3.14159f; };
	auto ModuloTwoPI = [=](float angle)
		{
			float a = angle;
			while (a < 0) a += 2.0f * 3.14159f;
			while (a >= 2.0f * 3.14159f) a -= 2.0f * 3.14159f;
			return a;
		};

	float tempx = object.getPos().x - player.fPlayerX;
	float tempy = object.getPos().y - player.fPlayerY;

	angle2player = ModuloTwoPI(atan2(tempx, tempy));
	float fAligneda = (2.0f * 3.14159f - Deg2Rad(player.fPlayerA_deg)) - 0.5f * 3.14159f;
	if (abs(ModuloTwoPI(fAligneda + 3.14159f) - angle2player) < fov)
		insight = true;
	else
		insight = false;


	return abs(ModuloTwoPI(fAligneda + 3.14159f) - angle2player) < fov;
}

void Powers::TKpower(olc::PixelGameEngine* pge,RC_Object& object, Player& player, RC_Map& map,float deltatime)
{
	
	tkMove(object, player, map);
	tkRotation(object, player, map,deltatime);
	
}

void Powers::tkRotation(RC_Object& object, Player& player, RC_Map& map, float deltatime)
{
  	auto Deg2Rad = [=](float angle) { return angle / 180.0f * 3.14159f; };

	float difference_x = object.getPos().x - player.fPlayerX;
	float difference_y = object.getPos().y - player.fPlayerY;

	float distance = sqrtf(difference_x * difference_x + difference_y * difference_y);

	
	float angle_player_to_object = atan2f(difference_y, difference_x);

	float angledifrad = Deg2Rad(player.getRotateDiff());
	
	float radtodeg = rad2deg(angle_player_to_object);
	

	float vx = distance * (cosf(angle_player_to_object + angledifrad) - cosf(angle_player_to_object));
	float vy = distance * (sinf(angle_player_to_object + angledifrad) - sinf(angle_player_to_object));
    
	

	float tryX = object.getPos().x + vx;
    float tryY = object.getPos().y + vy;

	std::cout << "object: " << radtodeg << std::endl;
	std::cout << "player angle: " << player.fPlayerA_deg << std::endl;
	float newX, newY;
	
	if (!map.Collides(tryX , object.getPos().y, object.getRadius(), object.getRadius(),
		vx, vy))
	{
		
		newX = tryX;
	}
	else
	{
		newX = object.getPos().x;
		player.fPlayerA_deg = radtodeg;
	}
	
	
	if (!map.Collides(object.getPos().x , tryY, object.getRadius(), object.getRadius(),
		vx, vy))
	{
		
	     newY = tryY;
	}
	else
	{
		newY = object.getPos().y;
		player.fPlayerA_deg = radtodeg;
	}
	
	object.setX(newX);
	object.SetY(newY);
	
	

}

void Powers::tkMove(RC_Object& object, Player& player, RC_Map& map)
{
	float newX = object.getPos().x + player.getMoveDiff().x;
	float newY = object.getPos().y + player.getMoveDiff().y;

	float difference_x = object.getPos().x - player.fPlayerX;
	float difference_y = object.getPos().y - player.fPlayerY;

	

	float angle_player_to_object = atan2f(difference_y, difference_x);

	

	float radtodeg = rad2deg(angle_player_to_object);

	if (!map.Collides( newX, object.getPos().y, object.getRadius(), object.getRadius(),
		player.getMoveDiff().x, player.getMoveDiff().y) && player.backtowall == false) 
	{
  		object.setX(newX);
	}
	else
	{
		object.setX(object.getPos().x);

	}


	if (!map.Collides(object.getPos().x, newY, object.getRadius(), object.getRadius(),
		player.getMoveDiff().x, player.getMoveDiff().y) && player.backtowall == false)
	{
		object.SetY(newY);
	}
	else
	{
		object.SetY(object.getPos().y);
	}
}

bool Powers::getinsight()
{
	return insight;
}

void Powers::setinsight(bool sight)
{
	insight = sight;
}

void Powers::distancecontrols(olc::PixelGameEngine* pge, RC_Object& object, 
	Player& player, RC_Map& map, float deltatime)
{

	float newX = object.getPos().x + player.getMoveDiff().x;
	float newY = object.getPos().y + player.getMoveDiff().y;

	float difference_x = object.getPos().x - player.fPlayerX;
	float difference_y = object.getPos().y - player.fPlayerY;



	float angle_player_to_object = atan2f(difference_y, difference_x);



	float radtodeg = rad2deg(angle_player_to_object);
	float vx = 0;
	float vy = 0;
	float fSpeedUp = 1.0f;

	if (pge->GetKey(olc::O).bHeld)
	{
		vx += lu_cos(player.fPlayerA_deg) * SPEED_MOVE * fSpeedUp * deltatime;
		vy += lu_sin(player.fPlayerA_deg) * SPEED_MOVE * fSpeedUp * deltatime;
	}	

	if (pge->GetKey(olc::L).bHeld)
	{
		vx -= lu_cos(player.fPlayerA_deg) * SPEED_MOVE * fSpeedUp * deltatime;
		vy -= lu_sin(player.fPlayerA_deg) * SPEED_MOVE * fSpeedUp * deltatime;
	}
	
	float fNewX = object.getPos().x + vx;
	float fNewY = object.getPos().y + vy;

    //if (!map.Collides(fNewX, object.getPos().y, object.getRadius(), object.getRadius(), vx, vy)) {
	//
	//	object.setX(fNewX);
	//	object.SetY(fNewY);
	//
	 //else if (map.Collides(fNewX, object.getPos().y, object.getRadius(), object.getRadius(), vx, vy))
	 //{
	//	 object.setX(fNewX);
	 //}
	 //else if (map.Collides(object.getPos().x, fNewY, object.getRadius(), object.getRadius(), vx, vy))
	 //{
	//	 object.SetY(fNewY);
	 //}

	if (!map.Collides(fNewX, object.getPos().y, object.getRadius(), object.getRadius(), vx, vy)) {
		
		object.setX(fNewX);
	}
	else
	{
		object.setX(object.getPos().x);
		player.fPlayerA_deg = radtodeg;
	}
	//// walk backwards
	//										   
	if (!map.Collides(object.getPos().x, fNewY,  object.getRadius(), object.getRadius(), vx, vy)) {
	
		object.SetY(fNewY);
	}
	else
	{
		object.SetY(object.getPos().y);
		player.fPlayerA_deg = radtodeg;
	}// walk backwards
	
}



