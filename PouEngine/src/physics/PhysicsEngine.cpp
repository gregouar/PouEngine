#include "PouEngine/physics/PhysicsEngine.h"

#include "PouEngine/system/Clock.h"

//#define DISABLE_PHYSICS

namespace pou
{

PhysicsEngine::PhysicsEngine() //:
    /*m_minPos(0),
    m_maxPos(0)*/
{
    //ctor
}

PhysicsEngine::~PhysicsEngine()
{
    //dtor
}


void PhysicsEngine::resolveCollisions(glm::vec2 minPos, glm::vec2 maxPos/*const Time &elapsedTime*/)
{
    #ifdef DISABLE_PHYSICS
        return;
    #endif // DISABLE_PHYSICS
    instance()->resolveCollisionsImpl(minPos, maxPos);
}

void PhysicsEngine::addBoxBody(const BoxBody &box)
{
    #ifdef DISABLE_PHYSICS
        return;
    #endif // DISABLE_PHYSICS
    instance()->addBoxBodyImpl(box);
}

void PhysicsEngine::addDiskBody(const DiskBody &disk)
{
    #ifdef DISABLE_PHYSICS
        return;
    #endif // DISABLE_PHYSICS
    instance()->addDiskBodyImpl(disk);
}


CollisionDetectionImpact PhysicsEngine::castCollisionDetectionRay(glm::vec2 startPoint, glm::vec2 endPoint, float rayThickness, float minMass)
{
    #ifdef DISABLE_PHYSICS
        return {false};
    #endif // DISABLE_PHYSICS
    return instance()->castCollisionDetectionRayImpl(startPoint, endPoint, rayThickness, minMass);
}

bool PhysicsEngine::detectCollisionWithBox(TransformComponent *transform, const MathTools::Box &box, float minMass)
{
    #ifdef DISABLE_PHYSICS
        return {false};
    #endif // DISABLE_PHYSICS
    return instance()->detectCollisionWithBoxImpl(transform,box,minMass);
}


///
///Protected
///



void PhysicsEngine::resolveCollisionsImpl(glm::vec2 minPos, glm::vec2 maxPos/*const Time &elapsedTime*/)
{
    /*m_minPos = minPos;
    m_maxPos = maxPos;*/

    for(auto bodyIt = m_rigidBodies.begin() ; bodyIt != m_rigidBodies.end() ; ++bodyIt)
    {
        auto &body1 = bodyIt->second;

        auto globalPos1 = body1.transform->getGlobalPosition();
        if(globalPos1.x < minPos.x)
            body1.transform->globalMove(minPos.x - globalPos1.x,0);
        if(globalPos1.y < minPos.y)
            body1.transform->globalMove(0,minPos.y - globalPos1.y);
        if(globalPos1.x > maxPos.x)
            body1.transform->globalMove(maxPos.x-globalPos1.x,0);
        if(globalPos1.y > maxPos.y)
            body1.transform->globalMove(0,maxPos.y-globalPos1.y);

        for(auto nextBodyIt = std::next(bodyIt) ;
            nextBodyIt != m_rigidBodies.end() && nextBodyIt->first <= body1.estimatedRightMost ;
            ++nextBodyIt)
        {
            //++nbrTests;

            auto &body2 = nextBodyIt->second;

            if(body1.isBox && body2.isBox)
                this->resolveBoxToBoxCollision(&body1, &body2);
            else if(body1.isBox && body2.isDisk)
                this->resolveBoxToDiskCollision(&body1, &body2);
            else if(body1.isDisk && body2.isBox)
                this->resolveBoxToDiskCollision(&body2, &body1);
            else
                this->resolveDiskToDiskCollision(&body1, &body2);
        }
    }

    //std::cout<<"Nbr tests: "<<nbrTests<<" in time:  "<<clockTest.restart().count()<<std::endl;

    /*m_boxBodies.clear();
    m_diskBodies.clear();*/
    m_oldRigidBodies.clear();
    m_oldRigidBodies.swap(m_rigidBodies);
}

void PhysicsEngine::addBoxBodyImpl(const BoxBody &box)
{
    float maxRadius = std::max(glm::abs(box.box.center.x), glm::abs(box.box.size.x - box.box.center.x))
                    + std::max(glm::abs(box.box.center.y), glm::abs(box.box.size.y - box.box.center.y)); ///I could do something finer, or even compute precise bounds
    float estimatedLeftMost = box.transform->getGlobalXYPosition().x - maxRadius;

    RigidBody body;
    body.isBox = true;
    body.isDisk = false;
    body.transform = box.transform;
    body.mass = box.mass;
    body.box = box.box;
    body.estimatedRightMost = estimatedLeftMost + maxRadius + maxRadius;

    m_rigidBodies.insert({estimatedLeftMost, body});
}

void PhysicsEngine::addDiskBodyImpl(const DiskBody &disk)
{
    float estimatedLeftMost = disk.transform->getGlobalXYPosition().x - disk.radius;

    RigidBody body;
    body.isBox = false;
    body.isDisk = true;
    body.transform = disk.transform;
    body.mass = disk.mass;
    body.radius = disk.radius;
    body.estimatedRightMost = estimatedLeftMost + disk.radius + disk.radius;

    m_rigidBodies.insert({estimatedLeftMost, body});
}

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
                                        TransformComponent *transform1, TransformComponent *transform2,
                                        float mass1, float mass2)
{
    auto minimalTranslationVector = transform1->getModelMatrix() * glm::vec4(closestPoint.x, closestPoint.y,0,1) ;
    minimalTranslationVector /= minimalTranslationVector.w;

    auto ratio = this->computeMassRatio(mass1, mass2);

    auto translationVector = glm::vec2(minimalTranslationVector) - glm::vec2(transform1->getGlobalXYPosition());

    transform2->globalMove(translationVector * ratio * 1.0f);
    transform1->globalMove(-translationVector * (1.0f-ratio) * 1.0f);
    transform1->update();
    transform2->update();
}

