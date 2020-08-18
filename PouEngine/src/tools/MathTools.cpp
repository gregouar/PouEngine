#include "PouEngine/tools/MathTools.h"

//#include "PouEngine/system/SimpleNode.h"


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
                        const TransformComponent *transform1, const TransformComponent *transform2)
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
    if(transform2)
        mat = transform2->getModelMatrix();
    if(transform1)
        mat = transform1->getInvModelMatrix()*mat;

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


bool isInBox(glm::vec2 position, const Box &box, const TransformComponent *boxTransform)
{
    if(boxTransform)
    {
        auto mat = boxTransform->getInvModelMatrix();
        glm::vec4 vec4Pos = glm::vec4(position,0,1);
        vec4Pos = mat*vec4Pos;
        vec4Pos /= vec4Pos.w;
        position = glm::vec2(vec4Pos);
    }

    if(position.x >= -box.center.x && position.y >= -box.center.y
    && position.x <= -box.center.x + box.size.x
    && position.y <= -box.center.y + box.size.y )
        return (true);
    return (false);
}


std::pair<bool, float> computeRayToLineSegmentIntersection(glm::vec2 u0, glm::vec2 v0,
                                                            glm::vec2 u1, glm::vec2 v1)
{
    float det = v1.x * v0.y - v0.x * v1.y;

    if(det == 0)
        return {false, 0};

    float s = (1.0f/det) * ((u0.x - u1.x) * v0.y - (u0.y - u1.y) * v0.x);
    if(s < 0 || s > 1)
        return {false, 0};

    float t = (1/det) * -(-(u0.x - u1.x) * v1.y + (u0.y - u1.y) * v1.x);
    if(t < 0 /*|| t > 1*/)
        return {false, 0};

    return {true, t /*u0 + v0*t*/};
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

float computeWantedRotation(float startingRotation, glm::vec2 position)
{
    float wantedRotation = glm::pi<float>()/2.0+glm::atan(position.y, position.x);

    return computeWantedRotation(glm::vec1(startingRotation), glm::vec1(wantedRotation), true).x;
}


}



bool uint32less(uint32_t lhs, uint32_t rhs)
{
    if(lhs == (uint32_t)(-1))
        return (rhs != (uint32_t)(-1));

    if(rhs == (uint32_t)(-1))
        return (false);

    return lhs < rhs;
}

bool uint32leq(uint32_t lhs, uint32_t rhs)
{
    if(lhs == (uint32_t)(-1))
        return (true);

    if(rhs == (uint32_t)(-1))
        return (false);

    return lhs <= rhs;
}

uint32_t uint32max(uint32_t lhs, uint32_t rhs)
{
    if(uint32less(lhs,rhs))
        return rhs;
    return lhs;
}

