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

#include "recorder.h"

#include <chrono>
#include <vector>
#include <thread>
#include <fstream>
#include <iostream>
#include "lodepng.h" // Include the LodePNG library

#define GLEQ_IMPLEMENTATION
#include "gleq.h"

using namespace std;

struct TimeStampEvent {
    std::time_t timeStamp;
    GLEQevent event;
};

void printEvents(const GLEQevent& event)
{
    switch (event.type)
    {
    case GLEQ_WINDOW_MOVED:
        printf("Window moved to %i,%i\n", event.pos.x, event.pos.y);
        break;

    case GLEQ_WINDOW_RESIZED:
        printf("Window resized to %ix%i\n",
            event.size.width,
            event.size.height);
        break;

    case GLEQ_WINDOW_CLOSED:
        printf("Window close request\n");
        break;

    case GLEQ_WINDOW_REFRESH:
        printf("Window refresh request\n");
        break;

    case GLEQ_WINDOW_FOCUSED:
        printf("Window focused\n");
        break;

    case GLEQ_WINDOW_DEFOCUSED:
        printf("Window defocused\n");
        break;

    case GLEQ_WINDOW_ICONIFIED:
        printf("Window iconified\n");
        break;

    case GLEQ_WINDOW_UNICONIFIED:
        printf("Window uniconified\n");
        break;

#if GLFW_VERSION_MINOR >= 3
    case GLEQ_WINDOW_MAXIMIZED:
        printf("Window maximized\n");
        break;

    case GLEQ_WINDOW_UNMAXIMIZED:
        printf("Window unmaximized\n");
        break;

    case GLEQ_WINDOW_SCALE_CHANGED:
        printf("Window content scale %0.2fx%0.2f\n",
            event.scale.x, event.scale.y);
        break;
#endif // GLFW_VERSION_MINOR

    case GLEQ_FRAMEBUFFER_RESIZED:
        printf("Framebuffer resized to %ix%i\n",
            event.size.width,
            event.size.height);
        break;

    case GLEQ_BUTTON_PRESSED:
        printf("Mouse button %i pressed (mods 0x%x)\n",
            event.mouse.button,
            event.mouse.mods);
        break;

    case GLEQ_BUTTON_RELEASED:
        printf("Mouse button %i released (mods 0x%x)\n",
            event.mouse.button,
            event.mouse.mods);
        break;

    case GLEQ_CURSOR_MOVED:
        printf("Cursor moved to %i,%i\n", event.pos.x, event.pos.y);
        break;

    case GLEQ_CURSOR_ENTERED:
        printf("Cursor entered window\n");
        break;

    case GLEQ_CURSOR_LEFT:
        printf("Cursor left window\n");
        break;

    case GLEQ_SCROLLED:
        printf("Scrolled %0.2f,%0.2f\n",
            event.scroll.x, event.scroll.y);
        break;

    case GLEQ_KEY_PRESSED:
        printf("Key 0x%02x pressed (scancode 0x%x mods 0x%x)\n",
            event.keyboard.key,
            event.keyboard.scancode,
            event.keyboard.mods);
        break;

    case GLEQ_KEY_REPEATED:
        printf("Key 0x%02x repeated (scancode 0x%x mods 0x%x)\n",
            event.keyboard.key,
            event.keyboard.scancode,
            event.keyboard.mods);
        break;

    case GLEQ_KEY_RELEASED:
        printf("Key 0x%02x released (scancode 0x%x mods 0x%x)\n",
            event.keyboard.key,
            event.keyboard.scancode,
            event.keyboard.mods);
        break;

    case GLEQ_CODEPOINT_INPUT:
        printf("Codepoint U+%05X input\n", event.codepoint);
        break;

#if GLFW_VERSION_MINOR >= 1
    case GLEQ_FILE_DROPPED:
    {
        int i;

        printf("%i files dropped\n", event.file.count);
        for (i = 0; i < event.file.count; i++)
            printf("\t%s\n", event.file.paths[i]);

        break;
    }
#endif // GLFW_VERSION_MINOR

    case GLEQ_MONITOR_CONNECTED:
        printf("Monitor \"%s\" connected\n",
            glfwGetMonitorName(event.monitor));
        break;

    case GLEQ_MONITOR_DISCONNECTED:
        printf("Monitor \"%s\" disconnected\n",
            glfwGetMonitorName(event.monitor));
        break;

#if GLFW_VERSION_MINOR >= 2
    case GLEQ_JOYSTICK_CONNECTED:
        printf("Joystick %i \"%s\" connected\n",
            event.joystick,
            glfwGetJoystickName(event.joystick));
        break;

    case GLEQ_JOYSTICK_DISCONNECTED:
        printf("Joystick %i disconnected\n", event.joystick);
        break;
#endif // GLFW_VERSION_MINOR

    default:
        fprintf(stderr, "Error: Unknown event %i\n", event.type);
        break;
    }
}

