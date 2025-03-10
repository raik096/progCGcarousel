#ifndef MATERIAL_aabb_H
#define MATERIAL_aabb_H

#include <algorithm>
#include <glm/glm.hpp>

/// Standalone axis aligned bounding box implemented built on top of GLM.
class AABB
{
public:
    /// Builds a null AABB.
    AABB();

    /// Builds an AABB that encompasses a sphere.
    /// \param[in]  center Center of the sphere.
    /// \param[in]  radius Radius of the sphere.
    AABB(const glm::vec3& center, float radius);

    /// Builds an AABB that contains the two points.
    AABB(const glm::vec3& p1, const glm::vec3& p2);

    AABB(const AABB& aabb);
    ~AABB() = default;

    /// Set the AABB as NULL (not set).
    void setNull() { mMin = glm::vec3(1.0); mMax = glm::vec3(-1.0); }

    /// Returns true if AABB is NULL (not set).
    bool isNull() const { return mMin.x > mMax.x || mMin.y > mMax.y || mMin.z > mMax.z; }

    /// Extend the bounding box on all sides by \p val.
    void extend(float val);

    /// Expand the AABB to include point \p p.
    void extend(const glm::vec3& p);

    /// Expand the AABB to include a sphere centered at \p center and of radius \p
    /// radius.
    /// \param[in]  center Center of sphere.
    /// \param[in]  radius Radius of sphere.
    void extend(const glm::vec3& center, float radius);

    /// Expand the AABB to encompass the given \p aabb.
    void extend(const AABB& aabb);

    /// Expand the AABB to include a disk centered at \p center, with normal \p
    /// normal, and radius \p radius.
    /// \xxx Untested -- This function is not represented in our unit tests.
    void extendDisk(const glm::vec3& center, const glm::vec3& normal,
        float radius);

    /// Translates AABB by vector \p v.
    void translate(const glm::vec3& v);

    /// Scale the AABB by \p scale, centered around \p origin.
    /// \param[in]  scale  3D vector specifying scale along each axis.
    /// \param[in]  origin Origin of scaling operation. Most useful origin would
    ///                    be the center of the AABB.
    void scale(const glm::vec3& scale, const glm::vec3& origin);

    /// Retrieves the center of the AABB.
    glm::vec3 getCenter() const;

    /// Retrieves the diagonal vector (computed as mMax - mMin).
    /// If the AABB is NULL, then a vector of all zeros is returned.
    glm::vec3 getDiagonal() const;

    /// Retrieves the longest edge.
    /// If the AABB is NULL, then 0 is returned.
    float getLongestEdge() const;

    /// Retrieves the shortest edge.
    /// If the AABB is NULL, then 0 is returned.
    float getShortestEdge() const;

    /// Retrieves the AABB's minimum point.
    glm::vec3 getMin() const { return mMin; }

    /// Retrieves the AABB's maximum point.
    glm::vec3 getMax() const { return mMax; }

    /// Returns true if AABBs share a face overlap.
    /// \xxx Untested -- This function is not represented in our unit tests.
    bool overlaps(const AABB& bb) const;

    /// Type returned from call to intersect.
    enum INTERSECTION_TYPE { INSIDE, INTERSECT, OUTSIDE };
    /// Returns one of the intersection types. If either of the aabbs are invalid,
    /// then OUTSIDE is returned.
    INTERSECTION_TYPE intersect(const AABB& bb) const;

    /// Function from SCIRun. Here is a summary of SCIRun's description:
    /// Returns true if the two AABB's are similar. If diff is 1.0, the two
    /// bboxes have to have about 50% overlap each for x,y,z. If diff is 0.0,
    /// they have to have 100% overlap.
    /// If either of the two AABBs is NULL, then false is returned.
    /// \xxx Untested -- This function is not represented in our unit tests.
    bool isSimilarTo(const AABB& b, float diff = 0.5) const;

private:

    glm::vec3 mMin;   ///< Minimum point.
    glm::vec3 mMax;   ///< Maximum point.
};

inline AABB::AABB()
{
    setNull();
}

inline AABB::AABB(const glm::vec3& center, float radius)
{
    setNull();
    extend(center, radius);
}

inline AABB::AABB(const glm::vec3& p1, const glm::vec3& p2)
{
    setNull();
    extend(p1);
    extend(p2);
}

inline AABB::AABB(const AABB& aabb)
{
    setNull();
    extend(aabb);
}

inline void AABB::extend(float val)
{
    if (!isNull())
    {
        mMin -= glm::vec3(val);
        mMax += glm::vec3(val);
    }
}

