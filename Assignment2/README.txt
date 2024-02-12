README for Assignment 2

This assignment consists of two exercises, both of which utilize OpenGL to draw and display images.
The first exercise is to draw a dog poly-line image at 8 different points along a circle, and rotate the dogs 1 degree
counterclockwise with each frame. The second exercise is to draw a dot plot using a random algorithm, taking the number
of points, screen width, and screen height as command line arguments.

Exercise 1: Dog Poly-Line Image
The dog.txt file contains a list of floating point numbers that represent the coordinates of vertices. The viewing
volume is defined as a rectangle with left=0, right=60, bottom=0, and top=60. The background is white and the dog
poly-line image is drawn in black color around a circle with a radius of 25, centered at (30,30). The dogs are rotated
counterclockwise 1 degree in each frame.

Exercise 2: Dot Plot
The program is a dot plot drawing algorithm that takes 3 command line arguments: N (number of points), screen width,
and screen height. The viewing volume is set to a fixed value of (-1.1, 1.1, -1.1, 1.1) using the glOrtho function,
with a white background and black dots. The points are drawn outside of the while loop and the window is kept open with
a loop and glfwSwapBuffers is called once. The program is designed to support a large number of points, up to 5 million.