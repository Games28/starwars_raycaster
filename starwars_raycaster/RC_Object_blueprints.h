#ifndef RC_OBJECT_BLUEPRINTS_H
#define RC_OBJECT_BLUEPRINTS_H
#include <string>
#include <vector>
#include <map>
//#include "Mapeditor.h"

typedef struct Objectblueprintstruct
{
	char cID;
	float scale;
	float radius;
	std::string location;
	bool isStationary;
	bool isbillboarded;
	bool isAnimated;
	bool hasPhysics;

}ObjectBluePrints;

extern std::vector<ObjectBluePrints> vinitobjectblueprints;

#endif // !RC_OBJECT_BLUEPRINTS_H

