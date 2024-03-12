#include "BSplineSurface.h"

BSplineSurface::BSplineSurface(BSplineSurfaceAttributes attributes)
    : m_Attributes(attributes)
{
}

BSplineSurface::~BSplineSurface()
{
    m_ControlPoints.clear();
    m_Attributes.U.Knots.clear();
    m_Attributes.V.Knots.clear();
    m_Points.clear();
    m_Curvatures.clear();
}

void BSplineSurface::Evaluate()
{
    int nbControlPointsU = m_ControlPoints.size();
    int nbControlPointsV = m_ControlPoints[0].size();
    int nbPointsU = nbControlPointsU * m_Precision;
    int nbPointsV = nbControlPointsV * m_Precision;

    uint8_t degreeU = m_Attributes.U.Degree;
    uint8_t degreeV = m_Attributes.V.Degree;
    auto knotsU = m_Attributes.U.Knots;
    auto knotsV = m_Attributes.V.Knots;

    m_Points.clear();
    m_Points.resize(nbPointsU, std::vector<glm::vec3>(nbPointsV, glm::vec3(0.0f)));

    float deltaU = knotsU[nbControlPointsU] - knotsU[degreeU];
    float deltaV = knotsV[nbControlPointsV] - knotsV[degreeV];

    for (int tDeltaU = 0; tDeltaU < nbPointsU; tDeltaU++) // precision
        for (int u = 0; u < nbControlPointsU; u++)
        {
            float tU = knotsU[degreeU] + ((float)tDeltaU * deltaU) / (float)nbPointsU;

            for (int tDeltaV = 0; tDeltaV < nbPointsV; tDeltaV++) // precision
                for (int v = 0; v < nbControlPointsV; v++)
                {
                    float tV = knotsV[degreeV] + ((float)tDeltaV * deltaV) / (float)nbPointsV;

                    float weight_u = ComputeWeight(knotsU, u, degreeU, tU);
                    float weight_v = ComputeWeight(knotsV, v, degreeV, tV);
                    m_Points[tDeltaU][tDeltaV] += weight_u * weight_v * m_ControlPoints[u][v];
                }
        }
}

void BSplineSurface::EvaluateCurvatures()
{
    int nbControlPointsU = m_ControlPoints.size();
    int nbControlPointsV = m_ControlPoints[0].size();
    int nbPointsU = nbControlPointsU * m_Precision;
    int nbPointsV = nbControlPointsV * m_Precision;

    uint8_t degreeU = m_Attributes.U.Degree;
    uint8_t degreeV = m_Attributes.V.Degree;
    auto knotsU = m_Attributes.U.Knots;
    auto knotsV = m_Attributes.V.Knots;

    float deltaU = knotsU[nbControlPointsU] - knotsU[degreeU];
    float deltaV = knotsV[nbControlPointsV] - knotsV[degreeV];

    m_Curvatures.clear();
    m_Curvatures.resize(m_Points.size(), std::vector<float>(m_Points[0].size(), 0.0f));

     for (int tDeltaU = 0; tDeltaU < nbPointsU; tDeltaU++) // precision
        for (int u = 0; u < nbControlPointsU; u++)
        {
            float tU = knotsU[degreeU] + ((float)tDeltaU * deltaU) / (float)nbPointsU;

            for (int tDeltaV = 0; tDeltaV < nbPointsV; tDeltaV++) // precision
                for (int v = 0; v < nbControlPointsV; v++)
                {
                    float tV = knotsV[degreeV] + ((float)tDeltaV * deltaV) / (float)nbPointsV;

                    m_Curvatures[tDeltaU][tDeltaV] = GetCurvaturesAt(tU, tV).GaussianCurvature;
                }
        }
}

glm::vec3 BSplineSurface::EvaluateAt(float u, float v) const
{
    glm::vec3 point(0.0f);

    int nbControlPointsU = m_ControlPoints.size();
    int nbControlPointsV = m_ControlPoints[0].size();
    uint8_t degreeU = m_Attributes.U.Degree;
    uint8_t degreeV = m_Attributes.V.Degree;

    for (int i = 0; i < nbControlPointsU; i++)
    {
        float weightU = ComputeWeight(m_Attributes.U.Knots, i, degreeU, u);

        for (int j = 0; j < nbControlPointsV; j++)
        {
            float weightV = ComputeWeight(m_Attributes.V.Knots, j, degreeV, v);
            point += weightU * weightV * m_ControlPoints[i][j];
        }
    }

    return point;
}

