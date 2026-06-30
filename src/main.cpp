#define SDL_MAIN_USE_CALLBACKS 1 // Use callback functions instead of main()

#include <GLES3/gl3.h> // Use GLES3 headers for WebGL 2.0
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

const char *vertexShaderSource =
    "#version 300 es\n"
    "layout (location = 0) in vec3 aPosition;\n"
    "void main()\n"
    "{\n"
    "    gl_Position = vec4(aPosition, 1.0);\n"
    "}\n";

const char *fragmentShaderSource =
    "#version 300 es\n"
    "precision mediump float;\n"
    "out vec4 fragColor;\n"
    "void main()\n"
    "{\n"
    "    fragColor = vec4(0.0, 1.0, 0.0, 1.0);\n"
    "}\n";

typedef struct {
    SDL_Window *window;
    SDL_GLContext glContext;
    unsigned int shaderProgram;
    unsigned int VAO, VBO;
} App;

SDL_AppResult SDL_AppInit(void **appstate, int argc, char *argv[])
{
    App *myApp = (App *)SDL_malloc(sizeof(App));
    *appstate = myApp; // This makes the pointer available to all other callbacks

    if (!SDL_Init(SDL_INIT_VIDEO))
        return SDL_APP_FAILURE;

    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1); // Enable MULTISAMPLE
    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 2); // can be 2, 4, 8 or 16
    SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 0);

    // WebGL 2 requires GLES 3.0 context
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);

    myApp->window = SDL_CreateWindow("SDL3 Wasm OpenGL", 380, 380, SDL_WINDOW_OPENGL);
    if (!myApp->window)
        return SDL_APP_FAILURE;

    myApp->glContext = SDL_GL_CreateContext(myApp->window);
    if (!myApp->glContext)
        return SDL_APP_FAILURE;

    SDL_GL_SetSwapInterval(1);

    unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);

    unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);

    myApp->shaderProgram = glCreateProgram();
    glAttachShader(myApp->shaderProgram, vertexShader);
    glAttachShader(myApp->shaderProgram, fragmentShader);
    glLinkProgram(myApp->shaderProgram);
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    float vertices[] = {
        -0.5f, -0.5f, 0.0f,
         0.5f, -0.5f, 0.0f,
         0.0f,  0.5f, 0.0f
    };

    glGenVertexArrays(1, &myApp->VAO);
    glGenBuffers(1, &myApp->VBO);
    glBindVertexArray(myApp->VAO);
    glBindBuffer(GL_ARRAY_BUFFER, myApp->VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);

    return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppEvent(void *appstate, SDL_Event *event)
{
    if (event->type == SDL_EVENT_QUIT) {
        return SDL_APP_SUCCESS;
    }
    return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppIterate(void *appstate)
{
    App *myApp = (App *)appstate;

    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    glUseProgram(myApp->shaderProgram);
    glBindVertexArray(myApp->VAO);
    glDrawArrays(GL_TRIANGLES, 0, 3);

    SDL_GL_SwapWindow(myApp->window);
    return SDL_APP_CONTINUE;
}

void SDL_AppQuit(void *appstate, SDL_AppResult result)
{
    // Cast the pointer back to your struct type
    App *myApp = (App *)appstate;

    // Only clean up if we actually successfully allocated the state
    if (myApp) {
        // Destroy SDL objects
        if (myApp->glContext)
        {
            // Delete OpenGL objects
            glDeleteVertexArrays(1, &myApp->VAO);
            glDeleteBuffers(1, &myApp->VBO);
            glDeleteProgram(myApp->shaderProgram);
            SDL_GL_DestroyContext(myApp->glContext);
        }
        if (myApp->window)
            SDL_DestroyWindow(myApp->window);

        // Free the memory we allocated in SDL_AppInit
        SDL_free(myApp);
    }

    // Final shutdown for the SDL subsystem
    SDL_Quit();
}
