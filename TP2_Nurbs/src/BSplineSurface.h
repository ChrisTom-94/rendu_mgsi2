#pragma once

#include "SmartGL.h"
#include "BSplineCurve.h"

/**
 * @brief Data used to store the attributes of the BSpline surface
 */
struct BSplineSurfaceAttributes
{
    BSplineAttributes U;
    BSplineAttributes V;

    BSplineSurfaceAttributes(uint8_t degree_u, uint8_t degree_v)
        : U(BSplineAttributes(degree_u)), V(BSplineAttributes(degree_v)) {}
};

/**
 * @brief Data used to store the components of the Frenet frame
 */
struct SurfaceFrenetFrameComponents
{
    CurveFrenetFrameComponents U;
    CurveFrenetFrameComponents V;
};

/**
 * @brief Data used to store the components of the surface derivatives
 */
struct SurfaceDerivativesComponenets
{
    CurveDerivativesComponenets U;
    CurveDerivativesComponenets V;
};

/**
 * @brief Data used to store the components of the surface curvatures
 */
struct SurfaceCurvaturesComponents
{
    float MeanCurvature;
    float GaussianCurvature;
    float AbsoluteCurvature;
};

class BSplineSurface
{
public:
    BSplineSurface() = default;
    BSplineSurface(BSplineSurfaceAttributes attributes);
    ~BSplineSurface();

    /**
     * @brief Evaluate all the points of the B-Spline surface
     */
    void Evaluate();

    /**
     * @brief Evaluate the Frenet frame at all the points of the B-Spline surface to compute the curvatures (Mean, Gaussian, Absolute)
    */
    void EvaluateCurvatures();

    /**
     * @brief Evaluate the B-Spline surface at a given u and v
     * @param u A value between the minimum value and the maximum value of the knots vector
     * @param v A value between the minimum value and the maximum value of the knots vector
     * @return The point on the surface at u and v
     */
    glm::vec3 EvaluateAt(float u, float v) const;

    /**
     * @brief Evaluate the Frenet frame at a given u and v
     * @param u A value between the minimum value and the maximum value of the knots vector
     * @param v A value between the minimum value and the maximum value of the knots vector
     * @return The tangent, normal and binormal vectors at u and v
     */
    SurfaceFrenetFrameComponents GetFrenetFrameAt(float u, float v);

    /**
     * @brief Compute the curvature at a given u and v
     * @param u A value between the minimum value and the maximum value of the knots vector
     * @param v A value between the minimum value and the maximum value of the knots vector
     * @return The curvature at u and v
     */
    SurfaceCurvaturesComponents GetCurvaturesAt(float u, float v);

    /**
     * @brief Compute the knots vector based on the type of B-Spline (uniform, open uniform)
     */
    void InitKnotVector();

    inline void SetAttributes(const BSplineSurfaceAttributes &attributes) { m_Attributes = attributes; }
    inline void SetControlPoints(const std::vector<std::vector<glm::vec3>> &controlPoints) { m_ControlPoints = controlPoints; }
    inline void SetKnots(const std::vector<float> &knotsU, const std::vector<float> &knotsV)
    {
        m_Attributes.U.Knots = knotsU;
        m_Attributes.V.Knots = knotsV;
    }
    inline void SetKnotsV(const std::vector<float> &knotsV) { m_Attributes.U.Knots = knotsV; }
    inline void SetKnotsU(const std::vector<float> &knotsU) { m_Attributes.V.Knots = knotsU; }

    inline const BSplineSurfaceAttributes &GetAttributes() const { return m_Attributes; }
    inline const std::vector<std::vector<glm::vec3>> &GetControlPoints() const { return m_ControlPoints; }
    inline const std::vector<std::vector<glm::vec3>> &GetPoints() const { return m_Points; }
    inline const std::vector<std::vector<float>> &GetCurvatures() const { return m_Curvatures; }

    inline void SetPrecision(uint32_t precision) { m_Precision = precision; }

    inline float GetMinT_U() const { return m_Attributes.U.Knots[m_Attributes.U.Degree]; }
    inline float GetMaxT_U() const { return m_Attributes.U.Knots[m_Attributes.U.Knots.size() - m_Attributes.U.Degree - 1]; }
    inline float GetMinT_V() const { return m_Attributes.V.Knots[m_Attributes.V.Degree]; }
    inline float GetMaxT_V() const { return m_Attributes.V.Knots[m_Attributes.V.Knots.size() - m_Attributes.V.Degree - 1]; }

private:
    /**
     * @brief Compute the weight of a control point at a given index and a given t
     * @param knots The knots vector
     * @param index The index of the control point
     * @param degree The degree of the B-Spline
     * @param t The value of t
     */
    float ComputeWeight(const std::vector<float> &knots, int index, uint8_t degree, float t) const;

    /**
     * @brief Comute the partial derivatives of the B-Spline surface at a given u and v
     * @param u The value of u
     * @param v The value of v
     */
    SurfaceDerivativesComponenets GetFiniteDifferencesPartialDerivatives(float u, float v) const;

    /**
     * @brief Comute the derivatives of the B-Spline surface at a given u and v
     * @param u The value of u
     * @param v The value of v
     */
    CurveDerivativesComponenets GetFiniteDifferencesDerivatives(float u, float v) const;

private:
    BSplineSurfaceAttributes m_Attributes;
    std::vector<std::vector<glm::vec3>> m_ControlPoints;
    std::vector<std::vector<glm::vec3>> m_Points;
    std::vector<std::vector<float>> m_Curvatures;

    uint32_t m_Precision = 6;
    BSplineType m_Type = BSplineType::Uniform;
};
