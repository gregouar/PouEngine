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

void PhysicsEngine::addBoxBody(const BoxBody &box)
{
    instance()->addBoxBodyImpl(box);
}

/*void PhysicsEngine::addBoxBody(SimpleNode *node, const BoxBody &box)
{
    instance()->addBoxBodyImpl(node, box);
}

void PhysicsEngine::addBoxBodies(SimpleNode *node, const std::vector<BoxBody> &boxes)
{
    instance()->addBoxBodiesImpl(node, boxes);
}*/

void PhysicsEngine::addDiskBody(const DiskBody &disk)
{
    instance()->addDiskBodyImpl(disk);
}

/*void PhysicsEngine::addDiskBody(SimpleNode *node, const DiskBody &disk)
{
    instance()->addDiskBodyImpl(node, disk);
}

void PhysicsEngine::addDiskBodies(SimpleNode *node, const std::vector<DiskBody> &disks)
{
    instance()->addDiskBodiesImpl(node, disks);
}*/


///
///Protected
///



void PhysicsEngine::resolveCollisionsImpl(/*const Time &elapsedTime*/)
{
    for(int k = 0 ; k < 1 ; ++k)
    {
        ///Need to find a better way to do this, for example with a map of X coord
        for(size_t i = 0 ; i < m_boxBodies.size() ; ++i)
        for(size_t j = i+1 ; j < m_boxBodies.size() ; ++j)
            this->resolveBoxToBoxCollision(&m_boxBodies[i], &m_boxBodies[j]);

        for(size_t i = 0 ; i < m_diskBodies.size() ; ++i)
        for(size_t j = i+1 ; j < m_diskBodies.size() ; ++j)
            this->resolveDiskToDiskCollision(&m_diskBodies[i], &m_diskBodies[j]);

        for(size_t i = 0 ; i < m_boxBodies.size() ; ++i)
        for(size_t j = 0 ; j < m_diskBodies.size() ; ++j)
            this->resolveBoxToDiskCollision(&m_boxBodies[i], &m_diskBodies[j]);
    }

    m_boxBodies.clear();
    m_diskBodies.clear();
}

void PhysicsEngine::addBoxBodyImpl(const BoxBody &box)
{
    m_boxBodies.push_back(box);
}

/*void PhysicsEngine::addBoxBodyImpl(SimpleNode *node, const BoxBody &box)
{
    m_boxBodies.push_back(box);
    m_boxBodies.back().node = node;
}

void PhysicsEngine::addBoxBodiesImpl(SimpleNode *node, const std::vector<BoxBody> &boxes)
{
    m_boxBodies.insert(m_boxBodies.end(), boxes.begin(), boxes.end());
    for(size_t i = m_boxBodies.size() - boxes.size() ; i < m_boxBodies.size() ; ++i)
        m_boxBodies[i].node = node;
}*/

void PhysicsEngine::addDiskBodyImpl(const DiskBody &disk)
{
    m_diskBodies.push_back(disk);
}

/*void PhysicsEngine::addDiskBodyImpl(SimpleNode *node, const DiskBody &disk)
{
    m_diskBodies.push_back(disk);
    m_diskBodies.back().node = node;
}

void PhysicsEngine::addDiskBodiesImpl(SimpleNode *node, const std::vector<DiskBody> &disks)
{
    m_diskBodies.insert(m_diskBodies.end(), disks.begin(), disks.end());
    for(size_t i = m_diskBodies.size() - disks.size() ; i < m_diskBodies.size() ; ++i)
        m_diskBodies[i].node = node;
}*/


float PhysicsEngine::computeMassRatio(float mass1, float mass2)
{
    float ratio = .5;

    if(mass1 == -1)
        ratio = 1.0;
    else if(mass2 == -1)
        ratio = 0.0;
    else if(!(mass1 == 0 && mass2 == 0))
        ratio = mass1/(mass1 + mass2);

    return ratio;
}

