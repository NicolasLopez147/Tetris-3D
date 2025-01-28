#include "src/Renderer.h"
#include "src/InputHandler.h"
#include "src/Menu.h"
#include "src/HowToPlayScreen.h"

InputHandler inputHandler;

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (action == GLFW_PRESS || action == GLFW_REPEAT) {
        Game* game = reinterpret_cast<Game*>(glfwGetWindowUserPointer(window));
        if (game) {
            inputHandler.handleInput(key, *game);
        }
    }
}


// Callback function to adjust the OpenGL viewport when the window is resized
void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

int main() {
    // Initialize GLFW
    if (!glfwInit()) {
        std::cerr << "Error: Failed to initialize GLFW" << std::endl;
        return -1;
    }

    // Create a GLFW window
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(1600, 1200, "Tetris 3D", nullptr, nullptr);
    if (!window) {
        std::cerr << "Error: Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    // Initialize GLEW
    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK) {
        std::cerr << "Error: Failed to initialize GLEW" << std::endl;
        glfwDestroyWindow(window);
        glfwTerminate();
        return -1;
    }

    // Configure OpenGL viewport and settings
    int viewportWidth, viewportHeight;
    glfwGetFramebufferSize(window, &viewportWidth, &viewportHeight);
    glViewport(0, 0, viewportWidth, viewportHeight);
    glEnable(GL_DEPTH_TEST);

    // Set the initial game state
    GameState state = MenuPrincipal;
    Game game(4, 16, 4);
    Renderer renderer;
    Menu menu(window, state);
    HowToPlayScreen howToPlayScreen(window, state);

    glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)viewportWidth / viewportHeight, 0.1f, 100.0f);
    glm::mat4 view = glm::lookAt(glm::vec3(15, 25, 15), glm::vec3(5, 10, 5), glm::vec3(0, 1, 0));

    // Main loop
    while (!glfwWindowShouldClose(window)) {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glfwSetWindowUserPointer(window, &game);
                glfwSetKeyCallback(window, key_callback);
        
        switch (state) {
            case MenuPrincipal:
                menu.displayMenu(); // display the menu
                break;
            case Playing:
                if(game.getIsRunning()){
                    game.update(0.016f); // Start the game
                    renderer.renderGame(game, projection, view);
                } else {
                    state = GameOver;
                }
                break;
            case HowToPlay:
                howToPlayScreen.display();
                break;
            case GameOver:
                game.start();
                // Optionally implement a game over screen if needed
                state = MenuPrincipal; // Ensure it loops back to the menu
                break;
        }

        glfwSwapBuffers(window);
        glfwPollEvents();
    }


    // Clean up
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
