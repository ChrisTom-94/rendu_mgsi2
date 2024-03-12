#include "Editor.h"

#include "FractalMountain.h"

#include "Renderer.h"

namespace TP_GeometrieFractale
{

    struct EditorData
    {
        FractalMountain Mountain;
        Maths::Transform Transform;
        Light SceneLight;
        bool IsWireframe;
    };

    static EditorData s_Data;

    Editor::Editor()
    {
        // setup camera
        float width = Core::Application::Get().GetWindow().GetWidth();
        float height = Core::Application::Get().GetWindow().GetHeight();

        float fov = 45.0f, aspect = width / height, nearClip = 0.1f, farClip = 100.0f;
        m_Camera = CreateShared<PerspectiveCamera>(fov, aspect, nearClip, farClip);

        glm::vec3 center = glm::vec3(0.0f, 0.0f, 0.0f);
        float radius = 15.0f;
        m_CameraController = CreateShared<ArcBallCameraController>(m_Camera, center, radius);
        m_CameraController->SetTrigger(Events::ButtonRight);
        m_CameraController->SetMaxRadius(40.0f);
    }

    Editor::~Editor()
    {
    }

    void Editor::OnAttach()
    {
        FractalMountainSettings settings;
        settings.Iterations = 5;
        settings.Roughness = 2.0f;
        settings.Size = 12.0f;
        s_Data.Mountain = FractalMountain(settings);

        s_Data.Transform.SetRotation({-90.0f, 0.0f, 0.0f});
        s_Data.Transform.Update();

        s_Data.SceneLight.Position = {0.0f, 12.0f, 2.0f};
        s_Data.SceneLight.Color = {1.0f, 1.0f, 1.0f};
        s_Data.SceneLight.Intensity = 10.0f;

        Renderer::Init();
        Renderer::UpdateMountain(s_Data.Mountain);
    }

    void Editor::OnDetach()
    {
    }

    void Editor::OnUpdate(Timestep deltaTime)
    {
        Renderer::BeginScene(m_Camera, s_Data.SceneLight);
        Renderer::DrawMountain(s_Data.Mountain, s_Data.Transform, s_Data.IsWireframe);
    }

    void Editor::OnUIRender()
    {
        ImGui::Begin("Settings", nullptr, ImGuiWindowFlags_AlwaysAutoResize);

        { // Fractal iterations
            int iterations = s_Data.Mountain.GetIterations();
            int min = 1, max = 5;
            if (ImGui::SliderInt("Iterations", &iterations, min, max))
            {
                s_Data.Mountain.SetIterations(iterations);
                Renderer::UpdateMountain(s_Data.Mountain);
            }
        }

        { // Fractal roughness
            float roughness = s_Data.Mountain.GetRoughness();
            float min = 1.0f, max = 6.0f;
            if (ImGui::SliderFloat("Roughness", &roughness, min, max))
            {
                s_Data.Mountain.SetRoughness(roughness);
                Renderer::UpdateMountain(s_Data.Mountain);
            }
        }

        if (ImGui::Button("Regenerate"))
        {
            s_Data.Mountain.Generate();
            Renderer::UpdateMountain(s_Data.Mountain);
        }

        if (ImGui::Checkbox("Wireframe", &s_Data.IsWireframe))
        {
            if (s_Data.IsWireframe)
                RenderCommand::SetWireframeMode();
            else
                RenderCommand::SetFillMode();
        }

        ImGui::End();

        // Light settings

        ImGui::Begin("Light", nullptr, ImGuiWindowFlags_AlwaysAutoResize);

        glm::vec3 position = s_Data.SceneLight.Position;
        if (ImGui::SliderFloat3("Position", glm::value_ptr(position), -10.0f, 10.0f))
            s_Data.SceneLight.Position = position;

        glm::vec3 color = s_Data.SceneLight.Color;
        if (ImGui::ColorEdit3("Color", glm::value_ptr(color)))
            s_Data.SceneLight.Color = color;

        float intensity = s_Data.SceneLight.Intensity;
        if (ImGui::SliderFloat("Intensity", &intensity, 0.0f, 10.0f))
            s_Data.SceneLight.Intensity = intensity;

        ImGui::End();
    }

    void Editor::OnEvent(Events::Event &event)
    {
        m_CameraController->OnEvent(event);

        Events::EventDispatcher dispatcher(event);
        dispatcher.Dispatch<Events::WindowResizeEvent>(BIND_EVENT_FN(Editor::OnWindowResize));
    }

    bool Editor::OnWindowResize(Events::WindowResizeEvent &event)
    {
        m_Camera->SetAspectRatio((float)event.GetWidth() / (float)event.GetHeight());
        return false;
    }
}
