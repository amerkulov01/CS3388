/*Andrei Merkulov, 251145994
 * The Hello World of OpenGL
 * This program draws a triangle using immediate mode
 * CS 3388 Assignment 1
 * 2023-01-19
 */

#include <GLFW/glfw3.h>

int main()
{
    GLFWwindow* window;

    /* Initialize the library */
    if (!glfwInit())
        return -1;

    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(1280, 1000, "Hello World", nullptr, nullptr);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(window);

    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {
        /* Poll for and process events */
        glfwPollEvents();

        /* Render here */
        glClear(GL_COLOR_BUFFER_BIT);
        glClearColor(1.0f, 1.0f, 0.1f, 1.0f);

        /* use immediate mode to draw a triangle */
        glBegin ( GL_TRIANGLE_FAN );
        glVertex2f(0.4, 0.4);
        glVertex2f(-0.3, 0.2);
        glVertex2f(0.2, -0.3);
        glEnd ();

        /* Swap front and back buffers */
        glfwSwapBuffers(window);


    }

    glfwTerminate();
    return 0;
}