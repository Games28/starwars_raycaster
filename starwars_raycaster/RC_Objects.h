 #ifndef RC_OBJECT_H
#define RC_OBJECT_H

#include "RC_DepthDrawer.h"
#include "RC_Map.h"
#include "RC_Misc.h"


// constants for collision detection with walls

#define RADIUS_ELF      0.2f

// test objects
#define TEST_OBJ_PERCENTAGE   0.02f    // this percent of *empty* tiles will be used as the nr of test objects
#define MIN_DYNAMIC_OBJS      2        // the first x objects will be dynamic objects

//////////////////////////////////  RC_Object   //////////////////////////////////////////

/* Besides background scenery (walls, floor, roof and ceilings), the game experience is built up using objects.
 * They can be stationary or moving around. These objects are modeled by the RC_Object class.
 */

 // ==============================/  class RC_Object   /==============================

 // used to be definition of object record
class RC_Object {

private:
    olc::vf2d position;   
    olc::vf2d velocity;
    olc::vf2d acceleration;
    float fradius;
    float fObjSpeed;        // position in the map
    float scale;            // 1.0f is 100%

    float fObjAngle_rad;
    bool billboarding;
    bool isStationary;
 
    bool isAnimated;

    float fDistToPlayer,
        fAngleToPlayer;  // w.r.t. player

    olc::Sprite* sprite = nullptr;
    olc::Decal* decal = nullptr;
  
    bool ispickedup = false;

public:
    RC_Object();
    RC_Object(float fX, float fY, float fS, float fD, float fA, olc::Sprite* pS);
    ~RC_Object();

    olc::vf2d getPos();
    void setPos(olc::vf2d pos);
    void setX(float x);
    void SetY(float y);
    void SetVX(float vx);
    void SetVY(float vy);

    void setPosOffset(olc::vf2d pos);
    void setRadius(float r);
    float getRadius();

    olc::vf2d getVel();
    void Setvel(olc::vf2d vel);


    void IsStationary(bool stationary);
    bool getStationary();
    void IsAnimated(bool animate);
    void IsBillboarded(bool board);

    void SetScale(float fS);
    void SetDistToPlayer(float fD);
    void SetAngleToPlayer(float fA);

    float GetScale();
    float GetDistToPlayer();
    float GetAngleToPlayer();

    void setispickedup(bool picked);
    bool getispickedup();


    void SetSprite(olc::Sprite* pS);
    void SetDecal();
    olc::Sprite* GetSprite();

   
    float GetAngle();   // in radians!!
    float GetSpeed();


   void UpdateObjAngle();
   void UpdateObjAngle(float vx, float vy);
   void UpdateObjSpeed();
   
   

    void Print();

    // work out distance and angle between object and player, and
    // store it in the object itself
    void PrepareRender(float fPx, float fPy, float fPa_deg);
    void Render(olc::PixelGameEngine* pge, RC_DepthDrawer& ddrwr, float fPh, float fFOV_rad, float fMaxDist, int nHorHeight);

public:
    

private:
  
};

#endif // RC_OBJECT_H

