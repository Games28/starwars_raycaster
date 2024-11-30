#include "Powers.h"

Powers::Powers()
{
	olc::Sprite* r1 = new olc::Sprite("powers/newicon.png");
	reticles.push_back(r1);
	olc::Sprite* r2 = new olc::Sprite("powers/newiconglow.png");
	reticles.push_back(r2);

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

void Powers::TKpower(RC_Object& object, Player& player, RC_Map& map)
{
}

void Powers::tkRotation(RC_Object& object, Player& player, RC_Map& map)
{
  	auto Deg2Rad = [=](float angle) { return angle / 180.0f * 3.14159f; };

	float difference_x = object.getPos().x - player.fPlayerX;
	float difference_y = object.getPos().y - player.fPlayerY;

	float distance = sqrtf(difference_x * difference_x + difference_y * difference_y);

	
	float angle_player_to_object = atan2f(difference_y, difference_x);

	float angledifrad = Deg2Rad(player.getRotateDiff());
	
	

	float vx = distance * (cosf(angle_player_to_object + angledifrad) - cosf(angle_player_to_object));
	float vy = distance * (sinf(angle_player_to_object + angledifrad) - sinf(angle_player_to_object));
    
	
	float newX = object.getPos().x + vx;
	float newY = object.getPos().y + vy;

	if (!map.Collides(newX, object.getPos().y, object.getRadius(), object.getRadius(),
		vx, vy))
	{
		
		object.setX(newX);
	}
	else
	{ 
		//////////////////////////////////////////////////////////////////////////////////
		difference_y = (object.getPos().y - 1.0f) - player.fPlayerY;
		distance = sqrtf(difference_x * difference_x + difference_y * difference_y);
		
		vy = distance * (sinf(angle_player_to_object + angledifrad) - sinf(angle_player_to_object));
		newY = object.getPos().y + vy;
		object.SetY(newY);
		//////////////////////////////////////////////////////////////////////////////////////
		
	}
	
	if (!map.Collides(object.getPos().x , newY, object.getRadius(), object.getRadius(),
		vx, vy))
	{
		
		object.SetY(newY);
	}
	else
	{
		///////////////////////////////////////////////////////////////////////////////////////
		difference_x = (object.getPos().x - 1.0f) - player.fPlayerX;
		distance = sqrtf(difference_x * difference_x + difference_y * difference_y);
		vx = distance * (cosf(angle_player_to_object + angledifrad) - cosf(angle_player_to_object));
		newX = object.getPos().x + vx;
		object.setX(newX);
		////////////////////////////////////////////////////////////////////////////////
	}

}

void Powers::tkMove(RC_Object& object, Player& player, RC_Map& map)
{
	if (!map.Collides(object.getPos().x, object.getPos().y, object.getRadius(), object.getRadius(),
		player.getMoveDiff().x, player.getMoveDiff().y) && player.backtowall == false) 
	{
		object.setPosOffset(player.getMoveDiff());
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

