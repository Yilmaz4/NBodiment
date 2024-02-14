#define IMGUI_DEFINE_MATH_OPERATORS
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <imgui_internal.h>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtx/vector_angle.hpp>
#include <glm/gtx/string_cast.hpp>

#include <Windows.h>
#include <exception>
#include <string>
#include <vector>
#include <random>
#include <iostream>

namespace ImGui {
    ImFont* font;

    inline static void load_theme() {
        ImGui::StyleColorsDark();

        ImVec4* colors = ImGui::GetStyle().Colors;
        colors[ImGuiCol_Text] = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
        colors[ImGuiCol_TextDisabled] = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);
        colors[ImGuiCol_WindowBg] = ImVec4(0.10f, 0.10f, 0.10f, 1.00f);
        colors[ImGuiCol_ChildBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
        colors[ImGuiCol_PopupBg] = ImVec4(0.19f, 0.19f, 0.19f, 0.92f);
        colors[ImGuiCol_Border] = ImVec4(0.19f, 0.19f, 0.19f, 0.29f);
        colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.24f);
        colors[ImGuiCol_FrameBg] = ImVec4(0.05f, 0.05f, 0.05f, 0.54f);
        colors[ImGuiCol_FrameBgHovered] = ImVec4(0.19f, 0.19f, 0.19f, 0.54f);
        colors[ImGuiCol_FrameBgActive] = ImVec4(0.20f, 0.22f, 0.23f, 1.00f);
        colors[ImGuiCol_TitleBg] = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
        colors[ImGuiCol_TitleBgActive] = ImVec4(0.06f, 0.06f, 0.06f, 1.00f);
        colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
        colors[ImGuiCol_MenuBarBg] = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
        colors[ImGuiCol_ScrollbarBg] = ImVec4(0.05f, 0.05f, 0.05f, 0.54f);
        colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.34f, 0.34f, 0.34f, 0.54f);
        colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.40f, 0.40f, 0.40f, 0.54f);
        colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.56f, 0.56f, 0.56f, 0.54f);
        colors[ImGuiCol_CheckMark] = ImVec4(0.33f, 0.67f, 0.86f, 1.00f);
        colors[ImGuiCol_SliderGrab] = ImVec4(0.34f, 0.34f, 0.34f, 0.54f);
        colors[ImGuiCol_SliderGrabActive] = ImVec4(0.56f, 0.56f, 0.56f, 0.54f);
        colors[ImGuiCol_Button] = ImVec4(0.25f, 0.25f, 0.25f, 0.54f);
        colors[ImGuiCol_ButtonHovered] = ImVec4(0.39f, 0.39f, 0.39f, 0.54f);
        colors[ImGuiCol_ButtonActive] = ImVec4(0.30f, 0.32f, 0.33f, 1.00f);
        colors[ImGuiCol_Header] = ImVec4(0.00f, 0.00f, 0.00f, 0.52f);
        colors[ImGuiCol_HeaderHovered] = ImVec4(0.00f, 0.00f, 0.00f, 0.36f);
        colors[ImGuiCol_HeaderActive] = ImVec4(0.20f, 0.22f, 0.23f, 0.33f);
        colors[ImGuiCol_Separator] = ImVec4(0.28f, 0.28f, 0.28f, 0.29f);
        colors[ImGuiCol_SeparatorHovered] = ImVec4(0.44f, 0.44f, 0.44f, 0.29f);
        colors[ImGuiCol_SeparatorActive] = ImVec4(0.40f, 0.44f, 0.47f, 1.00f);
        colors[ImGuiCol_ResizeGrip] = ImVec4(0.28f, 0.28f, 0.28f, 0.29f);
        colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.44f, 0.44f, 0.44f, 0.29f);
        colors[ImGuiCol_ResizeGripActive] = ImVec4(0.40f, 0.44f, 0.47f, 1.00f);
        colors[ImGuiCol_Tab] = ImVec4(0.00f, 0.00f, 0.00f, 0.52f);
        colors[ImGuiCol_TabHovered] = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
        colors[ImGuiCol_TabActive] = ImVec4(0.20f, 0.20f, 0.20f, 0.36f);
        colors[ImGuiCol_TabUnfocused] = ImVec4(0.00f, 0.00f, 0.00f, 0.52f);
        colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
        colors[ImGuiCol_DockingPreview] = ImVec4(0.33f, 0.67f, 0.86f, 1.00f);
        colors[ImGuiCol_DockingEmptyBg] = ImVec4(1.00f, 0.00f, 0.00f, 1.00f);
        colors[ImGuiCol_PlotLines] = ImVec4(1.00f, 0.00f, 0.00f, 1.00f);
        colors[ImGuiCol_PlotLinesHovered] = ImVec4(1.00f, 0.00f, 0.00f, 1.00f);
        colors[ImGuiCol_PlotHistogram] = ImVec4(1.00f, 0.00f, 0.00f, 1.00f);
        colors[ImGuiCol_PlotHistogramHovered] = ImVec4(1.00f, 0.00f, 0.00f, 1.00f);
        colors[ImGuiCol_TableHeaderBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.52f);
        colors[ImGuiCol_TableBorderStrong] = ImVec4(0.00f, 0.00f, 0.00f, 0.52f);
        colors[ImGuiCol_TableBorderLight] = ImVec4(0.28f, 0.28f, 0.28f, 0.29f);
        colors[ImGuiCol_TableRowBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
        colors[ImGuiCol_TableRowBgAlt] = ImVec4(1.00f, 1.00f, 1.00f, 0.06f);
        colors[ImGuiCol_TextSelectedBg] = ImVec4(0.20f, 0.22f, 0.23f, 1.00f);
        colors[ImGuiCol_DragDropTarget] = ImVec4(0.33f, 0.67f, 0.86f, 1.00f);
        colors[ImGuiCol_NavHighlight] = ImVec4(1.00f, 0.00f, 0.00f, 1.00f);
        colors[ImGuiCol_NavWindowingHighlight] = ImVec4(1.00f, 0.00f, 0.00f, 0.70f);
        colors[ImGuiCol_NavWindowingDimBg] = ImVec4(1.00f, 0.00f, 0.00f, 0.20f);
        colors[ImGuiCol_ModalWindowDimBg] = ImVec4(1.00f, 0.00f, 0.00f, 0.35f);

        ImGuiStyle& style = ImGui::GetStyle();
        style.WindowPadding = ImVec2(8.00f, 8.00f);
        style.FramePadding = ImVec2(5.00f, 2.00f);
        style.CellPadding = ImVec2(6.00f, 6.00f);
        style.ItemSpacing = ImVec2(6.00f, 6.00f);
        style.ItemInnerSpacing = ImVec2(6.00f, 6.00f);
        style.TouchExtraPadding = ImVec2(0.00f, 0.00f);
        style.IndentSpacing = 25;
        style.ScrollbarSize = 15;
        style.GrabMinSize = 10;
        style.WindowBorderSize = 1;
        style.ChildBorderSize = 1;
        style.PopupBorderSize = 1;
        style.FrameBorderSize = 1;
        style.TabBorderSize = 1;
        style.WindowRounding = 7;
        style.ChildRounding = 4;
        style.FrameRounding = 3;
        style.PopupRounding = 4;
        style.ScrollbarRounding = 9;
        style.GrabRounding = 3;
        style.LogSliderDeadzone = 4;
        style.TabRounding = 4;
    }
}