float BSplineSurface::ComputeWeight(const std::vector<float> &knots, int index, uint8_t degree, float t) const
{
    // get the knots of the current segment
    float currentKnot = knots[index];
    float nextKnot = knots[index + 1];

    // if the degree is 0, the weight is 1 if t is between the current knot and the next knot, 0 otherwise
    // that's the stopping condition of the recursive function
    if (degree == 0)
        return (t >= currentKnot && t < nextKnot) ? 1.0f : 0.0f;

    float weight = 0.0f;

    // the Box-Cox formula

    float denominator = knots[index + degree] - currentKnot;

    // current knot
    if (denominator != 0.0f)
        weight = (t - currentKnot) / denominator * ComputeWeight(knots, index, degree - 1, t);

    denominator = knots[index + degree + 1] - nextKnot;

    // next knot
    if (denominator != 0.0f)
        weight += (knots[index + degree + 1] - t) / denominator * ComputeWeight(knots, index + 1, degree - 1, t);

    return weight;
}

SurfaceFrenetFrameComponents BSplineSurface::GetFrenetFrameAt(float u, float v)
{
    SurfaceFrenetFrameComponents surfaceFrenetFrame;
    CurveFrenetFrameComponents frentFrameU;
    CurveFrenetFrameComponents frenetFrameV;

    SurfaceDerivativesComponenets derivatives = GetFiniteDifferencesPartialDerivatives(u, v);

    auto velocityU = derivatives.U.Velocity;
    auto velocityV = derivatives.V.Velocity;
    auto accelerationU = derivatives.U.Acceleration;
    auto accelerationV = derivatives.V.Acceleration;

    frentFrameU.Tangent = glm::normalize(velocityU);
    frenetFrameV.Tangent = glm::normalize(velocityV);

    frentFrameU.Normal = glm::normalize(glm::cross(velocityU, glm::cross(accelerationU, velocityU)));
    frenetFrameV.Normal = glm::normalize(glm::cross(velocityV, glm::cross(accelerationV, velocityV)));

    frentFrameU.Binormal = glm::cross(frentFrameU.Tangent, frentFrameU.Normal);
    frenetFrameV.Binormal = glm::cross(frenetFrameV.Tangent, frenetFrameV.Normal);

    surfaceFrenetFrame.U = frentFrameU;
    surfaceFrenetFrame.V = frenetFrameV;

    return surfaceFrenetFrame;
}

SurfaceCurvaturesComponents BSplineSurface::GetCurvaturesAt(float u, float v)
{
    SurfaceCurvaturesComponents curvatures;

    SurfaceFrenetFrameComponents frenetFrame = GetFrenetFrameAt(u, v);
    SurfaceDerivativesComponenets partialDerivatives = GetFiniteDifferencesPartialDerivatives(u, v);
    CurveDerivativesComponenets derivatives = GetFiniteDifferencesDerivatives(u, v);

    auto tangentU = frenetFrame.U.Tangent;
    auto tangentV = frenetFrame.V.Tangent;

    glm::vec3 tangentPlaneNormal = glm::normalize(glm::cross(tangentU, tangentV));

    auto velocityU = glm::normalize(partialDerivatives.U.Velocity);
    auto velocityV = glm::normalize(partialDerivatives.V.Velocity);
    auto accelerationU = glm::normalize(partialDerivatives.U.Acceleration);
    auto accelerationV = glm::normalize(partialDerivatives.V.Acceleration);

    float E = glm::dot(velocityU, velocityU);
    float F = glm::dot(velocityU, velocityV);
    float G = glm::dot(velocityV, velocityV);

    glm::vec3 nextPoint = EvaluateAt(u + 1e-2f, v + 1e-2f);
    glm::vec3 nextUPoint = EvaluateAt(u, v + 1e-2f);

    glm::vec3 dU = (nextPoint - nextUPoint) / 1e-2f;
    glm::vec3 f = glm::normalize((dU - velocityU) / 1e-2f);

    float L = glm::dot(accelerationU, tangentPlaneNormal);
    float M = glm::dot(f, tangentPlaneNormal);
    float N = glm::dot(accelerationV, tangentPlaneNormal);

    curvatures.GaussianCurvature = (L * N - M * M) / (E * G - F * F);
    curvatures.MeanCurvature = (E * N + G * L - 2.0f * F * M) / (2.0f * (E * G - F * F));
    curvatures.AbsoluteCurvature = (4 * curvatures.MeanCurvature * curvatures.MeanCurvature) - (2 * curvatures.GaussianCurvature);

    return curvatures;
}