//void PhysicsEngine::resolveBoxToBoxCollision(BoxBody *body1, BoxBody *body2)
void PhysicsEngine::resolveBoxToBoxCollision(RigidBody *body1, RigidBody *body2)
{
    auto &transform1 = body1->transform;
    auto &transform2 = body2->transform;
    auto &box1 = body1->box;
    auto &box2 = body2->box;

    if(body1->mass == -1 && body2->mass == -1)
        return;

    if(transform1 == transform2)
        return;

    ///We convert to local coord wrt body1
    glm::mat4 mat = glm::mat4(1);
    mat = transform2->getModelMatrix();
    mat = transform1->getInvModelMatrix()*mat;

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

    ///It would probably be faster to check whether I'm in a corner or not, and then it's easy to compute the closest point

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
        this->resolveBoxMinkowskiDiff(closestPoint, transform1, transform2, body1->mass, body2->mass);

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
                                             TransformComponent *transform1, TransformComponent *transform2,
                                             float mass1, float mass2)
{
    float deltaSquared = glm::dot(diskCenter,diskCenter);
    if(deltaSquared > diskRadius*diskRadius)
        return;

    float lengthDelta = sqrt(deltaSquared);
    auto ratio = this->computeMassRatio(mass1, mass2);
    auto translationVector = -diskCenter * (float)(diskRadius - lengthDelta)/lengthDelta;

    const glm::mat4 &mat = transform1->getModelMatrix();
    translationVector = glm::vec2(mat * glm::vec4(translationVector.x, translationVector.y, 0, 1));
    translationVector -= glm::vec2(transform1->getGlobalXYPosition());

    transform2->globalMove(translationVector * ratio * 1.0f);
    transform1->globalMove(-translationVector * (1.0f-ratio) * 1.0f);
    transform1->update();
    transform2->update();
}

