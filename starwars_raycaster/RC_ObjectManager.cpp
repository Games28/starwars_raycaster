#include "RC_ObjectManager.h"

RC_ObjectManager::RC_ObjectManager()
{
}

RC_ObjectManager::~RC_ObjectManager()
{
}

void RC_ObjectManager::initailize(RC_Map& map)
{
	for (int i = 0; i < objectmap.size(); i++)
	{
		std::string maplevel = objectmap[i];

		for (int y = 0; y < map.Width(); y++)
		{
			for (int x = 0; x < map.Hight(); x++)
			{
				char objID = maplevel[y * map.Width() + x];
				
				for (int j = 0; j < vinitobjectblueprints.size(); j++)
				{
					ObjectBluePrints blueprint = vinitobjectblueprints[j];
				
					if (objID == blueprint.cID)
					{
						RC_Object* tempobj =  new RC_Object();
						tempobj->setPos({ (float)x,(float)y });
						tempobj->SetScale(blueprint.scale);
						tempobj->SetSprite(new olc::Sprite(blueprint.location));
						tempobj->SetDistToPlayer(-1.0f);
						tempobj->SetAngleToPlayer(0.0f);
						tempobj->SetDecal();
						tempobj->IsAnimated(blueprint.isAnimated);
						tempobj->IsBillboarded(blueprint.isbillboarded);
						tempobj->IsStationary(blueprint.isStationary);
						if (!blueprint.isStationary) { tempobj->Setvel({ 2.0,2.0 }); }

						tempobj->setRadius(blueprint.radius);
						objectlist.push_back(tempobj);
					}
				}
				
			}
		}
	}
}

void RC_ObjectManager::Update(olc::PixelGameEngine* pge,Player& player, RC_Map& map, RC_DepthDrawer& ddraw, float deltatime)
{
	wallCollision(map, deltatime);
	if (heldObject == nullptr)
	{
		for (auto& obj : objectlist)
		{
			float fov;
			if (power.isinsight(*obj, player, 3.0f * (3.14159f / 180.0f), fov))
			{
				power.reticledraw(pge, power.getinsight());

				if (power.getinsight())
				{
					if (pge->GetKey(olc::SPACE).bHeld)
					{
						
						obj->IsStationary(true);
						
						heldObject = obj;
						
						break;
					}
				}

			}
		}
	}
	
	if (pge->GetKey(olc::SPACE).bReleased)
	{
		
		heldObject->IsStationary(false);
		
		heldObject = nullptr;
		

	}

	  if(heldObject != nullptr)
		{
		  
			power.tkMove(*heldObject, player,map);
			power.tkRotation(*heldObject, player, map);
		}
		
	
	  


	int nHorizonHeight = pge->ScreenHeight() * player.fPlayerH + (int)player.fLookUp;
	for (auto& obj : objectlist)
	{
		obj->PrepareRender(player.fPlayerX, player.fPlayerY, player.fPlayerA_deg);
	}

	for (auto& obj : objectlist)
	{
		obj->Render(pge,ddraw, player.fPlayerH, player.fPlayerFoV_rad, map.fMaxDistance, nHorizonHeight);
	}
}

void RC_ObjectManager::wallCollision(RC_Map& map, float deltatime)
{
	for (auto& obj : objectlist)
	{
		if (obj->getStationary() == false) {
			float newX = obj->getPos().x + obj->getVel().x * deltatime;
			float newY = obj->getPos().y + obj->getVel().y * deltatime;
			if (!map.Collides(newX, obj->getPos().y, obj->getRadius(),obj->getRadius(), obj->getVel().x, obj->getVel().y)) {
				obj->setX(newX);
			}
			else {
				obj->SetVX( -obj->getVel().x);
				obj->UpdateObjAngle();
				obj->UpdateObjSpeed();
			}
			if (!map.Collides(obj->getPos().x, newY, obj->getRadius(), obj->getRadius(), obj->getVel().x, obj->getVel().y)) {
				obj->SetY(newY);
			}
			else {
				obj->SetVY(-obj->getVel().y);
				obj->UpdateObjAngle();
				obj->UpdateObjSpeed();
			}
		}
	}
}
