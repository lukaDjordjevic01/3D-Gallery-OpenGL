
#include <iostream>
#include <fstream>
#include <sstream>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

unsigned int compileShader(GLenum type, const char* source);
unsigned int createShader(const char* vsSource, const char* fsSource);
static unsigned loadImageToTexture(const char* filePath);
void drawBackgroundOrInfoTexture(unsigned int shader, unsigned int VAO, unsigned int texture);
void drawFrames(unsigned int VAO, unsigned int wWidth, unsigned int wHeight, int polygonMode);
void drawImages(unsigned int VAO, unsigned int wWidth, unsigned int wHeight, unsigned int shader, unsigned int textures[]);

const int NUMBER_OF_BUFFERS = 5;
const int CIRCLE_POINTS = 30;

const int wallTextureIndex = 0;
const int frameShapeIndex = 1;
const int frameImageTextureIndex = 2;
const int personalInfoTextureIndex = 3;
const int circleIndex = 4;


int main()
{
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
    unsigned int wWidth = 1500;
    unsigned int wHeight = 700;
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
    #pragma endregion

    #pragma region WallTexture
    float wallVertices[] =
    {
        -1.0, 1.0,  0.0, 1.0,
        -1.0, -1.0, 0.0, 0.0,
        1.0, 1.0,   1.0, 1.0,
        1.0, -1.0,  1.0, 0.0
    };

    unsigned int wallIndecies[] =
    {
        0, 1, 2,
        1, 2, 3
    };

    unsigned int wallStride = (2 + 2) * sizeof(float);

    glBindVertexArray(VAO[wallTextureIndex]);

    glBindBuffer(GL_ARRAY_BUFFER, VBO[wallTextureIndex]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(wallVertices), wallVertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, wallStride, (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, wallStride, (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO[wallTextureIndex]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(wallIndecies), wallIndecies, GL_STATIC_DRAW);

    glBindVertexArray(0);

    unsigned int wallTextureShader = createShader("wallTexture.vert", "wallTexture.frag");
    unsigned wallTexture = loadImageToTexture("res/wall.jpg");

    glBindTexture(GL_TEXTURE_2D, wallTexture); 
    glGenerateMipmap(GL_TEXTURE_2D);  
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glBindTexture(GL_TEXTURE_2D, 0);

    glUseProgram(wallTextureShader);
    unsigned uTexLoc = glGetUniformLocation(wallTextureShader, "uTex");
    std::cout << uTexLoc << "\n";
    glUniform1i(uTexLoc, 0);
    glUseProgram(0);

    #pragma endregion

    #pragma region Frames
    //y -> +- 0.6
    //x -> +- 0.5
    float frameVertices[] =
    {
       -0.5, 0.6, //0
       -0.5, 0.5, //1
       -0.5, -0.5, //2
       -0.5, -0.6, //3
       -0.4, 0.6, //4
       -0.4, -0.6, //5
       0.4, 0.6, //6
       0.4, -0.6, //7
       0.5, 0.6, //8
       0.5, 0.5, //9
       0.5, -0.5, //10
       0.5, -0.6 //11
    };

    unsigned int frameIndecies[] =
    {
        0, 5, 3,
        0, 4, 5,

        0, 1, 8,
        8, 9, 1,

        2, 11, 3,
        10, 2, 11,

        6, 11, 8,
        7, 6, 11
    };
    unsigned int frameStride = 2 * sizeof(float);

    glBindVertexArray(VAO[frameShapeIndex]);

    glBindBuffer(GL_ARRAY_BUFFER, VBO[frameShapeIndex]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(frameVertices), frameVertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, frameStride, (void*)0);
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO[frameShapeIndex]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(frameIndecies), frameIndecies, GL_STATIC_DRAW);

    glBindVertexArray(0);
    #pragma endregion

    #pragma region GustavImages

    float imageVertices[] =
    {
        -0.4, 0.5,   0.0, 1.0,
        -0.4, -0.5,  0.0, 0.0,
        0.4, 0.5,    1.0, 1.0,
        0.4, -0.5,   1.0, 0.0 
    };

    unsigned int imageIndecies[] =
    {
        0, 1, 2,
        3, 1, 2
    };

    unsigned int frameImageTextureStride = (2 + 2) * sizeof(float);

    glBindVertexArray(VAO[frameImageTextureIndex]);

    glBindBuffer(GL_ARRAY_BUFFER, VBO[frameImageTextureIndex]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(imageVertices), imageVertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, frameImageTextureStride, (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, frameImageTextureStride, (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO[frameImageTextureIndex]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(imageIndecies), imageIndecies, GL_STATIC_DRAW);

    glBindVertexArray(0);

    unsigned int frameImageTextureShader = createShader("frameImageTexture.vert", "frameImageTexture.frag");
    unsigned firstImageTexture = loadImageToTexture("res/gustav_image1.jpg");
    unsigned secondImageTexture = loadImageToTexture("res/gustav_image2.jpg");
    unsigned thirdImageTexture = loadImageToTexture("res/gustav_image3.jpg");

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

    glUseProgram(frameImageTextureShader);
    unsigned imageTexLoc = glGetUniformLocation(frameImageTextureShader, "imageTex");
    std::cout << imageTexLoc << "\n";
    glUniform1i(imageTexLoc, 1);
    glUseProgram(0);

    #pragma endregion

    #pragma region PersonalInfoTexture
    float personalInfoVertices[] =
    {
        0.6, -0.6, 0.0, 1.0,
        0.6, -1.0, 0.0, 0.0,
        1.0, -0.6, 1.0, 1.0,
        1.0, -1.0, 1.0, 0.0
    };

    unsigned int personalInfoIndecies[] =
    {
        0, 1, 2,
        1, 2, 3
    };
    unsigned int personalInfoTextureStride = (2 + 2) * sizeof(float);
    glBindVertexArray(VAO[personalInfoTextureIndex]);

    glBindBuffer(GL_ARRAY_BUFFER, VBO[personalInfoTextureIndex]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(personalInfoVertices), personalInfoVertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, personalInfoTextureStride, (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, personalInfoTextureStride, (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO[personalInfoTextureIndex]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(personalInfoIndecies), personalInfoIndecies, GL_STATIC_DRAW);

    glBindVertexArray(0);

    unsigned personalInfoTexture = loadImageToTexture("res/personal_info.png");
    glBindTexture(GL_TEXTURE_2D, personalInfoTexture);
    glGenerateMipmap(GL_TEXTURE_2D);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glBindTexture(GL_TEXTURE_2D, 0);
    #pragma endregion

    #pragma region Circle
    float circle[CIRCLE_POINTS * 2 + 4];
    float r = 0.1;
    circle[0] = 0;
    circle[1] = 0;
    int i;
    for (i = 0; i <= CIRCLE_POINTS; i++)
    {

        circle[2 + 2 * i] = r * cos((3.141592 / 180) * (i * 360 / CIRCLE_POINTS));
        circle[2 + 2 * i + 1] = r * sin((3.141592 / 180) * (i * 360 / CIRCLE_POINTS));
    }

    glBindVertexArray(VAO[circleIndex]);

    glBindBuffer(GL_ARRAY_BUFFER, VBO[circleIndex]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(circle), circle, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindVertexArray(0);

    unsigned int circleShader = createShader("circle.vert", "circle.frag");
    unsigned int circlePosLoc = glGetUniformLocation(circleShader, "circlePos");
    unsigned int circlePulseLoc = glGetUniformLocation(circleShader, "circlePulse");
    #pragma endregion

    int polygonMode = GL_FILL;
    float circleX = 0;
    float circleY = 0;
    float boundryX = 0.3;
    float boundryY = 0.4;
    bool shouldDrawCircle = false;

    while (!glfwWindowShouldClose(window)) 
    {
        #pragma region KeyHandle
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        {
            glfwSetWindowShouldClose(window, GL_TRUE);
        }
        if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS)
        {
            polygonMode = GL_POINT;
            glUseProgram(frameImageTextureShader);
            glUniform1i(glGetUniformLocation(frameImageTextureShader, "flipHorizontal"), 1);
        }
        if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS)
        {
            polygonMode = GL_LINE;
            glUseProgram(frameImageTextureShader);
            glUniform1i(glGetUniformLocation(frameImageTextureShader, "flipVertical"), 1);
        }
        if (glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS)
        {
            polygonMode = GL_FILL;
            glUseProgram(frameImageTextureShader);
            glUniform1i(glGetUniformLocation(frameImageTextureShader, "flipHorizontal"), 0);
            glUniform1i(glGetUniformLocation(frameImageTextureShader, "flipVertical"), 0);
        }
        if (glfwGetKey(window, GLFW_KEY_4) == GLFW_PRESS)
        {
            circleX = 0;
            circleY = 0;
            shouldDrawCircle = true;
        }
        if (glfwGetKey(window, GLFW_KEY_5) == GLFW_PRESS)
        {
            shouldDrawCircle = false;
        }
        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        {
            if (shouldDrawCircle && circleY <= boundryY)
                circleY += 0.01;
        }
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        {
            if (shouldDrawCircle && circleY > -boundryY)
                circleY -= 0.01;
        }
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        {
            if (shouldDrawCircle && circleX <= boundryX)
                circleX += 0.01;
        }
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        {
            if (shouldDrawCircle && circleX >= -boundryX)
                circleX -= 0.01;
        }
        #pragma endregion

        glClear(GL_COLOR_BUFFER_BIT);
        glViewport(0, 0, wWidth, wHeight);
        drawBackgroundOrInfoTexture(wallTextureShader, VAO[wallTextureIndex], wallTexture);

        #pragma region FramesDrawing
        drawFrames(VAO[frameShapeIndex], wWidth, wHeight, polygonMode);
        #pragma endregion

        #pragma region ImagesDrawing
        unsigned int textures[] =
        {
            firstImageTexture, secondImageTexture, thirdImageTexture
        };
        drawImages(VAO[frameImageTextureIndex], wWidth, wHeight, frameImageTextureShader, textures);
        #pragma endregion

        #pragma region PersonalInfoDrawing
        drawBackgroundOrInfoTexture(wallTextureShader, VAO[personalInfoTextureIndex], personalInfoTexture);
        #pragma endregion

        #pragma region CircleDrawing
        if (shouldDrawCircle) 
        {
            glUseProgram(circleShader);
            glBindVertexArray(VAO[circleIndex]);
            float time = glfwGetTime();
            float pulsationSpeed = 0.8;
            glUniform1f(circlePulseLoc, pulsationSpeed* time);
            glUniform2f(circlePosLoc, circleX, circleY);

            glViewport(0, 0, wWidth / 3, wHeight);
            glDrawArrays(GL_TRIANGLE_FAN, 0, sizeof(circle) / (2 * sizeof(float)));

            glViewport(wWidth / 3, 0, wWidth / 3, wHeight);
            glDrawArrays(GL_TRIANGLE_FAN, 0, sizeof(circle) / (2 * sizeof(float)));

            glViewport(2 * wWidth / 3, 0, wWidth / 3, wHeight);
            glDrawArrays(GL_TRIANGLE_FAN, 0, sizeof(circle) / (2 * sizeof(float)));

            glUseProgram(0);
            glBindVertexArray(0);
            glViewport(0, 0, wWidth, wHeight);
        }
        #pragma endregion

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

void drawBackgroundOrInfoTexture(unsigned int shader, unsigned int VAO, unsigned int texture)
{
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glUseProgram(shader);
    glBindVertexArray(VAO);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture);

    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void*)(0 * sizeof(unsigned int)));

    glBindTexture(GL_TEXTURE_2D, 0);
    glDisable(GL_BLEND);
    glUseProgram(0);
    glBindVertexArray(0);
}
void drawFrames(unsigned int VAO, unsigned int wWidth, unsigned int wHeight, int polygonMode) 
{
    glBindVertexArray(VAO);
    glPolygonMode(GL_FRONT_AND_BACK, polygonMode);
    glLineWidth(3.0f);
    glPointSize(10.0f);

    glViewport(0, 0, wWidth / 3, wHeight);
    glDrawElements(GL_TRIANGLES, 24, GL_UNSIGNED_INT, (void*)(0 * sizeof(unsigned int)));

    glViewport(wWidth / 3, 0, wWidth / 3, wHeight);
    glDrawElements(GL_TRIANGLES, 24, GL_UNSIGNED_INT, (void*)(0 * sizeof(unsigned int)));

    glViewport(2 * wWidth / 3, 0, wWidth / 3, wHeight);
    glDrawElements(GL_TRIANGLES, 24, GL_UNSIGNED_INT, (void*)(0 * sizeof(unsigned int)));

    glViewport(0, 0, wWidth, wHeight);
    glBindVertexArray(0);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glLineWidth(1.0f);
    glPointSize(1.0f);
}
void drawImages(unsigned int VAO, unsigned int wWidth, unsigned int wHeight, unsigned int shader, unsigned int textures[])
{
    glUseProgram(shader);
    glBindVertexArray(VAO);
    glActiveTexture(GL_TEXTURE1);

    glBindTexture(GL_TEXTURE_2D, textures[0]);
    glViewport(0, 0, wWidth / 3, wHeight);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void*)(0 * sizeof(unsigned int)));
    glBindTexture(GL_TEXTURE_2D, 0);

    glBindTexture(GL_TEXTURE_2D, textures[1]);
    glViewport(wWidth / 3, 0, wWidth / 3, wHeight);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void*)(0 * sizeof(unsigned int)));
    glBindTexture(GL_TEXTURE_2D, 0);

    glBindTexture(GL_TEXTURE_2D, textures[2]);
    glViewport(2 * wWidth / 3, 0, wWidth / 3, wHeight);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void*)(0 * sizeof(unsigned int)));
    glBindTexture(GL_TEXTURE_2D, 0);

    glUseProgram(0);
    glBindVertexArray(0);
    glViewport(0, 0, wWidth, wHeight);
}
