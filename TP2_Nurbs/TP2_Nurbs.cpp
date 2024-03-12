#include "SmartGL.h"
#include "Core/EntryPoint.h"

#include "glm/glm.hpp"
#include "imgui/imgui.h"

#include "src/Editor.h"

using namespace SmartGL;

class Main : public SmartGL::Core::Application
{
public:
    Main(SmartGL::Core::ApplicationSpecification specification)
            : SmartGL::Core::Application(specification)
    {
        PushLayer(new TP2_Nurbs::Editor());
    }

    ~Main(){}
};

SmartGL::Core::Application * SmartGL::Core::CreateApplication()
{
    SmartGL::Core::ApplicationSpecification specification;
    specification.Name = "TP2_Nurbs";

#if defined(COMPILER_GCC) || defined(COMPILER_MINGW)
    specification.WorkingDirectory = "TP2_Nurbs/";
#else
    specification.WorkingDirectory = "../TP2_Nurbs/";
#endif

    return new Main(specification);
}
