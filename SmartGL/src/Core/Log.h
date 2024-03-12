#pragma once

#include "Core/TypesDefinitions.h"

#define GLM_ENABLE_EXPERIMENTAL
#include "glm/gtx/string_cast.hpp"

#pragma warning(push, 0)
#include "spdlog/spdlog.h"
#include "spdlog/fmt/ostr.h"
#pragma warning(pop)

namespace SmartGL
{
    namespace Core
    {
        class Logger
        {
        public:
            static void Init();

            inline static Shared<spdlog::logger> &Get() { return s_Logger; }

        private:
            static Shared<spdlog::logger> s_Logger;
        };
    }
}

template <typename OStream, glm::length_t L, typename T, glm::qualifier Q>
inline OStream &operator<<(OStream &os, const glm::vec<L, T, Q> &vector)
{
    return os << glm::to_string(vector);
};

template <typename OStream, glm::length_t C, glm::length_t R, typename T, glm::qualifier Q>
inline OStream &operator<<(OStream &os, const glm::mat<C, R, T, Q> &matrix)
{
    return os << glm::to_string(matrix);
};

template <typename OStream, typename T, glm::qualifier Q>
inline OStream &operator<<(OStream &os, glm::qua<T, Q> quaternion)
{
    return os << glm::to_string(quaternion);
};

#define SMART_LOG_TRACE(...) ::SmartGL::Core::Logger::Get()->trace(__VA_ARGS__)
#define SMART_LOG_INFO(...) ::SmartGL::Core::Logger::Get()->info(__VA_ARGS__)
#define SMART_LOG_WARN(...) ::SmartGL::Core::Logger::Get()->warn(__VA_ARGS__)
#define SMART_LOG_ERROR(...) ::SmartGL::Core::Logger::Get()->error(__VA_ARGS__)
#define SMART_LOG_CRITICAL(...) ::SmartGL::Core::Logger::Get()->critical(__VA_ARGS__)
