#include "trianglesurface.h"
#include "vertex.h"
#include "math_constants.h"
#include <cmath>
#include <QDebug>
#include <QDirIterator>
#include <iostream>
#include <sstream>
#include <fstream>
#include <string>

#include "triangle.h"

TriangleSurface::TriangleSurface() : VisualObject()
{

    mMatrix.setToIdentity();
}

TriangleSurface::TriangleSurface(std::string filename) : VisualObject()
{
    mMatrix.setToIdentity();
    readExamFile(filename);
    triangulate();
    makeNormals();

    writeFile("oppgave5Save.txt");
}

TriangleSurface::~TriangleSurface()
{
}



void TriangleSurface::init()
{
    //must call this to use OpenGL functions
    initializeOpenGLFunctions();

    //Vertex Array Object - VAO
    glGenVertexArrays( 1, &mVAO );
    glBindVertexArray( mVAO );

    glGenBuffers(1, &mEAB);
    //Vertex Buffer Object to hold vertices - VBO
    glGenBuffers( 1, &mVBO );
    glBindBuffer( GL_ARRAY_BUFFER, mVBO );

    glBufferData( GL_ARRAY_BUFFER, mVertices.size()*sizeof(Vertex), mVertices.data(), GL_STATIC_DRAW );


    if(!mIndices.empty())
    {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mEAB);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, static_cast<GLuint>(mIndices.size()) * sizeof(unsigned int), &mIndices[0], GL_STATIC_DRAW);
    }


    glBindBuffer(GL_ARRAY_BUFFER, mVBO);
    glVertexAttribPointer(0, 3, GL_FLOAT,GL_FALSE,sizeof(Vertex), (GLvoid*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE,  sizeof(Vertex), (GLvoid*)(3 * sizeof(GLfloat)) );
    glEnableVertexAttribArray(1);

    // 3rd attribute buffer : uvs
    glVertexAttribPointer(2, 2,  GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)( 6 * sizeof(GLfloat)) );
    glEnableVertexAttribArray(2);

    glBindVertexArray(0);
}

void TriangleSurface::draw()
{
    glBindVertexArray( mVAO );

    if(!mIndices.empty())
    {
        glDrawElements(GL_TRIANGLES,static_cast<GLsizei>(mIndices.size()),GL_UNSIGNED_INT, nullptr);
    }
    else
    {
        glPointSize(10.f);
        glDrawArrays(GL_POINTS, 0, mVertices.size());

    }

}

void TriangleSurface::readExamFile(std::string filename)
{
    std::ifstream inn;
    inn.open(filename.c_str());

    if (inn.is_open())
    {
        unsigned long n;
        Vertex vertex;

        inn >> n;
        //mVertices.reserve(n);
        for (unsigned int i=0; i<n; i++)
        {
            inn >> vertex;
            if(xMin <= vertex.mXYZ.x && vertex.mXYZ.x <= xMax && yMin <= vertex.mXYZ.y  && vertex.mXYZ.y <=yMax)
            {
//                vertex.mXYZ.x-=xMin;
//                vertex.mXYZ.y-=yMin;
                container.push_back(vertex);
            }
        }

        inn.close();
    }
    else
    {
        qDebug() << "Error: " << filename.c_str() << " could not be opened!";
    }

    mDiffX = xMax - xMin;
    mDiffY = yMax - yMin;
}

void TriangleSurface::readFile(std::string filename) {
    std::ifstream inn;
    inn.open(filename.c_str());

    if (inn.is_open())
    {
        unsigned long n;
        Vertex vertex;

        inn >> n;
        //mVertices.reserve(n);
        for (unsigned int i=0; i<n; i++)
        {
            inn >> vertex;
            if(614775.0f <= vertex.mXYZ.x && vertex.mXYZ.x <= 614800.0f &&
                    6758580.0f <= vertex.mXYZ.y  && vertex.mXYZ.y <=6758605.0f)
            container.push_back(vertex);
        }
        inn >> n;
        for(unsigned int i = 0; i < n; ++i)
        {
            int s;
            for (unsigned int j = 0; j < 3; ++j) {
                inn >> s;
                mIndices.push_back(static_cast<unsigned int>(s));
            }
            for (unsigned int j = 0; j < 3; ++j) {
                inn >> s;
                mNeighbor.push_back(static_cast<int>(s));
            }
        }
        inn.close();
    }
    else
    {
        qDebug() << "Error: " << filename.c_str() << " could not be opened!";
    }

}


