#include "Editor.h"

#include "glm/gtc/matrix_transform.hpp"

#include <algorithm>

namespace TP1_Nurbs
{

    /**
     * @brief Data used to store the state of the BSpline
     */
    struct SplineData
    {
        std::vector<ControlPoint> ControlPoints;
        ControlPoint *SelectedControlPoint = nullptr;
        uint8_t Degree = 3;
        float T = 0.0f;

        std::vector<glm::vec3> GetControlPoints()
        {
            std::vector<glm::vec3> points;
            for (auto &point : ControlPoints)
                points.push_back(point.Position);
            return points;
        }

        ControlPoint &GetHoveredControlPoint()
        {
            auto it = std::find_if(ControlPoints.begin(), ControlPoints.end(), [](const ControlPoint &point)
                                   { return point.Hovered; });
            return *it;
        }
    };

    /**
     * @brief Data used to store the state of the editor
     */
    struct EditorData
    {
        bool ShowFrenetFrame = true;
        bool ShowCurvature = true;
        bool ShowSurface = true;

        bool IsExtruding = false;
        bool IsDragging = false;
    };

    // static data (only accessible in this file)
    static SplineData s_SplineData;
    static EditorData s_EditorData;
    static glm::vec3 s_LastSelectedPointPosition;

    Editor::Editor() : Layer("Editor"), m_Spline(BSplineCurve(s_SplineData.Degree))
    {
        { // Init the camera
            float aspectRatio = SmartGL::Core::Application::Get().GetWindow().GetAspectRatio();
            PerspectiveCameraSettings cameraSettings({45.0f, aspectRatio, 0.1f, 100.0f});
            m_Camera = CreateShared<PerspectiveCamera>(cameraSettings);
            glm::vec3 center = glm::vec3(0.0f, 0.0f, 0.0f);
            float radius = 10.0f;
            m_CameraController = CreateShared<ArcBallCameraController>(m_Camera, center, radius);
            m_CameraController->SetTrigger(Events::ButtonRight);
        }

        { // Init the spline

            s_SplineData.ControlPoints.push_back({glm::vec3(-2.0f,  0.0f, 0.0f), 1});
            s_SplineData.ControlPoints.push_back({glm::vec3(-2.0f,  2.0f, 0.0f), 2});
            s_SplineData.ControlPoints.push_back({glm::vec3( 2.0f, -2.0f, 0.0f), 3});
            s_SplineData.ControlPoints.push_back({glm::vec3( 2.0f,  0.0f, 0.0f), 4});
            s_SplineData.T = m_Spline.GetMinT();

            m_Spline.SetControlPoints(s_SplineData.GetControlPoints());
            m_Spline.Evaluate();
        }
    }

    Editor::~Editor()
    {
        m_Camera.reset();
        m_CameraController.reset();
    }

    void Editor::OnAttach()
    {
        Renderer::Init();
    }

    void Editor::OnUpdate(Timestep timestep)
    {
        glm::mat4 viewProjection = m_Camera->GetViewProjectionMatrix();
        Renderer::BeginScene(viewProjection);

        // draw spline and control points
        Renderer::DrawBSpline(m_Spline.GetPoints());
        Renderer::DrawControlPoints(s_SplineData.ControlPoints);

        if (s_EditorData.ShowFrenetFrame || s_EditorData.ShowCurvature)
        {
            glm::vec3 currentPoint = m_Spline.EvaluateAt(s_SplineData.T);
            CurveFrenetFrameComponents frenetFrame = m_Spline.GetFrenetFrameAt(s_SplineData.T);

            if (s_EditorData.ShowCurvature) // curvature circle
            {
                float curvature = m_Spline.GetCurvatureAt(s_SplineData.T);
                Renderer::DrawCurvature(currentPoint, frenetFrame, curvature);
            }

            if (s_EditorData.ShowFrenetFrame) // frenet frame
                Renderer::DrawFrenetFrame(currentPoint, frenetFrame);
        }

        if (s_EditorData.ShowSurface) // circles along the curve
        {
            float minT = m_Spline.GetMinT();
            float maxT = m_Spline.GetMaxT();
            Renderer::DrawSurfaceAlongCurve(m_Spline, minT, maxT);
        }

        Renderer::EndScene();

        if (s_SplineData.SelectedControlPoint && s_EditorData.IsDragging)
            DragSelectedPoint();
    }

