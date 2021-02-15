#define GL_SILENCE_DEPRECATION

#ifdef _WINDOWS
#include <GL/glew.h>
#endif

#define GL_GLEXT_PROTOTYPES 1
#include <SDL.h>
#include <SDL_opengl.h>
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "ShaderProgram.h"

//Standard C++ Stuff
#include <math.h>

//Texture Stuff Below
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

SDL_Window* displayWindow;
bool gameIsRunning = true;

//Sun/Face Variables
float earth_x = 0;
float earth_y = 0;
float earth_rotate = 0;

//Superhero Variable
float hero_x = 2;
float hero_y = 0;
float hero_rotate = 0;

//Texture
GLuint playerTextureID, earthTextureID, heroTextureID;

ShaderProgram program;
glm::mat4 viewMatrix, projectionMatrix, earthMatrix, heroMatrix;


//Load Texture Function - Provided in Class
GLuint LoadTexture(const char* filePath) {
    int w, h, n;
    unsigned char* image = stbi_load(filePath, &w, &h, &n, STBI_rgb_alpha);

    if (image == NULL) {
        std::cout << "Unable to load image. Make sure the path is correct\n";
        assert(false);
    }

    //Use 64x64

    GLuint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    stbi_image_free(image);
    return textureID;
}

void Initialize() {
    SDL_Init(SDL_INIT_VIDEO);
    displayWindow = SDL_CreateWindow("Isaiah Mon Desir P1", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 480, SDL_WINDOW_OPENGL);
    SDL_GLContext context = SDL_GL_CreateContext(displayWindow);
    SDL_GL_MakeCurrent(displayWindow, context);

#ifdef _WINDOWS
    glewInit();
#endif

    glViewport(0, 0, 640, 480);

    program.Load("shaders/vertex_textured.glsl", "shaders/fragment_textured.glsl");

    viewMatrix = glm::mat4(1.0f);
    earthMatrix = glm::mat4(1.0f);
    heroMatrix = glm::mat4(1.0f);

    projectionMatrix = glm::ortho(-5.0f, 5.0f, -3.75f, 3.75f, -1.0f, 1.0f);

    program.SetProjectionMatrix(projectionMatrix);
    program.SetViewMatrix(viewMatrix);
    //program.SetColor(1.0f, 0.0f, 0.0f, 1.0f);

    glUseProgram(program.programID);

    glClearColor(0.2f, 0.5f, 0.7f, 1.0f);

    glEnable(GL_BLEND);
    // Good setting for transparency
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    earthTextureID = LoadTexture("earth.png");
    heroTextureID = LoadTexture("fly.png");
}

void ProcessInput() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT || event.type == SDL_WINDOWEVENT_CLOSE) {
            gameIsRunning = false;
        }
    }
}

float lastTicks = 0.0f;

void Update() { 

    //Implementing deltaTime
    float ticks = (float)SDL_GetTicks() / 1000.0f;
    float deltaTime = ticks - lastTicks;
    lastTicks = ticks;

    earth_rotate += -90.0f * deltaTime;
    hero_x += .05f * deltaTime;
    hero_rotate += 45.0f * deltaTime;

    earthMatrix = glm::mat4(1.0f);
    earthMatrix = glm::rotate(earthMatrix, glm::radians(earth_rotate), glm::vec3(0.0f, 0.0f, 1.0f));
    earthMatrix = glm::scale(earthMatrix, glm::vec3(2.0f, 2.00f, 1.0f)); //Scale increase by 1%. Don't change z

    heroMatrix = glm::mat4(1.0f);
    heroMatrix = glm::rotate(heroMatrix, glm::radians(hero_rotate), glm::vec3(0.0f, 0.0f, 1.0f));
    heroMatrix = glm::translate(heroMatrix, glm::vec3(hero_x, 1.0f, 1.0f));
    
}

void Drawearth() {
    program.SetModelMatrix(earthMatrix);
    glBindTexture(GL_TEXTURE_2D, earthTextureID);
    glDrawArrays(GL_TRIANGLES, 0, 6);
}
void DrawHero() {
    program.SetModelMatrix(heroMatrix);
    glBindTexture(GL_TEXTURE_2D, heroTextureID);
    glDrawArrays(GL_TRIANGLES, 0, 6);
}

void Render() {
    
    float vertices[] = { -0.5, -0.5, 0.5, -0.5, 0.5, 0.5, -0.5, -0.5, 0.5, 0.5, -0.5, 0.5 };
    float texCoords[] = { 0.0, 1.0, 1.0, 1.0, 1.0, 0.0, 0.0, 1.0, 1.0, 0.0, 0.0, 0.0 };

    glClear(GL_COLOR_BUFFER_BIT);
    
    glVertexAttribPointer(program.positionAttribute, 2, GL_FLOAT, false, 0, vertices);
    glEnableVertexAttribArray(program.positionAttribute);

    glVertexAttribPointer(program.texCoordAttribute, 2, GL_FLOAT, false, 0, texCoords);
    glEnableVertexAttribArray(program.texCoordAttribute);
    
    Drawearth();
    DrawHero();

    glDisableVertexAttribArray(program.positionAttribute);
    glDisableVertexAttribArray(program.texCoordAttribute);

    SDL_GL_SwapWindow(displayWindow);
}

void Shutdown() {
    SDL_Quit();
}

int main(int argc, char* argv[]) {
    Initialize();

    while (gameIsRunning) {
        ProcessInput();
        Update();
        Render();
    }

    Shutdown();
    return 0;
}
