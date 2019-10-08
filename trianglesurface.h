#ifndef TRIANGLESURFACE_H
#define TRIANGLESURFACE_H

#include "visualobject.h"

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
private:

    void calculateNormals();
    std::vector<int> mNeighbor;
    void triangulate();

    float mDiffX;
    float mDiffY;
    float mDiffZ;

    const int mTilesX = 100;
    const int mTilesZ = 100;

    const std::string terrainFileName = "terrainData.txt";
    void calculateIndices();
};

#endif //TRIANGLESURFACE_H
