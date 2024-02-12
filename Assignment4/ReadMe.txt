Assignment 4 - 3388

Introduction
This assignment focuses on working with triangular meshes, reading mesh data from files, and rendering textured triangle
meshes using Vertex Buffer Objects (VBOs) and Vertex Array Objects (VAOs). The objective of the assignment is to create
a program that loads triangle mesh data and textures from files, renders them, and allows the user to explore that
rendered world by moving the camera around via arrow keys.

Camera
The program creates a limited "first-person camera," where the user views the world as if they were in it. The camera is
directly controllable by the user. The initial position of the camera is (0.5, 0.4, 0.5) in world space, and it looks in
the direction of (0, 0, -1). The arrow keys (up, down, left, right) control the movement of the camera. Pressing the up
key moves the camera forward in the direction it is currently facing, while pressing the down key moves the camera
backward in the exact opposite direction. Pressing the left key rotates the camera counterclockwise without moving,
while pressing the right key rotates the camera clockwise without moving. The camera moves relatively smoothly.

File Reading
A function is created that, given a file name, returns a list of vertices and a list of faces. The function consists of
a VertexData class/struct, which has instance variables for position (x, y, z), a normal vector (nx, ny, nz),
color (r, g, b), and texture coordinates (u, v). A TriData class/struct is also implemented, which has three instance
variables for the indices of the three vertices making up the triangle. The readPLYFile function reads a PLY file and
returns two lists: a list of VertexData objects (one object for each vertex specified in the PLY file) and a list of
TriData objects (one object for each face specified in the PLY file).

Code Structure
The TexturedMesh class is created, which encapsulates a textured triangle mesh. The constructor of this class takes two
file paths: one for a PLY file and another for a bitmap image file. The class uses the readPLYFile function to read the
vertices and faces data from the PLY file. The class has instance variables for an integer ID for the VBO to store the
vertex positions, an integer ID for the VBO to store the texture coordinates, an integer ID for the VBO to store the
face's vertex indices, an integer ID for the Texture Object created to store the bitmap image, an integer ID for the VAO
used to render the texture mesh, and an integer ID for the shader program created and linked to render the particular
textured mesh.

Rendering
Each TexturedMesh object is created in the main method, and a projection matrix is set up with a vertical field of view
of 45°. In the render loop, user input is used to move the camera around, and TextureMesh.draw is called for each
TextureMesh object.

Conclusion
By the end of this assignment, the user will have gained an understanding of triangular meshes, working with the camera
in world space, reading mesh data from files, and rendering textured triangle meshes using VBOs and VAOs.