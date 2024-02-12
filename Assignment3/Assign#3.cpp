/*
 * Andrei Merkulov, 251145994
 * CS 3388 Assignment 3
 * 2023 - 02 - 17
 */

#include <GLFW/glfw3.h>
#include <stdio.h>
#include "vector"
#include <math.h>

int screenWidth = 500;
int screenHeight = 500;
int nClicks = 0;

struct Point {  // Point struct
    float x;
    float y;
};

struct Node : Point {   // Node struct
    bool hasContPoint1, hasContPoint2 ;
    bool actuallyContPoint2;
    Point contPoint1 ;
    Point contPoint2 ;  // Control points
};

float distance(Node A, Node B) {    // Distance between two points
    float dx = B.x - A.x;
    float dy = B.y - A.y;
    return sqrt(dx * dx + dy * dy);
}

float distancePoint(float x1, float y1, float x2, float y2) {   //same function but for points
    float dx = x2 - x1;
    float dy = y2 - y1;
    return sqrt(dx * dx + dy * dy);
}

std :: vector <Node> nodes ;        // Vector of nodes
Node *selectedNode = NULL;      // Selected node
Node *selectedContPointA = NULL;
Node *selectedContPointB = NULL;

void mouse_callback(GLFWwindow* window, int button, int action, int mods) {  // Mouse callback
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {     // Left click
        double x, y;
        glfwGetCursorPos(window, &x, &y);                    // Get cursor position

        for (int i = 0; i < nodes.size(); i++) {                        // Check if we clicked on a node
            if (distancePoint(nodes.at(i).x, nodes.at(i).y, x, screenHeight - y) <= 5) {    // If we did
                selectedNode = &nodes[i];                            // Select it
            }
            if (distancePoint(nodes.at(i).contPoint1.x, nodes.at(i).contPoint1.y, x, screenHeight - y) <= 5) {
                selectedContPointA = &nodes[i];
            }
            if (nodes.at(i).hasContPoint2) {    // If the node has a second control point
                if (distancePoint(nodes.at(i).contPoint2.x, nodes.at(i).contPoint2.y, x, screenHeight - y) <= 5) {
                    selectedContPointB = &nodes[i];
                }
            }
        }
        if (selectedNode == NULL && selectedContPointA == NULL && selectedContPointB == NULL) { // If we didn't click on a node
            Node tempNode;  // Create a new node
            tempNode.x = x;
            tempNode.y = screenHeight - y;
            tempNode.hasContPoint1 = true;
            tempNode.contPoint1.x = x;
            tempNode.contPoint1.y = screenHeight - y + 50;
            tempNode.hasContPoint2 = false;
            tempNode.contPoint2.x = x;
            tempNode.contPoint2.y = screenHeight - y;

            if (nodes.size() > 1) {                                       // If there are more than 1 nodes
                float distance1 = distance(nodes[0], tempNode);      // Distance between the first node and the new node
                float distance2 = distance(nodes[nodes.size() - 1], tempNode);
                Point swapHandle;
                if ((distance1 - distance2)<0) {                          // If the distance between the first node and the new node is smaller than the distance between the last node and the new node
                    nodes.insert(nodes.begin(), tempNode);          // Insert the new node at the beginning of the vector
                    nodes[1].hasContPoint2 = true;
                    nodes[1].actuallyContPoint2 = true;
                    nodes[1].contPoint2.x = nodes[1].x + (nodes[1].x - nodes[1].contPoint1.x);
                    nodes[1].contPoint2.y = nodes[1].y + (nodes[1].y - nodes[1].contPoint1.y);
                    swapHandle = nodes[1].contPoint2;
                    nodes[1].contPoint2 = nodes[1].contPoint1;
                    nodes[1].contPoint1 = swapHandle;
                } else {
                    nodes.push_back(tempNode);                          // Insert the new node at the end of the vector
                    nodes[nodes.size() - 2].hasContPoint2 = true;
                    nodes[nodes.size() - 2].actuallyContPoint2 = true;
                    nodes[nodes.size() - 2].contPoint2.y = nodes[nodes.size() - 2].y - 50;
                    nodes[nodes.size() - 2].contPoint2.x = nodes[nodes.size() - 2].x + (nodes[nodes.size() - 2].x - nodes[nodes.size() - 2].contPoint1.x);
                    nodes[nodes.size() - 2].contPoint2.y = nodes[nodes.size() - 2].y + (nodes[nodes.size() - 2].y - nodes[nodes.size() - 2].contPoint1.y);
                    swapHandle = nodes[nodes.size() - 2].contPoint2;
                    nodes[nodes.size() - 2].contPoint2 = nodes[nodes.size() - 2].contPoint1;
                    nodes[nodes.size() - 2].contPoint1 = swapHandle;
                }
            } else {
                nodes.push_back(tempNode);                              //if it's our first or second node, pushback.
            }
            nClicks++;                                                // Increment the number of clicks
        }
    }
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE) {    // Left click release
        selectedNode = NULL;
        selectedContPointA = NULL;
        selectedContPointB = NULL;
    }
}
void dragNode(GLFWwindow* window){  // Drag node
    double x, y;
    glfwGetCursorPos(window, &x, &y);

    float deltaX = x - selectedNode->x; // Delta x
    float deltaY = (screenHeight - y) - selectedNode->y;

    selectedNode->x = x;                // Set the node's position to the cursor's position
    selectedNode->y = screenHeight - y;

    selectedNode->contPoint1.x = selectedNode->contPoint1.x + deltaX;
    selectedNode->contPoint1.y = selectedNode->contPoint1.y + deltaY;

    if (selectedNode->actuallyContPoint2) {   // If the node has a second control point
        selectedNode->contPoint2.x = selectedNode->x + (selectedNode->x - selectedNode->contPoint1.x);
        selectedNode->contPoint2.y = selectedNode->y + (selectedNode->y - selectedNode->contPoint1.y);
    }
}