void TriangleSurface::readTxtFiles(std::string directory)
{
    QFile file(QString::fromStdString(terrainFileName));
    if(!file.exists())
    {
        double minX = 999999999999;
        double maxX = 0;
        double minY = 999999999999;
        double maxY = 0;
        double minZ = 999999999999;
        double maxZ = 0;
        QDirIterator dirIterator(QString::fromStdString(directory), QDir::AllEntries | QDir::NoDotAndDotDot);
        while(dirIterator.hasNext())
        {
            auto path = dirIterator.next();
            QFile file(path);
            file.open(QIODevice::ReadWrite | QIODevice::Text);
            QTextStream stream(&file);
            stream.setRealNumberPrecision(16);
            while(!stream.atEnd())
            {
                QString line = stream.readLine();
                auto split = line.split(" ");

                double x = split[0].toDouble();
                double y = split[2].toDouble();
                double z = split[1].toDouble();

                if(x < minX)
                {
                    minX = x;
                }
                else if(x > maxX)
                {
                    maxX = x;
                }

                if(y < minY)
                {
                    minY = y;
                }
                else if(y > maxY)
                {
                    maxY = y;
                }

                if(z < minZ)
                {
                    minZ = z;
                }
                else if(z > maxZ)
                {
                    maxZ = z;
                }
            }
            file.close();
        }

        mDiffX = maxX - minX;
        mDiffY = maxY - minY;
        mDiffZ = maxZ - minZ;

        QDirIterator dirIterator2(QString::fromStdString(directory), QDir::AllEntries | QDir::NoDotAndDotDot);
        while(dirIterator2.hasNext())
        {
            auto path = dirIterator2.next();
            QFile file(path);
            file.open(QIODevice::ReadWrite | QIODevice::Text);
            QTextStream stream(&file);
            stream.setRealNumberPrecision(16);
            while(!stream.atEnd())
            {
                QString line = stream.readLine();
                auto split = line.split(" ");

                float x = split[0].toFloat();
                float y = split[2].toFloat();
                float z = split[1].toFloat();

                const int scale = 1;
                auto vertexX = ((x - minX)) * scale;
                auto vertexY = ((y - minY)) * scale;
                auto vertexZ = ((z - minZ)) * scale;

                Vertex vertex;
                vertex.set_xyz(vertexX, vertexY, vertexZ);
                mVertices.push_back(vertex);
            }
            file.close();
        }
        triangulate();
    }
    else
    {
        readFile(terrainFileName);
    }

    calculateIndices();
    calculateNormals();
}


void TriangleSurface::writeFile(std::string filename)
{
    std::ofstream ut;
    ut.open(filename.c_str());

    if (ut.is_open())
    {
        auto n = mVertices.size();
        Vertex vertex;
        ut << n << std::endl;
        for (auto it=mVertices.begin(); it != mVertices.end(); it++)
        {
            vertex = *it;
            ut << vertex << std::endl;
        }
        n = mTriangles.size();
        ut << n << std::endl;
        for (auto t : mTriangles)
        {
            ut << t << std::endl;
        }

        ut.close();
    }
}

void TriangleSurface::triangulate()
{
    const float tileXsize = mDiffX/(float)(mTilesX+1);
    const float tileZsize = mDiffY/(float)(mTilesY+1);
    std::vector<Vertex> nodes;

    for(float i = 0; i < (mTilesX+1); i+=1.f)
    {
        for(float j = 0; j < (mTilesY+1); j+=1.f)
        {
            Vertex tempVertex(-1,-1,-1);
            int numOfVertices(0);

            float tileXStart = xMin + tileXsize*(i);
            float tileZStart = yMin + tileZsize*(j);
            float nextTileXStart = xMin + tileXsize*(i+1.f);
            float nextTileZStart = yMin + tileZsize*(j+1.f);

            for(auto v : container)
            {
                if(v.mXYZ.x >= tileXStart && v.mXYZ.x < nextTileXStart &&
                   v.mXYZ.y >= tileZStart && v.mXYZ.y < nextTileZStart)
                {
                    tempVertex.mXYZ.z+=v.mXYZ.z;
                    numOfVertices++;
                }
            }
            if(numOfVertices != 0)
            {
                tempVertex.mXYZ.z = tempVertex.mXYZ.z/(float)numOfVertices;
            }

            tempVertex.mXYZ.x = tileXStart;
            tempVertex.mXYZ.y = tileZStart;

            nodes.push_back(tempVertex);
        }
    }

    for(int i=0; i<nodes.size(); i++)
    {
        if(nodes[i].mXYZ.z ==-1) //ble ikke tildelt noen verdi)
        {
            float avgZ = 0;
            float num = 0;
            if(i-(mTilesX+1) >= 0 )
            {
                num++;
                avgZ += nodes[i-(mTilesX+1)].mXYZ.z;
            }
            if(i+(mTilesX+1) < nodes.size() )
            {
                num++;
                avgZ += nodes[i+(mTilesX+1)].mXYZ.z;
            }

            if(i-1 >= 0 )
            {
                num++;
                avgZ += nodes[i-1].mXYZ.z;
            }
            if(i+1 < nodes.size() )
            {
                num++;
                avgZ += nodes[i+1].mXYZ.z;
            }
            nodes[i].mXYZ.z = avgZ/num;



        }
    }


    mVertices = nodes;
    calculateIndices();
    generateTriangles();
    //writeFile(terrainFileName);
}

