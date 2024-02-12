This code is an implementation of the Marching Cubes algorithm, which generates a 3D triangular mesh from a scalar
field. The algorithm examines field values at the corners of cubes while marching through the 3D space to create
triangles. This implementation uses GLFW and GLEW for OpenGL support and GLM for matrix operations.

To install the program, a C++ compiler and the OpenGL, GLFW, GLM, and GLEW libraries are required.

The main features of the program include Marching Cubes mesh generation, camera controls for orbiting around the mesh,
vertex normals computation for shading, and optional PLY file output for saving the generated mesh. The program also
includes different sample 3D functions for testing.

To use the program, the arrow keys are used to move the camera around the mesh, and the left mouse button is clicked
and dragged to rotate the camera. The desired 3D function can be changed by commenting/uncommenting the desired function
in the code. The step size, min/max coordinates, iso-value, and output file name can be adjusted by modifying the
corresponding variables in the main function.