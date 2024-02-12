/*
 * Andrei Merkulov, 251145994
 * CS3388 - Assignment 4
 * 2023 - 03 - 12
 */
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
GLFWwindow* window;
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/rotate_vector.hpp>
using namespace glm;
#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>

/*
 * Function to load a BMP image which was provided :)
 */
void loadBitmap(const char* imagePath, unsigned char** data, unsigned int* width, unsigned int* height) {
    printf("Reading image %s\n", imagePath);
    unsigned char header[54];
    unsigned int dataPos;
    unsigned int imageSize;
    // Actual RGBA data

    // Open the file
    FILE * file = fopen(imagePath,"rb");
    if (!file){
        printf("%s could not be opened. Are you in the right directory?\n", imagePath);
        getchar();
        return;
    }

    // Read the header, i.e. the 54 first bytes

    // If less than 54 bytes are read, problem
    if ( fread(header, 1, 54, file)!=54 ){
        printf("Not a correct BMP file1\n");
        fclose(file);
        return;
    }

    // Read the information about the image
    dataPos    = *(int*)&(header[0x0A]);
    imageSize  = *(int*)&(header[0x22]);
    *width      = *(int*)&(header[0x12]);
    *height     = *(int*)&(header[0x16]);
    // A BMP files always begins with "BM"
    if ( header[0]!='B' || header[1]!='M' ){
        printf("Not a correct BMP file2\n");
        fclose(file);
        return;
    }
    // Make sure this is a 32bpp file
    if ( *(int*)&(header[0x1E])!=3  ) {
        printf("Not a correct BMP file3\n");
        fclose(file);
        return;
    }
    // fprintf(stderr, "header[0x1c]: %d\n", *(int*)&(header[0x1c]));
    // if ( *(int*)&(header[0x1C])!=32 ) {
    //     printf("Not a correct BMP file4\n");
    //     fclose(file);
    //     return;
    // }

    // Some BMP files are misformatted, guess missing information
    if (imageSize==0)    imageSize=(*width)* (*height)*4; // 4 : one byte for each Red, Green, Blue, Alpha component
    if (dataPos==0)      dataPos=54; // The BMP header is done that way

    // Create a buffer
    *data = new unsigned char [imageSize];

    if (dataPos != 54) {
        fread(header, 1, dataPos - 54, file);
    }
    // Read the actual data from the file into the buffer
    fread(*data,1,imageSize,file);
    // Everything is in memory now, the file can be closed.
    fclose (file);
}

/*
 * Function to control the camera in the scene
 */
void cameraControls(glm::mat4& V) {
    static const float movementSpeed = 0.04f; // adjust this for desired movement speed
    static const float rotationSpeed = 1.5f; // adjust this for desired rotation speed

    static glm::vec3 cameraPosition(0.5f, 0.4f, 0.5f);
    static glm::vec3 cameraDirection(0.0f, 0.0f, -1.0f);
    static glm::vec3 cameraUp(0.0f, 1.0f, 0.0f);

    /*
     * Taking in the input from the keyboard and moving the camera accordingly
     */
    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {    // Moving the camera forward
        cameraPosition += movementSpeed * cameraDirection;
    }
    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {  // Moving the camera backward
        cameraPosition -= movementSpeed * cameraDirection;
    }
    if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) {  // Rotating the camera left
        cameraDirection = glm::rotate(cameraDirection, glm::radians(rotationSpeed), cameraUp);
    }
    if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) { // Rotating the camera right
        cameraDirection = glm::rotate(cameraDirection, glm::radians(-rotationSpeed), cameraUp);
    }
    V = glm::lookAt(cameraPosition, cameraPosition + cameraDirection, cameraUp);    // Setting the view matrix
}

struct TriangleInfo{ //struct to hold triangle data
    GLuint v1,v2,v3;
};

struct VertexData{  //struct to hold vertex data
    GLfloat x,y,z,
    nx,ny,nz,
    r,g,b,
    u,v;
};

/*
 * This function reads in a PLY file and stores the data in the appropriate vectors
 */
