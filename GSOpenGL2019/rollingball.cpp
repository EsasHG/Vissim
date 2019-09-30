#include "rollingball.h"

RollingBall::RollingBall() : Sphere(3)
{

}

void RollingBall::move(VisualObject* plane)
{

    CalculateBarycentricCoordinates(plane);
    if(inputVector.length() >= 1)
    {
        inputVector.normalize();
        //Acceleration+=inputVector;
    }
    else
    //Velocity=gsl::Vector3D(0);
    //Velocity = gsl::Vector3D(0,0,0);
    mMatrix.translate(Velocity);
}

bool RollingBall::CalculateBarycentricCoordinates(VisualObject* plane)
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

            auto normal = gsl::Vector3D::cross(pos3 - pos1,pos2 - pos1);
            normal.normalize();
            CurrentTriangleNormal = normal;

            gsl::Vector3D playerTempPos = (pos1*bar.x + pos2*bar.y + pos3*bar.z); //1 = radius
            gsl::Vector3D  vectorToBall =  (mMatrix.getPosition()-playerTempPos);
            float distanceToBall = gsl::Vector3D::dot(vectorToBall,CurrentTriangleNormal);
            gsl::Vector3D N;
            //the formula is actually N = |G| * n * cos a
            //by taking dot(-G*n), we save time and get |G|* cos a in one swoop.
            //As gravity will always be the exact opposite direction of the xz-plane's normal vector,
            //this shouldnt be a problem
            N = CurrentTriangleNormal* gsl::Vector3D::dot(-Gravity, CurrentTriangleNormal);
            if(distanceToBall <= radius)
            {

                if(isFirstCollision)
                {
                    qDebug() << "Player collided with triangle";

                    Velocity = (Gravity+N).normalized() * gsl::Vector3D::dot(Velocity, (Gravity+N).normalized());
                    Acceleration = gsl::Vector3D(0);
                    isFirstCollision = false;
                }
                //Velocity= gsl::Vector3D(0);
                Acceleration = N+Gravity;
            }
            else
            {
                qDebug() << "No Collision";
                Acceleration = (Gravity);
            }
            Velocity+=Acceleration*speed;
            //LastLocation = gsl::Vector3D(mMatrix.getPosition().x,playerTempPos,mMatrix.getPosition().z);


            qDebug() << Acceleration << Velocity.normalized() << (Gravity+N).normalized() << CurrentTriangleNormal << gsl::Vector3D::dot(Gravity, CurrentTriangleNormal);
        }
    }
    //mMatrix.setPosition(LastLocation);
    return  isInTriangle;
}
