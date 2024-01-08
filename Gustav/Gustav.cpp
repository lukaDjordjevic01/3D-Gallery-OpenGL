
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

#pragma region HelpfulFunctionsDeclarations
unsigned int compileShader(GLenum type, const char* source);
unsigned int createShader(const char* vsSource, const char* fsSource);
static unsigned loadImageToTexture(const char* filePath);

void rotateCamera(GLFWwindow* window, double xposIn, double yposIn);
void zoomCamera(GLFWwindow* window, double xoffset, double yoffset);
bool checkCameraPosition();
#pragma endregion

#pragma region Globals
const int NUMBER_OF_BUFFERS = 5;
const int CIRCLE_POINTS = 30;

const int wallTextureIndex = 0;
const int frameShapeIndex = 1;
const int frameImageTextureIndex = 2;
const int personalInfoTextureIndex = 3;
const int circleIndex = 4;

unsigned int wWidth = 1600;
unsigned int wHeight = 800;


glm::vec3 cameraPos = glm::vec3(-1.0f, -0.5f, 0.0f);
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
float boxParameter = 1.0f;
float translationX = boxParameter * 2;
float translationY = 0.0f;
float translationZ = 0.0f;
float ceilingDown = 0.6f;

glm::vec3 minCameraCoords = glm::vec3(-boxParameter - translationX, -boxParameter - translationY, -boxParameter - translationZ);
glm::vec3 maxCameraCoords = glm::vec3(boxParameter + translationX, boxParameter + translationY - ceilingDown, boxParameter + translationZ);
glm::vec3 oldCameraPosition = camera.Position;
#pragma endregion


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

    #pragma region TextureSetup
    Shader wallTextureShader("wallTexture.vert", "wallTexture.frag");
    unsigned wallTexture = loadImageToTexture("res/wall.jpg");
    unsigned wallWoodTexture = loadImageToTexture("res/wallWood.jpg");
    unsigned floorTexture = loadImageToTexture("res/floor.jpg");

    unsigned firstImageTexture = loadImageToTexture("res/gustav_image1.jpg");
    unsigned secondImageTexture = loadImageToTexture("res/gustav_image2.jpg");
    unsigned thirdImageTexture = loadImageToTexture("res/gustav_image3.jpg");

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

    glBindTexture(GL_TEXTURE_2D, firstImageTexture);
    glGenerateMipmap(GL_TEXTURE_2D);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glBindTexture(GL_TEXTURE_2D, 0);

    glBindTexture(GL_TEXTURE_2D, secondImageTexture);
    glGenerateMipmap(GL_TEXTURE_2D);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glBindTexture(GL_TEXTURE_2D, 0);

    glBindTexture(GL_TEXTURE_2D, thirdImageTexture);
    glGenerateMipmap(GL_TEXTURE_2D);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glBindTexture(GL_TEXTURE_2D, 0);

    #pragma endregion
    
    Shader shader3D("basic3D.vert", "wallTexture.frag");
    glm::mat4 modelRoom1 = glm::mat4(1.0f);
    glm::mat4 modelRoom2 = glm::mat4(1.0f);
    glm::mat4 modelHallway = glm::mat4(1.0f);

    glm::mat4 modelFrame1 = glm::mat4(1.0f);
    glm::mat4 modelFrame2 = glm::mat4(1.0f);
    glm::mat4 modelFrame3 = glm::mat4(1.0f);
    
    glm::mat4 projectionP = glm::perspective(camera.Zoom, (float)wWidth / (float)wHeight, 0.1f, 100.0f); //Matrica perspektivne projekcije (FOV, Aspect Ratio, prednja ravan, zadnja ravan)

    shader3D.use();
    shader3D.setMat4("uP", projectionP);
    
    modelRoom1 = glm::translate(modelRoom1, glm::vec3(-translationX, translationY, translationZ));
    modelRoom2 = glm::translate(modelRoom2, glm::vec3(translationX, translationY, translationZ));
    modelRoom2 = glm::rotate(modelRoom2, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));

    modelFrame1 = glm::translate(modelFrame1, glm::vec3(-translationX, translationY, translationZ));
    modelFrame2 = glm::translate(modelFrame2, glm::vec3(-translationX, translationY, translationZ));
    modelFrame2 = glm::rotate(modelFrame2, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    modelFrame3 = glm::translate(modelFrame3, glm::vec3(-translationX, translationY, translationZ));
    modelFrame3 = glm::rotate(modelFrame3, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));

    glm::mat4 modelCircle1 = modelFrame1;
    glm::mat4 modelCircle2 = modelFrame2;
    glm::mat4 modelCircle3 = modelFrame3;

    shader3D.setMat4("uV", camera.GetViewMatrix());

    

    #pragma region RoomVertices
    float wall3DVertices[] =
    {
        //X             Y                           Z              NX     NY     NZ      TX   TY
        -boxParameter, -boxParameter,              -boxParameter,  0.0f,  0.0f, -1.0f,   0.0, 0.0,
         boxParameter, -boxParameter,              -boxParameter,  0.0f,  0.0f, -1.0f,   3.0, 0.0,
         boxParameter,  boxParameter -ceilingDown, -boxParameter,  0.0f,  0.0f, -1.0f,   3.0, 3.0,
         boxParameter,  boxParameter -ceilingDown, -boxParameter,  0.0f,  0.0f, -1.0f,   3.0, 3.0,
        -boxParameter,  boxParameter -ceilingDown, -boxParameter,  0.0f,  0.0f, -1.0f,   0.0, 3.0,
        -boxParameter, -boxParameter,              -boxParameter,  0.0f,  0.0f, -1.0f,   0.0, 0.0,

        -boxParameter, -boxParameter,               boxParameter,  0.0f,  0.0f, 1.0f,    0.0, 0.0,
         boxParameter, -boxParameter,               boxParameter,  0.0f,  0.0f, 1.0f,    3.0, 0.0,
         boxParameter,  boxParameter -ceilingDown,  boxParameter,  0.0f,  0.0f, 1.0f,    3.0, 3.0,
         boxParameter,  boxParameter -ceilingDown,  boxParameter,  0.0f,  0.0f, 1.0f,    3.0, 3.0,
        -boxParameter,  boxParameter -ceilingDown,  boxParameter,  0.0f,  0.0f, 1.0f,    0.0, 3.0,
        -boxParameter, -boxParameter,               boxParameter,  0.0f,  0.0f, 1.0f,    0.0, 0.0,

        -boxParameter,  boxParameter -ceilingDown,  boxParameter, -1.0f,  0.0f,  0.0f,   0.0, 3.0,
        -boxParameter,  boxParameter -ceilingDown, -boxParameter, -1.0f,  0.0f,  0.0f,   3.0, 3.0,
        -boxParameter, -boxParameter,              -boxParameter, -1.0f,  0.0f,  0.0f,   3.0, 0.0,
        -boxParameter, -boxParameter,              -boxParameter, -1.0f,  0.0f,  0.0f,   3.0, 0.0,
        -boxParameter, -boxParameter,               boxParameter, -1.0f,  0.0f,  0.0f,   0.0, 0.0,
        -boxParameter,  boxParameter -ceilingDown,  boxParameter, -1.0f,  0.0f,  0.0f,   0.0, 3.0,

        //Ovde su otvori za hodnik
        boxParameter,  boxParameter -ceilingDown,   boxParameter,  1.0f,  0.0f,  0.0f,   1.0, 3.0,
         boxParameter,  boxParameter -ceilingDown,  0.2f,          1.0f,  0.0f,  0.0f,   0.0, 3.0,
         boxParameter, -boxParameter,               0.2f,          1.0f,  0.0f,  0.0f,   0.0, 0.0,
         boxParameter, -boxParameter,               0.2f,          1.0f,  0.0f,  0.0f,   0.0, 0.0,
         boxParameter, -boxParameter,               boxParameter,  1.0f,  0.0f,  0.0f,   1.0, 0.0,
         boxParameter,  boxParameter -ceilingDown,  boxParameter,  1.0f,  0.0f,  0.0f,   1.0, 3.0,

         boxParameter,  boxParameter -ceilingDown, -0.2f,          1.0f,  0.0f,  0.0f,   1.0, 3.0,
         boxParameter,  boxParameter -ceilingDown, -boxParameter,  1.0f,  0.0f,  0.0f,   0.0, 3.0,
         boxParameter, -boxParameter,              -boxParameter,  1.0f,  0.0f,  0.0f,   0.0, 0.0,
         boxParameter, -boxParameter,              -boxParameter,  1.0f,  0.0f,  0.0f,   0.0, 0.0,
         boxParameter, -boxParameter,              -0.2f,          1.0f,  0.0f,  0.0f,   1.0, 0.0,
         boxParameter,  boxParameter -ceilingDown, -0.2f,          1.0f,  0.0f,  0.0f,   1.0, 3.0
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
      -boxParameter, -boxParameter, -boxParameter,  0.0f,  1.0f, 0.0f,   0.0, 3.0,
       -boxParameter, -boxParameter, boxParameter,  0.0f,  1.0f, 0.0f,   0.0, 0.0,
       boxParameter,  -boxParameter, boxParameter,  0.0f,  1.0f, 0.0f,   3.0, 0.0,
       boxParameter,  -boxParameter, boxParameter,  0.0f,  1.0f, 0.0f,   3.0, 0.0,
      boxParameter,  -boxParameter, -boxParameter,  0.0f,  1.0f, 0.0f,   3.0, 3.0,
      -boxParameter, -boxParameter, -boxParameter,  0.0f,  1.0f, 0.0f,   0.0, 3.0,
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
         //X            Y                            Z      NX     NY     NZ      TX   TY
        -boxParameter, -boxParameter,               -0.2f,  0.0f,  0.0f, -1.0f,   0.0, 0.0,
         boxParameter, -boxParameter,               -0.2f,  0.0f,  0.0f, -1.0f,   3.0, 0.0,
         boxParameter,  boxParameter - ceilingDown, -0.2f,  0.0f,  0.0f, -1.0f,   3.0, 3.0,
         boxParameter,  boxParameter - ceilingDown, -0.2f,  0.0f,  0.0f, -1.0f,   3.0, 3.0,
        -boxParameter,  boxParameter - ceilingDown, -0.2f,  0.0f,  0.0f, -1.0f,   0.0, 3.0,
        -boxParameter, -boxParameter,               -0.2f,  0.0f,  0.0f, -1.0f,   0.0, 0.0,

        -boxParameter, -boxParameter,                0.2f,  0.0f,  0.0f, 1.0f,    0.0, 0.0,
         boxParameter, -boxParameter,                0.2f,  0.0f,  0.0f, 1.0f,    3.0, 0.0,
         boxParameter,  boxParameter - ceilingDown,  0.2f,  0.0f,  0.0f, 1.0f,    3.0, 3.0,
         boxParameter,  boxParameter - ceilingDown,  0.2f,  0.0f,  0.0f, 1.0f,    3.0, 3.0,
        -boxParameter,  boxParameter - ceilingDown,  0.2f,  0.0f,  0.0f, 1.0f,    0.0, 3.0,
        -boxParameter, -boxParameter,                0.2f,  0.0f,  0.0f, 1.0f,    0.0, 0.0,
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
      -boxParameter, -boxParameter, -0.2f,  0.0f,  1.0f, 0.0f,   0.0, 3.0,
       -boxParameter, -boxParameter, 0.2f,  0.0f,  1.0f, 0.0f,   0.0, 0.0,
       boxParameter,  -boxParameter, 0.2f,  0.0f,  1.0f, 0.0f,   3.0, 0.0,
       boxParameter,  -boxParameter, 0.2f,  0.0f,  1.0f, 0.0f,   3.0, 0.0,
      boxParameter,  -boxParameter, -0.2f,  0.0f,  1.0f, 0.0f,   3.0, 3.0,
      -boxParameter, -boxParameter, -0.2f,  0.0f,  1.0f, 0.0f,   0.0, 3.0,
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
    float modelScalingFactor = 100;
    cout << angel.center.x << angel.center.y << angel.center.z << endl;
    angel.center /= modelScalingFactor;
    model2 = glm::scale(model2, glm::vec3(1.0/modelScalingFactor, 1.0 / modelScalingFactor, 1.0 / modelScalingFactor));
    model2 = glm::translate(model2, glm::vec3(boxParameter * 2, -boxParameter + 0.2, 0.0) * modelScalingFactor);
    angel.center += glm::vec3(boxParameter * 2, -boxParameter + 0.2, 0.0);
    cout << angel.center.x << angel.center.y << angel.center.z << endl;
    modelShader.use();
    modelShader.setMat4("uP", projectionP);
    modelShader.setMat4("uM", model2);
    modelShader.setMat4("uV", camera.GetViewMatrix());
    glUseProgram(0);
    #pragma endregion

    #pragma region PictureFrame
    Shader frameShader("basic3D.vert", "basic3D.frag");
    frameShader.use();
    frameShader.setMat4("uP", projectionP);
    frameShader.setMat4("uV", camera.GetViewMatrix());
    frameShader.setMat4("uM", modelRoom1);

    float frameYfactor = 1.15;
    float frameXfactor = 2.3;
    float innerFrameFactor = 0.05;

    float xLeft = -boxParameter / frameXfactor;
    float xLeftInner = xLeft + innerFrameFactor;
    float xRight = -xLeft;
    float xRightInner = xRight - innerFrameFactor;

    float yUp = boxParameter / frameYfactor - ceilingDown;
    float yUpInner = yUp - innerFrameFactor;
    float yDown = -boxParameter / frameYfactor;
    float yDownInner = yDown + innerFrameFactor;

    float zBack = -boxParameter + 0.001;
    float zFront = zBack + 0.05;

    float frame3DVertices[] =
    {
        //X  Y      Z    R    G    B
        xLeft, yUp, zBack,  //0,
        xLeft, yUpInner, zBack,  //1
        xLeft, yDownInner, zBack, //2
        xLeft, yDown, zBack,  //3

        xLeftInner, yUp, zBack,  //4
        xLeftInner, yDown, zBack,  //5

        xRightInner, yUp, zBack,  //6
        xRightInner, yDown, zBack,  //7

        xRight, yUp, zBack,  //8,
        xRight, yUpInner, zBack,  //9
        xRight, yDownInner, zBack, //10
        xRight, yDown, zBack,  //11



        xLeft, yUp, zFront,  //12,
        xLeft, yUpInner, zFront,  //13
        xLeft, yDownInner, zFront,  //14
        xLeft, yDown, zFront, //15

        xLeftInner, yUp, zFront,  //16
        xLeftInner, yDown, zFront,  //17

        xRightInner, yUp, zFront,  //18
        xRightInner, yDown, zFront,  //19

        xRight, yUp, zFront,  //20,
        xRight, yUpInner, zFront,  //21
        xRight, yDownInner, zFront,  //22
        xRight, yDown, zFront,  //23


    };

    unsigned int frameIndecies[] =
    {
        // zadnji deo rama
        0, 3, 5, 5, 4, 0,
        0, 1, 9, 9, 8, 0,
        2, 3, 11, 11, 10, 2,
        8, 6, 7, 7, 11, 8,

        //prednji
        12, 15, 17, 17, 16, 12,
        12, 13, 21, 21, 20, 12,
        14, 15, 23, 23, 22, 14,
        20, 18, 19, 19, 23, 20,

        //levo
        15, 3, 0, 0, 12, 15,
        
        //desno
        23, 11, 8, 8, 20, 23,

        //gore
        0, 8, 20, 20, 12, 0,

        //dole
        3, 11, 23, 23, 15, 3
    };

    unsigned int frameStride = 3 * sizeof(float);

    unsigned int VAOFRAME;
    glGenVertexArrays(1, &VAOFRAME);
    glBindVertexArray(VAOFRAME);

    unsigned int VBOFRAME;
    glGenBuffers(1, &VBOFRAME);
    glBindBuffer(GL_ARRAY_BUFFER, VBOFRAME);
    glBufferData(GL_ARRAY_BUFFER, sizeof(frame3DVertices), frame3DVertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, frameStride, (void*)0);
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    unsigned int EBOFRAME;
    glGenBuffers(1, &EBOFRAME);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBOFRAME);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(frameIndecies), frameIndecies, GL_STATIC_DRAW);

    glBindVertexArray(0);
    #pragma endregion

    #pragma region Picture
    float zPic = (zBack + zFront) / 2;
    float pictureVertices[] =
    {
        xLeftInner,  yUpInner,   zPic, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f,
        xLeftInner,  yDownInner, zPic, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f,
        xRightInner, yUpInner,   zPic, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f,
        xRightInner, yDownInner, zPic, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f
    };

    unsigned int pictureIndecies[] =
    {
        0, 1, 3,
        3, 2, 0
    };

    unsigned int pictureStride = (3 + 3 + 2) * sizeof(float);

    unsigned int VAOPICTURE;
    glGenVertexArrays(1, &VAOPICTURE);
    glBindVertexArray(VAOPICTURE);

    unsigned int VBOPICTURE;
    glGenBuffers(1, &VBOPICTURE);
    glBindBuffer(GL_ARRAY_BUFFER, VBOPICTURE);
    glBufferData(GL_ARRAY_BUFFER, sizeof(pictureVertices), pictureVertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, pictureStride, (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, pictureStride, (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, pictureStride, (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    unsigned int EBOPICTURE;
    glGenBuffers(1, &EBOPICTURE);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBOPICTURE);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(pictureIndecies), pictureIndecies, GL_STATIC_DRAW);
    #pragma endregion

    #pragma region Circle
    Shader circleShader("circle.vert", "circle.frag");
    float circle[CIRCLE_POINTS * 3 + 6];
    float r = 0.1;

    float circleX = (xLeftInner + xRightInner) / 2;
    float circleY = (yUpInner + yDownInner) / 2;
    bool shouldDrawCircle = false;

    circle[0] = (xLeftInner + xRightInner) / 2;
    circle[1] = (yUpInner + yDownInner) / 2;
    circle[2] = zPic + 0.001;
    int i;
    for (i = 0; i <= CIRCLE_POINTS; i++)
    {

        circle[3 + 3 * i] = ((xLeftInner + xRightInner) / 2) + r * cos((3.141592 / 180) * (i * 360 / CIRCLE_POINTS));
        circle[3 + 3 * i + 1] = ((yUpInner + yDownInner) / 2) + r * sin((3.141592 / 180) * (i * 360 / CIRCLE_POINTS));
        circle[3 + 3 * i + 2] = zPic + 0.001;
    }
    unsigned int VAOCIRCLE;
    glGenVertexArrays(1, &VAOCIRCLE);
    glBindVertexArray(VAOCIRCLE);

    unsigned int VBOCIRCLE;
    glGenBuffers(1, &VBOCIRCLE);
    glBindBuffer(GL_ARRAY_BUFFER, VBOCIRCLE);
    glBufferData(GL_ARRAY_BUFFER, sizeof(circle), circle, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindVertexArray(0);

    #pragma endregion
   
    glEnable(GL_DEPTH_TEST);
    glCullFace(GL_BACK);

    #pragma region RenderingLoop
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
        if (glfwGetKey(window, GLFW_KEY_4) == GLFW_PRESS)
        {
            modelCircle1 = modelFrame1;
            modelCircle2 = modelFrame2;
            modelCircle3 = modelFrame3;
            circleX = (xLeftInner + xRightInner) / 2;
            circleY = (yUpInner + yDownInner) / 2;

            shouldDrawCircle = true;
        }
        if (glfwGetKey(window, GLFW_KEY_KP_8) == GLFW_PRESS) 
        {
            if (shouldDrawCircle && circleY <= yUpInner - r) 
            {
                modelCircle1 = translate(modelCircle1, glm::vec3(0.0, 0.01, 0.0));
                modelCircle2 = translate(modelCircle2, glm::vec3(0.0, 0.01, 0.0));
                modelCircle3 = translate(modelCircle3, glm::vec3(0.0, 0.01, 0.0));
                circleY += 0.01;
            } 
        }
        if (glfwGetKey(window, GLFW_KEY_KP_2) == GLFW_PRESS)
        {
            if (shouldDrawCircle && circleY >= yDownInner + r)
            {
                modelCircle1 = translate(modelCircle1, glm::vec3(0.0, -0.01, 0.0));
                modelCircle2 = translate(modelCircle2, glm::vec3(0.0, -0.01, 0.0));
                modelCircle3 = translate(modelCircle3, glm::vec3(0.0, -0.01, 0.0));
                circleY -= 0.01;
            }
        }
        if (glfwGetKey(window, GLFW_KEY_KP_4) == GLFW_PRESS)
        {
            if (shouldDrawCircle && circleX >= xLeftInner + r)
            {
                modelCircle1 = translate(modelCircle1, glm::vec3(-0.01, 0.0, 0.0));
                modelCircle2 = translate(modelCircle2, glm::vec3(-0.01, 0.0, 0.0));
                modelCircle3 = translate(modelCircle3, glm::vec3(-0.01, 0.0, 0.0));
                circleX -= 0.01;
            }
        }
        if (glfwGetKey(window, GLFW_KEY_KP_6) == GLFW_PRESS)
        {
            if (shouldDrawCircle && circleX <= xRightInner - r)
            {
                modelCircle1 = translate(modelCircle1, glm::vec3(0.01, 0.0, 0.0));
                modelCircle2 = translate(modelCircle2, glm::vec3(0.01, 0.0, 0.0));
                modelCircle3 = translate(modelCircle3, glm::vec3(0.01, 0.0, 0.0));
                circleX += 0.01;
            }
        }
        if (glfwGetKey(window, GLFW_KEY_KP_5) == GLFW_PRESS) 
        {
            shouldDrawCircle = false;
            
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

        frameShader.use();
        frameShader.setMat4("uP", projectionP);
        frameShader.setMat4("uV", camera.GetViewMatrix());
        glUseProgram(0);

        modelShader.use();
        modelShader.setMat4("uV", camera.GetViewMatrix());
        modelShader.setMat4("uM", model2);
        modelShader.setMat4("uP", projectionP);
        glUseProgram(0);

        circleShader.use();
        circleShader.setMat4("uP", projectionP);
        circleShader.setMat4("uV", camera.GetViewMatrix());
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

        #pragma region Frames
        frameShader.use();
        frameShader.setVec3("color", glm::vec3(0.0, 0.0, 0.0));
        glBindVertexArray(VAOFRAME);

        frameShader.setMat4("uM", modelFrame1);
        glDrawElements(GL_TRIANGLES, 72, GL_UNSIGNED_INT, (void*)(0 * sizeof(unsigned int)));

        frameShader.setMat4("uM", modelFrame2);
        glDrawElements(GL_TRIANGLES, 72, GL_UNSIGNED_INT, (void*)(0 * sizeof(unsigned int)));

        frameShader.setMat4("uM", modelFrame3);
        glDrawElements(GL_TRIANGLES, 72, GL_UNSIGNED_INT, (void*)(0 * sizeof(unsigned int)));
        
        glUseProgram(0);
        glBindVertexArray(0);
        #pragma endregion

        #pragma region Pictures
        shader3D.use();
        glBindVertexArray(VAOPICTURE);
        glActiveTexture(GL_TEXTURE0);

        shader3D.setMat4("uM", modelFrame1);
        glBindTexture(GL_TEXTURE_2D, firstImageTexture);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void*)(0 * sizeof(unsigned int)));

        shader3D.setMat4("uM", modelFrame2);
        glBindTexture(GL_TEXTURE_2D, secondImageTexture);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void*)(0 * sizeof(unsigned int)));

        shader3D.setMat4("uM", modelFrame3);
        glBindTexture(GL_TEXTURE_2D, thirdImageTexture);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void*)(0 * sizeof(unsigned int)));

        glUseProgram(0);
        glBindVertexArray(0);
        #pragma endregion

        #pragma region Circles
        if (shouldDrawCircle)
        {
            circleShader.use();
            glBindVertexArray(VAOCIRCLE);

            float time = glfwGetTime();
            float pulsationSpeed = 0.8;
            circleShader.setFloat("circlePulse", pulsationSpeed * time);

            circleShader.setMat4("uM", modelCircle1);
            glDrawArrays(GL_TRIANGLE_FAN, 0, CIRCLE_POINTS + 2);

            circleShader.setMat4("uM", modelCircle2);
            glDrawArrays(GL_TRIANGLE_FAN, 0, CIRCLE_POINTS + 2);

            circleShader.setMat4("uM", modelCircle3);
            glDrawArrays(GL_TRIANGLE_FAN, 0, CIRCLE_POINTS + 2);

            glUseProgram(0);
        }
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
    #pragma endregion
}


#pragma region HelpfulFunctionsDefinitions
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
        (camera.Position.x >= -boxParameter - 0.1 && camera.Position.x <= boxParameter + 0.1)) return false;

    return true;

}
#pragma endregion
