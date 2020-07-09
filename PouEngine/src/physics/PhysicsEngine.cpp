#include "PouEngine/physics/PhysicsEngine.h"

namespace pou
{

PhysicsEngine::PhysicsEngine()
{
    //ctor
}

PhysicsEngine::~PhysicsEngine()
{
    //dtor
}


void PhysicsEngine::resolveCollisions(/*const Time &elapsedTime*/)
{
    instance()->resolveCollisionsImpl();
}

void PhysicsEngine::addBoxBody(SimpleNode *node, const BoxBody &box)
{
    instance()->addBoxBodyImpl(node, box);
}

void PhysicsEngine::addBoxBodies(SimpleNode *node, const std::vector<BoxBody> &boxes)
{
    instance()->addBoxBodiesImpl(node, boxes);
}


///
///Protected
///



void PhysicsEngine::resolveCollisionsImpl(/*const Time &elapsedTime*/)
{
    ///Need to find a much better way to do this
    for(size_t i = 0 ; i < m_boxBodies.size() ; ++i)
    for(size_t j = i+1 ; j < m_boxBodies.size() ; ++j)
        this->resolveBoxToBoxCollision(&m_boxBodies[i], &m_boxBodies[j]);

    m_boxBodies.clear();
}

void PhysicsEngine::addBoxBodyImpl(SimpleNode *node, const BoxBody &box)
{
    m_boxBodies.push_back(box);
    m_boxBodies.back().node = node;
}

void PhysicsEngine::addBoxBodiesImpl(SimpleNode *node, const std::vector<BoxBody> &boxes)
{
    m_boxBodies.insert(m_boxBodies.end(), boxes.begin(), boxes.end());
    for(size_t i = m_boxBodies.size() - boxes.size() ; i < m_boxBodies.size() ; ++i)
        m_boxBodies[i].node = node;
}

void PhysicsEngine::resolveBoxToBoxCollision(BoxBody *body1, BoxBody *body2)
{
    auto &node1 = body1->node;
    auto &node2 = body2->node;
    auto &box1 = body1->box;
    auto &box2 = body2->box;


    ///We convert to local coord wrt body1
    glm::mat4 mat = glm::mat4(1);
    mat = node2->getModelMatrix();
    mat = node1->getInvModelMatrix()*mat;

    glm::vec4 p1 = glm::vec4(-box2.center.x, -box2.center.y,0,1);
    glm::vec4 p2 = glm::vec4(box2.size.x-box2.center.x, -box2.center.y,0,1);
    glm::vec4 p3 = glm::vec4(-box2.center.x, box2.size.y-box2.center.y,0,1);
    //glm::vec4 p4 = glm::vec4(box2.size.x-box2.center.x, box2.size.y-box2.center.y,0,1);

    p1 = mat*p1;
    p2 = mat*p2;
    p3 = mat*p3;
    //p4 = mat*p4;

    p1 = p1/p1.w;
    p2 = p2/p2.w;
    p3 = p3/p3.w;
    //p4 = p4/p4.w;

    p1 += glm::vec4(box1.center.x, box1.center.y,0,0);
    p2 += glm::vec4(box1.center.x, box1.center.y,0,0);
    p3 += glm::vec4(box1.center.x, box1.center.y,0,0);
    //p4 += glm::vec4(box1.center.x, box1.center.y,0,0);

    //We compute the Minkowsky difference
    std::vector<glm::vec2> minkowskiDiff(8);
    this->computeMinkowskiDiff(box1.size, -p1, -p2, -p3, /*p4,*/ minkowskiDiff);

    //We test if the Minkowski difference contains the origin ?

    float closestSquaredDistance = -1;
    glm::vec2 closestPoint;
    bool outside = false;
    int sign = 0;

    //We compute the minimal separation vector in body1 coord by computing the point on the boundary of the Minkowsi difference closer to the origin
    for(int i = 0 ; i < 8 ; ++i)
    {
        auto ip = (i + 1) % 8;
        auto boundaryVector = minkowskiDiff[ip] - minkowskiDiff[i];
        auto originVector = - minkowskiDiff[i];

        float determinant = (-minkowskiDiff[i].x)*(minkowskiDiff[ip].y - minkowskiDiff[i].y)
                        - (-minkowskiDiff[i].y)*(minkowskiDiff[ip].x - minkowskiDiff[i].x);

        //std::cout<<minkowskiDiff[i].x<<" "<<minkowskiDiff[i].y<<std::endl;

        if(determinant * sign < 0)
            outside = true;
        sign = determinant;

        float dotBoundary = glm::dot(boundaryVector,boundaryVector);
        if(dotBoundary < .1)
            continue;

        float projectionFactor = glm::dot(boundaryVector, originVector)/dotBoundary;
        projectionFactor = glm::clamp(projectionFactor, 0.0f, 1.0f);

        auto projectionVector = projectionFactor * boundaryVector;
        auto boundaryPoint = minkowskiDiff[i] + projectionVector;
        float squaredDistance = glm::dot(boundaryPoint,boundaryPoint);
        if(squaredDistance < closestSquaredDistance || closestSquaredDistance == -1)
        {
            closestSquaredDistance = squaredDistance;
            closestPoint = boundaryPoint;
        }
    }

    if(!outside)
    {
        auto minimalTranslationVector = node1->getModelMatrix() * glm::vec4(closestPoint.x, closestPoint.y,0,1) ;
        float ratio = .5;

        auto translationVector = glm::vec2(minimalTranslationVector) - glm::vec2(node1->getGlobalXYPosition());

        std::cout<<translationVector.x<<" "<<translationVector.y<<std::endl;

        node2->move(translationVector * ratio);
        node1->move(-translationVector * (1.0f-ratio));
    }

}

void PhysicsEngine::computeMinkowskiDiff(const glm::vec2 &originBoxSize,
                                         glm::vec2 p1, glm::vec2 p2, glm::vec2 p3, /*glm::vec2 p4,*/
                                         std::vector<glm::vec2> &minkowskiDiff)
{
    minkowskiDiff.resize(8);

    auto p4 = p2 + p3 - p1;

    glm::vec2 minP = glm::vec2(std::min(std::min(std::min(p1.x,p2.x),p3.x),p4.x),
                               std::min(std::min(std::min(p1.y,p2.y),p3.y),p4.y));

    glm::vec2 maxP = glm::vec2(std::max(std::max(std::max(p1.x,p2.x),p3.x),p4.x),
                               std::max(std::max(std::max(p1.y,p2.y),p3.y),p4.y));



    //This is the box that will bound the Minkowski difference
    glm::vec2 outerSquare_ul, outerSquare_dr;

    outerSquare_ul = -maxP;
    outerSquare_dr = originBoxSize-minP;

    /*auto delta1 = p2 - p1;
    auto delta2 = p3 - p1;

    //Then the box are aligned and the space coincides with the bounding box
    if(delta1.x == 0 || delta1.y == 0 || delta2.x == 0 || delta2.y == 0)
    {
        minkowskiDiff.resize(4);
        minkowskiDiff[0] = outerSquare_ul;
        minkowskiDiff[1] = glm::vec2(outerSquare_dr.x, outerSquare_ul.y);
        minkowskiDiff[2] = outerSquare_dr;
        minkowskiDiff[3] = glm::vec2(outerSquare_ul.x, outerSquare_dr.y);
        return;
    }*/

    auto leftMost = p1;
    if(p2.x < leftMost.x)
        leftMost = p2;
    if(p3.x < leftMost.x)
        leftMost = p3;
    if(p4.x < leftMost.x)
        leftMost = p4;

    auto rightMost = p1;
    if(p2.x > rightMost.x)
        rightMost = p2;
    if(p3.x > rightMost.x)
        rightMost = p3;
    if(p4.x > rightMost.x)
        rightMost = p4;

    auto topMost = p1;
    if(p2.y < topMost.y)
        topMost = p2;
    if(p3.y < topMost.y)
        topMost = p3;
    if(p4.y < topMost.y)
        topMost = p4;

    auto bottomMost = p1;
    if(p2.y > bottomMost.y)
        bottomMost = p2;
    if(p3.y > bottomMost.y)
        bottomMost = p3;
    if(p4.y > bottomMost.y)
        bottomMost = p4;

    minkowskiDiff[0] = leftMost;
    minkowskiDiff[1] = topMost;
    minkowskiDiff[2] = topMost + glm::vec2(originBoxSize.x,0);
    minkowskiDiff[3] = rightMost + glm::vec2(originBoxSize.x,0);
    minkowskiDiff[4] = rightMost + originBoxSize;
    minkowskiDiff[5] = bottomMost + originBoxSize;
    minkowskiDiff[6] = bottomMost + glm::vec2(0,originBoxSize.y);
    minkowskiDiff[7] = leftMost + glm::vec2(0,originBoxSize.y);
}

}
