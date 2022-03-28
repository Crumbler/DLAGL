#pragma once

#include <glm/glm.hpp>
#include <nanoflann.hpp>

struct PointCloud
{
    std::vector<glm::vec2> pts;

    // Must return the number of data points
    inline size_t kdtree_get_point_count() const { return pts.size(); }

    // Returns the dim'th component of the idx'th point in the class:
    // Since this is inlined and the "dim" argument is typically an immediate value, the
    //  "if/else's" are actually solved at compile time.
    inline float kdtree_get_pt(const size_t idx, const size_t dim) const
    {
        return pts[idx][dim];
    }


    // Optional bounding-box computation: return false to default to a standard bbox computation loop.
    //   Return true if the BBOX was already computed by the class and returned in "bb" so it can be avoided to redo it again.
    //   Look at bb.size() to find out the expected dimensionality (e.g. 2 or 3 for point clouds)
    template<class BBOX> bool kdtree_get_bbox(BBOX& /* bb */) const { return false; }

};

using tPointCloud = PointCloud;
using tKDTreeDistanceFunc = nanoflann::L2_Adaptor<float, tPointCloud>;
using tKDTree = nanoflann::KDTreeSingleIndexDynamicAdaptor<tKDTreeDistanceFunc, tPointCloud, 2>;