    void Editor::OnEvent(Events::Event &event)
    {

        m_CameraController->OnEvent(event);

        // bind events to callback functions
        Events::EventDispatcher dispatcher(event);
        dispatcher.Dispatch<Events::MouseMovedEvent>(BIND_EVENT_FN(Editor::OnMouseMoved));
        dispatcher.Dispatch<Events::KeyPressedEvent>(BIND_EVENT_FN(Editor::OnKeyPressed));
        dispatcher.Dispatch<Events::WindowResizeEvent>(BIND_EVENT_FN(Editor::OnWindowResize));
        dispatcher.Dispatch<Events::MouseButtonPressedEvent>(BIND_EVENT_FN(Editor::OnMouseButtonPressed));
    }

    void Editor::OnUIRender()
    {
        ImGui::SetNextWindowBgAlpha(0.0f);

        ImGui::Begin("BSpline Editor", nullptr, ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoFocusOnAppearing);

        ImGui::SliderFloat("t", &s_SplineData.T, m_Spline.GetMinT(), m_Spline.GetMaxT());
        ImGui::Separator();
        ImGui::Checkbox("Render frenet frame", &s_EditorData.ShowFrenetFrame);
        ImGui::Separator();
        ImGui::Checkbox("Render curvature", &s_EditorData.ShowCurvature);
        ImGui::Separator();
        ImGui::Checkbox("Render surface", &s_EditorData.ShowSurface);

        if(s_SplineData.SelectedControlPoint)
        {
            ImGui::Text("Press 'G' to enable/disable dragging");
            ImGui::Text("Press 'E' to extrude a new control point");
            ImGui::Text("Press 'Delete' to delete a control point");
            ImGui::Text("Press 'Escape' to cancel dragging");
        }

        ImGui::End();
    }

    void Editor::AddControlPoint()
    {
        glm::vec3 currentPosition = s_SplineData.SelectedControlPoint->Position;

        ControlPoint newControlPoint;
        newControlPoint.Position = currentPosition;
        newControlPoint.Hovered = true;
        newControlPoint.Selected = true;
        newControlPoint.ID = s_SplineData.ControlPoints.size() + 1;

        auto it = std::find_if(s_SplineData.ControlPoints.begin(), s_SplineData.ControlPoints.end(), [this](const ControlPoint &point)
                               { return point.ID == s_SplineData.SelectedControlPoint->ID; });

        s_SplineData.SelectedControlPoint->Hovered = false;
        s_SplineData.SelectedControlPoint->Selected = false;
        s_SplineData.SelectedControlPoint = nullptr;

        std::vector<ControlPoint>::iterator new_it;

        if (it == s_SplineData.ControlPoints.begin()) // insert at the beginning if the selected point is the first
            new_it = s_SplineData.ControlPoints.insert(it, newControlPoint);
        else if (it == s_SplineData.ControlPoints.end() - 1) // insert at the end if the selected point is the last
            new_it = s_SplineData.ControlPoints.insert(s_SplineData.ControlPoints.end(), newControlPoint);
        else // insert between two points
            new_it = s_SplineData.ControlPoints.insert(it + 1, newControlPoint);

        m_Spline.SetControlPoints(s_SplineData.GetControlPoints());
        m_Spline.Evaluate();

        s_SplineData.SelectedControlPoint = &(*new_it);

        s_EditorData.IsExtruding = true;
        s_EditorData.IsDragging = true;
    }

    void Editor::DeleteControlPoint(const uint8_t &id)
    {
        if (s_SplineData.SelectedControlPoint && s_SplineData.SelectedControlPoint->ID == id)
            s_SplineData.SelectedControlPoint = nullptr;

        auto it = std::remove_if(
            s_SplineData.ControlPoints.begin(),
            s_SplineData.ControlPoints.end(),
            [id](const ControlPoint &point)
            { return point.ID == id; });

        s_SplineData.ControlPoints.erase(it, s_SplineData.ControlPoints.end());
        m_Spline.SetControlPoints(s_SplineData.GetControlPoints());
        m_Spline.Evaluate();
    }