void PhysicsEngine::resolveBoxMinkowskiDiff(const glm::vec2 &closestPoint,
                                        SimpleNode *node1, SimpleNode *node2,
                                        float mass1, float mass2)
{
    /*std::cout<<"Position Node1: "<<node1->getGlobalPosition().x<<" "<<node1->getGlobalPosition().y<<std::endl;
    std::cout<<"Box1: "<<box1.size.x<<" "<<box1.size.y<<std::endl;
    std::cout<<"Position Node2: "<<node2->getGlobalPosition().x<<" "<<node2->getGlobalPosition().y<<std::endl;
    std::cout<<"Box2: "<<box2.size.x<<" "<<box2.size.y<<std::endl;
    for(size_t i = 0 ; i < minkowskiDiff.size() ; ++i)
    {
        std::cout<<minkowskiDiff[i].x<<" "<<minkowskiDiff[i].y<<std::endl;

        auto ip = (i + 1) % minkowskiDiff.size();
        float determinant = (-minkowskiDiff[i].x)*(minkowskiDiff[ip].y - minkowskiDiff[i].y)
                    - (-minkowskiDiff[i].y)*(minkowskiDiff[ip].x - minkowskiDiff[i].x);


        //std::cout<<"det : "<<determinant<<std::endl;
        //std::cout<<"sign : "<<sign<<std::endl;
        //std::cout<<"det * sign : "<<determinant * sign<<std::endl;
        std::cout<<minkowskiDiff[i].x<<" "<<minkowskiDiff[i].y<<std::endl;

        if(abs(determinant) > 0.1)
            sign = determinant;
    }*/

    auto minimalTranslationVector = node1->getModelMatrix() * glm::vec4(closestPoint.x, closestPoint.y,0,1) ;
    minimalTranslationVector /= minimalTranslationVector.w;

    auto ratio = this->computeMassRatio(mass1, mass2);

    auto translationVector = glm::vec2(minimalTranslationVector) - glm::vec2(node1->getGlobalXYPosition());

    /*auto relVelocity = glm::vec2(node2->getVelocity()) - glm::vec2(node1->getVelocity());
    float velAlongNormal = glm::dot(relVelocity, glm::normalize(translationVector));
    if(velAlongNormal > 0)
        return;*/

    //translationVector = -velAlongNormal * glm::normalize(translationVector);

    //std::cout<<"Translation: "<<translationVector.x<<" "<<translationVector.y<<std::endl;
    //std::cout<<"Ratio:"<<ratio<<std::endl;

    node2->move(translationVector * ratio * 1.0f);
    node1->move(-translationVector * (1.0f-ratio) * 1.0f);
    node1->update();
    node2->update();
}

