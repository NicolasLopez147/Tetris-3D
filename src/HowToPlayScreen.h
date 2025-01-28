#ifndef HOW_TO_PLAY_SCREEN_H
#define HOW_TO_PLAY_SCREEN_H

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "GameState.h"
#include "TextShader.h"
#include <vector>

class HowToPlayScreen {
public:
    HowToPlayScreen(GLFWwindow* window, GameState& state)
        : window(window), state(state), textShader() {
        projection = glm::ortho(0.0f, 1600.0f, 0.0f, 1200.0f);
        textShader.use();
        textShader.setMat4("projection", projection);
    }

    void display() {
        int windowWidth, windowHeight;
        glfwGetWindowSize(window, &windowWidth, &windowHeight);

        double mouseX, mouseY;
        glfwGetCursorPos(window, &mouseX, &mouseY);
        mouseY = windowHeight - mouseY; // Adjust for OpenGL coordinates

        // Check hover state for the "RETURN TO MENU" button
        bool returnHovered = isMouseOverButton(mouseX, mouseY, windowWidth / 2 - 100.0f, 50.0f, 200.0f, 50.0f);

        // Draw instructions
        drawInstructions(windowWidth, windowHeight, returnHovered);

        // Handle click on "RETURN TO MENU"
        if (returnHovered && glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
            state = MenuPrincipal; // Transition to the main menu
        }
    }

private:
    GLFWwindow* window;
    GameState& state;
    TextShader textShader;
    glm::mat4 projection;

    void drawInstructions(float windowWidth, float windowHeight, bool returnHovered) {
        float introX = windowWidth / 2 - 200.0f;
        float introY = windowHeight - 100.0f;
        textShader.renderText("Welcome to Tetris 3D!", introX, introY, 0.9f, glm::vec3(1.0f, 1.0f, 1.0f));
        textShader.renderText("Use the following keys to play the game:", introX, introY - 30.0f, 0.7f, glm::vec3(1.0f, 1.0f, 1.0f));

        std::vector<std::pair<std::string, std::vector<std::string>>> groupedInstructions = {
            {"Movement Controls:", {
                "Key S: Move down",
                "Key A: Move left X-axis",
                "Key D: Move right X-axis",
                "Key Q: Move left Z-axis",
                "Key E: Rotate right Z-axis",
                "Key  SPACE: Move to projected position",
            }},
            {"Rotation Controls:", {
                "Key Z: Rotate around Z-axis",
                "Key X: Rotate around X-axis",
                "Key C: Rotate around Y-axis"
            }}
        };

        float x = windowWidth / 2 - 200.0f;
        float y = introY - 80.0f;
        float groupSpacing = 35.0f;
        float lineSpacing = 30.0f;

        for (const auto& group : groupedInstructions) {
            textShader.renderText(group.first, x, y, 0.7f, glm::vec3(1.0f, 1.0f, 0.0f));
            y -= groupSpacing;

            for (const auto& instruction : group.second) {
                textShader.renderText(instruction, x + 20.0f, y, 0.6f, glm::vec3(1.0f, 1.0f, 1.0f));
                y -= lineSpacing;
            }

            y -= groupSpacing;
        }

        float buttonX = windowWidth / 2 - 100.0f;
        float buttonY = 50.0f;
        glm::vec3 buttonColor = returnHovered ? glm::vec3(1.0f, 0.8f, 0.0f) : glm::vec3(1.0f, 1.0f, 0.0f);
        textShader.renderText("RETURN TO MENU", buttonX, buttonY, 0.8f, buttonColor);
    }

    bool isMouseOverButton(double mouseX, double mouseY, float buttonX, float buttonY, float buttonWidth, float buttonHeight) {
        return mouseX >= buttonX && mouseX <= buttonX + buttonWidth && mouseY >= buttonY && mouseY <= buttonY + buttonHeight;
    }
};

#endif