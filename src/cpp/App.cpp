#include <OpenGL/gl3.h>
#include <GLFW/glfw3.h>
#include <GLSLProgram.h>

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

glm::vec3 camera_position = glm::vec3(0.0f, 0.0f, 5.0f);

glm::mat4 triangle_model_matrix;
glm::mat4 view_matrix;
glm::mat4 projection_matrix;

const GLuint DEFAULT_WINDOW_WIDTH = 800, DEFAULT_WINDOW_HEIGHT = 800;
const GLfloat CAMERA_MOVEMENT_SPEED = 0.02f;

GLSLProgram* shaderProgram;

double ypos_old = -1;

void framebuffer_size_callback(GLFWwindow*, int new_screen_width, int new_screen_height)
{
    glViewport(0, 0, new_screen_width, new_screen_height);
    projection_matrix = glm::perspective(45.0f, (GLfloat)new_screen_width / (GLfloat)new_screen_height, 0.1f, 10.0f);
}

void cursor_pos_callback(GLFWwindow* window, double, double ypos) {

    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
    {
        if(ypos_old != -1)
        {
            camera_position.z += CAMERA_MOVEMENT_SPEED * (ypos_old - ypos);
            camera_position.z = glm::clamp(camera_position.z, 0.2f, 9.90f);
        }

        ypos_old = ypos;
    }
    else if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_RELEASE) {
        ypos_old = -1;
    }

}

void cleanUp()
{
    delete shaderProgram;
    glfwTerminate();
}

int main()
{
    GLFWwindow* window;

    if (!glfwInit())
        exit(1);

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#endif

    glfwWindowHint(GLFW_SAMPLES, 4);

    window = glfwCreateWindow(DEFAULT_WINDOW_WIDTH, DEFAULT_WINDOW_HEIGHT, "Hello World", NULL, NULL);

    if (!window)
    {
        glfwTerminate();
        exit(1);
    }

    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, cursor_pos_callback);
    glfwMakeContextCurrent(window);

    shaderProgram = new GLSLProgram();

    if(!shaderProgram->compileShaderFromFile("src/glsl/triangle.vs", GL_VERTEX_SHADER))
    {
        printf("Vertex shader failed to compile!\n%s", shaderProgram->log().c_str());
        exit(1);
    }

    if (!shaderProgram->compileShaderFromFile("src/glsl/triangle.fs", GL_FRAGMENT_SHADER))
    {
        printf("Fragment shader failed to compile!\n%s", shaderProgram->log().c_str());
        exit(1);
    }

    if (!shaderProgram->link())
    {
        printf("Shader program failed to link!\n%s", shaderProgram->log().c_str());
        exit(1);
    }

    shaderProgram->use();

    shaderProgram->printActiveUniforms();
    shaderProgram->printActiveAttribs();

    GLfloat triangle_vertices[] = {
            0.0f,  0.5f, 0.0f,
            0.5f, -0.5f, 0.0f,
            -0.5f, -0.5f, 0.0f
    };

    GLuint triangleVAO;
    glGenVertexArrays(1, &triangleVAO);
    glBindVertexArray(triangleVAO);

    GLuint triangleVertices;
    glGenBuffers(1, &triangleVertices);
    glBindBuffer(GL_ARRAY_BUFFER, triangleVertices);
    glBufferData(GL_ARRAY_BUFFER, sizeof(triangle_vertices), triangle_vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), 0);
    glEnableVertexAttribArray(0);

    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);

    projection_matrix = glm::perspective(45.0f, (GLfloat)DEFAULT_WINDOW_WIDTH / (GLfloat)DEFAULT_WINDOW_HEIGHT, 0.1f, 100.0f);
    glEnable(GL_MULTISAMPLE);

    while (!glfwWindowShouldClose(window))
    {
        glClear(GL_COLOR_BUFFER_BIT);

        GLfloat now = (GLfloat)glfwGetTime();

        triangle_model_matrix = glm::rotate(glm::mat4(), (float)sin(now), glm::vec3(0.0f, 0.0f, 1.0f));
        view_matrix = glm::lookAt(camera_position, glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));

        shaderProgram->setUniform("model_matrix", triangle_model_matrix);
        shaderProgram->setUniform("view_matrix", view_matrix);
        shaderProgram->setUniform("projection_matrix", projection_matrix);

        shaderProgram->setUniform("frag_color", glm::vec3((float)sin(now), 1.0f + (float)atan(now), (float)cos(now)));

        glBindVertexArray(triangleVAO);
        glDrawArrays(GL_TRIANGLES, 0, 3);
        glBindVertexArray(0);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    cleanUp();
    exit(0);
}