void dragHandleA(GLFWwindow* window){
    double x, y;
    glfwGetCursorPos(window, &x, &y);
    selectedContPointA->contPoint1.x = x;             // Set the node's position to the cursor's position
    selectedContPointA->contPoint1.y = screenHeight - y;    // Set the node's position to the cursor's position
    selectedContPointA->contPoint2.x = selectedContPointA->x + (selectedContPointA->x - selectedContPointA->contPoint1.x);
    selectedContPointA->contPoint2.y = selectedContPointA->y + (selectedContPointA->y - selectedContPointA->contPoint1.y);
}

void dragHandleB(GLFWwindow* window){
    double x, y;
    glfwGetCursorPos(window, &x, &y);
    selectedContPointB->contPoint2.x = x;
    selectedContPointB->contPoint2.y = screenHeight - y;    // Set the node's position to the cursor's position
    selectedContPointB->contPoint1.x = selectedContPointB->x + (selectedContPointB->x - selectedContPointB->contPoint2.x);  // Set the node's position to the cursor's position
    selectedContPointB->contPoint1.y = selectedContPointB->y + (selectedContPointB->y - selectedContPointB->contPoint2.y);  // Set the node's position to the cursor's position
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {    // Key callback
    if (key == GLFW_KEY_E && action == GLFW_PRESS){
        nodes.clear();
    }
}

void drawPoint(Point drawPoint){
    glVertex2f(drawPoint.x,drawPoint.y);
}

float bezier4(float p0, float p1, float p2, float p3, float t) { // 4 point bezier
    float u = 1 - t;
    float tt = t * t;
    float uu = u * u;
    float uuu = uu * u;
    float ttt = tt * t;
    float p = uuu * p0;
    p += 3 * uu * t * p1;
    p += 3 * u * tt * p2;
    p += ttt * p3;
    return p;
}

void renderCurves(){                            // Render the curves
    for (int j = 0; j < nodes.size() - 1; j++) {    // For each node
        for (int i = 0; i < 200; ++i) {            // For each point on the curve
            float t = (float)i;
            t /= 200;
            if (i > 1) {                        // If it's not the first point
                glVertex2f(bezier4(nodes[j].x, nodes[j].contPoint1.x, nodes[j + 1].contPoint2.x, nodes[j + 1].x, t - 1.0f / 200),
                           bezier4(nodes[j].y, nodes[j].contPoint1.y, nodes[j + 1].contPoint2.y, nodes[j + 1].y, t - 1.0f / 200));
            }
            glVertex2f(bezier4(nodes[j].x, nodes[j].contPoint1.x, nodes[j + 1].contPoint2.x, nodes[j + 1].x, t),
                       bezier4(nodes[j].y, nodes[j].contPoint1.y, nodes[j + 1].contPoint2.y, nodes[j + 1].y, t));
        }
    }
}

void renderStipple(){                // Render the stipple
    for (int i = 0; i <nodes.size(); i++) {   // For each node
        glVertex2f(nodes.at(i).contPoint1.x,nodes.at(i).contPoint1.y);  // Draw the control point
        glVertex2f(nodes.at(i).x, nodes.at(i).y);                    // Draw the node

        if (nodes.at(i).hasContPoint2){
            glVertex2f(nodes.at(i).contPoint2.x,nodes.at(i).contPoint2.y);
            glVertex2f(nodes.at(i).x, nodes.at(i).y);
        }
    }
}

void renderHandles(){        // Render the handles
    for (int i = 0; i <nodes.size(); i++) {
        drawPoint(nodes.at(i).contPoint1);

        if (nodes.at(i).hasContPoint2){
            drawPoint(nodes.at(i).contPoint2);
        }
    }
}

void renderPoints(){     // Render the points
    for (int i = 0; i <nodes.size(); i++) {
        drawPoint(nodes.at(i));
    }
}

int main(int argc, char** argv) {

    if (!glfwInit()) {
        fprintf(stderr, "Failed to initialize GLFW\n"); // Initialize GLFW
        return -1;
    }

    glfwWindowHint(GLFW_SAMPLES, 4);  //Create a window with 4 times multisampling enabled

    if (argc >= 2){ // If there is a command line argument
        screenWidth = atoi(argv[1]);
        screenHeight = atoi(argv[2]);
    }

    GLFWwindow *window = glfwCreateWindow(screenWidth, screenHeight, "Spline Tool", NULL, NULL);

    if (!window) {
        fprintf(stderr, "Failed to create GLFW window\n");
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glEnable(GL_MULTISAMPLE);   //enable multisampling

    glOrtho(0, screenWidth, 0, screenHeight, -1, 1); //setting viewport
    glViewport(0, 0, screenWidth, screenHeight);

    //Main loop
    while (!glfwWindowShouldClose(window)) {

        if (selectedContPointA != NULL) {       // If a control point is selected
            dragHandleA(window);
        }

        if (selectedContPointB != NULL) {       // If a control point is selected
            dragHandleB(window);
        }

        if (selectedNode != NULL) {         // If a node is selected
            dragNode(window);
        }
        glClearColor(1.0, 0.0, 0.0, 1.0);
        glClear(GL_COLOR_BUFFER_BIT);
        glColor3f(1,1,0);
        glfwSetMouseButtonCallback(window, mouse_callback);
        glfwSetKeyCallback(window, key_callback);

        //render curves
        glEnable (GL_LINE_SMOOTH );
        glEnable (GL_BLEND );
        glBlendFunc (GL_SRC_ALPHA , GL_ONE_MINUS_SRC_ALPHA );
        if (nodes.size() > 1) {
            glLineWidth(1.5f);
            glBegin(GL_LINES);
            renderCurves();
            glEnd();
        }
        glDisable(GL_LINE_SMOOTH );
        glDisable(GL_BLEND );

        glEnable ( GL_LINE_STIPPLE );   // Enable stipple
        glLineStipple (1 , 0x00FF );
        glBegin ( GL_LINES );
        glColor3f(1,1,0);
        renderStipple();
        glEnd ();
        glDisable( GL_LINE_STIPPLE );
        glColor3f(1,1,0);

        //Render points we click
        glPointSize (15);
        glDisable(GL_MULTISAMPLE);
        glBegin( GL_POINTS );
        renderPoints();
        glEnd ();
        glEnable(GL_MULTISAMPLE);

        //render handles
        glEnable(GL_POINT_SMOOTH );
        glEnable(GL_BLEND );
        glBlendFunc(GL_SRC_ALPHA , GL_ONE_MINUS_SRC_ALPHA );
        glBegin(GL_POINTS);
        renderHandles();
        glEnd();
        glDisable(GL_POINT_SMOOTH);
        glDisable(GL_BLEND);

        // Swap buffers and poll events
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    //Terminate GLFW
    glfwTerminate();
    return 0;
}