//int convertGleqToGlfwEvent(int type)
//{
//    switch (type)
//    {
//        case GLEQ_KEY_PRESSED: return GLFW_PRESS;
//        case GLEQ_KEY_RELEASED: return GLFW_RELEASE;
//        case GLEQ_KEY_REPEATED: return GLFW_REPEAT;
//    }
//
//    return 0xFFFFFFFF;
//}

void defaultEventHandler(GLFWwindow* window, const GLEQevent& event)
{
    switch (event.type)
    {
    case GLEQ_WINDOW_MOVED:
        glfwSetWindowPos(window, event.pos.x, event.pos.y);
        break;

    case GLEQ_WINDOW_CLOSED:
        glfwSetWindowShouldClose(window, true);
        break;

    case GLEQ_WINDOW_RESIZED:
        glfwSetWindowSize(window,
            event.size.width,
            event.size.height
        );
        break;

    case GLEQ_WINDOW_MAXIMIZED:
        glfwMaximizeWindow(window);
        break;

    case GLEQ_WINDOW_UNMAXIMIZED:
        glfwRestoreWindow(window);
        break;

    case GLEQ_CURSOR_MOVED:
        glfwSetCursorPos(window, event.pos.x, event.pos.y);
        break;

    default:
        break;
    }
}

int recordIntoLogFile(GLFWwindow* window, std::string file, const std::function<void()>& lambda, const std::function<void(GLEQevent)>& eventHandlerImpl)
{
    gleqTrackWindow(window);
    ofstream outFile(file, ios::binary);
    if (!outFile) {
        cerr << "Error opening file!" << endl;
        return 1;
    }

    int totalEventCount = 0;

    while (!glfwWindowShouldClose(window))
    {
        TimeStampEvent event;

        while (gleqNextEvent(&event.event))
        {
            printEvents(event.event);

            auto currentTime = std::chrono::system_clock::now();
            event.timeStamp = std::chrono::system_clock::to_time_t(currentTime);
            outFile.write(reinterpret_cast<const char*>(&event), sizeof(TimeStampEvent));
            totalEventCount++;

            eventHandlerImpl(event.event);

            gleqFreeEvent(&event.event);
        }

        lambda();
    }

    return 0;
}

int playbackFromLogFile(GLFWwindow* window, std::string file, const std::function<void()>& updateAndDraw, const std::function<void(GLEQevent)>& userEventHandler)
{
    ifstream inFile(file, ios::binary);
    if (!inFile) {
        cerr << "Error opening file!" << endl;
        return 1;
    }

    // Determine the size of the file
    inFile.seekg(0, ios::end);
    streampos fileSize = inFile.tellg();
    inFile.seekg(0, ios::beg);

    // Allocate memory to store the read data
    vector<TimeStampEvent> messageQueue(fileSize / sizeof(TimeStampEvent));

    // Read the data from the file into the allocated memory
    inFile.read(reinterpret_cast<char*>(&messageQueue[0]), fileSize);

    vector<TimeStampEvent>::iterator it;
    int totalEventCount = 0;

    std::time_t currentTime = time(NULL);
    std::time_t nextEventTime = time(NULL);

    std::cout << "Opening file events recorded: " << std::endl;
    for (it = messageQueue.begin(); it != messageQueue.end(); it++) {
        if (totalEventCount == 0) {
            currentTime = it->timeStamp;
            nextEventTime = it->timeStamp;
        }
        else {
            nextEventTime = it->timeStamp;
            std::chrono::duration<double> difference{ (nextEventTime - currentTime) };
            currentTime = nextEventTime;

            // Sleep until the next event
            std::this_thread::sleep_for(difference);
        }

        ++totalEventCount;
        // printEvents(it->event);

        defaultEventHandler(window, it->event);

        userEventHandler(it->event);
        
        updateAndDraw();
    }

    return 0;
}

