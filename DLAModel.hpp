#pragma once

#include <vector>
#include <string>
#include "pointcloud.hpp"
#include "DLABase.hpp"

class DLAModel
{
public:
    DLAModel();

    void Reset();
    void SetParticleSpacing(const float a);
    void SetAttractionDistance(const float a);
    void SetMinMoveDistance(const float a);
    const std::vector<glm::vec2>& GetPoints() const;
    const std::vector<int>& GetParents() const;
    const std::vector<int>& GetDistances() const;
    float GetBoundingRadius() const;
    float GetPointRadius() const;
    float GetMaxBound() const;
    int GetParentDist(const int ind) const;
    float DistToClosest(const glm::vec2& p) const;
    void Add(const glm::vec2& p, const int parent = -1);
    glm::vec2 Nearest(const glm::vec2& p, int& parentInd) const;
    glm::vec2 RandomStartPos(bool& stop) const;
    bool ShouldReset(const glm::vec2& p) const;
    glm::vec2 PlaceParticle(const glm::vec2& p, const glm::vec2& parentP) const;
    bool AddParticles(const int count);

    DLABase base;

private:
    /* m_ParticleSpacing defines the distance between particles that are
       joined together

       m_AttractionDistance defines how close together particles must be in
       order to join together

       m_MinMoveDistance defines the minimum distance that a particle will move
       during its random walk */
    float m_ParticleSpacing, m_AttractionDistance, m_MinMoveDistance;

    // m_BoundingRadius defines the radius of the bounding sphere that bounds
    // all of the particles

    // m_JoinAttempts tracks how many times other particles have attempted to
    // join with each finalized particle
    std::vector<int> m_Parents, m_ParentDistances;

    // m_Index is the spatial index used to accelerate nearest neighbor queries
    //Index m_Index;

    // m_Index is the spatial index used to accelerate nearest neighbor queries
    tKDTree* m_Index;
    // m_Points stores the final particle positions
    tPointCloud m_Points;
};
