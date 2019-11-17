#include "npc.h"
#include "bsplinecurve.h"
#include "trianglesurface.h"
//NPC::NPC(BSplineCurve& bsplinecurve, BSplineCurve& bsplinecurve2, int m, gsl::Vector3D color)
//    : bSplineCurve(bSplineCurve)
//{

//}
NPC::NPC(std::vector<TriangleSurface*> inItems,  gsl::Vector3D inStartPos, gsl::Vector3D inEndPos)
    :Sphere(2), mStartPos(inStartPos), mEndPos(inEndPos), mItems(inItems)
{
    std::vector<gsl::Vector3D> cPoints;
    cPoints.push_back(mStartPos);
    for(auto& i : mItems)
        cPoints.push_back(i->mMatrix.getPosition());
    cPoints.push_back(mEndPos);

    bSplineCurve = new BSplineCurve(cPoints);
    bSplineCurve->init();
    numOfItems = bSplineCurve->getNumberOfControlPoints();
}

void NPC::notify(NPCevents event)
{
    if(event == NPCevents::ENDPOINT_ARRIVED)
    {
        bEndpointArrived=true;
        mState = NPCstates::LEARN;
    }
    if(event == NPCevents::ITEM_TAKEN)
    {
        bItemTaken = true;
    }
}

void NPC::buildNewPath()
{
    std::vector<gsl::Vector3D> cPoints;
    cPoints.push_back(mStartPos);
    for(auto& i : mItems)
    {
        if(i)
            cPoints.push_back(i->mMatrix.getPosition());
    }
    cPoints.push_back(mEndPos);
    bSplineCurve->mControlPoints =cPoints;
    bSplineCurve->init();
}

void NPC::patrol()
{
    t+=speed;
    mMatrix.translate(bSplineCurve->evaluateBSpline(bSplineCurve->findKnotInterval(t),(t))-mMatrix.getPosition());
    if (t<=0.000001f || t>0.999999f)
    {
        notify(NPCevents::ENDPOINT_ARRIVED);
    }

    for(int i=0; i<mItems.size(); i++)
    {
        if (!mItems[i])
        {
            qDebug() << "Item Taken!";
            notify(NPCevents::ITEM_TAKEN);
            //TODO: generate new spline
        }
    }
    auto tempItems = mItems;
    mItems.clear();
    for (auto& i : tempItems)
    {
        if(i)
            mItems.push_back(i);
    }
//    if (player detected)
//    notify(player_position)
//    mState = NPCstates::LEARN
//    else if (obstacle detected)
//    notify(obstacle_position, control_point_index)
//    mState = NPCstates::LEARN
//    else
//    {
//        //previous_position = position

//    }
}

void NPC::learn()
{
//    if (item collected)
//    remove controlpoint and one (the middle) internal knot
//    if (player_detected)
//    build chase path by Chasing path algorithm
//    npc_state = CHASE
//    if (obstacle_detected)
//    mark edge unavailable
//    build obstacle path by Obstacle path algorithm
//    npc_state = CHASE
    if(bItemTaken)
    {
        buildNewPath();
        mState = NPCstates::PATROL;
        bItemTaken = false;
    }

    if (bEndpointArrived)
    {
        speed=-speed;
        mState = NPCstates::PATROL;
        bEndpointArrived = false;
        std::random_shuffle(bSplineCurve->mControlPoints.begin()+1, bSplineCurve->mControlPoints.end()-1);
        bSplineCurve->init();
    }
}

void NPC::update(float t)
{
    if (mState == NPCstates::PATROL)
        patrol();

    if (mState == NPCstates::LEARN)
        learn();
}
