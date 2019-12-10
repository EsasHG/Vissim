#ifndef TRIANGLESURFACE_H
#define TRIANGLESURFACE_H

#include "visualobject.h"

class Triangle;
class TriangleSurface : public VisualObject
{
public:
    TriangleSurface();
    TriangleSurface(std::string filename);
    ~TriangleSurface() override;

    virtual void init() override;
    virtual void draw() override;

    void readFile(std::string filename);
    void writeFile(std::string filename);
    void construct();

    void readTxtFiles(std::string directory);

    std::pair<std::vector<Vertex>, std::vector<unsigned int> > readOBJFile(std::string filename);

    void readExamFile(std::string filename);
    void makeNormals();
    void generateTriangles();
private:

    void calculateNormals();
    std::vector<int> mNeighbor;
    void triangulate();

    float mDiffX;
    float mDiffY;
    float mDiffZ;

    float xMin = 614775.0f;
    float xMax = 614800.0f;
    float yMin = 6758580.0f;
    float yMax = 6758605.0f;

    const int mTilesX = 4;
    const int mTilesY = 4;

    const std::string terrainFileName = "terrainData.txt";
    void calculateIndices();

    std::vector<Triangle> mTriangles;
    std::vector<Vertex> mNodes;
    std::vector<Vertex> container;
};

#endif //TRIANGLESURFACE_H