class Error : public std::exception {
    wchar_t msg[1024];
public:
    Error(const wchar_t* message) {
        wcsncpy_s(msg, message, 18);
        this->display();
    }
    Error(const char* message) {
        mbstowcs_s(nullptr, msg, message, 1024);
        this->display();
    }
    void display() {
        MessageBoxW(NULL, msg, L"Error", MB_OK | MB_ICONERROR);
    }
};

#include "resource.h"

class Shader {
    GLuint vertexShader;
    GLuint fragmentShader;

    static const char* read_resource(int name) {
        HMODULE handle = ::GetModuleHandle(NULL);
        HRSRC rc = ::FindResource(handle, MAKEINTRESOURCE(name), MAKEINTRESOURCE(TEXTFILE));
        if (rc == NULL) return nullptr;
        HGLOBAL rcData = ::LoadResource(handle, rc);
        if (rcData == NULL) return nullptr;
        return static_cast<const char*>(::LockResource(rcData));
    }
public:
    GLuint id;
    inline void create() {
        int success;
        char infoLog[1024];

        const char* vertexSource = this->read_resource(IDR_VRTX);
        const char* fragmentSource = this->read_resource(IDR_FRAG);

        vertexShader = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vertexShader, 1, &vertexSource, NULL);
        glCompileShader(vertexShader);
        glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
        if (!success) {
            glGetShaderInfoLog(vertexShader, 1024, NULL, infoLog);
            throw Error(infoLog);
        }

        fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fragmentShader, 1, &fragmentSource, NULL);
        glCompileShader(fragmentShader);
        glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
        if (!success) {
            glGetShaderInfoLog(fragmentShader, 1024, NULL, infoLog);
            throw Error(infoLog);
        }

        id = glCreateProgram();
        glAttachShader(id, vertexShader);
        glAttachShader(id, fragmentShader);
        glLinkProgram(id);

        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);
        this->use();
    }
    inline void use() { glUseProgram(id); }
    inline void free() { glDeleteProgram(id); }
};

class Camera {
public:
    glm::vec3 position  = { 1.f, 0.f, 0.f };
    glm::vec3 direction = { 0.f, 1.f, 0.f };
    float fov = 60.f;
    float speed = 0.1f;

    void proj_mat(float w, float h, float nearPlane, float farPlane, GLuint shaderID, const char* uniform) {
        auto view = glm::mat4(1.f);
        auto proj = glm::mat4(1.f);

        view = glm::lookAt(position, direction + position, { 0.f, 0.f, 1.f });
        proj = glm::perspective(glm::radians(fov), w / h, nearPlane, farPlane);

        glUniformMatrix4fv(glGetUniformLocation(shaderID, uniform), 1, GL_FALSE, glm::value_ptr(proj * view));
    }

    void processInput(glm::bvec4 keys) {
        float sign_x = (keys[1] && !keys[3] ? -1.f : (keys[3] && !keys[1] ? 1.f : 0.f));
        float sign_y = (keys[0] && !keys[2] ? 1.f : (keys[2] && !keys[0] ? -1.f : 0.f));
        position += glm::vec3({ sign_x * speed, sign_y * speed, 0.f });
    }
};

class NBodiment {
    Shader shader;
    Camera camera;
    GLuint ssbo;

    GLFWwindow* window;
    GLFWmonitor* monitor;

    glm::ivec2 res;
    std::vector<glm::vec4> pBuffer;

    glm::bvec4 keys = { 0, 0, 0, 0 }; // wasd
public:
    NBodiment() {
        glfwInit();
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

        monitor = glfwGetPrimaryMonitor();
        const GLFWvidmode* mode = glfwGetVideoMode(monitor);
        res = { mode->width, mode->height };

        glfwWindowHint(GLFW_RED_BITS, mode->redBits);
        glfwWindowHint(GLFW_GREEN_BITS, mode->greenBits);
        glfwWindowHint(GLFW_BLUE_BITS, mode->blueBits);
        glfwWindowHint(GLFW_REFRESH_RATE, mode->refreshRate);

        window = glfwCreateWindow(res.x, res.y, "N-Body Simulation", NULL, NULL);
        if (window == nullptr) {
            throw Error("Failed to create OpenGL context");
        }
        glfwMakeContextCurrent(window);
        // initialize callback functions
        glfwSetFramebufferSizeCallback(window, on_windowResize);
        glfwSetKeyCallback(window, on_keyPress);

        glfwSwapInterval(1);

        glfwSetWindowUserPointer(window, this);

        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO(); (void)io;
        io.Fonts->AddFontDefault();
        ImGui::font = io.Fonts->AddFontFromFileTTF("C:\\Windows\\Fonts\\consola.ttf", 12.f);
        IM_ASSERT(ImGui::font != NULL);
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
        io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
        ImGui::load_theme();
        ImGui_ImplGlfw_InitForOpenGL(window, true);
        ImGui_ImplOpenGL3_Init("#version 430");

        if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
            throw Error("Failed to load GLAD");
        }

