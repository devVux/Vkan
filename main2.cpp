
#include "Window.h"
#include "RenderSystem.h"
#include "Renderer.h"

int main() {

	glfwInit();

	const auto& context = ContextBuilder()
		.applicationName("App")
		.applicationVersion(1, 0, 0)
		.engineName("Engine")
		.engineVersion(1, 0, 0)
		.apiVersion(0, 1, 0)
		.create();

{
	Window window(context, "Test window");

	RenderSystem system;
	system.init(context, window);

	while (!window.shouldClose()) {
		window.update();

		FrameContext context = system.createFrameContext();

		system.begin(context);
		{

			RenderCommands::draw(context, 3, 1, 0, 0);

		}
		system.end(context);

	}
}


	glfwTerminate();
}
