/*
 * GLFW_LOGGER a simple logger for GLFW 3
 * Copyright © Parminder Singh <engineer.parminder@gmail.com>
 *
 * This software is provided 'as-is', without any express or implied
 * warranty. In no event will the authors be held liable for any damages
 * arising from the use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software. If you use this software
 *    in a product, an acknowledgment in the product documentation would
 *    be appreciated but is not required.
 *
 * 2. Altered source versions must be plainly marked as such, and must not
 *    be misrepresented as being the original software.
 *
 * 3. This notice may not be removed or altered from any source
 *    distribution.
 */

#ifndef GLFW_LOGGER_HEADER_FILE
#define GLFW_LOGGER_HEADER_FILE

#include <GLFW/glfw3.h>
#include <functional>
#include <string>
#include <chrono>

using namespace std;

#define UNUSED(expr) (void)(expr)

enum RendererType
{
    NONE,               // No Recording, No playback
    RECORD_LOG,         // Record the event in log
    PLAYBACK_LOG        // Playback the log
};
extern struct GLEQevent;
void printEvents(const GLEQevent& event);
int convertGleqToGlfwEvent(int action) { return 0;  }

void defaultEventHandler(GLFWwindow* window, const GLEQevent& event);

int recordIntoLogFile(GLFWwindow* window, std::string file, const std::function<void()>& lambda, const std::function<void(GLEQevent)>& eventHandlerImpl);
int playbackFromLogFile(GLFWwindow* window, std::string file, const std::function<void()>& updateAndDraw, const std::function<void(GLEQevent)>& userEventHandler);

void render(GLFWwindow* window, const std::function<void()>& updateAndDraw, const std::function<void(GLEQevent)>& userEventHandler, RendererType type, std::string file = "data.bin");

void saveTga(std::string filename, unsigned int width, unsigned int height);
int savePng(std::string filename, unsigned int width, unsigned int height);
int saveScreenshotToFileOrig(std::string filename, unsigned int width, unsigned int height);
#endif /* GLFW_LOGGER_HEADER_FILE */

