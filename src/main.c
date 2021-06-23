#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <GLFW/glfw3.h>
//#include <glmc.h>
#include <unistd.h>
#include <stdio.h>
#include <time.h>
#include "stb_image.h"

const char* vertexSource =
"#version 150\n\
in vec2 position;\n\
in vec3 vertexColor;\n\
out vec3 VertexColor;\n\
void main()\n\
{\n\
    VertexColor = vertexColor;\n\
    gl_Position = vec4(position, 0.0, 1.0);\n\
}\n";

//uniform vec3 triangleColor;\n
const char* fragmentSource = "#version 150\n\
in vec3 VertexColor;\n\
out vec4 outColor;\n\
void main()\n\
{\n\
    outColor = vec4(VertexColor, 1.0);\n\
}\n";

float vertices[] = {
    0.0f,  1.0f, 1.0f, 0.0f, 0.0f, // Vertex 1 (X, Y)
    1.0f, 0.0f, 0.0f, 1.0f, 0.0f, // Vertex 2 (X, Y)
    -1.0f, 0.0f, 0.0f, 0.0f, 1.0f, // Vertex 3 (X, Y)
    0.0f, -0.5f, 0.0f, 0.0f, 1.0f // Vertex 4 (X, Y)
};

GLuint elements[] = {
    0, 1, 2,
    1, 3, 2
};

int main() {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
    GLFWwindow* window = glfwCreateWindow(800, 600, "OpenGL", NULL, NULL); // Windowed
//GLFWwindow* window = //    glfwCreateWindow(800, 600, "OpenGL", glfwGetPrimaryMonitor(), nullptr); // Fullscreen
    glfwMakeContextCurrent(window);
    GLint uniColor;
    {
        glewExperimental = GL_TRUE;
        glewInit();

        GLuint vao;
        glGenVertexArrays(1, &vao);
        glBindVertexArray(vao);

        GLuint vbo;
        glGenBuffers(1, &vbo); // Generate 1 buffer
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

        GLuint ebo; 
        glGenBuffers(1, &ebo),
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(elements), elements, GL_STATIC_DRAW);
        
        GLint status;
        GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
        printf("compiling source %s", vertexSource);
        glShaderSource(vertexShader, 1, &vertexSource, NULL);
        printf("source OK\n");
        glCompileShader(vertexShader);
        printf("comp OK\n");

        glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &status);
        if (status == GL_TRUE) {
            printf("vertex shader OK\n");
        } else {
            printf("vertex shader BAD\n");
         char buffer[512];
         glGetShaderInfoLog(vertexShader, 512, NULL, buffer); 
         printf ("vertex shader compilation error: %s", buffer);
        }

        GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fragmentShader, 1, &fragmentSource, NULL);
        glCompileShader(fragmentShader);

        glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &status);
        if (status == GL_TRUE) {
            printf("fragment shader OK");
        } else {
         char buffer[512];
         glGetShaderInfoLog(fragmentShader, 512, NULL, buffer); 
         printf ("fragment shader compilation error: %s", buffer);
        }

        GLuint shaderProgram = glCreateProgram();
        glAttachShader(shaderProgram, vertexShader);
        glAttachShader(shaderProgram, fragmentShader);
        glBindFragDataLocation(shaderProgram, 0, "outColor");
        glLinkProgram(shaderProgram);
        glUseProgram(shaderProgram);

        GLint posAttrib = glGetAttribLocation(shaderProgram, "position");
        glEnableVertexAttribArray(posAttrib);
        glVertexAttribPointer(posAttrib, 2, GL_FLOAT, GL_FALSE, 5*sizeof(float), 0);

        GLint colAttrib = glGetAttribLocation(shaderProgram, "vertexColor");
        glEnableVertexAttribArray(colAttrib);
        glVertexAttribPointer(colAttrib, 3, GL_FLOAT, GL_FALSE, 5*sizeof(float), (void*)(2*sizeof(float)));


        GLuint tex;
        glGenTextures(1, &tex);
        glBindTexture(GL_TEXTURE_2D, tex);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        float color[] = { 1.0f, 0.0f, 0.0f, 1.0f };
        glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, color);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST_MIPMAP_NEAREST);
// load and generate the texture
        int width, height, nrChannels;
        unsigned char *data = stbi_load("container.jpg", &width, &height, &nrChannels, 0);
        if (data)
        {
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
            glGenerateMipmap(GL_TEXTURE_2D);
        }
        else
        {
           printf("Failed to load texture");
        }
        stbi_image_free(data);
        //uint16_t width;
        //uint16_t height;
        //void * pixeldata;
        //pixeldata = load_rgba();
        //glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 2, 2, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels); 
        //free(pixeldata)
        //uniColor = glGetUniformLocation(shaderProgram, "triangleColor");
    }


    while(!glfwWindowShouldClose(window))
    {
        float redness = (float)(time(NULL) % 5) /5.0f;
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        //glUniform3f(uniColor, redness, 1.0f, 1.0f);
        //glDrawArrays(GL_TRIANGLES, 0, 3);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        glfwSwapBuffers(window);
        glfwPollEvents();
    }


    /*glDeleteProgram(shaderProgram);
    glDeleteShader(fragmentShader);
    glDeleteShader(vertexShader);

    glDeleteBuffers(1, &vbo);
    glDeleteVertexArrays(1, &vao); */

    glfwTerminate();
}