//void PhysicsEngine::resolveBoxToDiskCollision(BoxBody *boxBody, DiskBody *diskBody)
void PhysicsEngine::resolveBoxToDiskCollision(RigidBody *boxBody, RigidBody *diskBody)
{
    auto &transform1 = boxBody->transform;
    auto &transform2 = diskBody->transform;
    auto &box = boxBody->box;
    auto diskCenter = transform2->getGlobalXYPosition();
    auto diskRadius = diskBody->radius;

    if(boxBody->mass == -1 && diskBody->mass == -1)
        return;

    if(transform1 == transform2)
        return;

    ///We convert to local coord wrt boxBody
    glm::mat4 mat = glm::mat4(1);
    mat = transform1->getInvModelMatrix();

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
                                       transform1, transform2, boxBody->mass, diskBody->mass);
        return;
    }

    ///If we are in the bottom left part of the outer bounding box, we can consider it is a disk
    if(0 < outerSquare_ul.x + diskRadius && 0 > outerSquare_dr.y - diskRadius)
    {
        this->resolveDiskMinkowskiDiff(glm::vec2(outerSquare_ul.x + diskRadius, outerSquare_dr.y - diskRadius), diskRadius,
                                       transform1, transform2, boxBody->mass, diskBody->mass);
        return;
    }

    ///If we are in the bottom right part of the outer bounding box, we can consider it is a disk
    if(0 > outerSquare_dr.x - diskRadius && 0 > outerSquare_dr.y - diskRadius)
    {
        this->resolveDiskMinkowskiDiff(outerSquare_dr - glm::vec2(diskRadius), diskRadius,
                                       transform1, transform2, boxBody->mass, diskBody->mass);
        return;
    }

    ///If we are in the top right part of the outer bounding box, we can consider it is a disk
    if(0 > outerSquare_dr.x - diskRadius && 0 < outerSquare_ul.y + diskRadius)
    {
        this->resolveDiskMinkowskiDiff(glm::vec2(outerSquare_dr.x - diskRadius,  outerSquare_ul.y + diskRadius), diskRadius,
                                       transform1, transform2, boxBody->mass, diskBody->mass);
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

    this->resolveBoxMinkowskiDiff(closestPoint, transform1, transform2, boxBody->mass, diskBody->mass);
}


//void PhysicsEngine::resolveDiskToDiskCollision(DiskBody *body1, DiskBody *body2)
void PhysicsEngine::resolveDiskToDiskCollision(RigidBody *body1, RigidBody *body2)
{
    auto &transform1 = body1->transform;
    auto &transform2 = body2->transform;

    if(body1->mass == -1 && body2->mass == -1)
        return;

    if(transform1 == transform2)
        return;

    auto pos1 = transform1->getGlobalXYPosition();
    auto pos2 = transform2->getGlobalXYPosition();

    auto delta = pos2 - pos1;
    auto deltaSquared = glm::dot(delta, delta);
    float sumRadii = body1->radius + body2->radius;

    //If collision
    if(sumRadii*sumRadii > deltaSquared)
    {
        float lengthDelta = sqrt(deltaSquared);

        if(lengthDelta == 0)
        {
            delta = glm::vec2(1,0);
            lengthDelta = 1;
        }

        auto ratio = this->computeMassRatio(body1->mass, body2->mass);
        auto translationVector = delta * (float)(sumRadii - lengthDelta)/lengthDelta;
        transform2->globalMove(translationVector * ratio * 1.0f);
        transform1->globalMove(-translationVector * (1.0f-ratio) * 1.0f);
        transform1->update();
        transform2->update();
    }
}

