#pragma once

#include "Events/KeyEvent.h"
#include "Events/MouseEvent.h"

#include "glm/glm.hpp"

namespace SmartGL {
    class Input
	{
	public:
		static bool IsKeyPressed(Events::KeyCode key);
		static bool IsKeyReleased(Events::KeyCode key);

		static bool IsMouseButtonPressed(Events::MouseCode button);
		static bool IsMouseButtonHeld(Events::MouseCode button);
		static glm::vec2 GetMousePosition();
		static float GetMouseX();
		static float GetMouseY();
	};
}
