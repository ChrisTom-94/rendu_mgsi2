#include "precompiled.h"

#include "UI/UILayer.h"

#include "Core/Application.h"

#include "GLFW/glfw3.h"

#include "imgui/imgui.h"
#include "imgui/imgui_internal.h"
#include "imgui/backends/imgui_impl_glfw.h"
#include "imgui/backends/imgui_impl_opengl3.h"

namespace SmartGL
{
    namespace UI
    {
        UILayer::UILayer() : Layer("UILayer") {}

        UILayer::~UILayer() {}

        void UILayer::OnAttach()
        {
            IMGUI_CHECKVERSION();
            ImGui::CreateContext();
            ImGuiIO &io = ImGui::GetIO();
            (void)io;
            io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; // Enable Keyboard Controls
            io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;     // Enable Docking
            // io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;   // Enable Multi-Viewport / Platform Windows

            ImGui::StyleColorsDark();

            ImGuiStyle &style = ImGui::GetStyle();
            if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
            {
                style.WindowRounding = 0.0f;
                style.Colors[ImGuiCol_WindowBg].w = 1.0f;
            }

            Core::Application &app = Core::Application::Get();
            GLFWwindow *window = static_cast<GLFWwindow *>(app.GetWindow().GetNativeWindow());
            ImGui_ImplGlfw_InitForOpenGL(window, true);
            ImGui_ImplOpenGL3_Init("#version 450");
        }

        void UILayer::OnDetach()
        {
            ImGui_ImplOpenGL3_Shutdown();
            ImGui_ImplGlfw_Shutdown();
            ImGui::DestroyContext();
        }

        void UILayer::OnUIRenderStart()
        {
            ImGui_ImplOpenGL3_NewFrame();
            ImGui_ImplGlfw_NewFrame();
            ImGui::NewFrame();
        }

        void UILayer::OnUIRenderEnd()
        {

            if (!ImGui::IsAnyItemHovered() && !ImGui::IsAnyItemActive())
                ImGui::SetWindowFocus(nullptr);

            ImGuiIO &io = ImGui::GetIO();
            Core::Application &app = Core::Application::Get();
            float width = (float)app.GetWindow().GetWidth();
            float height = (float)app.GetWindow().GetHeight();
            io.DisplaySize = ImVec2(width, height);

            ImGui::Render();
            ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

            // if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
            // {
            //     GLFWwindow *backup_current_context = glfwGetCurrentContext();
            //     ImGui::UpdatePlatformWindows();
            //     ImGui::RenderPlatformWindowsDefault();
            //     glfwMakeContextCurrent(backup_current_context);
            // }
        }

        void UILayer::OnUIRender()
        {
        }

        void UILayer::OnEvent(Events::Event &e)
        {
            if (!m_BlockEvents)
                return;

            ImGuiIO &io = ImGui::GetIO();
            e.IsHandled |= io.WantCaptureMouse;

            if (!io.WantCaptureMouse)
            {
                e.IsHandled |= io.WantCaptureKeyboard;
            }
        }
    }
}
