include 'dependencies.lua'

workspace 'MGSI'
    architecture 'x64'
    startproject 'TP1_Nurbs'
    platforms {"x64", "x86"}
    flags {"MultiProcessorCompile"}

    configurations
    {
        'Debug',
        'Release',
        'Dist'
    }

outputdir = '%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}'

group 'Dependencies'
    include 'SmartGL/includes/glfw'
    include 'SmartGL/includes/glad'
    include 'SmartGL/includes/imgui'
group ''

group 'SmartGL'
    include 'SmartGL'
group ''

group 'TPs'
    include 'TP1_Nurbs'
    include 'TP2_Nurbs'
    -- include 'TP_GeometrieFractale'
    include 'TP_Deformations'
group ''
