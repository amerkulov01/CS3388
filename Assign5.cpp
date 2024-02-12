#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <functional>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "TriTable.hpp"

#define FRONT_TOP_LEFT     128
#define FRONT_TOP_RIGHT     64
#define BACK_TOP_RIGHT      32
#define BACK_TOP_LEFT       16
#define FRONT_BOTTOM_LEFT    8
#define FRONT_BOTTOM_RIGHT   4
#define BACK_BOTTOM_RIGHT    2
#define BACK_BOTTOM_LEFT     1

float f(float x, float y, float z){

    return x * x - y * y - z * z - z;

}

GLFWwindow* window;

void cameraControlsGlobe(GLFWwindow* window, glm::mat4& V, float& r, float& theta, float& phi) {
    static const float move_speed = 0.05f;  // units per second
    static const float rotate_speed = 0.5f; // degrees per second

    glm::vec3 camera_position(r * sin(glm::radians(theta)) * cos(glm::radians(phi)),    // x
                              r * sin(glm::radians(phi)),                            // y
                              r * cos(glm::radians(theta)) * cos(glm::radians(phi)));   // z
    glm::vec3 camera_direction = glm::normalize(-camera_position);
    glm::vec3 camera_up(0.0f, 1.0f, 0.0f);

    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {    // zoom in
        r = std::max(r - move_speed, 0.1f);
    }
    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {  // zoom out
        r += move_speed;
    }
    if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) {  // rotate left
        theta += rotate_speed;
    }
    if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) { // rotate right
        theta -= rotate_speed;
    }
    static double last_xpos = -1, last_ypos = -1;   // last cursor position
    double xpos, ypos;
    glfwGetCursorPos(window, &xpos, &ypos);         // current cursor position
    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) { // rotate camera
        if (last_xpos >= 0 && last_ypos >= 0) { // if cursor was moved
            theta -= rotate_speed * float(xpos - last_xpos);    // rotate around y-axis
            phi += rotate_speed * float(ypos - last_ypos);    // rotate around x-axis
            phi = glm::clamp(phi, -89.0f, 89.0f);          // clamp phi to [-89, 89]
        }
    }
    last_xpos = xpos;   // update last cursor position
    last_ypos = ypos;   // update last cursor position

    camera_position.x = r * sin(glm::radians(theta)) * cos(glm::radians(phi));
    camera_position.y = r * sin(glm::radians(phi));
    camera_position.z = r * cos(glm::radians(theta)) * cos(glm::radians(phi));
    camera_direction = glm::normalize(-camera_position);    // update camera direction
    V = glm::lookAt(camera_position, camera_position + camera_direction, camera_up);    // update view matrix
}
const char *textured_vertex_shader = R"glsl(
#version 330 core
layout(location = 0) in vec3 vertexPosition;
layout(location = 1) in vec3 vertexNormal;
out vec3 normal;
out vec3 eye_direction;
out vec3 light_direction;
uniform mat4 MVP;
uniform mat4 V;
uniform mat4 M;
uniform vec3 lightDir;
void main() {
    gl_Position =  MVP * vec4(vertexPosition,1);
    normal = mat3(V) * vertexNormal;
    eye_direction = vec3(5, 5, 5) - (V * vec4(-vertexPosition, 1)).xyz;
    light_direction = (mat3(V) * lightDir) + eye_direction;
}
)glsl";

const char *textured_fragment_shader = R"glsl(
#version 330 core
in vec3 normal;
in vec3 light_direction;
in vec3 eye_direction;
out vec4 color;
uniform vec4 modelColor;
void main() {
    vec4 diffuseColor = modelColor;
    vec4 ambient = vec4(0.2, 0.2, 0.2, 1) * diffuseColor; // Reduced ambient intensity
    vec4 specular = vec4(1, 1, 1, 1); // Reduced specular intensity
    float alpha = 64;
    vec3 n = normalize(normal);
    vec3 l = normalize(light_direction);
    vec3 e = normalize(eye_direction);
    vec3 r = reflect(-l, n);
    float cosTheta = clamp(dot(n, l), 0, 1);
    float cosAlpha = clamp(dot(e, r), 0, 1);
    color = ambient + (modelColor * cosTheta * 0.8) + (specular * pow(cosAlpha, alpha)); // Reduced diffuse intensity
}
)glsl";

