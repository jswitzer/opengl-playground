#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <string.h>
#include <time.h>
#include "log.h"
#include "render.h"


typedef struct rendering_config_s {
    char configured;
    char fullscreen;
    char windowtitle[256];
    int xres;
    int yres;
} rendering_config_t;

GLFWwindow * g_window = NULL;
rendering_config_t g_renderconf = {0};

int g_mode = 0;

#define RENDER_MODE_NONE 0

#define RENDER_MODE_CTRIANGLE 1
void render_ctriangle_loop();
void render_ctriangle_terminate();
typedef struct ctriangle_state_s {
    GLuint vertexShader;
    GLuint fragmentShader;
    GLuint shaderProgram;
    GLuint vbo;
    GLuint vao;
} ctriangle_state_t;
ctriangle_state_t g_ctriangle = {0};


int render_configure() {
    strcpy(g_renderconf.windowtitle, "OpenGL");
    g_renderconf.xres = 800;
    g_renderconf.yres = 600;
    g_renderconf.configured = 1;
    g_renderconf.fullscreen = 0;
    return 0;
}

void GLFW_error(int error, const char* description)
{
    log_error("GLFW Error %d: %s\n", error, description);
}

int render_initialize() {
	glfwSetErrorCallback(GLFW_error);

    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
    if (!g_renderconf.configured) {
      log_error("call render_configure before render_initialize");
      return RENDER_ERR_UNCONFIGURED;
    }
    if (g_renderconf.fullscreen) {
        g_window = glfwCreateWindow(g_renderconf.xres, g_renderconf.yres, (const char *)&g_renderconf.windowtitle, glfwGetPrimaryMonitor(), NULL); // Windowed
    } else {
        g_window = glfwCreateWindow(g_renderconf.xres, g_renderconf.yres, (const char *)&g_renderconf.windowtitle, NULL, NULL); // Windowed
    }
    glfwMakeContextCurrent(g_window);
    glewExperimental = GL_TRUE;
    glewInit();
    return 0;
}

int render_mkshader(const char * source, GLenum type, GLuint *result) {
    char * type_name;
	GLint status;
	GLuint shader;
	if (type == GL_VERTEX_SHADER) {type_name = "vertex";}
	if (type == GL_FRAGMENT_SHADER) {type_name = "fragment";}

    shader = glCreateShader(type);
    log_debug("compiling source %s", source);
    glShaderSource(shader, 1, &source, NULL);
    log_debug("source OK\n");
    glCompileShader(shader);
    log_debug("comp OK\n");

    glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
    if (status == GL_TRUE) {
        log_debug("%s shader OK\n", type_name);
        *result = shader;
        return 0;
    } else {
        char buffer[512];
		log_error("%s shader BAD\n", type_name);
        glGetShaderInfoLog(shader, 512, NULL, buffer); 
        log_error("%s shader compilation error: %s", type_name, buffer);
        return RENDER_ERR_BADSHADER;
    }
}

int render_loop() {
    if (g_mode == RENDER_MODE_CTRIANGLE) {
        render_ctriangle_loop();
    }
    glfwSwapBuffers(g_window);
    glfwPollEvents();
    return 0;
}

int render_terminate() {
    if (g_mode == RENDER_MODE_CTRIANGLE) {
        render_ctriangle_terminate();
    }
    glfwTerminate();
    return 0;
}

int render_should_close() {
    if (!g_window) {
        log_error("glfw window shut down unexpectedly, exiting render loop");
        return 1;
    }
    return glfwWindowShouldClose(g_window);
}



int render_ctriangle() {
    int err;
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
    const char* fragmentSource = 
    "#version 150\n\
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


    GLuint vao, vbo, ebo;
    GLuint vertexShader, fragmentShader;
	GLuint shaderProgram;
	GLuint tex;
	GLint posAttrib, colAttrib;
	float color[] = { 1.0f, 0.0f, 0.0f, 1.0f };

    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    glGenBuffers(1, &vbo); // Generate 1 buffer
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
 
    glGenBuffers(1, &ebo),
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(elements), elements, GL_STATIC_DRAW);


    err = render_mkshader(vertexSource, GL_VERTEX_SHADER, &vertexShader);
    if (err) return err;
    err = render_mkshader(fragmentSource, GL_FRAGMENT_SHADER, &fragmentShader);
    if (err) return err;

    shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glBindFragDataLocation(shaderProgram, 0, "outColor");
    glLinkProgram(shaderProgram);
    glUseProgram(shaderProgram);


    posAttrib = glGetAttribLocation(shaderProgram, "position");
    glEnableVertexAttribArray(posAttrib);
    glVertexAttribPointer(posAttrib, 2, GL_FLOAT, GL_FALSE, 5*sizeof(float), 0);

    colAttrib = glGetAttribLocation(shaderProgram, "vertexColor");
    glEnableVertexAttribArray(colAttrib);
    glVertexAttribPointer(colAttrib, 3, GL_FLOAT, GL_FALSE, 5*sizeof(float), (void*)(2*sizeof(float)));


    glGenTextures(1, &tex);
    glBindTexture(GL_TEXTURE_2D, tex);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, color);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST_MIPMAP_NEAREST);

    g_ctriangle.vertexShader = vertexShader;
    g_ctriangle.fragmentShader = fragmentShader;
    g_ctriangle.shaderProgram = shaderProgram;
    g_ctriangle.vbo = vbo;
    g_ctriangle.vao = vao;
    g_mode = RENDER_MODE_CTRIANGLE;
    return 0;
}

void render_ctriangle_loop() {
    float redness = (float)(time(NULL) % 5) /5.0f;
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}

void render_ctriangle_terminate() {
    
    glDeleteProgram(g_ctriangle.shaderProgram);
    glDeleteShader(g_ctriangle.fragmentShader);
    glDeleteShader(g_ctriangle.vertexShader);

    glDeleteBuffers(1, &g_ctriangle.vbo);
    glDeleteVertexArrays(1, &g_ctriangle.vao); 
    g_mode = RENDER_MODE_NONE;
}