        shader = Shader();
        shader.create();

        GLuint VAO;
        glGenVertexArrays(1, &VAO);
        glBindVertexArray(VAO);

        camera = Camera();

        std::random_device rd;
        std::mt19937 rng(rd());
        std::uniform_real_distribution<float> pos(-1.0f, 1.0f);
        std::uniform_real_distribution<float> vel(-0.0f, 0.0f);

        for (int i = 0; i < 100; i++) {
            pBuffer.push_back({ pos(rng),pos(rng),pos(rng), 1.0 });
            pBuffer.push_back({ vel(rng),vel(rng),vel(rng), 1.0 });
        }

        glGenBuffers(1, &ssbo);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo);
        glBufferData(GL_SHADER_STORAGE_BUFFER, pBuffer.size() * sizeof(glm::vec4), pBuffer.data(), GL_STREAM_DRAW);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, ssbo);
        GLuint block_index = glGetProgramResourceIndex(shader.id, GL_SHADER_STORAGE_BLOCK, "vBuffer");
        glShaderStorageBlockBinding(shader.id, block_index, 0);

        this->mainloop();
    }

    static inline void on_windowResize(GLFWwindow* window, int w, int h) {
        NBodiment* app = static_cast<NBodiment*>(glfwGetWindowUserPointer(window));
        glViewport(0, 0, app->res.x, app->res.y);
    }

    static inline void on_keyPress(GLFWwindow* window, int key, int scancode, int action, int mods) {
        NBodiment* app = static_cast<NBodiment*>(glfwGetWindowUserPointer(window));
        switch (action) {
        case GLFW_PRESS:
            app->keys |= glm::bvec4({ key == GLFW_KEY_W, key == GLFW_KEY_A, key == GLFW_KEY_S, key == GLFW_KEY_D });
            switch (key) {
            case GLFW_KEY_ESCAPE:
                glfwSetWindowShouldClose(window, true);
                break;
            }
            break;
        case GLFW_RELEASE:
            app->keys &= glm::bvec4({ key != GLFW_KEY_W, key != GLFW_KEY_A, key != GLFW_KEY_S, key != GLFW_KEY_D });
        }
        
    }

    void mainloop() {
        double lastFrame = glfwGetTime();
        double fps = 0;
        do {
            glfwPollEvents();
            camera.processInput(this->keys);
            camera.proj_mat(res.x, res.y, 0.1f, 100.f, shader.id, "uMatrix");
            ImGui_ImplOpenGL3_NewFrame();
            ImGui_ImplGlfw_NewFrame();
            ImGui::NewFrame();

            double currentTime = glfwGetTime();
            fps = 1 / (currentTime - lastFrame);
            

            ImGui::PushFont(ImGui::font);
            ImGui::SetNextWindowPos({ 10, 10 });
            ImGui::SetNextWindowCollapsed(true, 1 << 1);
            if (ImGui::Begin("Settings", nullptr,
                ImGuiWindowFlags_NoScrollbar |
                ImGuiWindowFlags_NoScrollWithMouse |
                ImGuiWindowFlags_AlwaysAutoResize |
                ImGuiWindowFlags_NoMove
            )) {
                
                ImGui::Text("FPS: %.3g   Frametime: %.3g ms", fps, 1000.0 * (currentTime - lastFrame));
                
            }
            ImGui::PopFont();
            ImGui::End();

            glViewport(0, 0, res.x, res.y);

            glUniform1d(glGetUniformLocation(shader.id, "uTimeDelta"), currentTime - lastFrame);
            lastFrame = currentTime;

            ImGui::Render();
            glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT);
            glPointSize(5.f);
            glDrawArrays(GL_POINTS, 0, pBuffer.size() / 2);

            ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
            glfwSwapBuffers(window);
        } while (!glfwWindowShouldClose(this->window));
        glfwDestroyWindow(window);
        glfwTerminate();
    }
};

int main() {
    NBodiment proj = NBodiment();
    return 0;
}