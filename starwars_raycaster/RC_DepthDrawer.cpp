#include "RC_DepthDrawer.h"

#include <cfloat>

// ==============================/  class RC_DepthDrawer   /==============================

RC_DepthDrawer::RC_DepthDrawer() {}
RC_DepthDrawer::~RC_DepthDrawer() {
    delete fDepthBuffer;
}

void RC_DepthDrawer::Init(olc::PixelGameEngine* gfx) {
    // store pointer to pge to enable calling it's (render) functions
    pgePtr = gfx;
    // Initialize depth buffer
    fDepthBuffer = new float[gfx->ScreenWidth() * gfx->ScreenHeight()];
}

int RC_DepthDrawer::ScreenWidth() { return pgePtr->ScreenWidth(); }
int RC_DepthDrawer::ScreenHeight() { return pgePtr->ScreenHeight(); }

// Variant on Draw() that takes fDepth and the depth buffer into account.
// Pixel col is only drawn if fDepth is less than the depth buffer at that screen location (in which case the depth buffer is updated)
void RC_DepthDrawer::Draw(float fDepth, int x, int y, olc::Pixel col) {
    // prevent out of bounds drawing
    if (x >= 0 && x < pgePtr->ScreenWidth() &&
        y >= 0 && y < pgePtr->ScreenHeight()) {

        if (fDepth <= fDepthBuffer[y * pgePtr->ScreenWidth() + x]) {
            fDepthBuffer[y * pgePtr->ScreenWidth() + x] = fDepth;
            pgePtr->Draw(x, y, col);
        }
    }
}

void RC_DepthDrawer::DecalDraw(float fDepth, olc::vf2d* pos, olc::Decal* decal)
{
    if (pos->x >= 0 && pos->x < pgePtr->ScreenWidth() &&
        pos->y >= 0 && pos->y < pgePtr->ScreenHeight())
    {
        if (fDepth <= fDepthBuffer[(int)pos->y * pgePtr->ScreenWidth() + (int)pos->x])
        {
            fDepthBuffer[(int)pos->y * pgePtr->ScreenWidth() + (int)pos->x] = fDepth;
            
        }
    }
}

// sets all pixels of the depth buffer to absolute max depth value
void RC_DepthDrawer::Reset() {
    for (int i = 0; i < pgePtr->ScreenHeight() * pgePtr->ScreenWidth(); i++) {
        fDepthBuffer[i] = FLT_MAX;
    }
}

void RC_DepthDrawer::DrawWarpedSprite(olc::PixelGameEngine* gfx, olc::Sprite* pSprite, const std::array<olc::vf2d, 4>& cornerPoints, float fdepth)
{
    // The b1 - b3 values can be determined once per quad. The q value is associated per pixel
    auto Get_q = [=](const std::array<olc::vd2d, 4>& cPts, const olc::vd2d& curVert) -> olc::vd2d { return curVert - cPts[0];                     };
    auto Get_b1 = [=](const std::array<olc::vd2d, 4>& cPts) -> olc::vd2d { return cPts[1] - cPts[0];                     };
    auto Get_b2 = [=](const std::array<olc::vd2d, 4>& cPts) -> olc::vd2d { return cPts[2] - cPts[0];                     };
    auto Get_b3 = [=](const std::array<olc::vd2d, 4>& cPts) -> olc::vd2d { return cPts[0] - cPts[1] - cPts[2] + cPts[3]; };

    // note that the corner points are passed in order: ul, ll, lr, ur, but the WarpedSample() algorithm
    // assumes the order ll, lr, ul, ur. This rearrangement is done here
    std::array<olc::vd2d, 4> localCornerPoints;
    localCornerPoints[0] = cornerPoints[1];
    localCornerPoints[1] = cornerPoints[2];
    localCornerPoints[2] = cornerPoints[0];
    localCornerPoints[3] = cornerPoints[3];

    // get b1-b3 values from the quad corner points
    // NOTE: the q value is associated per pixel and is obtained in the nested loop below
    olc::vd2d b1 = Get_b1(localCornerPoints);
    olc::vd2d b2 = Get_b2(localCornerPoints);
    olc::vd2d b3 = Get_b3(localCornerPoints);

    // determine the bounding box around the quad
    olc::vi2d UpperLeft, LowerRight;
    GetQuadBoundingBox(localCornerPoints, UpperLeft, LowerRight);

    // iterate all pixels within the bounding box of the quad...
    for (int y = UpperLeft.y; y <= LowerRight.y; y++) {
        for (int x = UpperLeft.x; x <= LowerRight.x; x++) {
            // ... and render them if sampling produces valid pixel
            olc::Pixel pix2render;
            olc::vd2d q = Get_q(localCornerPoints, { (double)x, (double)y });

            if (WarpedSample(q, b1, b2, b3, pSprite, pix2render)) {
                if(pix2render != olc::MAGENTA)
                    Draw(fdepth,x, y, pix2render);
            }
        }
    }
}

bool RC_DepthDrawer::WarpedSample(olc::vd2d q, olc::vd2d b1, olc::vd2d b2, olc::vd2d b3, olc::Sprite* pSprite, olc::Pixel& colour)
{
    auto wedge_2d = [=](olc::vd2d v, olc::vd2d w) {
        return v.x * w.y - v.y * w.x;
        };

    // Set up quadratic formula
    double A = wedge_2d(b2, b3);
    double B = wedge_2d(b3, q) - wedge_2d(b1, b2);
    double C = wedge_2d(b1, q);

    // Solve for v
    olc::vd2d uv = { 0.0, 0.0 };
    if (fabs(A) < NEAR_ZERO) {
        // Linear form
        if (fabs(B) < NEAR_ZERO) {
            return false;
        }
        uv.y = -C / B;
    }
    else {
        // Quadratic form: Take positive root for CCW winding with V-up
        double D = B * B - 4 * A * C;
        if (D <= 0.0) {         // if discriminant <= 0, then the point is not inside the quad
            return false;
        }
        uv.y = 0.5 * (-B + sqrt(D)) / A;
    }
    // Solve for u, using largest magnitude component
    olc::vd2d denom = b1 + b3 * uv.y;
    if (fabs(denom.x) > fabs(denom.y)) {
        if (fabs(denom.x) < NEAR_ZERO) {
            return false;
        }
        uv.x = (q.x - b2.x * uv.y) / denom.x;
    }
    else {
        if (fabs(denom.y) < NEAR_ZERO) {
            return false;
        }
        uv.x = (q.y - b2.y * uv.y) / denom.y;
    }
    // Note that vertical texel coord is mirrored because the algorithm assumes positive y to go up
    colour = pSprite->Sample(uv.x, 1.0 - uv.y);

    // return whether sampling produced a valid pixel
    return (uv.x >= 0.0 && uv.x <= 1.0 &&
        uv.y >= 0.0 && uv.y <= 1.0);
}

void RC_DepthDrawer::GetQuadBoundingBox(std::array<olc::vd2d, 4> points, olc::vi2d& UpLeft, olc::vi2d& LwRght)
{
    UpLeft = { INT_MAX, INT_MAX };
    LwRght = { INT_MIN, INT_MIN };
    for (int i = 0; i < 4; i++) {
        UpLeft = UpLeft.min(points[i]);
        LwRght = LwRght.max(points[i]);
    }
}