CollisionDetectionImpact PhysicsEngine::castCollisionDetectionRayImpl(glm::vec2 startPoint, glm::vec2 endPoint, float rayThickness, float minMass)
{
    CollisionDetectionImpact impact;
    impact.detectImpact = false;
    impact.collisionImpact = endPoint;

    float leftMost = std::min(startPoint.x - rayThickness, endPoint.x - rayThickness);
    float rightMost= std::max(startPoint.x + rayThickness, endPoint.x + rayThickness);

    auto rayDelta = endPoint - startPoint;
    float squaredMaxDistance = glm::dot(rayDelta, rayDelta);
    auto normalizedRayVect = rayDelta/(float)sqrt(squaredMaxDistance);

    float       closestSquaredDistance = -1;
    int         closestCollisionSide(-1);
    glm::vec2   closestImpact(0);
    RigidBody  *closestBody(nullptr);

    for(auto bodyIt = m_oldRigidBodies.begin() ;
        bodyIt != m_oldRigidBodies.end() && bodyIt->first < rightMost ;
        ++bodyIt)
    {
        auto &body = bodyIt->second;
        if(body.estimatedRightMost < leftMost)
            continue;
        if((minMass == -1 && body.mass != -1) || (minMass > body.mass && body.mass != -1))
            continue;

        auto [collisionSide, collisionPos] = this->testRayCollision(startPoint, normalizedRayVect, rayThickness, &body);

        if(collisionSide != -1)
        {
            auto delta = collisionPos - startPoint;
            float squaredDistance = glm::dot(delta, delta);
            if((closestSquaredDistance == -1 || squaredDistance < closestSquaredDistance)
            && squaredDistance < squaredMaxDistance)
            {
                closestSquaredDistance = squaredDistance;
                closestImpact = collisionPos;
                closestBody = &body;
                closestCollisionSide = collisionSide;
            }
        }
    }

    if(closestBody != nullptr)
    {
        impact.detectImpact = true;
        impact.collisionImpact = closestImpact;

        ///Need to compute left and right corner exploration nodes
        if(closestBody->isBox)
        {
            auto mat = closestBody->transform->getModelMatrix();

            rayThickness += 10;

            auto p1 = -closestBody->box.center - glm::vec2(rayThickness); //Top left
            auto p2 = -closestBody->box.center + glm::vec2(closestBody->box.size.x, 0) + glm::vec2(rayThickness, -rayThickness); //Top right
            auto p3 = -closestBody->box.center + closestBody->box.size + glm::vec2(rayThickness); //Bottom right
            auto p4 = -closestBody->box.center + glm::vec2(0,closestBody->box.size.y) + glm::vec2(-rayThickness, rayThickness);//Bottom left

            auto p1v = mat * glm::vec4(p1.x,p1.y,0,1);
            auto p2v = mat * glm::vec4(p2.x,p2.y,0,1);
            auto p3v = mat * glm::vec4(p3.x,p3.y,0,1);
            auto p4v = mat * glm::vec4(p4.x,p4.y,0,1);

            if(closestCollisionSide == 0) //Top side
            {
                impact.corner1_1 = glm::vec2(p1v)/p1v.w;
                impact.corner1_2 = glm::vec2(p4v)/p4v.w;

                impact.corner2_1 = glm::vec2(p2v)/p2v.w;
                impact.corner2_2 = glm::vec2(p3v)/p3v.w;
            }

            if(closestCollisionSide == 1) //Left side
            {
                impact.corner1_1 = glm::vec2(p1v)/p1v.w;
                impact.corner1_2 = glm::vec2(p2v)/p2v.w;

                impact.corner2_1 = glm::vec2(p4v)/p4v.w;
                impact.corner2_2 = glm::vec2(p3v)/p3v.w;
            }

            if(closestCollisionSide == 2) //Bottom side
            {
                impact.corner1_1 = glm::vec2(p4v)/p4v.w;
                impact.corner1_2 = glm::vec2(p1v)/p1v.w;

                impact.corner2_1 = glm::vec2(p3v)/p3v.w;
                impact.corner2_2 = glm::vec2(p2v)/p2v.w;
            }

            if(closestCollisionSide == 3) //Right side
            {
                impact.corner1_1 = glm::vec2(p2v)/p2v.w;
                impact.corner1_2 = glm::vec2(p1v)/p1v.w;

                impact.corner2_1 = glm::vec2(p3v)/p3v.w;
                impact.corner2_2 = glm::vec2(p4v)/p4v.w;
            }
        }

        if(closestBody->isDisk)
        {
            auto coVect = glm::vec2(normalizedRayVect.y,-normalizedRayVect.x);
            float squareLength = rayThickness+closestBody->radius;

            impact.corner1_1 = closestImpact + coVect * squareLength;
            impact.corner1_2 = impact.corner1_1 + normalizedRayVect * squareLength;

            impact.corner2_1 = closestImpact - coVect * squareLength;
            impact.corner2_2 = impact.corner2_1 + normalizedRayVect * squareLength;
        }
    }

    return impact;
}