void readPLYFile(std::string fname, std::vector<VertexData>& vertices, std::vector<TriangleInfo>& faces) {
    std::ifstream input(fname); //open fil
    if (!input) {   //check if file is open
        std::cerr << "Unable to open file " << fname << std::endl;
        exit(1);    //exit if file is not open
    }
    std::string line;   //string to hold each line of the file
    bool header = true; //flag to check if the file has a header
    bool has_color = false;   //flag to check if the file has color data
    int num_vertices = 0, num_faces = 0;    //variables to hold the number of vertices and faces

    while (getline(input, line)) {  //read each line of the file
        if (line == "end_header") {
            header = false;
            continue;
        }
        if (header) {   //if the file has a header, read the header
            std::string prefix, type;
            int count;
            std::stringstream ss(line); //use stringstream to parse the line
            ss >> prefix >> type >> count;
            if (prefix == "element" && type == "vertex") {  //if the line is an element vertex line, store the number of vertices
                num_vertices = count;
            } else if (prefix == "element" && type == "face") { //if the line is an element face line, store the number of faces
                num_faces = count;
            } else if (prefix == "property" && type == "uchar" && line.find("red") != std::string::npos) {  //if the line is a property line with red, the file has color data
                has_color = true;
            }
        } else {    //if the file does not have a header, read the data
            if (num_vertices > 0) { //if there are still vertices to read, read the vertex data
                VertexData vertex;
                std::stringstream ss(line);
                ss >> vertex.x >> vertex.y >> vertex.z >> vertex.nx >> vertex.ny >> vertex.nz;  //read in the vertex data
                if (has_color) {
                    ss >> vertex.r >> vertex.g >> vertex.b;
                }
                ss >> vertex.u >> vertex.v; //read in the texture coordinates
                vertices.push_back(vertex); //add the vertex to the vector
                num_vertices--;
            } else if (num_faces > 0) { //if there are still faces to read, read the face data
                TriangleInfo face;
                std::stringstream ss(line); //use stringstream to parse the line
                int num_indices;
                ss >> num_indices;
                if (num_indices != 3) { //check if the face is a triangle
                    std::cerr << "Error: Only triangular faces are supported." << std::endl;    
                    exit(1);    //exit if the face is not a triangle
                }
                ss >> face.v1 >> face.v2 >> face.v3;
                faces.push_back(face);
                num_faces--;
            }
        }
    }
    input.close();
}
/*
 * This function reads in a BMP file and stores the data in the appropriate vectors
 */
class TexturedMesh{         //class to store data for textured meshes

    std::vector<VertexData> vertices;   // vectors to store vertex and face data  
    std::vector<TriangleInfo> faces;         
    std::string ply_Path;            //variables to store paths to ply and bmp files
    std::string bmp_Path;         

    GLuint vertex_posVBO,       // VBOs to store vertex data
    texture_coordVBO,           // VBOs to store texture coordinates
    vertex_indexVBO,            // VBO to store vertex indices
    textureObj,                 // texture object
    meshVAO,                    // VAO to store mesh data
    programID;                  // shader program ID

    unsigned char* data;            //pointer to store bmp data        
    unsigned int width, height;     //variables to store width and height of bmp

public: 

