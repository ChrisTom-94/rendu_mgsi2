#include "precompiled.h"

#include "Core/Layer.h"

namespace SmartGL
{
    namespace Core
    {
        Layer::Layer(const std::string &name)
            : m_Name(name)
        {
        }

        Layer::~Layer()
        {
        }
    }
}