void BSplineSurface::InitKnotVector()
{
    m_Attributes.U.Knots.clear();
    m_Attributes.V.Knots.clear();

    int nbControlPointsU = m_ControlPoints.size();
    int nbControlPointsV = m_ControlPoints[0].size();

    int numKnotsU = nbControlPointsU + m_Attributes.U.Degree + 1;
    int numKnotsV = nbControlPointsV + m_Attributes.V.Degree + 1;

    uint8_t degreeU = m_Attributes.U.Degree;
    uint8_t degreeV = m_Attributes.V.Degree;

    m_Attributes.U.Knots.resize(numKnotsU);
    m_Attributes.V.Knots.resize(numKnotsV);

    auto setKnots = [this](int numKnots, int degree, int nbControlPoints, auto &knots)
    {
        for (int i = 0; i < numKnots; i++)
        {
            switch (m_Type)
            {
            case BSplineType::Uniform:
                knots[i] = static_cast<float>(i);
                break;

            case BSplineType::OpenUniform:
                if (i <= degree)
                    knots[i] = 0.0f; // Repeat the first knot
                else if (degree < i && i <= knots.size() - degree - 1)
                    knots[i] = static_cast<float>(i - degree);
                else
                    knots[i] = static_cast<float>(nbControlPoints - (degree + 1) + 2); // Repeat the last knot
                break;

            default:
                SMART_LOG_CRITICAL("Unknown BSplineType");
                exit(1);
                break;
            }
        }
    };

    setKnots(numKnotsU, m_Attributes.U.Degree, nbControlPointsU, m_Attributes.U.Knots);
    setKnots(numKnotsV, m_Attributes.V.Degree, nbControlPointsV, m_Attributes.V.Knots);
}

SurfaceDerivativesComponenets BSplineSurface::GetFiniteDifferencesPartialDerivatives(float u, float v) const
{
    float epsilon = 1e-2f;

    glm::vec3 currentPoint = EvaluateAt(u, v);

    glm::vec3 nextU = EvaluateAt(u + epsilon, v);
    glm::vec3 nextV = EvaluateAt(u, v + epsilon);
    glm::vec3 prevU = EvaluateAt(u - epsilon, v);
    glm::vec3 prevV = EvaluateAt(u, v - epsilon);

    glm::vec3 velocityU = (nextU - prevU) / (2.0f * epsilon);
    glm::vec3 velocityV = (nextV - prevV) / (2.0f * epsilon);

    glm::vec3 nextNextU = EvaluateAt(u + 2.0f * epsilon, v);
    glm::vec3 nextNextV = EvaluateAt(u, v + 2.0f * epsilon);

    // glm::vec3 accelerationU = (nextU - 2.0f * currentPoint + prevU) / (epsilon * epsilon);
    // glm::vec3 accelerationV = (nextV - 2.0f * currentPoint + prevV) / (epsilon * epsilon);

    glm::vec3 accelerationU = (nextNextU - (2.0f * nextU) + currentPoint) / (epsilon * epsilon);
    glm::vec3 accelerationV = (nextNextV - (2.0f * nextV) + currentPoint) / (epsilon * epsilon);

    return {
        CurveDerivativesComponenets{velocityU, accelerationU},
        CurveDerivativesComponenets{velocityV, accelerationV}
    };
}

CurveDerivativesComponenets BSplineSurface::GetFiniteDifferencesDerivatives(float u, float v) const
{
    float epsilon = 1e-2f;

    glm::vec3 currentPoint = EvaluateAt(u, v);

    glm::vec3 next = EvaluateAt(u + epsilon, v + epsilon);
    glm::vec3 prev = EvaluateAt(u - epsilon, v - epsilon);

    glm::vec3 velocity = (next - prev) / (2.0f * epsilon);

    glm::vec3 acceleration = (next - 2.0f * currentPoint + prev) / (epsilon * epsilon);

    return CurveDerivativesComponenets{velocity, acceleration};
}