    TexturedMesh(std::string plyPath, std::string bmpPath){ //constructor to initialize variables

        ply_Path = plyPath;
        bmp_Path = bmpPath;

        loadBitmap(bmp_Path.data(), &data, &width, &height);    // load bmp file and store data in data pointer
        readPLYFile(ply_Path, vertices, faces);     // read ply file and store data in vertices and faces vectors
        glGenVertexArrays(1, &meshVAO);    //create VAO
        glBindVertexArray(meshVAO);       //bind VAO

        int nVerts = vertices.size();   //get number of vertices
        glGenBuffers(1, &vertex_posVBO);    //create vertex position VBO
        glBindBuffer(GL_ARRAY_BUFFER, vertex_posVBO);   //bind vertex position VBO
        glBufferData(GL_ARRAY_BUFFER, sizeof(GL_FLOAT) * 11 * nVerts, &(vertices[0]), GL_STATIC_DRAW);  // store vertex data in VBO
        glEnableVertexAttribArray(0);   //enable vertex attribute
        glVertexAttribPointer(  //set vertex attribute
                0,              
                3,                  // size
                GL_FLOAT,           // type
                GL_FALSE,           //normalized
                sizeof(GL_FLOAT) * 11,  // stride
                (void*) 0               // data -> takes 0 values to get to vertex position, 0 x 4 bytes = 0
        );
        glGenBuffers(1, &texture_coordVBO); //create texture coordinate VBO
        glBindBuffer(GL_ARRAY_BUFFER, texture_coordVBO);    //bind texture coordinate VBO
        glBufferData(GL_ARRAY_BUFFER, sizeof(GL_FLOAT) * 11 * nVerts, &(vertices[0]), GL_STATIC_DRAW);  // store texture coordinate data in VBO
        glEnableVertexAttribArray(1);   //enable vertex attribute
        glVertexAttribPointer(
                1,                                // attribute
                2,                                // size
                GL_FLOAT,                         // type
                GL_FALSE,                         // normalized
                sizeof(GL_FLOAT) * 11,            // stride
                (void*) 36 			              // data -> takes 9 values to get to texture coordinates, 9 x 4 bytes = 36
        );

        glGenBuffers(1, &vertex_indexVBO);  //create vertex index VBO
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vertex_indexVBO);  //bind vertex index VBO
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GL_UNSIGNED_INT) * 3 * faces.size(), &(faces[0]), GL_STATIC_DRAW);
        glBindVertexArray(0);   //unbind VAO

        GLuint VertexShaderID = glCreateShader(GL_VERTEX_SHADER);   //create vertex shader
        GLuint FragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);   //create fragment shader
        std::string VertexShaderCode = "\   
    	#version 330 core\n\
		// Input vertex data, different for all executions of this shader.\n\
		layout(location = 0) in vec3 vertexPosition;\n\
		layout(location = 1) in vec2 uv;\n\
		// Output data ; will be interpolated for each fragment.\n\
		out vec2 uv_out;\n\
		// Values that stay constant for the whole mesh.\n\
		uniform mat4 MVP;\n\
		void main(){ \n\
			// Output position of the vertex, in clip space : MVP * position\n\
			gl_Position =  MVP * vec4(vertexPosition,1);\n\
			// The color will be interpolated to produce the color of each fragment\n\
			uv_out = uv;\n\
		}\n";
        /*
         * Fragment shader code
         */
        std::string FragmentShaderCode = "\
		#version 330 core\n\
		in vec2 uv_out; \n\
		uniform sampler2D tex;\n\
		void main() {\n\
			gl_FragColor = texture(tex, uv_out);\n\
		}\n";

        char const * VertexSourcePointer = VertexShaderCode.c_str();    //convert vertex shader code to char pointer
        glShaderSource(VertexShaderID, 1, &VertexSourcePointer , NULL); //set vertex shader source
        glCompileShader(VertexShaderID);    //compile vertex shader

        char const * FragmentSourcePointer = FragmentShaderCode.c_str();    //convert fragment shader code to char pointer
        glShaderSource(FragmentShaderID, 1, &FragmentSourcePointer , NULL); //set fragment shader source
        glCompileShader(FragmentShaderID);  //compile fragment shader
        programID = glCreateProgram();  //create shader program
        glAttachShader(programID, VertexShaderID);  //attach vertex shader to shader program
        glAttachShader(programID, FragmentShaderID);    //attach fragment shader to shader program
        glLinkProgram(programID);   //link shader program
        glDetachShader(programID, VertexShaderID);  //detach vertex shader from shader program
        glDetachShader(programID, FragmentShaderID);    //detach fragment shader from shader program
        glDeleteShader(VertexShaderID); //delete vertex shader
        glDeleteShader(FragmentShaderID);   //delete fragment shader

        glGenTextures(1, &textureObj);  //create texture object
        glBindTexture(GL_TEXTURE_2D, textureObj);   //bind texture object
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_BGRA, GL_UNSIGNED_BYTE, data);    //set texture data
        glGenerateMipmap(GL_TEXTURE_2D);    //generate mipmaps
        glBindTexture(GL_TEXTURE_2D, 0);    //unbind texture object
    }
    /*
     * Draw mesh
     */
    void draw(glm::mat4 mvp){

        glActiveTexture(GL_TEXTURE0);   //activate texture unit
        glEnable(GL_TEXTURE_2D);        //enable 2D texturing
        glBindTexture(GL_TEXTURE_2D, textureObj);

        glEnable(GL_BLEND); //enable blending
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);  //set blending function
        
        GLuint MatrixID = glGetUniformLocation(programID, "MVP");   //get uniform location

        glUseProgram(programID);    //use shader program
        glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &mvp[0][0]);  //set uniform value

        glBindVertexArray(meshVAO); //bind VAO

        glDrawElements(GL_TRIANGLES, faces.size() * 3, GL_UNSIGNED_INT,(void*) 0); // draw mesh
        glBindVertexArray(0);   //unbind VAO
        glUseProgram(0);    //unbind shader program
        glBindTexture(GL_TEXTURE_2D, 0);    //unbind texture object
    }
};

