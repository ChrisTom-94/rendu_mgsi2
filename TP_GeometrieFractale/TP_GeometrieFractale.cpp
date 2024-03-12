#include "SmartGL.h"
#include "Core/EntryPoint.h"

#include "src/Editor.h"

using namespace SmartGL;

class Main : public Core::Application
{
public:
    Main(Core::ApplicationSpecification specification)
        : SmartGL::Core::Application(specification)
    {
        PushLayer(new TP_GeometrieFractale::Editor());
    }

    ~Main() {}
};

Core::Application *Core::CreateApplication()
{
    Core::ApplicationSpecification specification;
    specification.Name = "TP_GeometrieFractale";
#if defined(COMPILER_GCC) || defined(COMPILER_MINGW)
    specification.WorkingDirectory = "TP_GeometrieFractale/";
#else
    specification.WorkingDirectory = "../TP_GeometrieFractale/";
#endif
    return new Main(specification);
}
