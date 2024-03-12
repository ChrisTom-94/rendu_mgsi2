#pragma once

#include "glm/glm.hpp"

#include <cstdint>

namespace SmartGL
{
    /**
     * @brief 2D Renderer -
     * This class is used to draw 2D primitives like quads, lines, circles, etc.
     * It uses a batch system to optimize draw calls.
    */
    class Renderer2D
    {

    public:
        /**
         * @brief Initialize the renderer (Called internally)
        */
        static void Init();

        /**
         * @brief Shutdown the renderer (Called internally)
        */
        static void Shutdown();

        /**
         * @brief Setup the scene and prepare for drawing
         * @param viewProjection The view projection matrix
         * @note - Should be called before any drawing
         * @note - Should be called only once per frame
        */
        static void BeginScene(const glm::mat4 &viewProjection);

        /**
         * @brief End the scene and flush all the draw calls
        */
        static void EndScene();

        // Primitives
        static void DrawQuad(const glm::vec2 &position, const glm::vec2 &size = glm::vec2(1.0f), const glm::vec4 &color = glm::vec4(1.0f), int entityID = -1);
        static void DrawQuad(const glm::vec3 &position, const glm::vec2 &size = glm::vec2(1.0f), const glm::vec4 &color = glm::vec4(1.0f), int entityID = -1);
        static void DrawQuad(const glm::vec3 &position, const glm::mat4 &transform, const glm::vec2 &size = glm::vec2(1.0f), const glm::vec4 &color = glm::vec4(1.0f), int entityID = -1);
        static void DrawQuad(const glm::mat4 &transform, const glm::vec4 &color, int entityID);

        static void DrawCircle(const glm::vec2 &position, const glm::vec4 &color = glm::vec4(1.0f), float radius = 1.0f, float thickness = 1.0f, float fade = 0.005, int entityID = -1);
        static void DrawCircle(const glm::vec3 &position, const glm::vec4 &color = glm::vec4(1.0f), float radius = 1.0f, float thickness = 1.0f, float fade = 0.005, int entityID = -1);
        static void DrawCircle(const glm::vec3 &position, const glm::mat4 &transform, const glm::vec4 &color = glm::vec4(1.0f), float radius = 1.0f, float thickness = 1.0f, float fade = 0.005, int entityID = -1);
        static void DrawCircle(const glm::mat4 &transform, const glm::vec4 &color, float thickness, float fade, int entityID);

        static void DrawLine(const glm::vec2 &start, const glm::vec2 &end, const glm::vec4 &color = glm::vec4(1.0));
        static void DrawLine(const glm::vec3 &start, const glm::vec3 &end, const glm::vec4 &color = glm::vec4(1.0));

        static void DrawPoint(const glm::vec2 &position, const glm::vec4 &color = glm::vec4(1.0f), float size = 1.0f);
        static void DrawPoint(const glm::vec3 &position, const glm::vec4 &color = glm::vec4(1.0f), float size = 1.0f);

        // Settings
        static void SetLineWidth(float width);

        // Stats
        struct Statistics
        {
            uint32_t DrawCalls = 0;
            uint32_t QuadCount = 0;
            uint32_t LineCount = 0;
            uint32_t PointCount = 0;

            uint32_t GetTotalVertexQuadCount() const { return QuadCount * 4; };
            uint32_t GetTotalIndexQuadCount() const { return QuadCount * 6; };
            uint32_t GetTotalVertexLineCount() const { return LineCount * 2; };
            uint32_t GetTotalIndexLineCount() const { return LineCount * 2; };

            uint32_t GetTotalVertexCount() const { return GetTotalVertexQuadCount() + GetTotalVertexLineCount() + PointCount;};
            uint32_t GetTotalIndexCount() const { return GetTotalIndexQuadCount() + GetTotalIndexLineCount(); };

            void Log() const;
        };

        static const Statistics &GetStats();
        static void ResetStats();

    private:
        static void StartBatch();
        static void NextBatch();
        static void Flush();
    };

}