/*
 * Main function 
 */
int main( int argc, char* argv[]) { 

    // Initialise GLFW
    if( !glfwInit() )
    {
        fprintf( stderr, "Failed to initialize GLFW\n" );
        getchar();
        return -1;
    }

    glfwWindowHint(GLFW_SAMPLES, 4);

    // Open a window and create its OpenGL context
    float screenW = 1400;
    float screenH = 900;
    window = glfwCreateWindow( screenW, screenH, "Wood House", NULL, NULL);
    if( window == NULL ){
        fprintf( stderr, "Failed to open GLFW window. If you have an Intel GPU, they are not 3.3 compatible. Try the 2.1 version of the tutorials.\n" );
        getchar();
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    // Initialize GLEW
    glewExperimental = true;    
    if (glewInit() != GLEW_OK) {    
        fprintf(stderr, "Failed to initialize GLEW\n");
        getchar();
        glfwTerminate();
        return -1;
    }

    glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);    // Ensure we can capture the escape key being pressed below

    std::vector<TexturedMesh> meshes;   //vector of meshes
    meshes.push_back(TexturedMesh("Walls.ply", "walls.bmp"));   //add meshes to vector
    meshes.push_back(TexturedMesh("WoodObjects.ply", "woodobjects.bmp"));   
    meshes.push_back(TexturedMesh("Table.ply", "table.bmp"));   
    meshes.push_back(TexturedMesh("WindowBG.ply", "windowbg.bmp"));
    meshes.push_back(TexturedMesh("Patio.ply", "patio.bmp"));
    meshes.push_back(TexturedMesh("Floor.ply", "floor.bmp"));
    meshes.push_back(TexturedMesh("Bottles.ply", "bottles.bmp"));

    meshes.push_back(TexturedMesh("DoorBG.ply", "doorbg.bmp"));
    meshes.push_back(TexturedMesh("MetalObjects.ply", "metalobjects.bmp"));
    meshes.push_back(TexturedMesh("Curtains.ply", "curtains.bmp"));

    glEnable(GL_DEPTH_TEST);    //enable depth testing
    glDepthFunc(GL_LESS);   //set depth function

    glClearColor(0.2f, 0.2f, 0.3f, 0.0f);   //set clear colour

    glm::mat4 MVP;  //model view projection matrix
    glm::mat4 Projection = glm::perspective(glm::radians(45.0f), screenW/screenH, 0.001f, 1000.0f); //perspective projection matrix

    do{ 
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); //clear colour and depth buffers

        glMatrixMode(GL_PROJECTION);
        glPushMatrix();

        glLoadMatrixf(glm::value_ptr(Projection));  //set projection matrix

        glMatrixMode( GL_MODELVIEW );   //set model view matrix
        glPushMatrix(); 
        glm::vec3 eye = {5.0f, 2.0f, 5.0f}; //camera position
        glm::vec3 up = {0.0f, 1.0f, 0.0f};  //camera up vector
        glm::vec3 center = {0.0f, 0.0f, 0.0f};  //camera look at point

        glm::mat4 V = glm::lookAt(eye, center, up); //view matrix

        glm::mat4 M = glm::mat4(1.0f);  //model matrix

        cameraControls(V);         //call camera controls function

        glm::mat4 MV = V * M;   //model view matrix
        glLoadMatrixf(glm::value_ptr(V));   //set model view matrix

        MVP = Projection * V * M;

        for (int i = 0; i < meshes.size(); i++){      //draw meshes
            meshes[i].draw(MVP);
        }
        glfwSwapBuffers(window);    //swap buffers
        glfwPollEvents();
    } 
    while( glfwGetKey(window, GLFW_KEY_ESCAPE ) != GLFW_PRESS &&    // Check if the ESC key was pressed or the window was closed   
           glfwWindowShouldClose(window) == 0 );
    glfwTerminate();
    return 0;
}
