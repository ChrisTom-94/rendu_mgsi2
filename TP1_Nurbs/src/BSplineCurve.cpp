#include "BSplineCurve.h"
#include "Core/Log.h"

#include "glm/gtc/constants.hpp"

#include <iostream>
#include <algorithm>

BSplineCurve::BSplineCurve(uint8_t degree)
    : m_Attributes(BSplineAttributes(degree))
{
    InitKnotVector();
}

BSplineCurve::BSplineCurve(uint8_t degree, std::vector<glm::vec3> controlPoints)
    : m_ControlPoints(controlPoints), m_Attributes(BSplineAttributes(degree))
{
    InitKnotVector();
}

BSplineCurve::~BSplineCurve()
{
    m_ControlPoints.clear();
    m_Attributes.Knots.clear();
    m_Points.clear();
    m_Knots.clear();
}

void BSplineCurve::Evaluate()
{
    int nbControlPoints = m_ControlPoints.size();
    int nbPoints = m_Precision * nbControlPoints;
    uint8_t degree = m_Attributes.Degree;

    m_Points.clear();
    m_Points.resize(nbPoints);

    float delta = m_Attributes.Knots[nbControlPoints] - m_Attributes.Knots[degree];

    for (int i = 0; i < nbPoints; i++) // precision
        for (int j = 0; j < nbControlPoints; j++)
        {
            float t = m_Attributes.Knots[degree] + ((float)i * delta) / (float)nbPoints;

            float weight = ComputeWeight(j, degree, t);
            m_Points[i] += weight * m_ControlPoints[j];
        }
}

glm::vec3 BSplineCurve::EvaluateAt(float t) const
{
    uint8_t degree = m_Attributes.Degree;

    glm::vec3 point(0.0f);

    // compute the point on the curve at t
    // by summing the weighted control points
    for (int i = 0; i < m_ControlPoints.size(); ++i)
        point += ComputeWeight(i, degree, t) * m_ControlPoints[i];

    return point;
}

void BSplineCurve::InitKnotVector()
{
    int numControlPoints = m_ControlPoints.size();

    // the number of knots is equal to the number of control points + the degree + 1
    int numKnots = numControlPoints + m_Attributes.Order;

    m_Attributes.Knots.clear();
    m_Attributes.Knots.resize(numKnots);

    for (int i = 0; i < numKnots; ++i)
    {
        switch (m_Type)
        {
        case BSplineType::Uniform:
            m_Attributes.Knots[i] = static_cast<float>(i);
            break;

        case BSplineType::OpenUniform:
            if (i <= m_Attributes.Degree)
                m_Attributes.Knots[i] = 0.0f; // Repeat the first knot
            else if (m_Attributes.Degree < i && i <= m_Attributes.Knots.size() - m_Attributes.Degree - 1)
                m_Attributes.Knots[i] = static_cast<float>(i - m_Attributes.Degree);
            else
                m_Attributes.Knots[i] = static_cast<float>(numControlPoints - m_Attributes.Order + 2); // Repeat the last knot
            break;
            exit(1);

        default:
            SMART_LOG_CRITICAL("Unknown BSplineType");
            exit(1);
            break;
        }
    }
}

float BSplineCurve::ComputeWeight(int index, uint8_t degree, float t) const
{

    // get the knots of the current segment
    float currentKnot = m_Attributes.Knots[index];
    float nextKnot = m_Attributes.Knots[index + 1];

    // if the degree is 0, the weight is 1 if t is between the current knot and the next knot, 0 otherwise
    // that's the stopping condition of the recursive function
    if (degree == 0)
        return (t >= currentKnot && t < nextKnot) ? 1.0f : 0.0f;

    float weight = 0.0f;

    // the Box-Cox formula

    float denominator = m_Attributes.Knots[index + degree] - currentKnot;

    // current knot
    if (denominator != 0.0f)
        weight = (t - currentKnot) / denominator * ComputeWeight(index, degree - 1, t);

    denominator = m_Attributes.Knots[index + degree + 1] - nextKnot;

    // next knot
    if (denominator != 0.0f)
        weight += (m_Attributes.Knots[index + degree + 1] - t) / denominator * ComputeWeight(index + 1, degree - 1, t);

    return weight;
}

CurveFrenetFrameComponents BSplineCurve::GetFrenetFrameAt(float t) const
{
    auto [Velocity, Acceleration] = GetFiniteDifferencesDerivatives(t);

    glm::vec3 tangent = glm::normalize(Velocity);
    glm::vec3 normal = glm::normalize(glm::cross(Velocity, glm::cross(Acceleration, Velocity)));
    glm::vec3 binormal = glm::cross(tangent, normal);

    return CurveFrenetFrameComponents{tangent, normal, binormal};
}

float BSplineCurve::GetCurvatureAt(float t) const
{
    auto [Velocity, Acceleration] = GetFiniteDifferencesDerivatives(t);

    float curvature = glm::length(glm::cross(Velocity, Acceleration)) / glm::pow<float>(glm::length(Velocity), 3);
    return curvature;
}

CurveDerivativesComponenets BSplineCurve::GetFiniteDifferencesDerivatives(float t) const
{
    float epsilon = 1e-2f;

    glm::vec3 current_t = EvaluateAt(t);
    glm::vec3 next_t = EvaluateAt(t + epsilon);
    glm::vec3 prev_t = EvaluateAt(t - epsilon);

    glm::vec3 velocity = (next_t - prev_t) / (2.0f * epsilon);
    glm::vec3 acceleration = (next_t - 2.0f * current_t + prev_t) / (epsilon * epsilon);

    return CurveDerivativesComponenets{velocity, acceleration};
}