class Cube {
    float min;
    float max;
    glm::vec3 xcol = glm::vec3(1.0f, 0.0f, 0.0f);
    glm::vec3 ycol = glm::vec3(0.0f, 1.0f, 0.0f);
    glm::vec3 zcol = glm::vec3(0.0f, 0.0f, 1.0f);

public:
    Cube(float min, float max) : min(min+0.1), max(max+0.1) {}
    void draw() {
        float axisExtension = 0.5f; // length of the axis extension
        float arrowSize = 0.25f;    // size of the arrow

        glMatrixMode(GL_MODELVIEW); // set the modelview matrix
        glPushMatrix();

        glLineWidth(5.0f);
        glBegin(GL_LINES);
        // draw the axis
        glColor4f(1.0, 0.0f, 0.0f, 1.0f);
        glVertex3f(min, min, min);
        glVertex3f(max + axisExtension, min, min);

        glColor4f(0.0, 1.0f, 0.0f, 1.0f);
        glVertex3f(min, min, min);
        glVertex3f(min, max + axisExtension, min);

        glColor4f(0.0, 0.0f, 1.0f, 1.0f);
        glVertex3f(min, min, min);
        glVertex3f(min, min, max + axisExtension);

        glColor4f(0.5f, 0.5f, 0.5f, 1.0f);
        glColor4f(0.5f, 0.5f, 0.5f, 1.0f);
        glVertex3f(min, max, min);
        glVertex3f(max, max, min);

        glVertex3f(max, min, min);
        glVertex3f(max, max, min);

        glVertex3f(max, min, min);
        glVertex3f(max, min, max);

        glVertex3f(max, min, max);
        glVertex3f(max, max, max);

        glVertex3f(max, max, min);
        glVertex3f(max, max, max);

        glVertex3f(min, max, min);
        glVertex3f(min, max, max);

        glVertex3f(min, max, max);
        glVertex3f(min, min, max);

        glVertex3f(min, min, max);
        glVertex3f(max, min, max);

        glVertex3f(min, max, max);
        glVertex3f(max, max, max);

        glEnd();
        glBegin(GL_TRIANGLES);  // draw the arrows

        glColor4f(1.0f, 0.0f, 0.0f, 1.0f);
        glVertex3f(max + axisExtension, min, min);
        glVertex3f(max + axisExtension - arrowSize, min + arrowSize/2, min);
        glVertex3f(max + axisExtension - arrowSize, min - arrowSize/2, min);

        glColor4f(0.0f, 1.0f, 0.0f, 1.0f);
        glVertex3f(min, max + axisExtension, min);
        glVertex3f(min + arrowSize/2, max + axisExtension - arrowSize, min);
        glVertex3f(min - arrowSize/2, max + axisExtension - arrowSize, min);

        glColor4f(0.0f, 0.0f, 1.0f, 1.0f);
        glVertex3f(min, min, max + axisExtension);
        glVertex3f(min, min + arrowSize/2, max + axisExtension - arrowSize);
        glVertex3f(min, min - arrowSize/2, max + axisExtension - arrowSize);

        glEnd();

        glPopMatrix();
    }
};

class MarchingCubes {   // does the marching cube mesh generation
    std::function<float(float, float, float)> generationFunction;
    float isoValue = 0;     // the iso value for the mesh
    float min_coord = 0;    // the minimum coordinate of the mesh
    float max_coord = 1;    // the maximum coordinate of the mesh
    float stepSize = 0.1;   // the step size for the marching cube algorithm
    float current_iteration = 0;    // the current iteration of the marching cube algorithm
    std::vector<float> vertices;    // the vertices of the mesh

