#include "trianglesurface.h"
#include "vertex.h"
#include "math_constants.h"
#include <cmath>
#include <QDebug>
#include <QDirIterator>

TriangleSurface::TriangleSurface() : VisualObject() {
    //    int Offset = 2 ;
    //    int m_Height = 3;
    //    int m_Width = 3;
    //    for (float z = -3; z < m_Width; z+=Offset)
    //    {
    //        for (float x = -4; x < m_Height; x+= Offset)
    //        {
    //            mVertices.push_back({x,0,z,0,1,0});
    //            mVertices.push_back({x+Offset,0,z,0,1,0});
    //            mVertices.push_back({x+Offset,0,z+Offset,0,1,0});

    //            mVertices.push_back({x,0,z,0,1,0});
    //            mVertices.push_back({x+Offset,0,z+Offset,0,1,0});
    //            mVertices.push_back({x,0,z+Offset,0,1,0});
    //        }
    //    }

//    mVertices.push_back({0,0,0,0,1,0});
//    mVertices.push_back({10,5,0,0,1,0});
//    mVertices.push_back({10,0,10,0,1,0});

//    mVertices.push_back({10,0,10,1,0,0});
//    mVertices.push_back({0,5,10,1,0,0});
//    mVertices.push_back({0,0,0,1,0,0});

//    //trekant 1
//    auto pos1 = mVertices[0].mXYZ;
//    auto pos2 = mVertices[1].mXYZ;
//    auto pos3 = mVertices[2].mXYZ;

//    auto normal = gsl::Vector3D::cross(pos2-pos1,pos3-pos1);
//    normal.normalize();

//    mVertices[0].set_normal(normal);
//    mVertices[1].set_normal(normal);
//    mVertices[2].set_normal(normal);


//    pos1 = mVertices[3].mXYZ;
//    pos2 = mVertices[4].mXYZ;
//    pos3 = mVertices[5].mXYZ;

//    normal = gsl::Vector3D::cross(pos2-pos1,pos3-pos1);
//    normal.normalize();

//    mVertices[3].set_normal(normal);
//    mVertices[4].set_normal(normal);
//    mVertices[5].set_normal(normal);

    mMatrix.setToIdentity();
}

TriangleSurface::TriangleSurface(std::string filename) : VisualObject()
{
    readFile(filename);
    calculateNormals();
    mMatrix.setToIdentity();
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

void TriangleSurface::readFile(std::string filename) {
    std::ifstream inn;
    inn.open(filename.c_str());

    if (inn.is_open())
    {
        unsigned long n;
        Vertex vertex;

        inn >> n;
        mVertices.reserve(n);
        for (unsigned int i=0; i<n; i++)
        {
            inn >> vertex;
            mVertices.push_back(vertex);
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
        ut.close();
    }
}

void TriangleSurface::triangulate()
{
    const float tileXsize = mDiffX/(float)mTilesX;
    const float tileZsize = mDiffZ/(float)mTilesZ;
    std::vector<Vertex> tempVertices;

    for(float i = 0; i < mTilesX; i+=1.f)
    {
        for(float j = 0; j < mTilesZ; j+=1.f)
        {
            Vertex tempVertex(-1,0,-1);
            int numOfVertices(0);

            float tileXStart = tileXsize*(i);
            float tileZStart = tileZsize*(j);
            float nextTileXStart = tileXsize*(i+1.f);
            float nextTileZStart = tileZsize*(j+1.f);

            for(auto v : mVertices)
            {
                if(v.mXYZ.x >= tileXStart && v.mXYZ.x < nextTileXStart &&
                   v.mXYZ.z >= tileZStart && v.mXYZ.z < nextTileZStart)
                {
                    tempVertex.mXYZ.y+=v.mXYZ.y;
                    numOfVertices++;
                }
            }
            if(numOfVertices != 0)
            {
                tempVertex.mXYZ.y = tempVertex.mXYZ.y/(float)numOfVertices;
            }

            tempVertex.mXYZ.x = tileXStart/* + ((float)tileXsize/2.f)*/;
            tempVertex.mXYZ.z = tileZStart/* + ((float)tileZsize/2.f)*/;

            tempVertices.push_back(tempVertex);
        }
    }

    mVertices = tempVertices;
    writeFile(terrainFileName);
}

void TriangleSurface::calculateIndices()
{
    mIndices.clear();
    for(int i = 0; i < mTilesZ*mTilesX; ++i)
    {
        if(i == (mTilesX*mTilesZ)-mTilesZ) break;
        if(i > 1 && (i+1) % mTilesX == 0)  continue;

        mIndices.emplace_back(i);
        mIndices.emplace_back(i+mTilesZ+1);
        mIndices.emplace_back(i+1);

        mIndices.emplace_back(i);
        mIndices.emplace_back(i+mTilesZ);
        mIndices.emplace_back(i+mTilesZ+1);
    }
}


void TriangleSurface::construct()
{
    float xmin=0.0f, xmax=1.0f, ymin=0.0f, ymax=1.0f, h=0.25f;
    for (auto x=xmin; x<xmax; x+=h)
    {
        for (auto y=ymin; y<ymax; y+=h)
        {
            float z = sin(gsl::PI*x)*sin(gsl::PI*y);
            mVertices.push_back(Vertex{x,y,z,x,y,z});
            z = sin(gsl::PI*(x+h))*sin(gsl::PI*y);
            mVertices.push_back(Vertex{x+h,y,z,x,y,z});
            z = sin(gsl::PI*x)*sin(gsl::PI*(y+h));
            mVertices.push_back(Vertex{x,y+h,z,x,y,z});
            mVertices.push_back(Vertex{x,y+h,z,x,y,z});
            z = sin(gsl::PI*(x+h))*sin(gsl::PI*y);
            mVertices.push_back(Vertex{x+h,y,z,x,y,z});
            z = sin(gsl::PI*(x+h))*sin(gsl::PI*(y+h));
            mVertices.push_back(Vertex{x+h,y+h,z,x,y,z});
        }
    }
}

void TriangleSurface::calculateNormals()
{
    for (unsigned int i=0; i<mIndices.size(); i+=3)
    {
        auto pos1 = mVertices[mIndices[i+0]].mXYZ;
        auto pos2 = mVertices[mIndices[i+1]].mXYZ;
        auto pos3 = mVertices[mIndices[i+2]].mXYZ;

        auto normal = gsl::Vector3D::cross(pos2-pos1,pos3-pos1);
        normal.normalize();

        mVertices[mIndices[i+0]].set_normal(normal);
        mVertices[mIndices[i+1]].set_normal(normal);
        mVertices[mIndices[i+2]].set_normal(normal);

    }
    std::vector<gsl::Vector3D> tempNormals;
    for (unsigned int i=0; i<mIndices.size(); i+=3)
    {

    }
}
