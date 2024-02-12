/*
 * Andrei Merkulov, 251145994
 * draw a dot plot using chaos game algorithm
 * CS 3388 Assignment 2 exercise 2
 * 2023-02-02
 */

#include <iostream>
#include <random>
#include <GLFW/glfw3.h>
#include <cmath>

const int N = 5000000;  // number of dots to draw
const int WIDTH = 800;  // width of the window
const int HEIGHT = 800; // height of the window

int random_corner() {   // random number generator
    static std::mt19937 rng(std::random_device{}());
    static std::uniform_int_distribution<int> dist(0, 3);
    return dist(rng);
}

void draw_dot(double x, double y) { // drawing a dot
    glColor3f(0, 0, 0);            // black
    glBegin(GL_POINTS);
    glVertex2d(x, y);
    glEnd();
}

int main(int argc, char** argv) {   // main function
    if (!glfwInit())
        return -1;

    GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "Exercise 2", NULL, NULL); // creating a window
    if (!window) {
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);

    glOrtho(-1.1, 1.1, -1.1, 1.1, -1.0, 1.0);   // setting the coordinate system
    glClearColor(1.0, 1.0, 1.0, 1.0);        // setting the background color
    glClear(GL_COLOR_BUFFER_BIT);

    glPointSize(2.0f);

    int c = random_corner();
    double x = (c == 0 || c == 3) ? -1.0 : 1.0; // setting the initial coordinates
    double y = (c == 0 || c == 1) ? -1.0 : 1.0; // of the dot

    draw_dot(x, y);

    for (int i = 1; i < N; ++i) {   // drawing the dots
        int next_corner = random_corner();  // choosing the next corner
        while ((next_corner + 2) % 4 == c)  // making sure the next corner is not the opposite of the current one
            next_corner = random_corner();

        c = next_corner;    // setting the current corner to the next one
        x = (x + (c == 0 || c == 3 ? -1.0 : 1.0)) / 2.0;    // setting the x coordinate of the dot
        y = (y + (c == 0 || c == 1 ? -1.0 : 1.0)) / 2.0;    // setting the y coordinate of the dot

        draw_dot(x, y);
    }

    glfwSwapBuffers(window);

    while (!glfwWindowShouldClose(window)) {    // waiting for the user to close the window
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}
