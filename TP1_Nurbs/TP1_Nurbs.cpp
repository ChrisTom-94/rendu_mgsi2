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
        PushLayer(new TP1_Nurbs::Editor());
    }

    ~Main() {}
};

Core::Application *Core::CreateApplication()
{
    Core::ApplicationSpecification specification;
    specification.Name = "TP1_Nurbs";

#if defined(COMPILER_GCC) || defined(COMPILER_MINGW)
    specification.WorkingDirectory = "TP1_Nurbs/";
#else
    specification.WorkingDirectory = "../TP1_Nurbs/";
#endif
    return new Main(specification);
}