std::pair<int, glm::vec2> PhysicsEngine::testRayCollision(glm::vec2 startPoint, glm::vec2 normalizedRayVect,
                                                            float rayThickness, RigidBody *body)
{
    int collisionSide = -1;
    glm::vec2 collisionPos(0);

    if(body->isDisk)
    {
        auto bodyCenterPos = body->transform->getGlobalXYPosition();
        auto relBodyCenterPos = bodyCenterPos - startPoint;

        //We project body center on ray vector
        auto projectedCenter = glm::dot(relBodyCenterPos, normalizedRayVect) * normalizedRayVect;

        auto perpVect = relBodyCenterPos - projectedCenter;
        float squaredPerpVectLength = glm::dot(perpVect,perpVect);
        float sumRadii = rayThickness + body->radius;

        if(squaredPerpVectLength < sumRadii*sumRadii)
        {
            collisionSide = 1;
            collisionPos = projectedCenter - normalizedRayVect * sumRadii;
            collisionPos += startPoint;
        }
    }
    else if(body->isBox)
    {
        auto augmentedBox = body->box;
        augmentedBox.size += glm::vec2((rayThickness) * 2);
        augmentedBox.center += glm::vec2(rayThickness);

        auto invMat = body->transform->getInvModelMatrix();

        auto relStartPoint = invMat * glm::vec4(startPoint.x, startPoint.y, 0, 1);
        relStartPoint /= relStartPoint.w;
        //relStartPoint += glm::vec4(body->box.center.x, body->box.center.y, 0, 0);

        auto relRayVect = invMat * glm::vec4(normalizedRayVect.x, normalizedRayVect.y, 0, 1);
        relRayVect /= relRayVect.w;
        relRayVect -= invMat * glm::vec4(0,0,0,1);//glm::vec4(body->node->getGlobalXYPosition().x, body->node->getGlobalXYPosition().y, 0, 0);

        float minRayRatio = -1;

        {
            auto [intersected, rayRatio]
                = MathTools::computeRayToLineSegmentIntersection(relStartPoint, relRayVect,
                                                                -augmentedBox.center, glm::vec2(augmentedBox.size.x,0));
            if(intersected && (rayRatio < minRayRatio || minRayRatio == -1))
            {
                minRayRatio = rayRatio;
                collisionSide = 0; //top side
            }
        }

        {
            auto [intersected, rayRatio]
                = MathTools::computeRayToLineSegmentIntersection(relStartPoint, relRayVect,
                                                                -augmentedBox.center, glm::vec2(0,augmentedBox.size.y));
            if(intersected && (rayRatio < minRayRatio || minRayRatio == -1))
            {
                minRayRatio = rayRatio;
                collisionSide = 1 ; //left side
            }
        }

        {
            auto [intersected, rayRatio]
                = MathTools::computeRayToLineSegmentIntersection(relStartPoint, relRayVect,
                                                                -augmentedBox.center+glm::vec2(0,augmentedBox.size.y), glm::vec2(augmentedBox.size.x,0));
            if(intersected && (rayRatio < minRayRatio || minRayRatio == -1))
            {
                minRayRatio = rayRatio;
                collisionSide = 2; //bottom side
            }
        }

        {
            auto [intersected, rayRatio]
                = MathTools::computeRayToLineSegmentIntersection(relStartPoint, relRayVect,
                                                                -augmentedBox.center+glm::vec2(augmentedBox.size.x, 0), glm::vec2(0,augmentedBox.size.y));
            if(intersected && (rayRatio < minRayRatio || minRayRatio == -1))
            {
                minRayRatio = rayRatio;
                collisionSide = 3; //right side
            }
        }

        if(minRayRatio != -1)
            return {collisionSide, startPoint + (minRayRatio - 5 /*- rayThickness*/) * normalizedRayVect};
    }

    return {collisionSide, collisionPos};
}


bool PhysicsEngine::detectCollisionWithBoxImpl(TransformComponent *transform, const MathTools::Box &box, float minMass)
{
    auto globalPos = transform->getGlobalXYPosition();
    auto maxSize = std::max(box.size.x, box.size.y) * sqrt(2);

    float leftMost = globalPos.x - maxSize;
    float rightMost= globalPos.x + maxSize;

    for(auto bodyIt = m_oldRigidBodies.begin() ;
        bodyIt != m_oldRigidBodies.end() && bodyIt->first < rightMost ;
        ++bodyIt)
    {
        auto &body = bodyIt->second;
        if(body.estimatedRightMost < leftMost)
            continue;
        if((minMass == -1 && body.mass != -1) || (minMass > body.mass && body.mass != -1))
            continue;

        if(body.isBox)
        {
           // if(MathTools::detectBoxCollision(box, bodyIt->second.box,
            //                                 transform, bodyIt->second.transform))
            if(MathTools::detectBoxCollision(bodyIt->second.box, box,
                                             bodyIt->second.transform, transform))
                return (true);
        } else if(body.isDisk) {
            /** do something **/
        }
    }

    return (false);
}



}
