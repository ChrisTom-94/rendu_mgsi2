#pragma once

#include <string>
#include <vector>

#include "glm/glm.hpp"

struct BSplineAttributes
{

    /**
     * @brief The degree of the B-Spline (cubic = 3, quartic = 4, quintic = 5, etc.)
     */
    uint8_t Degree = 3;

    /**
     * @brief The order of the B-Spline (degree + 1), the number of control points that influence each segment
     */
    uint8_t Order = 4;

    std::vector<float> Knots = std::vector<float>();

    BSplineAttributes(uint8_t degree)
        : Degree(degree), Order(degree + 1), Knots(std::vector<float>()){};
};

enum class BSplineType
{
    /**
     * @brief Uniform knots vector (Ex: [0, 0.33, 0.66, 1])
     */
    Uniform,

    /**
     * @brief Open uniform knots vector (Ex: [0, 0, 0.33, 0.66, 1, 1])
     * @note The courve starts at the first control point and ends at the last control point
     */
    OpenUniform,
};

struct CurveFrenetFrameComponents
{
    glm::vec3 Tangent;
    glm::vec3 Normal;
    glm::vec3 Binormal;
};

struct CurveDerivativesComponenets
{
    glm::vec3 Velocity;
    glm::vec3 Acceleration;
};

class BSplineCurve
{
public:
    BSplineCurve() = default;
    BSplineCurve(uint8_t degree);
    BSplineCurve(uint8_t degree, std::vector<glm::vec3> controlPoints);
    ~BSplineCurve();

    /**
     * @brief Evaluate all the points of the B-Spline curve
     */
    void Evaluate();

    /**
     * @brief Evaluate the B-Spline curve at a given t
     * @param t A value between the minimum value and the maximum value of the knots vector
     * @return The point on the curve at t
     */
    glm::vec3 EvaluateAt(float t) const;

    /**
     * @brief Evaluate the frenet frame at a given t
     * @param t A value between the minimum value and the maximum value of the knots vector
     * @return The tangent, normal and binormal vectors at t
     */
    CurveFrenetFrameComponents GetFrenetFrameAt(float t) const;

    /**
     * @brief Compute the knots vector based on the type of B-Spline (uniform, open uniform)
     */
    void InitKnotVector();

    /**
     * @brief Compute the curvature at a given t
     * @param t A value between the minimum value and the maximum value of the knots vector
     * @return The curvature at t
     */
    float GetCurvatureAt(float t) const;

    inline const std::vector<glm::vec3> &GetPoints() const { return m_Points; }

    inline void SetControlPoints(const std::vector<glm::vec3> &controlPoints)
    {
        m_ControlPoints = controlPoints;
        InitKnotVector();
    }
    inline const std::vector<glm::vec3> &GetControlPoints() const { return m_ControlPoints; }
    inline const std::size_t GetControlPointsCount() const { return m_ControlPoints.size(); }

    inline void SetKnotVector(std::vector<float> &knots)
    {
        m_Attributes.Knots = knots;
    }
    inline const std::vector<float> &GetKnotsVector() const { return m_Attributes.Knots; }
    inline const std::vector<glm::vec3> &GetKnotsPoints() const { return m_Knots; }

    inline void SetDegree(uint8_t degree)
    {
        m_Attributes.Degree = degree;
        m_Attributes.Order = degree + 1;
    }
    inline uint8_t GetDegree() const { return m_Attributes.Degree; }
    inline uint8_t GetOrder() const { return m_Attributes.Order; }

    inline uint32_t GetSegmentsCount() const { return (m_ControlPoints.size() + 1) - m_Attributes.Order; }

    inline void SetPrecision(uint16_t precision) { m_Precision = glm::clamp(precision, (uint16_t)64, (uint16_t)1024); }
    inline uint16_t GetPrecision() const { return m_Precision; }

    inline float GetMinT() const { return m_Attributes.Knots[m_Attributes.Degree]; }
    inline float GetMaxT() const { return m_Attributes.Knots[m_Attributes.Knots.size() - m_Attributes.Degree - 1]; }

private:
    /**
     * @brief Recursive function to compute the weight of a control point at a given t
     * @param index The index of the knot concerned by the segment
     * @param degree The current degree in the recursive function
     * @param t
     */
    float ComputeWeight(int index, uint8_t degree, float t) const;

    /**
     * @brief Compute the derivatives of the curve at a given t
     * @param t
     * @param count The number of derivatives to compute (default = 3)
     * @return The velocity and acceleration vectors at t
     */
    CurveDerivativesComponenets GetFiniteDifferencesDerivatives(float t) const;

private:
    BSplineType m_Type = BSplineType::Uniform;
    BSplineAttributes m_Attributes;

    std::vector<glm::vec3> m_ControlPoints;
    std::vector<glm::vec3> m_Points;
    std::vector<glm::vec3> m_Knots;

    uint16_t m_Precision = 256;
};
