#ifndef TEXTSHADER_H
#define TEXTSHADER_H

#include "Shader.h"
#include <glm/glm.hpp>
#include <ft2build.h>
#include FT_FREETYPE_H
#include <map>
#include <string>
#include <iostream>

class TextShader : public Shader {
private:
    GLuint VAO, VBO;

    // Struct to store character data
    struct Character {
        GLuint TextureID;   // Texture ID for the glyph
        glm::ivec2 Size;    // Size of the glyph
        glm::ivec2 Bearing; // Offset from the baseline
        GLuint Advance;     // Advance to the next character
    };

    std::map<char, Character> Characters;

    // Vertex Shader source
    const char* vertexShaderSource = R"(
    #version 330 core
    layout (location = 0) in vec4 vertex; // <vec2 pos, vec2 tex>
    out vec2 TexCoords;

    uniform mat4 projection;

    void main()
    {
        gl_Position = projection * vec4(vertex.xy, 0.0, 1.0);
        TexCoords = vertex.zw;
    }
    )";

    // Fragment Shader source
    const char* fragmentShaderSource = R"(
    #version 330 core
    in vec2 TexCoords;
    out vec4 color;

    uniform sampler2D text;
    uniform vec3 textColor;

    void main()
    {
        vec4 sampled = vec4(1.0, 1.0, 1.0, texture(text, TexCoords).r);
        color = vec4(textColor, 1.0) * sampled;
    }
    )";

    void initializeFont(const char* fontPath) {
        FT_Library ft;
        if (FT_Init_FreeType(&ft)) {
            std::cerr << "[Error] Could not initialize FreeType Library!" << std::endl;
            return;
        }

        FT_Face face;
        if (FT_New_Face(ft, fontPath, 0, &face)) {
            std::cerr << "[Error] Failed to load font: " << fontPath << std::endl;
            return;
        }

        FT_Set_Pixel_Sizes(face, 0, 48);
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

        for (unsigned char c = 0; c < 128; c++) {
            if (FT_Load_Char(face, c, FT_LOAD_RENDER)) {
                std::cerr << "[Error] Failed to load character: " << c << std::endl;
                continue;
            }

            if(Characters.find(c) != Characters.end()){
                continue;
            }

            GLuint texture;
            glGenTextures(1, &texture);
            glBindTexture(GL_TEXTURE_2D, texture);
            glTexImage2D(
                GL_TEXTURE_2D,
                0,
                GL_RED,
                face->glyph->bitmap.width,
                face->glyph->bitmap.rows,
                0,
                GL_RED,
                GL_UNSIGNED_BYTE,
                face->glyph->bitmap.buffer
            );

            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

            Character character = {
                texture,
                glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
                glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
                static_cast<GLuint>(face->glyph->advance.x)
            };
            Characters.insert(std::pair<char, Character>(c, character));
        }

        FT_Done_Face(face);
        FT_Done_FreeType(ft);

        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
    }

public:
    TextShader() {
        ID = createShaderProgram(vertexShaderSource, fragmentShaderSource);
        initializeFont("./utils/Super_cartoon.ttf");
    }

    void cleanup() {
        for (auto& c : Characters) {
            glDeleteTextures(1, &c.second.TextureID);
        }
        glDeleteVertexArrays(1, &VAO);
        glDeleteBuffers(1, &VBO);
        glDeleteProgram(ID);
    }

    void use() {
        glUseProgram(ID);
    }

    void setVec3(const std::string& name, const glm::vec3& value) const {
        glUniform3fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
    }

    void setMat4(const std::string& name, const glm::mat4& mat) const {
        glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
    }

    void renderText(const std::string& text, float x, float y, float scale, glm::vec3 color) {
        use();
        setVec3("textColor", color);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glActiveTexture(GL_TEXTURE0);
        glBindVertexArray(VAO);

        for (char c : text) {
            if (Characters.find(c) == Characters.end()) {
                std::cerr << "[Warning] Character '" << c << "' not found! Skipping..." << std::endl;
                continue;
            }

            Character ch = Characters[c];

            float xpos = x + ch.Bearing.x * scale;
            float ypos = y - (ch.Size.y - ch.Bearing.y) * scale;

            float w = ch.Size.x * scale;
            float h = ch.Size.y * scale;

            float vertices[6][4] = {
                { xpos,     ypos + h,   0.0f, 0.0f },
                { xpos,     ypos,       0.0f, 1.0f },
                { xpos + w, ypos,       1.0f, 1.0f },

                { xpos,     ypos + h,   0.0f, 0.0f },
                { xpos + w, ypos,       1.0f, 1.0f },
                { xpos + w, ypos + h,   1.0f, 0.0f }
            };

            glBindTexture(GL_TEXTURE_2D, ch.TextureID);
            glBindBuffer(GL_ARRAY_BUFFER, VBO);
            glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
            glBindBuffer(GL_ARRAY_BUFFER, 0);

            glDrawArrays(GL_TRIANGLES, 0, 6);

            x += (ch.Advance >> 6) * scale; // Bitshift by 6 to convert from 1/64th pixels to pixels
        }

        glBindVertexArray(0);
        glBindTexture(GL_TEXTURE_2D, 0);
    }
    
    ~TextShader(){
        cleanup();
    }
};

#endif // TEXTSHADER_H
