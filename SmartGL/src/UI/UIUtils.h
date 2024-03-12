#pragma once

#include "imgui/imgui.h"

namespace SmartGL
{
    namespace ImGuiUtils
    {
        static void BeginDockSpace()
        {
            ImGuiViewport *viewport = ImGui::GetMainViewport();
            ImGui::SetNextWindowPos(viewport->WorkPos);
            ImGui::SetNextWindowSize(viewport->WorkSize);
            ImGui::SetNextWindowViewport(viewport->ID);

            ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
            ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
            ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));

            ImGui::Begin("MainDockspace", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus | ImGuiWindowFlags_NoBackground);
            ImGui::PopStyleVar(3);

            ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
            ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), ImGuiDockNodeFlags_PassthruCentralNode);

            ImGui::End();
        }

        static void ActiveButton(const char *label, bool active, const ImVec4 &activeColor, const ImVec4 &inactiveColor, std::function<void()> callback = nullptr)
        {
            if(active)
                ImGui::PushStyleColor(ImGuiCol_Button, activeColor);
            else
                ImGui::PushStyleColor(ImGuiCol_Button, inactiveColor);

            if(ImGui::Button(label))
            {
                active = !active;
                if(callback)
                    callback();
            }

            ImGui::PopStyleColor();
        }
    }
}