void PhysicsEngine::resolveBoxToBoxCollision(BoxBody *body1, BoxBody *body2)
{
    auto &node1 = body1->node;
    auto &node2 = body2->node;
    auto &box1 = body1->box;
    auto &box2 = body2->box;

    if(body1->mass == -1 && body2->mass == -1)
        return;

    if(node1 == node2)
        return;

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

    //If we are outside of the bounding box, there can be no collision
    if(minkowskiDiff.empty())
        return;

    float closestSquaredDistance = -1;
    glm::vec2 closestPoint(0.0f);
    bool outside = false;
    double sign = 0;

    //We compute the minimal separation vector in body1 coord by computing the point on the boundary of the Minkowsi difference closer to the origin
    for(size_t i = 0 ; i < minkowskiDiff.size() ; ++i)
    {
        auto ip = (i + 1) % minkowskiDiff.size();
        auto boundaryVector = minkowskiDiff[ip] - minkowskiDiff[i];
        auto originVector = - minkowskiDiff[i];

        //std::cout<<minkowskiDiff[i].x<<" "<<minkowskiDiff[i].y<<std::endl;

        float dotBoundary = glm::dot(boundaryVector,boundaryVector);
        if(dotBoundary < 1)
            continue;

        float determinant = (-minkowskiDiff[i].x)*(minkowskiDiff[ip].y - minkowskiDiff[i].y)
                        - (-minkowskiDiff[i].y)*(minkowskiDiff[ip].x - minkowskiDiff[i].x);

        if(determinant * sign < 0)
            outside = true;
        if(abs(determinant) > 0.1)
            sign = determinant;

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

    sign = 0;

    if(!outside && closestSquaredDistance > 1)
        this->resolveBoxMinkowskiDiff(closestPoint, node1, node2, body1->mass, body2->mass);

}

void PhysicsEngine::computeMinkowskiDiff(const glm::vec2 &originBoxSize,
                                         glm::vec2 p1, glm::vec2 p2, glm::vec2 p3, /*glm::vec2 p4,*/
                                         std::vector<glm::vec2> &minkowskiDiff)
{
    minkowskiDiff.resize(8);

    auto p4 = p2 + p3 - p1;

    auto leftMost = p1;
    if(p2.x < leftMost.x)
        leftMost = p2;
    if(p3.x < leftMost.x)
        leftMost = p3;
    if(p4.x < leftMost.x)
        leftMost = p4;

    if(leftMost.x > 0)
    {
        minkowskiDiff.clear();
        return;
    }

    auto rightMost = p1;
    if(p2.x > rightMost.x)
        rightMost = p2;
    if(p3.x > rightMost.x)
        rightMost = p3;
    if(p4.x > rightMost.x)
        rightMost = p4;

    if(rightMost.x + originBoxSize.x < 0)
    {
        minkowskiDiff.clear();
        return;
    }

    auto topMost = p1;
    if(p2.y < topMost.y)
        topMost = p2;
    if(p3.y < topMost.y)
        topMost = p3;
    if(p4.y < topMost.y)
        topMost = p4;

    if(topMost.y > 0)
    {
        minkowskiDiff.clear();
        return;
    }

    auto bottomMost = p1;
    if(p2.y > bottomMost.y)
        bottomMost = p2;
    if(p3.y > bottomMost.y)
        bottomMost = p3;
    if(p4.y > bottomMost.y)
        bottomMost = p4;

    if(bottomMost.y + originBoxSize.y < 0)
    {
        minkowskiDiff.clear();
        return;
    }

    minkowskiDiff[0] = leftMost;
    minkowskiDiff[1] = topMost;
    minkowskiDiff[2] = topMost + glm::vec2(originBoxSize.x,0);
    minkowskiDiff[3] = rightMost + glm::vec2(originBoxSize.x,0);
    minkowskiDiff[4] = rightMost + originBoxSize;
    minkowskiDiff[5] = bottomMost + originBoxSize;
    minkowskiDiff[6] = bottomMost + glm::vec2(0,originBoxSize.y);
    minkowskiDiff[7] = leftMost + glm::vec2(0,originBoxSize.y);
}

void PhysicsEngine::resolveDiskMinkowskiDiff(const glm::vec2 &diskCenter, float diskRadius,
                                             SimpleNode *node1, SimpleNode *node2,
                                             float mass1, float mass2)
{
    float deltaSquared = glm::dot(diskCenter,diskCenter);
    if(deltaSquared > diskRadius*diskRadius)
        return;

    float lengthDelta = sqrt(deltaSquared);
    auto ratio = this->computeMassRatio(mass1, mass2);
    auto translationVector = -diskCenter * (float)(diskRadius - lengthDelta)/lengthDelta;

    const glm::mat4 &mat = node1->getModelMatrix();
    translationVector = glm::vec2(mat * glm::vec4(translationVector.x, translationVector.y, 0, 1));
    translationVector -= glm::vec2(node1->getGlobalXYPosition());

    node2->move(translationVector * ratio * 1.0f);
    node1->move(-translationVector * (1.0f-ratio) * 1.0f);
    node1->update();
    node2->update();
}

void PhysicsEngine::resolveBoxToDiskCollision(BoxBody *boxBody, DiskBody *diskBody)
{
    auto &node1 = boxBody->node;
    auto &node2 = diskBody->node;
    auto &box = boxBody->box;
    auto diskCenter = node2->getGlobalXYPosition();
    auto diskRadius = diskBody->radius;

    if(boxBody->mass == -1 && diskBody->mass == -1)
        return;

    if(node1 == node2)
        return;

    ///We convert to local coord wrt boxBody
    glm::mat4 mat = glm::mat4(1);
    mat = node1->getInvModelMatrix();

    glm::vec4 p = glm::vec4(diskCenter.x, diskCenter.y,0,1);
    p = mat*p;
    p = p/p.w;
    p += glm::vec4(box.center.x, box.center.y,0,0);

    ///We compute the bounding box of the Minkowski difference
    glm::vec2 outerSquare_ul, outerSquare_dr;

    outerSquare_ul = -glm::vec2(p) - glm::vec2(1.0f)*diskRadius;
    outerSquare_dr = -glm::vec2(p) + box.size + glm::vec2(1.0f)*diskRadius;

    if(outerSquare_ul.x > 0 || outerSquare_ul.y > 0 || outerSquare_dr.x < 0 || outerSquare_dr.y < 0)
        return;

    ///If we are in the top left part of the outer bounding box, we can consider it is a disk
    if(0 < outerSquare_ul.x + diskRadius && 0 < outerSquare_ul.y + diskRadius)
    {
        this->resolveDiskMinkowskiDiff(outerSquare_ul + glm::vec2(diskRadius), diskRadius,
                                       node1, node2, boxBody->mass, diskBody->mass);
        return;
    }

    ///If we are in the bottom left part of the outer bounding box, we can consider it is a disk
    if(0 < outerSquare_ul.x + diskRadius && 0 > outerSquare_dr.y - diskRadius)
    {
        this->resolveDiskMinkowskiDiff(glm::vec2(outerSquare_ul.x + diskRadius, outerSquare_dr.y - diskRadius), diskRadius,
                                       node1, node2, boxBody->mass, diskBody->mass);
        return;
    }

    ///If we are in the bottom right part of the outer bounding box, we can consider it is a disk
    if(0 > outerSquare_dr.x - diskRadius && 0 > outerSquare_dr.y - diskRadius)
    {
        this->resolveDiskMinkowskiDiff(outerSquare_dr - glm::vec2(diskRadius), diskRadius,
                                       node1, node2, boxBody->mass, diskBody->mass);
        return;
    }

    ///If we are in the top right part of the outer bounding box, we can consider it is a disk
    if(0 > outerSquare_dr.x - diskRadius && 0 < outerSquare_ul.y + diskRadius)
    {
        this->resolveDiskMinkowskiDiff(glm::vec2(outerSquare_dr.x - diskRadius,  outerSquare_ul.y + diskRadius), diskRadius,
                                       node1, node2, boxBody->mass, diskBody->mass);
        return;
    }

    glm::vec2 closestPoint;
    ///Otherwise, we can suppose the Minkowski difference is a rectangle
    if(-outerSquare_ul.x < -outerSquare_ul.y
    && -outerSquare_ul.x < outerSquare_dr.x
    && -outerSquare_ul.x < outerSquare_dr.y)
        closestPoint = glm::vec2(outerSquare_ul.x, 0);
    else if(-outerSquare_ul.y < outerSquare_dr.x
         && -outerSquare_ul.y < outerSquare_dr.y)
        closestPoint = glm::vec2(0, outerSquare_ul.y);
    else if(outerSquare_dr.x < outerSquare_dr.y)
        closestPoint = glm::vec2(outerSquare_dr.x, 0);
    else
        closestPoint = glm::vec2(0,outerSquare_dr.y);

    this->resolveBoxMinkowskiDiff(closestPoint, node1, node2, boxBody->mass, diskBody->mass);
}


void PhysicsEngine::resolveDiskToDiskCollision(DiskBody *body1, DiskBody *body2)
{
    auto &node1 = body1->node;
    auto &node2 = body2->node;

    if(body1->mass == -1 && body2->mass == -1)
        return;

    if(node1 == node2)
        return;

    auto pos1 = node1->getGlobalXYPosition();
    auto pos2 = node2->getGlobalXYPosition();

    auto delta = pos2 - pos1;
    auto deltaSquared = glm::dot(delta, delta);
    float sumRadii = body1->radius + body2->radius;

    //If collision
    if(sumRadii*sumRadii > deltaSquared)
    {
        float lengthDelta = sqrt(deltaSquared);
        auto ratio = this->computeMassRatio(body1->mass, body2->mass);
        auto translationVector = delta * (float)(sumRadii - lengthDelta)/lengthDelta;
        node2->move(translationVector * ratio * 1.0f);
        node1->move(-translationVector * (1.0f-ratio) * 1.0f);
        node1->update();
        node2->update();
    }
}

}
