#pragma once
/****************
GLFW Wrapper with BASIC interface.

Extend functionality as you see fit.

This class should ONLY use GLFW!

No OpenGL!!!
*****************/

#include <gl_core_4_4.h>
#include <glm\glm.hpp>
#include <GLFW\glfw3.h>

struct GLFWwindow;

namespace nsfw
{
	class Window
	{
		GLFWwindow *window;
		unsigned width, height;		// keep track of width height- needed for glViewport FBO integration
		double currentXCursor = 0, currentYCursor = 0;

		float deltaTime;
		float lastTime = 0;
		float currentTime = 0;

		Window():window(nullptr),width(0),height(0) {}
	public:
		static Window &instance() { static Window w; return w; }
		
		//create the context!
		void init(unsigned width, unsigned height);
		
		// swapBuffers and pollEvents!
		void step();

		// kill the context!
		void term();

		// just wrap what GLFW already does
		float		getTime()					const;
		float		getDeltaTime();
		bool		getKey(unsigned k)			const;
		bool		getShouldClose()			const;
		unsigned	getWidth()					const;
		unsigned	getHeight()					const;
		glm::mat4	getTexelAdjustmentMatrix()	const;
		glm::vec2	getCursorPos();
		int			getMouseButton();
	};
}