    void add_triangles(int* verts, float x, float y, float z) { // add the triangles to the vertices
        for (int i = 0; verts[i] >= 0; i += 3) {    // for each triangle
            for (int j = 0; j < 3; ++j) {   // for each vertex
                vertices.emplace_back(x + stepSize * vertTable[verts[i + j]][0]);   // add the vertex
                vertices.emplace_back(y + stepSize * vertTable[verts[i + j]][1]);
                vertices.emplace_back(z + stepSize * vertTable[verts[i + j]][2]);
            }
        }
    }
    void generate_iterative_new() { // generate the mesh
        float ftl, ftr, fbr, fbl, btl, btr, bbr, bbl;
        int which = 0;
        int *verts;
        for (float a = min_coord; a < max_coord; a += stepSize) {   // for each square
            for (float b = min_coord; b < max_coord; b += stepSize) {
                bbl = generationFunction(a, b, current_iteration);  // get the values of the corners
                btl = generationFunction(a, b + stepSize, current_iteration);
                btr = generationFunction(a + stepSize, b + stepSize, current_iteration);
                bbr = generationFunction(a + stepSize, b, current_iteration);
                fbl = generationFunction(a, b, current_iteration + stepSize);
                ftl = generationFunction(a, b + stepSize, current_iteration + stepSize);
                ftr = generationFunction(a + stepSize, b + stepSize, current_iteration + stepSize);
                fbr = generationFunction(a + stepSize, b, current_iteration + stepSize);
                which = 0;
                if (ftl < isoValue) {   // get the corners that are below the iso value
                    which |= FRONT_TOP_LEFT;
                }
                if (ftr < isoValue) {
                    which |= FRONT_TOP_RIGHT;
                }
                if (btr < isoValue) {
                    which |= BACK_TOP_RIGHT;
                }
                if (btl < isoValue) {
                    which |= BACK_TOP_LEFT;
                }
                if (fbl < isoValue) {
                    which |= FRONT_BOTTOM_LEFT;
                }
                if (fbr < isoValue) {
                    which |= FRONT_BOTTOM_RIGHT;
                }
                if (bbr < isoValue) {
                    which |= BACK_BOTTOM_RIGHT;
                }
                if (bbl < isoValue) {
                    which |= BACK_BOTTOM_LEFT;
                }
                verts = marching_cubes_lut[which];
                add_triangles(verts, a, b, current_iteration);
            }
        }
        current_iteration += stepSize;
        if (current_iteration > max_coord){
            finished = true;
        }
    }

std::vector<float> compute_normals(std::vector<float> vertices){    // compute the normals of the mesh
    std::vector<float> normals; // the normals of the mesh
    glm::vec3 p1, p2, p3, p12, p13, n;
    glm::vec3 cross;
    for(int i = 0;i < vertices.size();i += 9){  // for each triangle
        p1 = {vertices[i], vertices[i+1], vertices[i+2]};
        p2 = {vertices[i+3], vertices[i+4], vertices[i+5]};
        p3 = {vertices[i+6], vertices[i+7], vertices[i+8]};

        float p12x = p2.x - p1.x;   // compute the cross product of the two edges of the triangle
        float p12y = p2.y - p1.y;
        float p12z = p2.z - p1.z;
        float p13x = p3.x - p1.x;
        float p13y = p3.y - p1.y;
        float p13z = p3.z - p1.z;

        float crossX = (p12y*p13z)-(p12z*p13y);   // compute the cross product
        float crossY = (p12z*p13x)-(p12x*p13z);
        float crossZ = (p12x*p13y)-(p12y*p13x);

        cross = glm::vec3(crossX, crossY, crossZ);      // normalize the cross product
        n = glm::normalize(cross);

        for(int j = 0;j < 3;j++){   // add the normal to the normal vector
            normals.emplace_back(n.z);
            normals.emplace_back(n.y);
            normals.emplace_back(n.x);
        }
    }
    return normals;
}

public:
bool finished = false;  // if the mesh generation is finished

MarchingCubes(std::function<float(float, float, float)> f, float isoval, float min, float max, float step)
: generationFunction(f), isoValue(isoval), min_coord(min), max_coord(max), stepSize(step), current_iteration(min) {}
void generate() {
    generate_iterative_new();
}
std::vector<float> get_vertices() {
    return vertices;
}
};

