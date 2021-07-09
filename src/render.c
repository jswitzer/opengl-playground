#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <string.h>
#include <sys/time.h>
#include <SOIL/SOIL.h>
#include <cglm/cglm.h>
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
#define RENDER_MODE_LOOK 2

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

void render_look_loop();
void render_look_terminate();
typedef struct look_state_s {
    struct timeval start_time;
    GLuint vertexShader;
    GLuint fragmentShader;
    GLuint shaderProgram;
    GLuint vbo;
    GLuint ebo;
    GLuint vao;
    GLuint textures[2];
    GLuint uniModel;
} look_state_t;
look_state_t g_look = {0};



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
    if (g_mode == RENDER_MODE_LOOK) {
        render_look_loop();
    }
    glfwSwapBuffers(g_window);
    glfwPollEvents();
    return 0;
}

int render_terminate() {
    if (g_mode == RENDER_MODE_CTRIANGLE) {
        render_ctriangle_terminate();
    }
    if (g_mode == RENDER_MODE_LOOK) {
        render_look_terminate();
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

int render_look() {
    int err;
    const char* vertexSource =
    "#version 150\n\
    in vec2 position;\n\
    in vec3 color;\n\
    in vec2 texcoord;\n\
    out vec3 Color;\n\
    out vec2 Texcoord;\n\
    uniform mat4 model;\n\
    uniform mat4 view;\n\
    uniform mat4 proj;\n\
    void main()\n\
    {\n\
        Color = color;\n\
        Texcoord = texcoord;\n\
        gl_Position = proj * view * model * vec4(position, 0.0, 1.0);\n\
    }\n";

    #if 1
    //uniform vec3 triangleColor;\n
    const char* fragmentSource = 
	"#version 150\n\
    in vec3 Color;\n\
    in vec2 Texcoord;\n\
    out vec4 outColor;\n\
    uniform sampler2D texKitten;\n\
    uniform sampler2D texPuppy;\n\
    void main()\n\
    {\n\
        outColor = mix(texture(texKitten, Texcoord), texture(texPuppy, Texcoord), 0.5);\n\
    }\n";
    #else
    const char* fragmentSource = 
    "#version 150\n\
    in vec3 VertexColor;\n\
    out vec4 outColor;\n\
    void main()\n\
    {\n\
        outColor = vec4(VertexColor, 1.0);\n\
    }\n";
    #endif

    float vertices[] = {
    //  Position      Color             Texcoords
        -0.5f,  0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, // Top-left
         0.5f,  0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, // Top-right
         0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, // Bottom-right
        -0.5f, -0.5f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f  // Bottom-left
    };

    /*
    float vertices[] = {
        0.0f,  1.0f, 1.0f, 0.0f, 0.0f, // Vertex 1 (X, Y)
        1.0f, 0.0f, 0.0f, 1.0f, 0.0f, // Vertex 2 (X, Y)
        -1.0f, 0.0f, 0.0f, 0.0f, 1.0f, // Vertex 3 (X, Y)
        0.0f, -0.5f, 0.0f, 0.0f, 1.0f // Vertex 4 (X, Y)
    }; */

    GLuint elements[] = {
        0, 1, 2,
        2, 3, 0
    };


    GLuint vao, vbo, ebo;
    GLuint vertexShader, fragmentShader;
	GLuint shaderProgram;
	GLuint tex;
	GLint posAttrib, colAttrib, texAttrib;
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

    // Specify the layout of the vertex data
    posAttrib = glGetAttribLocation(shaderProgram, "position");
    glEnableVertexAttribArray(posAttrib);
    glVertexAttribPointer(posAttrib, 2, GL_FLOAT, GL_FALSE, 7 * sizeof(float), 0);

    colAttrib = glGetAttribLocation(shaderProgram, "color");
    glEnableVertexAttribArray(colAttrib);
    glVertexAttribPointer(colAttrib, 3, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void*)(2 * sizeof(float)));

    texAttrib = glGetAttribLocation(shaderProgram, "texcoord");
    glEnableVertexAttribArray(texAttrib);
    glVertexAttribPointer(texAttrib, 2, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void*)(5 * sizeof(float)));


    // Load textures
    GLuint textures[2];
    glGenTextures(2, textures);

    #if 1
    int width, height;
    unsigned char* image;

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, textures[0]);
        image = SOIL_load_image("sample.png", &width, &height, 0, SOIL_LOAD_RGB);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
        SOIL_free_image_data(image);
    glUniform1i(glGetUniformLocation(shaderProgram, "texKitten"), 0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
 
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, textures[1]);
        image = SOIL_load_image("sample2.png", &width, &height, 0, SOIL_LOAD_RGB);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
        SOIL_free_image_data(image);
    glUniform1i(glGetUniformLocation(shaderProgram, "texPuppy"), 1);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    #else 
    #endif

    vec3 view_eye    = {1.2f, 1.2f, 1.2f};
    vec3 view_center = {0.0f, 0.0f, 0.0f};
    vec3 view_up     = {0.0f, 0.0f, 1.0f};
    mat4 view;
    mat4 proj;
    // Set up view and projection matrixes
    glm_lookat( view_eye, view_center, view_up, view);
    glm_perspective(glm_rad(45.0f), 800.0f / 600.0f, 1.0f, 10.0f, proj);


    GLint uniView = glGetUniformLocation(shaderProgram, "view");
    glUniformMatrix4fv(uniView, 1, GL_FALSE, (float*)view);
    GLint uniProj = glGetUniformLocation(shaderProgram, "proj");
    glUniformMatrix4fv(uniProj, 1, GL_FALSE, (float*)proj);


    gettimeofday(&g_look.start_time, NULL);
    g_look.uniModel = glGetUniformLocation(shaderProgram, "model");
    g_look.vertexShader = vertexShader;
    g_look.fragmentShader = fragmentShader;
    g_look.shaderProgram = shaderProgram;
    g_look.vbo = vbo;
    g_look.vao = vao;
    g_look.ebo = ebo;
    g_look.textures[0] = textures[0];
    g_look.textures[1] = textures[1];
    g_mode = RENDER_MODE_LOOK;
    return 0;
}



void render_look_loop() {
    struct timeval curr_time;
    gettimeofday(&curr_time, NULL);
    float elapsed = (curr_time.tv_sec - g_look.start_time.tv_sec) * 1.0f \
                + ((curr_time.tv_usec - g_look.start_time.tv_usec) / 1000000.0f);
    mat4 model = GLM_MAT4_IDENTITY_INIT;
    vec3 rotation_normal = {0.0f, 0.0f, 1.0f};
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    glm_rotate(model, elapsed * glm_rad(180.0f), rotation_normal);
    glUniformMatrix4fv(g_look.uniModel, 1, GL_FALSE, (float *)model);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}


void render_look_terminate() {
    glDeleteTextures(2, g_look.textures);
    glDeleteProgram(g_look.shaderProgram);
    glDeleteShader(g_look.fragmentShader);
    glDeleteShader(g_look.vertexShader);

    glDeleteBuffers(1, &g_look.ebo);
    glDeleteBuffers(1, &g_look.vbo);
    glDeleteVertexArrays(1, &g_look.vao); 
    g_mode = RENDER_MODE_NONE;
}


