#include "Editor.h"

#include <algorithm>

namespace TP2_Nurbs
{
    /**
     * @brief Data used to store the state of the BSpline surface
     */
    struct SurfaceData
    {
        std::vector<std::vector<ControlPoint>> ControlPoints;
        ControlPoint *SelectedControlPoint = nullptr;
        glm::vec2 LastMousePosition = glm::vec2(0.0);
        uint8_t DegreeU = 3;
        uint8_t DegreeV = 3;
        float T_U = 0.0f;
        float T_V = 0.0f;

        void Reset()
        {
            ControlPoints.clear();
            LastMousePosition = glm::vec2(0.0);
            DegreeU = 3;
            DegreeV = 3;
            T_U = 0.0f;
            T_V = 0.0f;
        }

        std::vector<std::vector<glm::vec3>> GetControlPoints()
        {
            std::vector<std::vector<glm::vec3>> points;
            for (auto &row : ControlPoints)
            {
                std::vector<glm::vec3> rowPoints;
                for (auto &point : row)
                    rowPoints.push_back(point.Position);
                points.push_back(rowPoints);
            }
            return points;
        }

        ControlPoint *GetHoveredControlPoint()
        {
            for (auto &row : ControlPoints)
                for (auto &point : row)
                    if (point.Hovered)
                        return &point;
            return nullptr;
        }
    };

    /**
     * @brief Data used to store the state of the editor
     */
    struct EditorData
    {
        int SurfacePrecision = 6;

        bool ShowFrenetFrame = false;
        bool ShowCurvatureMap = false;

        bool IsDragging = false;
    };

    static SurfaceData s_SurfaceData;
    static EditorData s_EditorData;
    static glm::vec3 s_LastSelectedPointPosition; // used to store the position of the selected point before dragging

    Editor::Editor()
        : Layer("Editor"),
          m_Surface(BSplineSurfaceAttributes(s_SurfaceData.DegreeU, s_SurfaceData.DegreeV))
    {
        // set camera
        float fov = 45.0f;
        float aspectRatio = Core::Application::Get().GetWindow().GetAspectRatio();
        float nearClip = 0.1f;
        float farClip = 100.0f;
        PerspectiveCameraSettings cameraSettings = {fov, aspectRatio, nearClip, farClip};
        m_Camera = CreateShared<PerspectiveCamera>(cameraSettings);
        m_CameraController = CreateShared<ArcBallCameraController>(m_Camera, glm::vec3(0.0f, 0.0f, 0.0f), 10.0f);
        m_CameraController->SetTrigger(Events::ButtonRight);

        // add control points
        s_SurfaceData.ControlPoints = std::vector<std::vector<ControlPoint>>{};

        s_SurfaceData.ControlPoints.push_back(std::vector<ControlPoint>{
            ControlPoint{glm::vec3(-3.0f, 0.0f, 3.0f), 1},
            ControlPoint{glm::vec3(-1.0f, 0.0f, 3.0f), 2},
            ControlPoint{glm::vec3( 1.0f, 0.0f, 3.0f), 3},
            ControlPoint{glm::vec3( 3.0f, 0.0f, 3.0f), 4},
        });

        s_SurfaceData.ControlPoints.push_back(std::vector<ControlPoint>{
            ControlPoint{glm::vec3(-3.0f, 0.0f, 1.0f), 5},
            ControlPoint{glm::vec3(-1.0f, 3.0f, 1.0f), 6},
            ControlPoint{glm::vec3( 1.0f, 3.0f, 1.0f), 7},
            ControlPoint{glm::vec3( 3.0f, 0.0f, 1.0f), 8},
        });

        s_SurfaceData.ControlPoints.push_back(std::vector<ControlPoint>{
            ControlPoint{glm::vec3(-3.0f, 0.0f, -1.0f), 9},
            ControlPoint{glm::vec3(-1.0f, 3.0f, -1.0f), 10},
            ControlPoint{glm::vec3( 1.0f, 3.0f, -1.0f), 11},
            ControlPoint{glm::vec3( 3.0f, 0.0f, -1.0f), 12},
        });

        s_SurfaceData.ControlPoints.push_back(std::vector<ControlPoint>{
            ControlPoint{glm::vec3(-3.0f, 0.0f, -3.0f), 13},
            ControlPoint{glm::vec3(-1.0f, 0.0f, -3.0f), 14},
            ControlPoint{glm::vec3( 1.0f, 0.0f, -3.0f), 15},
            ControlPoint{glm::vec3( 3.0f, 0.0f, -3.0f), 16},
        });

        s_SurfaceData.ControlPoints.push_back(std::vector<ControlPoint>{
            ControlPoint{glm::vec3(-3.0f, 0.0f, -5.0f), 17},
            ControlPoint{glm::vec3(-1.0f, 0.0f, -5.0f), 18},
            ControlPoint{glm::vec3( 1.0f, 0.0f, -5.0f), 19},
            ControlPoint{glm::vec3( 3.0f, 0.0f, -5.0f), 20},
        });

        // init surface
        m_Surface.SetControlPoints(s_SurfaceData.GetControlPoints());
        m_Surface.InitKnotVector();
        m_Surface.Evaluate();
        m_Surface.EvaluateCurvatures();

        s_SurfaceData.T_U = m_Surface.GetMinT_U();
        s_SurfaceData.T_V = m_Surface.GetMinT_V();
    }

