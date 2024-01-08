
#include <iostream>
#include <fstream>
#include <sstream>

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "shader.hpp"
#include "camera.hpp"
#include "model.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>


unsigned int compileShader(GLenum type, const char* source);
unsigned int createShader(const char* vsSource, const char* fsSource);
static unsigned loadImageToTexture(const char* filePath);

void rotateCamera(GLFWwindow* window, double xposIn, double yposIn);
void zoomCamera(GLFWwindow* window, double xoffset, double yoffset);
bool checkCameraPosition();

const int NUMBER_OF_BUFFERS = 5;
const int CIRCLE_POINTS = 30;

const int wallTextureIndex = 0;
const int frameShapeIndex = 1;
const int frameImageTextureIndex = 2;
const int personalInfoTextureIndex = 3;
const int circleIndex = 4;

unsigned int wWidth = 1600;
unsigned int wHeight = 800;


glm::vec3 cameraPos = glm::vec3(-1.0f, -0.2f, 0.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, 0.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
glm::vec3 cameraRight = glm::normalize(glm::cross(cameraFront, cameraUp));
//za kretanje kamere
float deltaTime = 0.0f;
float lastFrame = 0.0f;

//za rotaciju kamere
float lastX = wWidth / 2.0f;
float lastY = wHeight / 2.0f;

bool firstMouse = true;
Camera camera(cameraPos, cameraUp, cameraFront, 0.0);
float boxParameter = 0.5f;
float translationX = 1.0f;
float translationY = 0.0f;
float translationZ = 0.0f;

glm::vec3 minCameraCoords = glm::vec3(-boxParameter - translationX, -boxParameter - translationY, -boxParameter - translationZ);
glm::vec3 maxCameraCoords = glm::vec3(boxParameter + translationX, boxParameter + translationY, boxParameter + translationZ);
glm::vec3 oldCameraPosition = camera.Position;


int main()
{
    cout << camera.Position.x << camera.Position.y << camera.Position.z << endl;
    camera.MovementSpeed = 1.0f;
    #pragma region Setup
    if (!glfwInit()) // !0 == 1  | glfwInit inicijalizuje GLFW i vrati 1 ako je inicijalizovana uspjesno, a 0 ako nije
    {
        std::cout << "GLFW Biblioteka se nije ucitala! :(\n";
        return 1;
    }
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window;
    const char wTitle[] = "[Generic Title]";
    window = glfwCreateWindow(wWidth, wHeight, wTitle, NULL, NULL);
    if (window == NULL) //Ako prozor nije napravljen
    {
        std::cout << "Prozor nije napravljen! :(\n";
        glfwTerminate(); //Gasi GLFW
        return 2; //Vrati kod za gresku
    }
    glfwMakeContextCurrent(window);
    if (glewInit() != GLEW_OK)
    {
        std::cout << "GLEW nije mogao da se ucita! :'(\n";
        return 3;
    }
    unsigned int VAO[NUMBER_OF_BUFFERS];
    glGenVertexArrays(NUMBER_OF_BUFFERS, VAO);
    unsigned int VBO[NUMBER_OF_BUFFERS];
    glGenBuffers(NUMBER_OF_BUFFERS, VBO);
    unsigned int EBO[NUMBER_OF_BUFFERS];
    glGenBuffers(NUMBER_OF_BUFFERS, EBO);

    glfwSetCursorPosCallback(window, rotateCamera);
    glfwSetScrollCallback(window, zoomCamera);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    #pragma endregion

    int polygonMode = GL_FILL;
    float circleX = 0;
    float circleY = 0;
    float boundryX = 0.3;
    float boundryY = 0.4;
    bool shouldDrawCircle = false;

    Shader wallTextureShader("wallTexture.vert", "wallTexture.frag");
    unsigned wallTexture = loadImageToTexture("res/wall.jpg");
    unsigned wallWoodTexture = loadImageToTexture("res/wallWood.jpg");
    unsigned floorTexture = loadImageToTexture("res/floor.jpg");

    glBindTexture(GL_TEXTURE_2D, wallTexture);
    glGenerateMipmap(GL_TEXTURE_2D);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glBindTexture(GL_TEXTURE_2D, 0);

    glBindTexture(GL_TEXTURE_2D, wallWoodTexture);
    glGenerateMipmap(GL_TEXTURE_2D);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glBindTexture(GL_TEXTURE_2D, 0);

    glBindTexture(GL_TEXTURE_2D, floorTexture);
    glGenerateMipmap(GL_TEXTURE_2D);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glBindTexture(GL_TEXTURE_2D, 0);
    
    Shader shader3D("basic3D.vert", "wallTexture.frag");
    glm::mat4 modelRoom1 = glm::mat4(1.0f);
    glm::mat4 modelRoom2 = glm::mat4(1.0f);
    glm::mat4 modelHallway = glm::mat4(1.0f);

    glm::mat4 projectionP = glm::perspective(camera.Zoom, (float)wWidth / (float)wHeight, 0.1f, 100.0f); //Matrica perspektivne projekcije (FOV, Aspect Ratio, prednja ravan, zadnja ravan)

    shader3D.use();
    shader3D.setMat4("uP", projectionP);
    
    modelRoom1 = glm::translate(modelRoom1, glm::vec3(-translationX, translationY, translationZ));
    modelRoom2 = glm::translate(modelRoom2, glm::vec3(translationX, translationY, translationZ));
    modelRoom2 = glm::rotate(modelRoom2, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    shader3D.setMat4("uV", camera.GetViewMatrix());

    

    #pragma region RoomVertices
    float wall3DVertices[] =
    {
        //X     Y      Z       NX    NY     NZ
    -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,   0.0, 0.0,
     0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,   3.0, 0.0,
     0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,   3.0, 3.0,
     0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,   3.0, 3.0,
    -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,   0.0, 3.0,
    -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,   0.0, 0.0,

    -0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,    0.0, 0.0,
     0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,    3.0, 0.0,
     0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,    3.0, 3.0,
     0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,    3.0, 3.0,
    -0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,    0.0, 3.0,
    -0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,    0.0, 0.0,

    -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,   0.0, 3.0,
    -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,   3.0, 3.0,
    -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,   3.0, 0.0,
    -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,   3.0, 0.0,
    -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,   0.0, 0.0,
    -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,   0.0, 3.0,

    //Ovde su otvori za hodnik
    0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,    1.0, 3.0,
     0.5f,  0.5f, 0.2f,  1.0f,  0.0f,  0.0f,   0.0, 3.0,
     0.5f, -0.5f, 0.2f,  1.0f,  0.0f,  0.0f,   0.0, 0.0,
     0.5f, -0.5f, 0.2f,  1.0f,  0.0f,  0.0f,   0.0, 0.0,
     0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,   1.0, 0.0,
     0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,   1.0, 3.0,

     0.5f,  0.5f, -0.2f,  1.0f,  0.0f,  0.0f,    1.0, 3.0,
     0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,   0.0, 3.0,
     0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,   0.0, 0.0,
     0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,   0.0, 0.0,
     0.5f, -0.5f,  -0.2f,  1.0f,  0.0f,  0.0f,   1.0, 0.0,
     0.5f,  0.5f,  -0.2f,  1.0f,  0.0f,  0.0f,   1.0, 3.0
    };

    unsigned int strideWall3D = (3 + 3 + 2) * sizeof(float);
    unsigned int VAO3D;
    glGenVertexArrays(1, &VAO3D);
    glBindVertexArray(VAO3D);

    unsigned int VBO3D;
    glGenBuffers(1, &VBO3D);
    glBindBuffer(GL_ARRAY_BUFFER, VBO3D);
    glBufferData(GL_ARRAY_BUFFER, sizeof(wall3DVertices), wall3DVertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, strideWall3D, (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, strideWall3D, (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, strideWall3D, (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    #pragma endregion

    #pragma region Floors
    float floorVertices[] =
    {
        //X     Y      Z       NX    NY     NZ
      -0.5f, -0.5f, -0.5f,  0.0f,  1.0f, 0.0f,   0.0, 3.0,
       -0.5f, -0.5f, 0.5f,  0.0f,  1.0f, 0.0f,   0.0, 0.0,
       0.5f,  -0.5f, 0.5f,  0.0f,  1.0f, 0.0f,   3.0, 0.0,
       0.5f,  -0.5f, 0.5f,  0.0f,  1.0f, 0.0f,   3.0, 0.0,
      0.5f,  -0.5f, -0.5f,  0.0f,  1.0f, 0.0f,   3.0, 3.0,
      -0.5f, -0.5f, -0.5f,  0.0f,  1.0f, 0.0f,   0.0, 3.0,
    };

    unsigned int strideFloor = (3 + 3 + 2) * sizeof(float);
    unsigned int VAOFLOOR;
    glGenVertexArrays(1, &VAOFLOOR);
    glBindVertexArray(VAOFLOOR);

    unsigned int VBOFLOOR;
    glGenBuffers(1, &VBOFLOOR);
    glBindBuffer(GL_ARRAY_BUFFER, VBOFLOOR);
    glBufferData(GL_ARRAY_BUFFER, sizeof(floorVertices), floorVertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, strideFloor, (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, strideFloor, (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, strideFloor, (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);


    #pragma endregion

    #pragma region Hallway
    float hallwayVertices[] =
    {
           //X     Y      Z       NX    NY     NZ
        -0.5f, -0.5f, -0.2f,  0.0f,  0.0f, -1.0f,   0.0, 0.0,
         0.5f, -0.5f, -0.2f,  0.0f,  0.0f, -1.0f,   3.0, 0.0,
         0.5f,  0.5f, -0.2f,  0.0f,  0.0f, -1.0f,   3.0, 3.0,
         0.5f,  0.5f, -0.2f,  0.0f,  0.0f, -1.0f,   3.0, 3.0,
        -0.5f,  0.5f, -0.2f,  0.0f,  0.0f, -1.0f,   0.0, 3.0,
        -0.5f, -0.5f, -0.2f,  0.0f,  0.0f, -1.0f,   0.0, 0.0,

        -0.5f, -0.5f,  0.2f,  0.0f,  0.0f, 1.0f,    0.0, 0.0,
         0.5f, -0.5f,  0.2f,  0.0f,  0.0f, 1.0f,    3.0, 0.0,
         0.5f,  0.5f,  0.2f,  0.0f,  0.0f, 1.0f,    3.0, 3.0,
         0.5f,  0.5f,  0.2f,  0.0f,  0.0f, 1.0f,    3.0, 3.0,
        -0.5f,  0.5f,  0.2f,  0.0f,  0.0f, 1.0f,    0.0, 3.0,
        -0.5f, -0.5f,  0.2f,  0.0f,  0.0f, 1.0f,    0.0, 0.0,
    };

    unsigned int strideHallway = (3 + 3 + 2) * sizeof(float);
    unsigned int VAOHALLWAY;
    glGenVertexArrays(1, &VAOHALLWAY);
    glBindVertexArray(VAOHALLWAY);

    unsigned int VBOHALLWAY;
    glGenBuffers(1, &VBOHALLWAY);
    glBindBuffer(GL_ARRAY_BUFFER, VBOHALLWAY);
    glBufferData(GL_ARRAY_BUFFER, sizeof(hallwayVertices), hallwayVertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, strideHallway, (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, strideHallway, (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, strideHallway, (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    #pragma endregion

    #pragma region HallwayFloor
    float hallwayFloorVertices[] =
    {
        //X     Y      Z       NX    NY     NZ
      -0.5f, -0.5f, -0.2f,  0.0f,  1.0f, 0.0f,   0.0, 3.0,
       -0.5f, -0.5f, 0.2f,  0.0f,  1.0f, 0.0f,   0.0, 0.0,
       0.5f,  -0.5f, 0.2f,  0.0f,  1.0f, 0.0f,   3.0, 0.0,
       0.5f,  -0.5f, 0.2f,  0.0f,  1.0f, 0.0f,   3.0, 0.0,
      0.5f,  -0.5f, -0.2f,  0.0f,  1.0f, 0.0f,   3.0, 3.0,
      -0.5f, -0.5f, -0.2f,  0.0f,  1.0f, 0.0f,   0.0, 3.0,
    };

    unsigned int strideHallwayFloor = (3 + 3 + 2) * sizeof(float);
    unsigned int VAOHALLWAYFLOOR;
    glGenVertexArrays(1, &VAOHALLWAYFLOOR);
    glBindVertexArray(VAOHALLWAYFLOOR);

    unsigned int VBOHALLWAYFLOOR;
    glGenBuffers(1, &VBOHALLWAYFLOOR);
    glBindBuffer(GL_ARRAY_BUFFER, VBOHALLWAYFLOOR);
    glBufferData(GL_ARRAY_BUFFER, sizeof(hallwayFloorVertices), hallwayFloorVertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, strideHallwayFloor, (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, strideHallwayFloor, (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, strideHallwayFloor, (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    #pragma endregion

    #pragma region AngelModel
    Model angel("res/angel-model/12147_angelwings_V2_L2.obj");
    Shader modelShader("model-shader/model.vert", "model-shader/model.frag");
    glm::mat4 model2 = glm::mat4(1.0f);
    float modelScalingFactor = 220;
    cout << angel.center.x << angel.center.y << angel.center.z << endl;
    angel.center /= modelScalingFactor;
    model2 = glm::scale(model2, glm::vec3(1.0/modelScalingFactor, 1.0 / modelScalingFactor, 1.0 / modelScalingFactor));
    model2 = glm::translate(model2, glm::vec3(1.0, -0.4, 0.0) * modelScalingFactor);
    angel.center += glm::vec3(1.0, -0.4, 0.0);
    cout << angel.center.x << angel.center.y << angel.center.z << endl;
    modelShader.use();
    modelShader.setMat4("uP", projectionP);
    modelShader.setMat4("uM", model2);
    modelShader.setMat4("uV", camera.GetViewMatrix());
    glUseProgram(0);
    #pragma endregion
   
    glEnable(GL_DEPTH_TEST);
    glCullFace(GL_BACK);
    while (!glfwWindowShouldClose(window)) 
    {

        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        #pragma region KeyHandle
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        {
            glfwSetWindowShouldClose(window, GL_TRUE);
        }
        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        {
            oldCameraPosition = camera.Position;
            camera.ProcessKeyboard(FORWARD, deltaTime);
            if (!checkCameraPosition())
                camera.Position = oldCameraPosition;
        }
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        {
            oldCameraPosition = camera.Position;
            camera.ProcessKeyboard(BACKWARD, deltaTime);
            if (!checkCameraPosition())
                camera.Position = oldCameraPosition;
        }
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        {
            oldCameraPosition = camera.Position;
            camera.ProcessKeyboard(RIGHT, deltaTime);
            if (!checkCameraPosition())
                camera.Position = oldCameraPosition;
        }
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        {
            oldCameraPosition = camera.Position;
            camera.ProcessKeyboard(LEFT, deltaTime);
            if (!checkCameraPosition())
                camera.Position = oldCameraPosition;
        }
        if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS)
        {
            model2 = glm::rotate(model2, glm::radians(-1.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        }
        #pragma endregion
        projectionP = glm::perspective(camera.Zoom, (float)wWidth / (float)wHeight, 0.1f, 100.0f);
        shader3D.use();
        shader3D.setMat4("uV", camera.GetViewMatrix());
        shader3D.setMat4("uP", projectionP);
        glUseProgram(0);

        modelShader.use();
        modelShader.setMat4("uV", camera.GetViewMatrix());
        modelShader.setMat4("uM", model2);
        modelShader.setMat4("uP", projectionP);
        glUseProgram(0);
        glClearColor(0.0, 0.0, 0.0, 1.0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        #pragma region GoldenRoom
        shader3D.use();
        shader3D.setMat4("uM", modelRoom1);
        glBindVertexArray(VAO3D);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, wallTexture);

        glDrawArrays(GL_TRIANGLES, 0, 30);

        glBindTexture(GL_TEXTURE_2D, 0);
        glDisable(GL_BLEND);
        glUseProgram(0);
        glBindVertexArray(0);
        #pragma endregion

        #pragma region WoodenRoom
        shader3D.use();
        shader3D.setMat4("uM", modelRoom2);
        glBindVertexArray(VAO3D);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, wallWoodTexture);

        glDrawArrays(GL_TRIANGLES, 0, 30);

        glBindTexture(GL_TEXTURE_2D, 0);
        glDisable(GL_BLEND);
        glUseProgram(0);
        glBindVertexArray(0);
        #pragma endregion

        #pragma region FloorInGolden
        shader3D.use();
        shader3D.setMat4("uM", modelRoom1);
        glBindVertexArray(VAOFLOOR);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, floorTexture);

        glDrawArrays(GL_TRIANGLES, 0, 6);

        glBindTexture(GL_TEXTURE_2D, 0);
        glDisable(GL_BLEND);
        glUseProgram(0);
        glBindVertexArray(0);
        #pragma endregion 

        #pragma region FloorInWooden
        shader3D.use();
        shader3D.setMat4("uM", modelRoom2);
        glBindVertexArray(VAOFLOOR);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, floorTexture);

        glDrawArrays(GL_TRIANGLES, 0, 6);

        glBindTexture(GL_TEXTURE_2D, 0);
        glDisable(GL_BLEND);
        glUseProgram(0);
        glBindVertexArray(0);
        #pragma endregion 

        #pragma region Hallway
        shader3D.use();
        shader3D.setMat4("uM", modelHallway);
        glBindVertexArray(VAOHALLWAY);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, floorTexture);

        glDrawArrays(GL_TRIANGLES, 0, 12);

        glBindTexture(GL_TEXTURE_2D, 0);
        glDisable(GL_BLEND);
        glUseProgram(0);
        glBindVertexArray(0);
        #pragma endregion

        #pragma region HallwayFloor
        shader3D.use();
        shader3D.setMat4("uM", modelHallway);
        glBindVertexArray(VAOHALLWAYFLOOR);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, floorTexture);

        glDrawArrays(GL_TRIANGLES, 0, 6);

        glBindTexture(GL_TEXTURE_2D, 0);
        glDisable(GL_BLEND);
        glUseProgram(0);
        glBindVertexArray(0);
        #pragma endregion

        glm::vec3 toModelVector = angel.center - camera.Position;
        const float rotationThreshold = glm::radians(30.0f);
        float angle = glm::acos(glm::dot(glm::normalize(toModelVector), camera.Front));

        glm::vec3 crossResult = glm::cross(glm::normalize(toModelVector), camera.Front);
        float sign = glm::dot(crossResult, glm::vec3(0.0f, 1.0f, 0.0f)) < 0.0f ? -1.0f : 1.0f;

        if (glm::abs(angle) < rotationThreshold) {
            model2 = glm::rotate(model2, glm::radians(-0.3f), glm::vec3(0.0f, 1.0f, 0.0f));
        }

        modelShader.use();
        angel.Draw(modelShader);
        glUseProgram(0);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

}

unsigned int compileShader(GLenum type, const char* source)
{
    std::string content = "";
    std::ifstream file(source);
    std::stringstream ss;
    if (file.is_open())
    {
        ss << file.rdbuf();
        file.close();
        std::cout << "Uspjesno procitao fajl sa putanje \"" << source << "\"!" << std::endl;
    }
    else {
        ss << "";
        std::cout << "Greska pri citanju fajla sa putanje \"" << source << "\"!" << std::endl;
    }
    std::string temp = ss.str();
    const char* sourceCode = temp.c_str();

    int shader = glCreateShader(type);

    int success;
    char infoLog[512];
    glShaderSource(shader, 1, &sourceCode, NULL);
    glCompileShader(shader);

    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (success == GL_FALSE)
    {
        glGetShaderInfoLog(shader, 512, NULL, infoLog);
        if (type == GL_VERTEX_SHADER)
            printf("VERTEX");
        else if (type == GL_FRAGMENT_SHADER)
            printf("FRAGMENT");
        printf(" sejder ima gresku! Greska: \n");
        printf(infoLog);
    }
    return shader;
}
unsigned int createShader(const char* vsSource, const char* fsSource)
{

    unsigned int program;
    unsigned int vertexShader;
    unsigned int fragmentShader;

    program = glCreateProgram();

    vertexShader = compileShader(GL_VERTEX_SHADER, vsSource);
    fragmentShader = compileShader(GL_FRAGMENT_SHADER, fsSource);


    glAttachShader(program, vertexShader);
    glAttachShader(program, fragmentShader);

    glLinkProgram(program);
    glValidateProgram(program);

    int success;
    char infoLog[512];
    glGetProgramiv(program, GL_VALIDATE_STATUS, &success);
    if (success == GL_FALSE)
    {
        glGetShaderInfoLog(program, 512, NULL, infoLog);
        std::cout << "Objedinjeni sejder ima gresku! Greska: \n";
        std::cout << infoLog << std::endl;
    }

    glDetachShader(program, vertexShader);
    glDeleteShader(vertexShader);
    glDetachShader(program, fragmentShader);
    glDeleteShader(fragmentShader);

    return program;
}
static unsigned loadImageToTexture(const char* filePath) {
    int TextureWidth;
    int TextureHeight;
    int TextureChannels;
    unsigned char* ImageData = stbi_load(filePath, &TextureWidth, &TextureHeight, &TextureChannels, 0);
    if (ImageData != NULL)
    {
        //Slike se osnovno ucitavaju naopako pa se moraju ispraviti da budu uspravne
        stbi__vertical_flip(ImageData, TextureWidth, TextureHeight, TextureChannels);

        // Provjerava koji je format boja ucitane slike
        GLint InternalFormat = -1;
        switch (TextureChannels) {
        case 1: InternalFormat = GL_RED; break;
        case 3: InternalFormat = GL_RGB; break;
        case 4: InternalFormat = GL_RGBA; break;
        default: InternalFormat = GL_RGB; break;
        }

        unsigned int Texture;
        glGenTextures(1, &Texture);
        glBindTexture(GL_TEXTURE_2D, Texture);
        glTexImage2D(GL_TEXTURE_2D, 0, InternalFormat, TextureWidth, TextureHeight, 0, InternalFormat, GL_UNSIGNED_BYTE, ImageData);
        glBindTexture(GL_TEXTURE_2D, 0);
        // oslobadjanje memorije zauzete sa stbi_load posto vise nije potrebna
        stbi_image_free(ImageData);
        return Texture;
    }
    else
    {
        std::cout << "Textura nije ucitana! Putanja texture: " << filePath << std::endl;
        stbi_image_free(ImageData);
        return 0;
    }
}

void rotateCamera(GLFWwindow* window, double xposIn, double yposIn)
{
    float xpos = static_cast<float>(xposIn);
    float ypos = static_cast<float>(yposIn);

    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

    lastX = xpos;
    lastY = ypos;

    camera.ProcessMouseMovement(xoffset, yoffset);
}
void zoomCamera(GLFWwindow* window, double xoffset, double yoffset)
{
    float realOffset = (yoffset / abs(yoffset)) * 0.1;
    camera.ProcessMouseScroll(realOffset);
}
bool checkCameraPosition() {
    if (camera.Position.x <= minCameraCoords.x + 0.1 || camera.Position.x >= maxCameraCoords.x - 0.1 ||
        camera.Position.y <= minCameraCoords.y + 0.1 || camera.Position.y >= maxCameraCoords.y - 0.1 ||
        camera.Position.z <= minCameraCoords.z + 0.1 || camera.Position.z >= maxCameraCoords.z - 0.1)
        return false;

    if ((camera.Position.z <= -0.12 || camera.Position.z >= 0.12) &&
        (camera.Position.x >= -0.6 && camera.Position.x <= 0.6)) return false;

    return true;

}
