#ifndef RC_OBJECTMANAGER_H
#define RC_OBJECTMANAGER_H
#include "olcPixelGameEngine.h"
//#include "RC_Objects.h"
#include "RC_Object_blueprints.h"
#include "RC_Map.h"
//#include "Player.h"
#include "Powers.h"
#include "RC_DepthDrawer.h"



class RC_ObjectManager
{
public:
	RC_ObjectManager();
	~RC_ObjectManager();

	void initailize(RC_Map& map);
    void Update(olc::PixelGameEngine *pge,Player& player, RC_Map& map,RC_DepthDrawer& ddraw,float deltatime);

    void wallCollision(RC_Map& map, float deltatime);

private:
    Powers power;
	std::vector<RC_Object*> objectlist;
    RC_Object* heldObject = nullptr;

    std::vector<std::string> objectmap =
    {
         "######%%%%%%%%.."
         "#....#.........#"
         "#....#.........#"
         "######.....r...."
         "#............r.."
         "#........#......"
         "#..............#"
         "#..........p...#"
         "#.....t......###"
         "#.#.........##.#"
         "#..........##..#"
         "#.....######...#"
         "#.....#........#"
         "#.....#........#"
         "......#........#"
         ".###.#####*####."
    };
};
#endif // !RC_OBJECTMANGER_H