    Editor::~Editor()
    {
        m_Camera.reset();
    }

    void Editor::OnAttach()
    {
        Renderer::Init();
    }

    void Editor::OnDetach()
    {
        Renderer::ShutDown();
    }

    void Editor::OnUpdate(Timestep timestep)
    {
        Renderer::BeginScene(m_Camera->GetViewProjectionMatrix());

        Renderer::DrawControlPoints(s_SurfaceData.ControlPoints);

        auto surfacePoints = m_Surface.GetPoints();

        if (s_EditorData.ShowCurvatureMap)
        {
            auto surfaceCurvatures = m_Surface.GetCurvatures();
            Renderer::DrawSurface(surfacePoints, surfaceCurvatures, s_EditorData.ShowCurvatureMap);
        }
        else
            Renderer::DrawSurface(surfacePoints);

        if (s_EditorData.ShowFrenetFrame)
        {
            glm::vec3 currentPoint = m_Surface.EvaluateAt(s_SurfaceData.T_U, s_SurfaceData.T_V);
            SurfaceFrenetFrameComponents frenetFrame = m_Surface.GetFrenetFrameAt(s_SurfaceData.T_U, s_SurfaceData.T_V);
            glm::vec3 tangentU = frenetFrame.U.Tangent;
            glm::vec3 tangentV = frenetFrame.V.Tangent;
            glm::vec3 normal = glm::normalize(glm::cross(tangentU, tangentV));

            Renderer2D::DrawLine(currentPoint, currentPoint + tangentU, glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));
            Renderer2D::DrawLine(currentPoint, currentPoint + tangentV, glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));
            Renderer2D::DrawLine(currentPoint, currentPoint + normal, glm::vec4(0.0f, 1.0f, 0.0f, 1.0f));
        }

        Renderer::EndScene();

        if (s_EditorData.IsDragging && s_SurfaceData.SelectedControlPoint)
        {
            s_EditorData.ShowCurvatureMap = false;
            DragSelectedPoint();
        }
    }

    void Editor::OnEvent(Events::Event &event)
    {

        m_CameraController->OnEvent(event);

        if (event.GetEventType() == Events::EventType::WindowResize)
            OnWindowResize((Events::WindowResizeEvent &)event);

        Events::EventDispatcher dispatcher(event);
        dispatcher.Dispatch<Events::MouseButtonPressedEvent>(BIND_EVENT_FN(Editor::OnMouseButtonPressed));
        dispatcher.Dispatch<Events::KeyPressedEvent>(BIND_EVENT_FN(Editor::OnKeyPressed));
        dispatcher.Dispatch<Events::WindowResizeEvent>(BIND_EVENT_FN(Editor::OnWindowResize));
        dispatcher.Dispatch<Events::MouseMovedEvent>(BIND_EVENT_FN(Editor::OnMouseMoved));
    }

    void Editor::OnUIRender()
    {
        ImGui::Begin("Surface Editor", nullptr, ImGuiWindowFlags_AlwaysAutoResize);

        if (ImGui::SliderInt("Suface Precision", &s_EditorData.SurfacePrecision, 1, 6))
        {
            m_Surface.SetPrecision(s_EditorData.SurfacePrecision);
            m_Surface.Evaluate();
        }

        ImGui::SliderFloat("T_U", &s_SurfaceData.T_U, m_Surface.GetMinT_U(), m_Surface.GetMaxT_U());
        ImGui::SliderFloat("T_V", &s_SurfaceData.T_V, m_Surface.GetMinT_V(), m_Surface.GetMaxT_V());

        ImGui::Checkbox("Show Frenet Frame", &s_EditorData.ShowFrenetFrame);

        if (ImGui::Checkbox("Show Curvature Map", &s_EditorData.ShowCurvatureMap))
        {
            if(s_EditorData.ShowCurvatureMap)
                m_Surface.EvaluateCurvatures();
        }

        if(s_EditorData.ShowCurvatureMap)
        {
            ImGui::Text("Curvature Map");

            ImGui::Text("Positive : ");
            ImGui::SameLine();
            ImGui::Text("Red");

            ImGui::Text("Near Zero : ");
            ImGui::SameLine();
            ImGui::Text("Black");

            ImGui::Text("Negative : ");
            ImGui::SameLine();
            ImGui::Text("Blue");
        }

        if(s_SurfaceData.SelectedControlPoint)
            ImGui::Text("Press 'G' to enable/disable dragging");

        ImGui::End();
    }

    void Editor::DragSelectedPoint()
    {
        glm::vec2 mousePosition = Input::GetMousePosition();

        float width = Core::Application::Get().GetWindow().GetWidth();
        float height = Core::Application::Get().GetWindow().GetHeight();

        glm::vec3 controlPointPosition = s_SurfaceData.SelectedControlPoint->Position;

        glm::vec3 worldPoint = m_Camera->ScreenToWorld(mousePosition, {width, height});

        glm::vec3 cameraPosition = m_CameraController->GetPosition();
        glm::vec3 planeNormal = glm::normalize(cameraPosition - controlPointPosition);

        glm::vec3 projectedPoint = Maths::ProjectPointOnPlane(worldPoint, planeNormal, controlPointPosition);

        s_SurfaceData.SelectedControlPoint->Position = projectedPoint;

        m_Surface.SetControlPoints(s_SurfaceData.GetControlPoints());
        m_Surface.Evaluate();

        if (s_EditorData.ShowCurvatureMap)
            m_Surface.EvaluateCurvatures();
    }

    bool Editor::OnMouseMoved(const Events::MouseMovedEvent &e)
    {
        auto mousePosition = Input::GetMousePosition();

        // check if control point is hovered
        uint32_t pixelID = Renderer::ReadPixel(1, (int)mousePosition.x, (int)mousePosition.y);
        for (auto &point : s_SurfaceData.ControlPoints)
            for (auto &p : point)
                p.Hovered = p.ID == pixelID;

        return false;
    }

    bool Editor::OnMouseButtonPressed(const Events::MouseButtonPressedEvent &e)
    {
        if (e.GetMouseButton() != Events::ButtonLeft)
            return false;

        ControlPoint &point = *s_SurfaceData.GetHoveredControlPoint();

        if (!&point || !point.Hovered || s_SurfaceData.SelectedControlPoint == &point)
        {
            if (s_EditorData.IsDragging)
            {
                s_EditorData.IsDragging = false;
                return false;
            }

            if (s_SurfaceData.SelectedControlPoint)
                s_SurfaceData.SelectedControlPoint->Selected = false;
            s_SurfaceData.SelectedControlPoint = nullptr;

            return false;
        }

        if (s_SurfaceData.SelectedControlPoint)
            s_SurfaceData.SelectedControlPoint->Selected = false;

        s_SurfaceData.SelectedControlPoint = &point;
        s_SurfaceData.SelectedControlPoint->Selected = true;
        s_LastSelectedPointPosition = point.Position;

        return false;
    }

    bool Editor::OnKeyPressed(const Events::KeyPressedEvent &e)
    {
        if (s_SurfaceData.SelectedControlPoint == nullptr)
            return false;

        switch (e.GetKeyCode())
        {
        case Events::Key::G:
            s_EditorData.IsDragging = !s_EditorData.IsDragging;
            if (!s_EditorData.IsDragging)
                s_EditorData.ShowCurvatureMap = false;
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
