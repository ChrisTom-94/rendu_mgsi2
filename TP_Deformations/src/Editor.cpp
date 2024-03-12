#include "Editor.h"

#include "Renderer.h"

namespace TP_Deformations
{

    /// @brief Data of the model that will be rendered.
    struct EditorData
    {
        Geometry Model;
        Maths::Transform ModelTransform;
        Material ModelMaterial;
        Light SceneLight;
    };

    /// @brief Data of the deformation that will be applied to the model.
    struct DeformationData
    {
        DeformationShader ActiveShader = DeformationShader::Twist;
        float MinT = -1.0f;
        float MaxT = 1.0f;
    };

    static EditorData s_EditorData;
    static DeformationData s_DeformationData;

    Editor::Editor()
    {
        { // camera setup
            float width = Core::Application::Get().GetWindow().GetWidth();
            float height = Core::Application::Get().GetWindow().GetHeight();
            float fov = 45.0f, aspectRatio = width / height, nearClip = 0.1f, farClip = 100.0f;
            m_Camera = CreateShared<PerspectiveCamera>(fov, aspectRatio, nearClip, farClip);

            glm::vec3 center = {0.0f, 0.0f, 0.0f};
            float radius = 5.0f;
            m_CameraController = CreateShared<ArcBallCameraController>(m_Camera, center, radius);
            m_CameraController->SetTrigger(Events::ButtonRight);
        }
    }

    Editor::~Editor()
    {
    }

    void Editor::OnAttach()
    {
        { // init model data
            TorusGeometrySettings settings;
            settings.RadialSegments = 32;
            settings.TubularSegments = 32;
            s_EditorData.Model = TorusGeometry(settings);
            s_EditorData.ModelTransform = Maths::Transform();

            s_EditorData.ModelMaterial.Albedo = {1.0f, 0.0f, 0.0f};
            s_EditorData.ModelMaterial.Shininess = 32.0f;
        }

        { // init light data
            s_EditorData.SceneLight.AmbientColor = {0.2f, 0.2f, 0.2f};
            s_EditorData.SceneLight.Position = {2.0f, 3.0f, 0.0f};
            s_EditorData.SceneLight.Color = {1.0f, 1.0f, 1.0f};
            s_EditorData.SceneLight.Intensity = 6.0f;
        }

        { // init renderer
            Renderer::Init();
            RenderCommand::SetClearColor({0.2f, 0.2f, 0.2f, 1.0f});
            Renderer::SetGeometry(s_EditorData.Model);
        }
    }

    void Editor::OnUpdate(Timestep timestep)
    {

        auto [Model, ModelTransform, ModelMaterial, SceneLight] = s_EditorData;

        Renderer::BeginScene(m_Camera, ModelTransform.GetLocalMatrix(), ModelMaterial, SceneLight);
        Renderer::DrawGeometry(s_DeformationData.MinT, s_DeformationData.MaxT);
        Renderer::EndScene();
    }

    void Editor::OnEvent(Events::Event &event)
    {
        m_CameraController->OnEvent(event);

        Events::EventDispatcher dispatcher(event);
        dispatcher.Dispatch<Events::WindowResizeEvent>(BIND_EVENT_FN(Editor::OnWindowResize));
    }

    void Editor::OnUIRender()
    {
        {
            ImGui::Begin("Deformation", nullptr, ImGuiWindowFlags_AlwaysAutoResize);

            ImVec4 activeColor = {0.0f, 0.5f, 0.8f, 1.0f};
            ImVec4 inactiveColor = {0.0f, 0.0f, 0.7f, 0.5f};
            bool isActivated = s_DeformationData.ActiveShader == DeformationShader::Twist;
            SmartGL::ImGuiUtils::ActiveButton("Twist", isActivated, activeColor, inactiveColor, [&]() {
                Renderer::SetActiveShader(DeformationShader::Twist);
                s_DeformationData.ActiveShader = DeformationShader::Twist;
            });

            ImGui::SameLine();

            isActivated = s_DeformationData.ActiveShader == DeformationShader::Pinch;
            SmartGL::ImGuiUtils::ActiveButton("Pinch", isActivated, activeColor, inactiveColor, [&]() {
                Renderer::SetActiveShader(DeformationShader::Pinch);
                s_DeformationData.ActiveShader = DeformationShader::Pinch;
            });

            ImGui::SliderFloat("Min T", &s_DeformationData.MinT, -2.0f, 2.0f);
            ImGui::SliderFloat("Max T", &s_DeformationData.MaxT, -2.0f, 2.0f);

            ImGui::End();
        }
    }

    void Editor::OnDetach()
    {
        Renderer::ShutDown();
    }

    bool Editor::OnWindowResize(const Events::WindowResizeEvent &event)
    {
        auto width = event.GetWidth();
        auto height = event.GetHeight();
        m_Camera->SetAspectRatio((float)width / (float)height);
        Renderer::Resize(width, height);

        return false;
    }
}
