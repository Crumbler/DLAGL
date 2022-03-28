#include "DLAModel.hpp"
#include "utils.hpp"

#include <glm/gtx/norm.hpp>

constexpr float DefaultAttractionDistance = 1.5f,
                DefaultParticleSpacing = DefaultAttractionDistance / 3.0f,
                DefaultMinMoveDistance = DefaultParticleSpacing,
                maxSize = 100.0f,
                maxR = sqrt(maxSize * maxSize);

DLAModel::DLAModel() :
        m_ParticleSpacing(DefaultParticleSpacing),
        m_AttractionDistance(DefaultAttractionDistance),
        m_MinMoveDistance(DefaultMinMoveDistance)
{
    m_Index = new tKDTree(2, m_Points, nanoflann::KDTreeSingleIndexAdaptorParams(10 /*max leaf*/));
}

void DLAModel::Reset()
{
    m_Points.pts.clear();
    m_Parents.clear();

    delete m_Index;
    m_Index = new tKDTree(2, m_Points, nanoflann::KDTreeSingleIndexAdaptorParams(10));

    m_ParentDistances.clear();

    for (const glm::vec2& p : base.points)
        Add(p);
}


void DLAModel::SetParticleSpacing(const float a)
{
    m_ParticleSpacing = a;
}


void DLAModel::SetAttractionDistance(const float a)
{
    m_AttractionDistance = a;
}


void DLAModel::SetMinMoveDistance(const float a)
{
    m_MinMoveDistance = a;
}

const std::vector<glm::vec2>& DLAModel::GetPoints() const
{
    return m_Points.pts;
}


const std::vector<int>& DLAModel::GetParents() const
{
    return m_Parents;
}

const std::vector<int>& DLAModel::GetDistances() const
{
    return m_ParentDistances;
}

float DLAModel::GetPointRadius() const
{
    return m_ParticleSpacing / 2.0f;
}

float DLAModel::GetMaxBound() const
{
    return maxSize;
}

int DLAModel::GetParentDist(const int ind) const
{
    return m_ParentDistances[ind];
}

float DLAModel::DistToClosest(const glm::vec2& p) const
{
    float res = 300.0f;

    if (m_Points.pts.size())
    {
        size_t ret_index;
        float out_dist_sqr = m_AttractionDistance;
        nanoflann::KNNResultSet<float> resultSet(1);
        resultSet.init(&ret_index, &out_dist_sqr );
        m_Index->findNeighbors(resultSet, (const float *)&p,
                               nanoflann::SearchParams(0,
                                                       m_AttractionDistance * 0.5f));

        res = sqrt(out_dist_sqr);
    }

    for (const Line& l : base.lines)
        res = std::min(res, Utils::sdSegment(p, l.a, l.b));

    for (const Circle& circle : base.circles)
        res = std::min(res, Utils::sdCircle(p, circle));

    for (const Box& box : base.boxes)
        res = std::min(res, Utils::sdBox(p, box));

    for (const Bezier& bezier : base.beziers)
        res = std::min(res, Utils::sdBezier(p, bezier));

    return res;
}

void DLAModel::Add(const glm::vec2& p, const int parent)
{
    const size_t id = m_Points.pts.size();

    if (parent == -1)
        m_ParentDistances.push_back(0);
    else
        m_ParentDistances.push_back(m_ParentDistances[parent] + 1);


    m_Parents.push_back(parent);

    m_Points.pts.push_back(p);

    m_Index->addPoints(id, id);
}


glm::vec2 DLAModel::Nearest(const glm::vec2& p, int& parentInd) const
{
    glm::vec2 res(300.0f);
    float dist = 300.0f;

    if (m_Points.pts.size())
    {
        size_t ret_index;
        float out_dist_sqr = m_AttractionDistance;
        nanoflann::KNNResultSet<float> resultSet(1);
        resultSet.init(&ret_index, &out_dist_sqr );
        m_Index->findNeighbors(resultSet, (const float *)&p,
                                nanoflann::SearchParams(0,
                                                        m_AttractionDistance * 0.5f));

        parentInd = ret_index;
        res = m_Points.pts[ret_index];
        dist = glm::distance2(p, res);
    }

    for (const Line& v : base.lines)
    {
        const glm::vec2 linePoint = Utils::sdSegmentPoint(p, v.a, v.b);
        const float newDist = glm::distance2(p, linePoint) - 0.04f;

        if (newDist < dist)
        {
            parentInd = -1;
            dist = newDist;
            res = linePoint;
        }
    }

    for (const Circle& circle : base.circles)
    {
        const glm::vec2 circlePoint = Utils::sdCirclePoint(p, circle);
        const float newDist = glm::distance2(p, circlePoint);

        if (newDist < dist)
        {
            parentInd = -1;
            dist = newDist;
            res = circlePoint;
        }
    }

    for (const Box& box : base.boxes)
    {
        const glm::vec2 boxPoint = Utils::sdBoxPoint(p, box);
        const float newDist = glm::distance2(p, boxPoint);

        if (newDist < dist)
        {
            parentInd = -1;
            dist = newDist;
            res = boxPoint;
        }
    }

    for (const Bezier& bezier : base.beziers)
    {
        const glm::vec2 bezierPoint = Utils::sdBezierPoint(p, bezier);
        const float newDist = glm::distance2(p, bezierPoint) - 0.04f;

        if (newDist < dist)
        {
            parentInd = -1;
            dist = newDist;
            res = bezierPoint;
        }
    }

    return res;
}


glm::vec2 DLAModel::RandomStartPos(bool& stop) const
{
    glm::vec2 p;
    int tries = 500;
    stop = false;

    do
    {
        p = glm::vec2(Utils::Random(-1.0f, 1.0f), Utils::Random(-1.0f, 1.0f)) * maxSize;

        --tries;

        if (!tries)
        {
            stop = true;
            return glm::vec2();
        }
    } while(DistToClosest(p) <= m_AttractionDistance * 2.0f);

    return p;
}


bool DLAModel::ShouldReset(const glm::vec2& p) const
{
    return std::max(std::abs(p.x), std::abs(p.y)) > maxSize;
}


glm::vec2 DLAModel::PlaceParticle(const glm::vec2& p, const glm::vec2& parentP) const
{
    return Utils::Lerp(parentP, p, m_ParticleSpacing);
}

bool DLAModel::AddParticles(const int count)
{
    for (int i = 0; i < count; ++i)
    {
        bool stop;

        glm::vec2 p = RandomStartPos(stop);

        if (stop)
            return false;

        while (true)
        {
            int parentInd;

            const glm::vec2 nearestPoint = Nearest(p, parentInd);

            const float d = glm::distance(p, nearestPoint);

            if (d < m_AttractionDistance)
            {
                p = PlaceParticle(p, nearestPoint);

                Add(p, parentInd);

                break;
            }

            const float m = std::max(m_MinMoveDistance,
                                     d - m_AttractionDistance);

            p += glm::normalize(Utils::RandInUnitSphere()) * m;

            if (ShouldReset(p))
            {
                p = RandomStartPos(stop);

                if (stop)
                    return false;
            }
        }
    }

    return true;
}

