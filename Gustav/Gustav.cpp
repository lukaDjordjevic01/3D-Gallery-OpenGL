
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
void drawBackgroundTexture(unsigned int shader, unsigned int VAO, unsigned int texture);
void drawFrames(unsigned int VAO, unsigned int wWidth, unsigned int wHeight, int polygonMode);

const int NUMBER_OF_BUFFERS = 3;

const int wallTextureIndex = 0;
const int frameShapeIndex = 1;
const int frameImageTextureIndex = 2;


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
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
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

    int polygonMode = GL_FILL;

    while (!glfwWindowShouldClose(window)) 
    {
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        {
            glfwSetWindowShouldClose(window, GL_TRUE);
        }
        glClear(GL_COLOR_BUFFER_BIT);

        glViewport(0, 0, wWidth, wHeight);
        drawBackgroundTexture(wallTextureShader, VAO[wallTextureIndex], wallTexture);

        drawFrames(VAO[frameShapeIndex], wWidth, wHeight, polygonMode);

        #pragma region ImagesDrawing
        glUseProgram(frameImageTextureShader);
        glBindVertexArray(VAO[frameImageTextureIndex]);
        glActiveTexture(GL_TEXTURE1);

        glBindTexture(GL_TEXTURE_2D, firstImageTexture);
        glViewport(0, 0, wWidth / 3, wHeight);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void*)(0 * sizeof(unsigned int)));
        glBindTexture(GL_TEXTURE_2D, 0);

        glBindTexture(GL_TEXTURE_2D, secondImageTexture);
        glViewport(wWidth / 3, 0, wWidth / 3, wHeight);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void*)(0 * sizeof(unsigned int)));
        glBindTexture(GL_TEXTURE_2D, 0);

        glBindTexture(GL_TEXTURE_2D, thirdImageTexture);
        glViewport(2 * wWidth / 3, 0, wWidth / 3, wHeight);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void*)(0 * sizeof(unsigned int)));
        glBindTexture(GL_TEXTURE_2D, 0);
        
        glUseProgram(0);
        glBindVertexArray(0);
        glViewport(0, 0, wWidth, wHeight);
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

void drawBackgroundTexture(unsigned int shader, unsigned int VAO, unsigned int texture)
{
    glUseProgram(shader);
    glBindVertexArray(VAO);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture);

    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void*)(0 * sizeof(unsigned int)));

    glBindTexture(GL_TEXTURE_2D, 0);
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