void TriangleSurface::generateTriangles()
{
    for(unsigned i=0; i<mIndices.size(); i+=3)
    {
        mTriangles.push_back(Triangle(mIndices[i], mIndices[i+1], mIndices[i+2]));
    }

}

void TriangleSurface::calculateIndices()
{
    mIndices.clear();

    for(int i = 0; i < (mTilesY+1)*(mTilesX+1); ++i)
    {
        if(i == ((mTilesX+1)*(mTilesY+1))-(mTilesY+1)) break;
        if(i > 1 && (i+1) % (mTilesX+1) == 0)  continue;

        mIndices.emplace_back(i);
        mIndices.emplace_back(i+(mTilesY+1)+1);
        mIndices.emplace_back(i+1);

        mIndices.emplace_back(i);
        mIndices.emplace_back(i+(mTilesY+1));
        mIndices.emplace_back(i+(mTilesY+1)+1);
    }
}


//this function is not entirely accurate
void TriangleSurface::calculateNormals()
{
    for (unsigned int i=0; i<mIndices.size(); i+=3)
    {
        auto pos1 = mVertices[mIndices[i+0]].mXYZ;
        auto pos2 = mVertices[mIndices[i+1]].mXYZ;
        auto pos3 = mVertices[mIndices[i+2]].mXYZ;

        auto normal = gsl::Vector3D::cross(pos3-pos1,pos2-pos1);
        normal.normalize();

        mVertices[mIndices[i+0]].set_normal(normal);
        mVertices[mIndices[i+1]].set_normal(normal);
        mVertices[mIndices[i+2]].set_normal(normal);

    }
}

void TriangleSurface::makeNormals()
{

    for (int i=0;i<mVertices.size(); i++)
    {
        gsl::Vector3D normal = gsl::Vector3D(0,0,0);
        gsl::Vector3D p1 = mVertices[i].mXYZ;
        gsl::Vector3D p2, p3;
        //trekant 1
        if(((i%(mTilesX+1))-1)>=0 && i >= (mTilesX+1))
        {
            p2 = mVertices[i-1].mXYZ - p1 ;
            p3 = mVertices[i-(mTilesX+1)].mXYZ -p1;
            normal+= p2^p3;
        }
        else
        {
            normal += gsl::Vector3D(0,0,1);
        }
        //trekant 2
        if(((i%(mTilesX+1))-1)>=0 && i >= (mTilesX+1))
        {
            p2 = mVertices[i-(mTilesX+1)].mXYZ -p1;
            p3 = mVertices[i-(mTilesX+1)+1].mXYZ -p1;
            normal+= p2^p3;
        }
        else
        {
            normal += gsl::Vector3D(0,0,1);
        }
        //trekant 3
        if(((i%(mTilesX+1))+1)<(mTilesX+1) && i >= (mTilesX+1))
        {
            p2 = mVertices[i-(mTilesX+1)+1].mXYZ -p1;
            p3 = mVertices[i+1].mXYZ -p1;
            normal+= p2^p3;
        }
        else
        {
            normal += gsl::Vector3D(0,0,1);
        }
        //trekant 4
        if(((i%(mTilesX+1))+1)<(mTilesX+1)&& i <mVertices.size()-(mTilesX+1))
        {
            p2 = mVertices[i+1].mXYZ -p1;
            p3 = mVertices[i+(mTilesX+1)].mXYZ -p1;
            normal+= p2^p3;
        }
        else
        {
            normal += gsl::Vector3D(0,0,1);
        }
        //trekant 5
        if(((i%(mTilesX+1))+1)<(mTilesX+1)&& i < mVertices.size()-(mTilesX+1))
        {
            p2 = mVertices[i+(mTilesX+1)].mXYZ -p1;
            p3 = mVertices[i+(mTilesX+1)-1].mXYZ -p1;
            normal+= p2^p3;
        }
        else
        {
            normal += gsl::Vector3D(0,0,1);
        }
        //trekant 6
        if(((i%(mTilesX+1))-1)<(mTilesX+1) && i < mVertices.size()-(mTilesX+1))
        {
            p2 = mVertices[i+(mTilesX+1)-1].mXYZ -p1;
            p3 = mVertices[i-1].mXYZ -p1;
            normal+= p2^p3;
        }
        else
        {
            normal += gsl::Vector3D(0,0,1);
        }
        normal.normalize();
        mVertices[i].set_normal(normal.x,normal.y,normal.z);
    }
}