    void Editor::DragSelectedPoint()
    {
        glm::vec2 mousePosition = Input::GetMousePosition();

        float width = Core::Application::Get().GetWindow().GetWidth();
        float height = Core::Application::Get().GetWindow().GetHeight();

        glm::vec3 controlPointPosition = s_SplineData.SelectedControlPoint->Position;

        glm::vec3 worldPoint = m_Camera->ScreenToWorld(mousePosition, {width, height});

        glm::vec3 cameraPosition = m_CameraController->GetPosition();
        glm::vec3 planeNormal = glm::normalize(cameraPosition - controlPointPosition);

        glm::vec3 projectedPoint = Maths::ProjectPointOnPlane(worldPoint, planeNormal, controlPointPosition);

        s_SplineData.SelectedControlPoint->Position = projectedPoint;

        m_Spline.SetControlPoints(s_SplineData.GetControlPoints());
        m_Spline.Evaluate();
    }

    void Editor::CancelDragging()
    {
        if (s_EditorData.IsExtruding)
        {
            s_EditorData.IsExtruding = false;
            DeleteControlPoint(s_SplineData.SelectedControlPoint->ID);
            return;
        }

        s_SplineData.SelectedControlPoint->Position = s_LastSelectedPointPosition;
        s_SplineData.SelectedControlPoint->Selected = false;
        s_SplineData.SelectedControlPoint = nullptr;
        m_Spline.SetControlPoints(s_SplineData.GetControlPoints());
        m_Spline.Evaluate();
    }

    bool Editor::OnMouseMoved(const Events::MouseMovedEvent &e)
    {
        auto mousePosition = Input::GetMousePosition();

        // check if control point is hovered
        uint32_t pixelID = Renderer::ReadPixel(1, (int)mousePosition.x, (int)mousePosition.y);
        for (auto &point : s_SplineData.ControlPoints)
            point.Hovered = point.ID == pixelID;

        return false;
    }

    bool Editor::OnMouseButtonPressed(const Events::MouseButtonPressedEvent &e)
    {
        if (e.GetMouseButton() != Events::ButtonLeft)
            return false;

        ControlPoint &point = s_SplineData.GetHoveredControlPoint();

        if (!point.Hovered || s_SplineData.SelectedControlPoint == &point)
        {
            s_EditorData.IsExtruding = false;

            if (s_EditorData.IsDragging)
            {
                s_EditorData.IsDragging = false;
                return false;
            }

            if (s_SplineData.SelectedControlPoint)
                s_SplineData.SelectedControlPoint->Selected = false;
            s_SplineData.SelectedControlPoint = nullptr;

            return false;
        }

        if (s_SplineData.SelectedControlPoint)
            s_SplineData.SelectedControlPoint->Selected = false;

        s_SplineData.SelectedControlPoint = &point;
        s_SplineData.SelectedControlPoint->Selected = true;
        s_LastSelectedPointPosition = point.Position;

        return false;
    }

    bool Editor::OnKeyPressed(const Events::KeyPressedEvent &e)
    {
        if (s_SplineData.SelectedControlPoint == nullptr)
            return false;

        switch (e.GetKeyCode())
        {
        case Events::Key::Delete:
            DeleteControlPoint(s_SplineData.SelectedControlPoint->ID);
            break;
        case Events::Key::Escape:
            CancelDragging();
            break;
        case Events::Key::E:
            AddControlPoint();
            break;
        case Events::Key::G:
            s_EditorData.IsDragging = !s_EditorData.IsDragging;
            break;
        }

        return false;
    }

    bool Editor::OnWindowResize(const Events::WindowResizeEvent &event)
    {
        m_Camera->SetAspectRatio(event.GetWidth() / (float)event.GetHeight());
        Renderer::Resize(event.GetWidth(), event.GetHeight());

        return false;
    }
}
