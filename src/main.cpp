#include <SDL2/SDL.h>
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "fragment.h"
#include "uniform.h"

#include "print.h"
#include "color.h"
#include "shaders.h"

const int WINDOW_WIDTH = 800;
const int WINDOW_HEIGHT = 800;

SDL_Renderer* renderer;

Uniform uniform;


// Declare a global clearColor of type Color
Color clearColor = {0, 0, 0}; // Initially set to black

// Declare a global currentColor of type Color
Color currentColor = {255, 255, 255}; // Initially set to white

// Function to clear the framebuffer with the clearColor
void clear() {
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
    SDL_RenderClear(renderer);
}

// Function to set a specific pixel in the framebuffer to the currentColor
void point(int x, int y, Color color) {
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
    SDL_RenderDrawPoint(renderer, x, y);
}


std::vector<Fragment> line(glm::vec3 A, glm::vec3 B) {
    int x1 = A.x;
    int y1 = A.y;
    int x2 = B.x;
    int y2 = B.y;
    int dx = abs(x2 - x1), sx = x1 < x2 ? 1 : -1;
    int dy = -abs(y2 - y1), sy = y1 < y2 ? 1 : -1;
    int err = dx + dy, e2; /* error value e_xy */


    std::vector<Fragment> lineFragments;

    while (true) { /* loop */
        lineFragments.push_back(Fragment{glm::ivec2(x1, y1)});
        if (x1 == x2 && y1 == y2) break;
        e2 = 2 * err;
        if (e2 >= dy) { /* e_xy + e_x > 0 */
            err += dy;
            x1 += sx;
        }
        if (e2 <= dx) { /* e_xy + e_y < 0 */
            err += dx;
            y1 += sy;
        }
    }

    return lineFragments;
}

std::vector<Fragment> triangle(glm::vec3 A, glm::vec3 B, glm::vec3 C) {
    std::vector<Fragment> triangleFragments;
    
    std::vector<Fragment> line1 = line(A, B);
    std::vector<Fragment> line2 = line(B, C);
    std::vector<Fragment> line3 = line(C, A);

    triangleFragments.insert(
        triangleFragments.end(),
        line1.begin(),
        line1.end()
    );

    triangleFragments.insert(
        triangleFragments.end(),
        line2.begin(),
        line2.end()
    );

    triangleFragments.insert(
        triangleFragments.end(),
        line3.begin(),
        line3.end()
    );

    return triangleFragments;
}

std::vector<std::vector<glm::vec3>> primitiveAssembly(
    const std::vector<glm::vec3>& transformedVertices
) {
    std::vector<std::vector<glm::vec3>> groupedVertices;

    for (int i = 0; i < transformedVertices.size(); i += 3) {
        std::vector<glm::vec3> triangle;
        triangle.push_back(transformedVertices[i]);
        triangle.push_back(transformedVertices[i+1]);
        triangle.push_back(transformedVertices[i+2]);
        
        groupedVertices.push_back(triangle);
    }

    return groupedVertices;
}


void render(std::vector<glm::vec3> vertices) {
    // 1. Vertex Shader
    // vertex -> trasnformedVertices
    
    std::vector<glm::vec3> transformedVertices;
    for(glm::vec3 vertex : vertices) {
        glm::vec3 transformedVertex = vertexShader(vertex, uniform);
        transformedVertices.push_back(transformedVertex);
    }


    // 2. Primitive Assembly
    // transformedVertices -> triangles
    std::vector<std::vector<glm::vec3>> triangles = primitiveAssembly(transformedVertices);

    // 3. Rasterize
    // triangles -> Fragments
    std::vector<Fragment> fragments;
    for (const std::vector<glm::vec3>& triangleVertices : triangles) {
        std::vector<Fragment> rasterizedTriangle = triangle(
            triangleVertices[0],
            triangleVertices[1],
            triangleVertices[2]
        );
        
        fragments.insert(
            fragments.end(),
            rasterizedTriangle.begin(),
            rasterizedTriangle.end()
        );
    }

    // 4. Fragment Shader
    // Fragments -> colors

    for (Fragment fragment : fragments) {
        Color fragColor = fragmentShader(fragment);
        point(fragment.position.x, fragment.position.y, fragColor);
    }
}


glm::mat4 createModelMatrix() {
    return glm::mat4(1);
}

glm::mat4 createViewMatrix() {
    return glm::lookAt(
        // donde esta
        glm::vec3(0, 0, -5),
        // hacia adonde mira
        glm::vec3(0, 0, 0),
        // arriba
        glm::vec3(0, 1, 0)
    );
}

glm::mat4 createProjectionMatrix() {
  float fovInDegrees = 45.0f;
  float aspectRatio = WINDOW_WIDTH / WINDOW_HEIGHT;
  float nearClip = 0.1f;
  float farClip = 100.0f;

  return glm::perspective(glm::radians(fovInDegrees), aspectRatio, nearClip, farClip);
}

glm::mat4 createViewportMatrix() {
    glm::mat4 viewport = glm::mat4(1.0f);

    // Scale
    viewport = glm::scale(viewport, glm::vec3(WINDOW_WIDTH / 2.0f, WINDOW_HEIGHT / 2.0f, 0.5f));

    // Translate
    viewport = glm::translate(viewport, glm::vec3(1.0f, 1.0f, 0.5f));

    return viewport;
}


int main() {
    SDL_Init(SDL_INIT_EVERYTHING);

    SDL_Window* window = SDL_CreateWindow("life", 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, 0);

    renderer = SDL_CreateRenderer(
        window,
        -1,
        SDL_RENDERER_ACCELERATED
    );

    bool running = true;
    SDL_Event event;

    std::vector<glm::vec3> vertices = {
        {0.0f, 1.0f, 0.0f},
        {-0.87f, -0.5f, 0.0f},
        {0.87f,  -0.5f, 0.0f}
    };

    while (running) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = false;
            }
            if (event. type == SDL_KEYDOWN) {
                switch (event.key.keysym.sym) {
                    case SDLK_UP:
                        break;
                    case SDLK_DOWN:
                        break;
                    case SDLK_LEFT:
                        break;
                    case SDLK_RIGHT:
                        break;
                    case SDLK_s:
                        break;
                    case SDLK_a:
                        break;
                }
            }
        }

        uniform.model = createModelMatrix();
        uniform.view = createViewMatrix();
        uniform.projection = createProjectionMatrix();
        uniform.viewport = createViewportMatrix();


        clear();



        // Call our render function
        render(vertices);
        // point(10, 10, Color{255, 255, 255});




        // Present the frame buffer to the screen
        SDL_RenderPresent(renderer);

        // Delay to limit the frame rate
        SDL_Delay(1000 / 60);
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}