/*
 * Andrei Merkulov, 251145994
 * Output a spinning dog
 * CS 3388 Assignment 2 exercise 1
 * 2023-02-02
 */

#include <GLFW/glfw3.h>
#include <cstdio>
#include <cmath>

struct pair {           // storing coordinates in a struct
    float x;
    float y;
};

int main() {
    FILE *ptr = fopen("dog.txt", "r");      // reading the file
    if (ptr == nullptr) {
        printf("file can't be opened \n");
        return -1;
    }

    int counter = 0; // counter to find the number of pairs in the file
    char ch;
    do {
        ch = fgetc(ptr);
        if (ch == ' ') {            // counting spaces
            counter++;
        }
    } while (ch != EOF);

    struct pair pairs[counter / 2]; // creating a struct of coordinate pairs, which we now know the size of
    fseek(ptr, 0, SEEK_SET);        // resetting the pointer
    int index = 0;                  // placing the coordinate pairs into the array of structs
    while (fscanf(ptr, "%f %f", &pairs[index].x, &pairs[index].y) == 2) {
        index++;
    }
    fclose(ptr);                    // closing the file

    GLFWwindow* window;            // window declaration

    // Initialize the library
    if (!glfwInit())
        return -1;

    // Create a windowed mode window and its OpenGL context
    window = glfwCreateWindow(800, 800, "Exercise 1", nullptr, nullptr);
    if (!window) {
        glfwTerminate();
        return -1;
    }

    //Make the window's context current
    glfwMakeContextCurrent(window);

    // setting the projection matrix
    // to centre the viewing volume, we had to translate it by 30,30,0
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, 60, 0, 60, -1, 1);
    glTranslatef(30, 30, 0);
    float angle = 0.0; // angle for spinning dogs

    // Loop until the user closes the window
    while (!glfwWindowShouldClose(window)) {
        // Render here
        glClearColor(1.0, 1.0, 1.0, 1.0);   // setting the background colour to white
        glClear(GL_COLOR_BUFFER_BIT);   // clearing the buffer
        glColor3f(0.0, 0.0, 0.0);   // setting the colour of dog to black

        float angleIncrement = 45.0 * (M_PI / 180.0); // converting to radians and incrementing the angle for each dog
        float rotate = angle * (M_PI / 180.0); // converting to radians and rotating the dog

        //for loop for 8 dogs
        for (int i = 0; i < 8; i++) {
            glBegin(GL_LINE_STRIP); // drawing the dog
            for (int j = 0; j < counter/2; j++) {
                float x = pairs[j].x * cos(rotate) - pairs[j].y * sin(rotate);
                float y = pairs[j].x * sin(rotate) + pairs[j].y * cos(rotate);
                glVertex2f(x+25*cos(angleIncrement*i), y+25*sin(angleIncrement*i)); // rotating the dog by the angle increment about radius 25
            }
            glEnd();
        }
        angle += 1.0; // incrementing the angle for the next frame
        // Swap front and back buffers
        glfwSwapBuffers(window);
        // Poll for and process events
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}
