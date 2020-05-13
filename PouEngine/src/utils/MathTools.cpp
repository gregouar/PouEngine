#include "PouEngine/utils/MathTools.h"

#include "PouEngine/utils/SimpleNode.h"


namespace pou::MathTools
{

bool detectSegmentCollision(glm::vec2 seg1, glm::vec2 seg2)
{
    if(seg1.x > seg1.y)
    {
        float temp = seg1.y;
        seg1.y = seg1.x;
        seg1.x = temp;
    }

    if(seg2.x > seg2.y)
    {
        float temp = seg2.y;
        seg2.y = seg2.x;
        seg2.x = temp;
    }

    if(seg1.x > seg2.x)
    {
        glm::vec2 temp = seg2;
        seg2 = seg1;
        seg1 = temp;
    }

    return (seg1.y >= seg2.x);
}

bool detectBoxCollision(const Box &box1, const Box &box2,
                        const SimpleNode *node1, const SimpleNode *node2)
{
   /* glm::vec4 base1 = glm::vec4(-box1.center.x,-box1.center.y,0,1);
    glm::vec4 v1 = glm::vec4(box1.size.x,0,0,1);
    glm::vec4 v2 = glm::vec4(0,box1.size.y,0,1);*/

    /*glm::mat4 idMat = glm::mat4(1);
    const glm::mat4 &mat1 = idMat;
    const glm::mat4 &mat2 = idMat;

    if(node1 != nullptr)
        mat1 = node1->getInvModelMatrix();
    if(node2 != nullptr)
        mat2 = node2->getModelMatrix();*/

    glm::mat4 mat = glm::mat4(1);
    if(node2 != nullptr)
        mat = node2->getModelMatrix();
    if(node1 != nullptr)
        mat = node1->getInvModelMatrix()*mat;

    glm::vec4 p1 = glm::vec4(-box2.center.x, -box2.center.y,0,1);
    glm::vec4 p2 = glm::vec4(box2.size.x-box2.center.x, -box2.center.y,0,1);
    glm::vec4 p3 = glm::vec4(-box2.center.x, box2.size.y-box2.center.y,0,1);
    glm::vec4 p4 = glm::vec4(box2.size.x-box2.center.x, box2.size.y-box2.center.y,0,1);

    p1 = mat*p1;
    p2 = mat*p2;
    p3 = mat*p3;
    p4 = mat*p4;

    p1 = p1/p1.w;
    p2 = p2/p2.w;
    p3 = p3/p3.w;
    p4 = p4/p4.w;


    glm::vec2 minP = glm::vec2(std::min(std::min(std::min(p1.x,p2.x),p3.x),p4.x),
                               std::min(std::min(std::min(p1.y,p2.y),p3.y),p4.y));

    glm::vec2 maxP = glm::vec2(std::max(std::max(std::max(p1.x,p2.x),p3.x),p4.x),
                               std::max(std::max(std::max(p1.y,p2.y),p3.y),p4.y));

    return (detectSegmentCollision(glm::vec2(minP.x, maxP.x), glm::vec2(-box1.center.x, box1.size.x-box1.center.x))
          & detectSegmentCollision(glm::vec2(minP.y, maxP.y), glm::vec2(-box1.center.y, box1.size.y-box1.center.y)) );
}


int intLog2(int v)
{
    int r = 1;
    while(v > 0)
    {
        v = v/2;
        ++r;
    }
    return r;
}


}


