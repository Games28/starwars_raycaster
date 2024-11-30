#ifndef RC_DEPTHDRAWER_H
#define RC_DEPTHDRAWER_H

#include "olcPixelGameEngine.h"

//////////////////////////////////  RC_DepthDrawer   //////////////////////////////////////////

/* I need a uniform way to draw to screen using the PGE, and incorporating a shared depth buffer (2D).
 * This class implements that functionality.
 */

 // ==============================/  class RC_DepthDrawer   /==============================
#define NEAR_ZERO   0.000001 
class RC_DepthDrawer {
private:
    // the 2D depth buffer
    float* fDepthBuffer = nullptr;
    olc::PixelGameEngine* pgePtr = nullptr;

public:
    RC_DepthDrawer();

    ~RC_DepthDrawer();

    void Init(olc::PixelGameEngine* gfx);

    int ScreenWidth();
    int ScreenHeight();

    // Variant on Draw() that takes fDepth and the depth buffer into account.
    // Pixel col is only drawn if fDepth is less than the depth buffer at that screen location (in which case the depth buffer is updated)
    void Draw(float fDepth, int x, int y, olc::Pixel col);
    void DecalDraw(float fDepth, olc::vf2d* pos, olc::Decal* decal);

    // sets all pixels of the depth buffer to absolute max depth value
    void Reset();


    //test wrapped sprite
    void DrawWarpedSprite(olc::PixelGameEngine* gfx, olc::Sprite* pSprite, const std::array<olc::vf2d, 4>& cornerPoints, float fdepth);
    bool WarpedSample(olc::vd2d q, olc::vd2d b1, olc::vd2d b2, olc::vd2d b3, olc::Sprite* pSprite, olc::Pixel& colour);
    void GetQuadBoundingBox(std::array<olc::vd2d, 4> points, olc::vi2d& UpLeft, olc::vi2d& LwRght);
};


#endif // RC_DEPTHDRAWER_H

