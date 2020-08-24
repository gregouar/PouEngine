#ifndef MATHTOOLS_H
#define MATHTOOLS_H

#include "PouEngine/math/TransformComponent.h"
#include "PouEngine/tools/RNGesus.h"

namespace pou::MathTools
{

struct Box
{
    glm::vec2 size;
    glm::vec2 center;
};

bool detectSegmentCollision(glm::vec2 seg1, glm::vec2 seg2);

bool detectBoxCollision(const Box &box1, const Box &box2,
                        const TransformComponent *transform1 = nullptr, const TransformComponent *transform2 = nullptr);

bool isInBox(glm::vec2 position, const Box &box, const TransformComponent *boxTransform = nullptr);

std::pair<bool, float> computeRayToLineSegmentIntersection(glm::vec2 startRay, glm::vec2 directionRay,
                                                        glm::vec2 startLineSegment, glm::vec2 directionLineSegment);


int intLog2(int v);


float computeWantedRotation(float startingRotation, glm::vec2 position);

template<typename T>
T computeWantedRotation(T startingRotation, T finalRotation, bool useRad)
{
    T modulo = useRad ? T(glm::pi<float>()*2.0f) : T(360.0f);

    finalRotation += modulo * T(glm::greaterThan(glm::abs(finalRotation-startingRotation), glm::abs(finalRotation-startingRotation+modulo)));
    finalRotation -= modulo * T(glm::greaterThan(glm::abs(finalRotation-startingRotation), glm::abs(finalRotation-startingRotation-modulo)));

    return finalRotation;
}

float quintic(float v);

//std::vector<float>
void    generatePerlinNoise(std::vector<float> &grid,
                            glm::ivec2 noiseSize, int kernelSize, float intensityFactor=1.0f,
                            RNGenerator *rng = nullptr, bool tilable = false);

}

bool uint32less(uint32_t lhs, uint32_t rhs);
bool uint32leq(uint32_t lhs, uint32_t rhs);
uint32_t uint32max(uint32_t lhs, uint32_t rhs);


#endif // MATHTOOLS_H
