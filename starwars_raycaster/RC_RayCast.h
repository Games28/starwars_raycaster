#ifndef RC_RAYCAST_H
#define RC_RAYCAST_H
#include "Defs.h"
#include "olcPixelGameEngine.h"
#include "RC_Face.h"
#include "Player.h"
#include "RC_Map.h"
#include "RC_Misc.h"
#include "RC_DepthDrawer.h"
#include <algorithm>





typedef struct sIntersectInfo {
    float fHitX,         // world space
        fHitY;
    int   nHitX,        // tile space
        nHitY;
    float fDistFrnt,     // distances to front and back faces of hit map cell
        fDistBack;
    float fHeight;       // height within the layer
    int   nLayer = -1;   // nLayer == 0 --> ground layer

    // these are on screen projected values (y coordinate in pixel space)
    int osp_bot_frnt = -1;    // on screen projected bottom  of wall slice
    int osp_bot_back = -1;    //                     bottom  of wall at back
    int osp_top_frnt = -1;    //                     ceiling
    int osp_top_back = -1;    //                     ceiling of wall at back

    int nFaceHit = FACE_UNKNOWN;     // which face was hit?
    bool bHorizHit;                  // was the hit on a horizontal grid line?
} IntersectInfo;

class Raycast
{
public:
    Raycast() = default;
    ~Raycast() = default;

    int clamp(int value, int min, int max);
    bool CastRayPerLevelAndAngle(RC_Map& map, Player& player, float fPx, float fPy, int layer, float fRayAngle_deg, std::vector<IntersectInfo>& vHitList);

    void CalculateBlockProjections(Player& player, RC_Map* map, float fCorrectedDistToWall, int nHorHeight, int nLayerHeight, float fWallHeight, int& nOspTop, int& nOspBottom);

    void raycaster(olc::PixelGameEngine* pge,RC_DepthDrawer& ddraw, Player& player, RC_Map* map);

    olc::Pixel ShadePixel(const olc::Pixel& p, float fDistance);

public:
    float fObjectIntensity = MULTI_LAYERS ? OBJECT_INTENSITY : 0.2f;
    float fIntensityMultiplier = MULTI_LAYERS ? MULTIPLIER_INTENSITY : 10.0f;
    float fAnglePerPixel_deg;

    std::vector<olc::Sprite*> vWallSprites;
    std::vector<olc::Sprite*> vCeilSprites;
    std::vector<olc::Sprite*> vRoofSprites;
    std::vector<olc::Sprite*> vFlorSprites;
    std::vector<olc::Sprite*> vObjtSprites;
};
#endif // !RC_RAYCAST_H


