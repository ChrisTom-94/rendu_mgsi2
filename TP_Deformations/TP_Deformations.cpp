#include "SmartGL.h"
#include "Core/EntryPoint.h"

#include "glm/glm.hpp"
#include "imgui/imgui.h"

#include "src/Editor.h"

using namespace SmartGL;

class Main : public Core::Application
{
public:
    Main(Core::ApplicationSpecification specification)
        : SmartGL::Core::Application(specification)
    {
        PushLayer(new TP_Deformations::Editor());
    }

    ~Main() {}
};

Core::Application *Core::CreateApplication()
{
    Core::ApplicationSpecification specification;
    specification.Name = "TP_Deformations";

#if defined(COMPILER_GCC) || defined(COMPILER_MINGW)
    specification.WorkingDirectory = "TP_Deformations/";
#else
    specification.WorkingDirectory = "../TP_Deformations/";
#endif

    return new Main(specification);
}
