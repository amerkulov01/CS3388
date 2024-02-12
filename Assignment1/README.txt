The Hello World of OpenGL, written in C++

This is a simple example of how to use OpenGL to draw a triangle on the screen.
I first created a window of dimensions 1280x1000, titled Hello World, and
then I drew a triangle on the screen. choosing my colour and placing the vertices at (0, 0.5), (0.5,−0.25), (−0.5,−0.25)
using immediate mode.

To compile this with g++:
g++ HelloWorld.cpp -lOpenGL -lglfw