void render(GLFWwindow* window, const std::function<void()>& updateAndDraw, const std::function<void(GLEQevent)>& userEventHandler, RendererType type, std::string file)
{
    if (type == RendererType::PLAYBACK_LOG) {
        playbackFromLogFile(window, file, updateAndDraw, userEventHandler);
    }
    else if (type == RendererType::RECORD_LOG) {
        recordIntoLogFile(window, file, updateAndDraw, userEventHandler);
    }
    else {
        while (!glfwWindowShouldClose(window)) {
            updateAndDraw();
        }
    }
}

void saveTga(std::string filename, unsigned int width, unsigned int height) {
    size_t numberOfPixels = static_cast<size_t>(width * height * 3);
    unsigned char* pixels = new unsigned char[numberOfPixels];

    glPixelStorei(GL_PACK_ALIGNMENT, 1);
    glReadBuffer(GL_FRONT);
    glReadPixels(0, 0, width, height, GL_BGR_EXT, GL_UNSIGNED_BYTE, pixels);

    //FILE* outputFile = fopen(filename.c_str(), "w");
    //short header[] = { 0, 2, 0, 0, 0, 0, (short)width, (short)height, 24 };

    //fwrite(&header, sizeof(header), 1, outputFile);
    //fwrite(pixels, numberOfPixels, 1, outputFile);
    //fclose(outputFile);

    delete pixels;
    printf("Finish writing to file.\n");
}

int savePng(std::string filename, unsigned int width, unsigned int height) {

    // Create a vector to store pixel data (RGBA format)
    std::vector<unsigned char> image;
    image.resize(width * height * 4);

    size_t numberOfPixels = static_cast<size_t>(width * height * 3);
    unsigned char* pixels = new unsigned char[numberOfPixels];

    glPixelStorei(GL_PACK_ALIGNMENT, 1);
    glReadBuffer(GL_FRONT);
    glReadPixels(0, 0, width, height, GL_BGR_EXT, GL_UNSIGNED_BYTE, pixels);

    // Correct the upside-down pixel data
    unsigned char* flippedPixels = new unsigned char[width * height * 3];
    for (unsigned int y = 0; y < height; y++) {
        memcpy(&flippedPixels[y * width * 3], &pixels[(height - y - 1) * width * 3], width * 3);
    }

    // Fill the image with some sample data (e.g., a gradient)
    for (unsigned int y = 0; y < height; y++) {
        for (unsigned int x = 0; x < width; x++) {
            unsigned int index = 4 * (y * width + x);
            image[index] = flippedPixels[3 * (y * width + x) + 2] % 256; // Blue component
            image[index + 1] = flippedPixels[3 * (y * width + x) + 1] % 256; // Green component
            image[index + 2] = flippedPixels[3 * (y * width + x)] % 256; // Red component
            image[index + 3] = 255; // Alpha component (fully opaque)
        }
    }

    short header[] = { 0, 2, 0, 0, 0, 0, (short)width, (short)height, 24 };

    // Encode the pixel data into a PNG file
    unsigned error = lodepng::encode(filename, image, width, height);

    // Check for encoding errors
    if (error) {
        std::cout << "PNG encoding error: " << lodepng_error_text(error) << std::endl;
        return 1;
    }

    std::cout << "PNG file created successfully! " << width << "x" << height << std::endl;
    return 0;
}

int saveScreenshotToFileOrig(std::string filename, unsigned int width, unsigned int height) {

    // Create a vector to store pixel data (RGBA format)
    std::vector<unsigned char> image;
    image.resize(width * height * 4);

    // Fill the image with some sample data (e.g., a gradient)
    for (unsigned int y = 0; y < height; y++) {
        for (unsigned int x = 0; x < width; x++) {
            unsigned int index = 4 * (y * width + x);
            image[index + 0] = x % 256; // Red component
            image[index + 1] = y % 256; // Green component
            image[index + 2] = (x + y) % 256; // Blue component
            image[index + 3] = 255; // Alpha component (fully opaque)
        }
    }

    // Encode the pixel data into a PNG file
    unsigned error = lodepng::encode("output.png", image, width, height);

    // Check for encoding errors
    if (error) {
        std::cout << "PNG encoding error: " << lodepng_error_text(error) << std::endl;
        return 1;
    }

    std::cout << "PNG file created successfully! "<< width << "x" << height << std::endl;
    return 0;
}
