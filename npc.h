#ifndef NPC_H
#define NPC_H
#include <queue>
#include "sphere.h"

class BSplineCurve;
class TriangleSurface;

enum class NPCstates {SLEEP, PATROL, LEARN, CHASE};
enum class NPCevents {ENDPOINT_ARRIVED, ITEM_TAKEN, OBSTACLE_DETECTED, PLAYER_DETECTED, CHASE_ENDPOINT_ARRIVED};

class NPC : public Sphere
{
public:
    //NPC(BSplineCurve& bsplinecurve, BSplineCurve& bsplinecurve2, int m=3, gsl::Vector3D color=gsl::Vector3D(1.0, 0.0, 0.0));
    NPC(std::vector<TriangleSurface*> inItems,  gsl::Vector3D inStartPos = {0,0,0}, gsl::Vector3D inEndPos= {0,0,0});

    void patrol();
    void learn();
    void update(float t=0.05f); //t ikke brukt i funksjonen
    void initVertexBufferObjects();
    BSplineCurve* bSplineCurve;
    std::vector<TriangleSurface*> mItems;
private:

    void notify(NPCevents event);
    NPCstates mState = NPCstates::PATROL;
//    std::vector<float> knots;
    bool bEndpointArrived = false;
    bool bItemTaken = false;
    int numOfItems;
    void buildNewPath();
    float t=0.f;
    float speed = 0.01f;
    gsl::Vector3D mStartPos;
    gsl::Vector3D mEndPos;
};

#endif // NPC_H