void writePLY(std::string outFile, std::vector<float> vertex_data, std::vector<float> normal_data){  // write the mesh to a PLY file
    std::ofstream plyFile(outFile);
    if (plyFile.fail()){    // if the file could not be opened
        return;
    }

    int vertex_count = vertex_data.size() / 3;
    int face_count = vertex_count / 3;
    plyFile << "ply" << std::endl;  // write the header
    plyFile << "format ascii 1.0" << std::endl;
    plyFile << "element vertex " << vertex_count << std::endl;
    plyFile << "property float x" << std::endl;
    plyFile << "property float y" << std::endl;
    plyFile << "property float z" << std::endl;
    plyFile << "property float nx" << std::endl;
    plyFile << "property float ny" << std::endl;
    plyFile << "property float nz" << std::endl;
    plyFile << "element face " << face_count << std::endl;
    plyFile << "property list uchar uint vertex_indices" << std::endl;
    plyFile << "end_header" << std::endl;

    for (size_t idx = 2; idx < vertex_data.size(); idx += 3){   // write vertex data
        plyFile << vertex_data[idx - 2] << " " << vertex_data[idx - 1] << " " << vertex_data[idx] << " " << normal_data[idx - 2] << " " << normal_data[idx - 1] << " " << normal_data[idx] << std::endl;
    }
    for (size_t idx = 2; idx < vertex_count; idx += 3){   // write face data
        plyFile << "3 " << idx - 2 << " " << idx - 1 << " " << idx << std::endl;
    }
    plyFile.close();
}

