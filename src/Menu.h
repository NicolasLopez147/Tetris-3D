#ifndef MENU_H
#define MENU_H

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "GameState.h"
#include "TextShader.h"
#include <vector>

class Menu {
public:
    Menu(GLFWwindow* window, GameState& state)
        : window(window), state(state), textShader() {
        projection = glm::ortho(0.0f, 1600.0f, 0.0f, 1200.0f);
    }

    void displayMenu() {
        textShader.use();
        textShader.setMat4("projection", projection);

        int windowWidth, windowHeight;
        glfwGetWindowSize(window, &windowWidth, &windowHeight);

        double mouseX, mouseY;
        glfwGetCursorPos(window, &mouseX, &mouseY);
        mouseY = windowHeight - mouseY;

        // Calculate button positions
        float startX = (windowWidth / 2) - 120.0f;
        float startY = windowHeight - 600.0f;
        float howToPlayX = startX - 50.0f;
        float howToPlayY = startY - 80.0f;
        float quitX = startX;
        float quitY = howToPlayY - 80.0f;

        // Check hover states
        bool startHovered = isMouseOverButton(mouseX, mouseY, startX, startY, buttonWidth, buttonHeight);
        bool howToPlayHovered = isMouseOverButton(mouseX, mouseY, howToPlayX, howToPlayY, buttonWidth, buttonHeight);
        bool quitHovered = isMouseOverButton(mouseX, mouseY, quitX, quitY, buttonWidth, buttonHeight);

        // Draw elements
        drawTitle(windowWidth, windowHeight);
        drawButton(startX, startY, buttonWidth, buttonHeight, "START", glm::vec3(1.0f, 1.0f, 1.0f), startHovered);
        drawButton(howToPlayX, howToPlayY, buttonWidth, buttonHeight, "HOW TO PLAY", glm::vec3(1.0f, 1.0f, 1.0f), howToPlayHovered);
        drawButton(quitX, quitY, buttonWidth, buttonHeight, "QUIT", glm::vec3(1.0f, 1.0f, 1.0f), quitHovered);
        drawFooter(windowWidth);

        // Handle click events
        if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
            if (startHovered) {
                state = Playing;
            }
            if (howToPlayHovered) {
                state = HowToPlay;
            }
            if (quitHovered) {
                textShader.cleanup();
                glfwSetWindowShouldClose(window, true);
            }
        }
    }

private:
    GLFWwindow* window;
    GameState& state;
    TextShader textShader;
    glm::mat4 projection;
    float fallingOffset = 0.0f;
    const float buttonWidth = 200.0f, buttonHeight = 50.0f;

    void drawTitle(float windowWidth, float windowHeight) {
        float size = 3.5f;
        float titleX = (windowWidth / 2) - 350.0f;
        float titleY = windowHeight - 120.0f;
        std::vector<glm::vec3> colors = {
            glm::vec3(1.0f, 0.0f, 0.0f),
            glm::vec3(1.0f, 0.5f, 0.0f),
            glm::vec3(1.0f, 1.0f, 0.0f),
            glm::vec3(0.0f, 1.0f, 0.0f),
            glm::vec3(0.0f, 1.0f, 1.0f),
            glm::vec3(1.0f, 0.0f, 1.0f)
        };

        std::string title = "TETRIS";
        for (size_t i = 0; i < title.size(); ++i) {
            float letterX = titleX + i * 120.0f;
            float letterY = (title[i] == 'S') ? titleY - fallingOffset : titleY;
            textShader.renderText(std::string(1, title[i]), letterX, letterY, size, colors[i]);

            if (title[i] == 'S') {
                fallingOffset = (fallingOffset > 100.0f) ? 0.0f : fallingOffset + 0.5f;
            }
        }
        textShader.renderText("3D", titleX + 270.0f, titleY - 190.0f, size, glm::vec3(1.0f, 1.0f, 1.0f));
    }

    void drawFooter(float windowWidth) {
        float footerX = (windowWidth / 2) - 180.0f;
        float footerY = 20.0f;
        textShader.renderText("Copyright: Nicolas LOPEZ and Nicolas RINCON", footerX, footerY, 0.4f, glm::vec3(1.0f, 1.0f, 1.0f));
    }

    bool isMouseOverButton(double mouseX, double mouseY, float buttonX, float buttonY, float buttonWidth, float buttonHeight) {
        return mouseX >= buttonX && mouseX <= buttonX + buttonWidth && mouseY >= buttonY && mouseY <= buttonY + buttonHeight;
    }

    void drawButton(float x, float y, float width, float height, const std::string& text, glm::vec3 textColor, bool isHovered) {
        glBegin(GL_QUADS);
        glColor3f(0.2f, 0.2f, 0.2f);
        glVertex2f(x, y);
        glVertex2f(x + width, y);
        glVertex2f(x + width, y + height);
        glVertex2f(x, y + height);
        glEnd();

        glm::vec3 finalTextColor = isHovered ? glm::vec3(1.0f, 0.8f, 0.0f) : textColor;
        float textX = x + (width / 2) - (text.size() * 10.0f) / 2;
        float textY = y + height+(height/2);
        textShader.renderText(text, textX, textY, 1.0f, finalTextColor);
    }
};

#endif