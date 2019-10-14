#include "rollingball.h"

RollingBall::RollingBall() : Sphere(3)
{

}

void RollingBall::move(VisualObject* plane)
{
    normal = gsl::Vector3D(0);
    calculateBarycentricCoordinates(plane);
    changeVelocity();
    mMatrix.translate(velocity);
}

void RollingBall::calculateBarycentricCoordinates(VisualObject* plane)
{
    bool isInTriangle = false;

    for (unsigned int i=0; i<plane->mIndices.size(); i+=3)
    {
        gsl::Vector3D pos1;
        gsl::Vector3D pos2;
        gsl::Vector3D pos3;
        pos1 = plane->mVertices[plane->mIndices[i+0]].mXYZ;
        pos2 = plane->mVertices[plane->mIndices[i+1]].mXYZ;
        pos3 = plane->mVertices[plane->mIndices[i+2]].mXYZ;

        gsl::Vector2D temp = gsl::Vector2D(mMatrix.getPosition().x, mMatrix.getPosition().z);
        gsl::Vector3D bar = temp.barycentricCoordinates(gsl::Vector2D(pos1.x,pos1.z),gsl::Vector2D(pos2.x, pos2.z), gsl::Vector2D(pos3.x,pos3.z));

        if(bar.x>=0 && bar.x<=1 && bar.y>=0 && bar.y<=1 && bar.z>=0 && bar.z <=1)
        {
            isInTriangle = true;
            playerTempPos = (pos1*bar.x + pos2*bar.y + pos3*bar.z);
            normal = gsl::Vector3D::cross(pos3 - pos1,pos2 - pos1);
            normal.normalize();
        }
    }
}

void RollingBall::changeVelocity()
{
    gsl::Vector3D N;
    gsl::Vector3D  vectorToBall =  (mMatrix.getPosition()-playerTempPos);
    float distanceToBall = gsl::Vector3D::dot(vectorToBall,normal);

    if(distanceToBall>radius)
    {
        normal = gsl::Vector3D(0);
        N = gsl::Vector3D(0);
    }
    else
    {
        N = normal* gsl::Vector3D::dot(-gravity, normal);
        float distance = radius - distanceToBall;
        if(distance >0.5f)
        {
            mMatrix.translate(normal*distance);
        }
    }
    if(normal != prevTriangleNormal)
    {
        //qDebug() << "Same Normals!";
        if(normal == gsl::Vector3D(0)) //går til lufta
        {
        }
        else if(prevTriangleNormal== gsl::Vector3D(0))//kommer fra lufta
        {
            velocity = (gravity+N).normalized() * gsl::Vector3D::dot(velocity, (gravity+N).normalized());
        }
        else    //bytter trekant
        {
             gsl::Vector3D tempNormal = normal + prevTriangleNormal;
             tempNormal.normalize();
             gsl::Vector3D tempVel = tempNormal*gsl::Vector3D::dot(velocity,tempNormal);
             tempVel= velocity - tempVel*2;
             velocity = tempVel;
        }
    }
    prevTriangleNormal = normal;
    acceleration = (N+gravity);
    velocity+=acceleration*speed;
}