int main(int argc, char* argv[]) {  // main function
    std::vector<float> normals;
    float step = 0.05;
    float min = -5.0f;
    float max = 5.0f;
    float isoval = -1.5;
    std::string filename = "rendering.ply";
    bool generateFile = true;

    // Initialize window
    if (!glfwInit()) {
        printf("Failed to initialize GLFW\n");
        return -1;
    }
    glfwWindowHint(GLFW_SAMPLES, 4);
    window = glfwCreateWindow(1000, 1000, "Assignment 5", NULL, NULL);
    if (window == NULL) {
        printf("Failed to open window\n");
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    // Initialize GLEW
    glewExperimental = true;
    if (glewInit() != GLEW_OK) {
        printf("Failed to initialize GLEW\n");
        glfwTerminate();
        return -1;
    }

    glClearColor(0.2, 0.2, 0.3, 0);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    glm::mat4 mvp;  // model view projection matrix
    glm::vec3 eyePos(5, 5, 5);
    glm::vec3 zero(0, 0, 0);
    glm::vec3 up(0, 1, 0);
    glm::mat4 view = glm::lookAt(eyePos, zero, up);
    glm::mat4 projection = glm::perspective(glm::radians(45.0f), 1.0f, 0.001f, 1000.0f);
    glm::mat4 model = glm::mat4(1.0f);
    mvp = projection * view * model;

    MarchingCubes cubes(f, isoval, min, max, step);
    Cube drawCube(min,max);

    GLuint vao, vertex_VBO, normal_VBO, program_ID; // vertex array object, vertex buffer object, normal buffer object, shader program ID
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);
    glGenBuffers(1, &vertex_VBO);
    glBindBuffer(GL_ARRAY_BUFFER, vertex_VBO);
    glBufferData(GL_ARRAY_BUFFER, 0, &cubes.get_vertices()[0], GL_DYNAMIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(
            0,
            3,
            GL_FLOAT,
            GL_FALSE,
            0,
            (void *) 0
    );
    glGenBuffers(1, &normal_VBO);
    glBindBuffer(GL_ARRAY_BUFFER, normal_VBO);
    glBufferData(GL_ARRAY_BUFFER, 0, &normals, GL_DYNAMIC_DRAW);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(
            1,
            3,
            GL_FLOAT,
            GL_FALSE,
            0,
            (void *) 0
    );
    glBindVertexArray(0);

    GLuint vertex_shader_ID = glCreateShader(GL_VERTEX_SHADER); // create and compile shaders
    GLuint fragment_shader_ID = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(vertex_shader_ID, 1, &textured_vertex_shader, NULL);
    glCompileShader(vertex_shader_ID);
    glShaderSource(fragment_shader_ID, 1, &textured_fragment_shader, NULL);
    glCompileShader(fragment_shader_ID);
    program_ID = glCreateProgram();
    glAttachShader(program_ID, vertex_shader_ID);
    glAttachShader(program_ID, fragment_shader_ID);
    glLinkProgram(program_ID);
    glDetachShader(program_ID, vertex_shader_ID);
    glDetachShader(program_ID, fragment_shader_ID);
    glDeleteShader(vertex_shader_ID);
    glDeleteShader(fragment_shader_ID);
    glClear(GL_COLOR_BUFFER_BIT);
    glfwSwapBuffers(window);

    float r = 30.0f;    // camera controls
    float theta = 45.0f;
    float phi = 45.0f;
    GLfloat MODEL_COLOR[4] = {0.0f, 1.0f, 1.0f, 1.0f};  // model color
    GLfloat LIGHT_DIRECTION[3] = {5.0f, 5.0f, 5.0f};    // light direction
    double prevTime = glfwGetTime();
    bool wroteFile = false;

    while (!glfwWindowShouldClose(window)) {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        double currentTime = glfwGetTime(); // get time
        float deltaTime = currentTime - prevTime;
        prevTime = currentTime;

        cameraControlsGlobe(window, view, r, theta, phi);   // camera controls
        mvp = projection * view * model;

        if (!cubes.finished) {  // if not finished, generate mesh
            cubes.generate();
            std::vector<float> vertices = cubes.get_vertices();
            normals = compute_normals(vertices);

            glBindVertexArray(vao); // update buffers
            glBindBuffer(GL_ARRAY_BUFFER, normal_VBO);
            glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(GL_FLOAT), &normals[0], GL_DYNAMIC_DRAW);
            glBindBuffer(GL_ARRAY_BUFFER, vertex_VBO);
            glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(GL_FLOAT), &vertices[0], GL_DYNAMIC_DRAW);
            glBindVertexArray(0);

        } else if (!wroteFile && generateFile) { // if finished, write file
            std::vector<float> vertices = cubes.get_vertices();
            normals = compute_normals(vertices);
            writePLY("rendering.ply", vertices, normals);
            wroteFile = true;
        }

        glMatrixMode(GL_PROJECTION);    // draw cube
        glPushMatrix();
        glLoadMatrixf(glm::value_ptr(projection));
        glMatrixMode(GL_MODELVIEW);
        glPushMatrix();
        glLoadMatrixf(glm::value_ptr(view));
        drawCube.draw();

        glUseProgram(program_ID);   // draw mesh

        GLuint matrixID = glGetUniformLocation(program_ID, "MVP");  // get uniform locations
        glUniformMatrix4fv(matrixID, 1, GL_FALSE, &mvp[0][0]);

        GLuint viewID = glGetUniformLocation(program_ID, "V");
        glUniformMatrix4fv(viewID, 1, GL_FALSE, &view[0][0]);

        GLuint colorID = glGetUniformLocation(program_ID, "modelColor");    // set uniform values
        glUniform4fv(colorID, 1, MODEL_COLOR);

        GLuint lightDirID = glGetUniformLocation(program_ID, "lightDir");
        glUniform3fv(lightDirID, 1, LIGHT_DIRECTION);

        glBindVertexArray(vao);
        glDrawArrays(GL_TRIANGLES, 0, normals.size());
        glBindVertexArray(0);
        glUseProgram(0);


        glfwPollEvents();
        glfwSwapBuffers(window);
    }

    return 0;
}