inline void AABB::extend(const glm::vec3& p)
{
    if (!isNull())
    {
        mMin = glm::min(p, mMin);
        mMax = glm::max(p, mMax);
    }
    else
    {
        mMin = p;
        mMax = p;
    }
}

inline void AABB::extend(const glm::vec3& p, float radius)
{
    glm::vec3 r(radius);
    if (!isNull())
    {
        mMin = glm::min(p - r, mMin);
        mMax = glm::max(p + r, mMax);
    }
    else
    {
        mMin = p - r;
        mMax = p + r;
    }
}

inline void AABB::extend(const AABB& aabb)
{
    if (!aabb.isNull())
    {
        extend(aabb.mMin);
        extend(aabb.mMax);
    }
}

inline void AABB::extendDisk(const glm::vec3& c, const glm::vec3& n, float r)
{
    if (glm::length(n) < 1.e-12) { extend(c); return; }
    glm::vec3 norm = glm::normalize(n);
    float x = sqrt(1 - norm.x) * r;
    float y = sqrt(1 - norm.y) * r;
    float z = sqrt(1 - norm.z) * r;
    extend(c + glm::vec3(x, y, z));
    extend(c - glm::vec3(x, y, z));
}

inline glm::vec3 AABB::getDiagonal() const
{
    if (!isNull())
        return mMax - mMin;
    else
        return glm::vec3(0);
}

inline float AABB::getLongestEdge() const
{
    glm::vec3 d = getDiagonal();
    return std::max(d.x, std::max(d.y, d.z));
}

inline float AABB::getShortestEdge() const
{
    glm::vec3 d = getDiagonal();
    return std::min(d.x, std::min(d.y, d.z));
}

inline glm::vec3 AABB::getCenter() const
{
    if (!isNull())
    {
        glm::vec3 d = getDiagonal();
        return mMin + (d * float(0.5f));
    }
    else
    {
        return glm::vec3(0.0);
    }
}

inline void AABB::translate(const glm::vec3& v)
{
    if (!isNull())
    {
        mMin += v;
        mMax += v;
    }
}

inline void AABB::scale(const glm::vec3& s, const glm::vec3& o)
{
    if (!isNull())
    {
        mMin -= o;
        mMax -= o;

        mMin *= s;
        mMax *= s;

        mMin += o;
        mMax += o;
    }
}

inline bool AABB::overlaps(const AABB& bb) const
{
    if (isNull() || bb.isNull())
        return false;

    if (bb.mMin.x > mMax.x || bb.mMax.x < mMin.x)
        return false;
    else if (bb.mMin.y > mMax.y || bb.mMax.y < mMin.y)
        return false;
    else if (bb.mMin.z > mMax.z || bb.mMax.z < mMin.z)
        return false;

    return true;
}

inline AABB::INTERSECTION_TYPE AABB::intersect(const AABB& b) const
{
    if (isNull() || b.isNull())
        return OUTSIDE;

    if ((mMax.x < b.mMin.x) || (mMin.x > b.mMax.x) ||
        (mMax.y < b.mMin.y) || (mMin.y > b.mMax.y) ||
        (mMax.z < b.mMin.z) || (mMin.z > b.mMax.z))
    {
        return OUTSIDE;
    }

    if ((mMin.x <= b.mMin.x) && (mMax.x >= b.mMax.x) &&
        (mMin.y <= b.mMin.y) && (mMax.y >= b.mMax.y) &&
        (mMin.z <= b.mMin.z) && (mMax.z >= b.mMax.z))
    {
        return INSIDE;
    }

    return INTERSECT;
}

inline bool AABB::isSimilarTo(const AABB& b, float diff) const
{
    if (isNull() || b.isNull()) return false;

    glm::vec3 acceptable_diff = ((getDiagonal() + b.getDiagonal()) / float(2.0f)) * diff;
    glm::vec3 min_diff(mMin - b.mMin);
    min_diff = glm::vec3(fabs(min_diff.x), fabs(min_diff.y), fabs(min_diff.z));
    if (min_diff.x > acceptable_diff.x) return false;
    if (min_diff.y > acceptable_diff.y) return false;
    if (min_diff.z > acceptable_diff.z) return false;
    glm::vec3 max_diff(mMax - b.mMax);
    max_diff = glm::vec3(fabs(max_diff.x), fabs(max_diff.y), fabs(max_diff.z));
    if (max_diff.x > acceptable_diff.x) return false;
    if (max_diff.y > acceptable_diff.y) return false;
    if (max_diff.z > acceptable_diff.z) return false;
    return true;
}

#endif