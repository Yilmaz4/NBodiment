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
#include <bitset>

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

enum Integration {
    ImplicitEuler,
    ExplicitEuler,
    SemiImplicitEuler,
    Midpoint,
    RangaKatta4,
    Verlet,
};

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
#include <bitset>

class Shader {
    GLuint vertexShader;
    GLuint geometryShader;
    GLuint fragmentShader;
protected:
    static inline char* read_resource(int name) {
        HMODULE handle = GetModuleHandleW(NULL);
        HRSRC rc = FindResourceW(handle, MAKEINTRESOURCE(name), MAKEINTRESOURCE(TEXTFILE));
        if (rc == NULL) return nullptr;
        HGLOBAL rcData = LoadResource(handle, rc);
        if (rcData == NULL) return nullptr;
        DWORD size = SizeofResource(handle, rc);
        char* res = new char[size + 1];
        memcpy(res, static_cast<const char*>(LockResource(rcData)), size);
        res[size] = '\0';
        return res;
    }
public:
    GLuint id;
    inline void create() {
        int success;
        char infoLog[1024];

        char* vertexSource = this->read_resource(IDR_VRTX);
        char* geometrySource = this->read_resource(IDR_GEOM);
        char* fragmentSource = this->read_resource(IDR_FRAG);

        vertexShader = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vertexShader, 1, &vertexSource, NULL);
        glCompileShader(vertexShader);
        glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
        if (!success) {
            glGetShaderInfoLog(vertexShader, 1024, NULL, infoLog);
            throw Error(infoLog);
        }

        geometryShader = glCreateShader(GL_GEOMETRY_SHADER);
        glShaderSource(geometryShader, 1, &geometrySource, NULL);
        glCompileShader(geometryShader);
        glGetShaderiv(geometryShader, GL_COMPILE_STATUS, &success);
        if (!success) {
            glGetShaderInfoLog(geometryShader, 1024, NULL, infoLog);
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
        glAttachShader(id, geometryShader);
        glAttachShader(id, fragmentShader);
        glLinkProgram(id);

        glDeleteShader(vertexShader);
        glDeleteShader(geometryShader);
        glDeleteShader(fragmentShader);
    }
    inline void use() { glUseProgram(id); }
    inline void free() { glDeleteProgram(id); }
};

class ComputeShader : public Shader {
    GLuint computeShader;
public:
    inline void create() {
        int success;
        char infoLog[1024];
        char* computeSource = this->read_resource(IDR_CMPT);

        computeShader = glCreateShader(GL_COMPUTE_SHADER);
        glShaderSource(computeShader, 1, &computeSource, NULL);
        glCompileShader(computeShader);
        glGetShaderiv(computeShader, GL_COMPILE_STATUS, &success);
        if (!success) {
            glGetShaderInfoLog(computeShader, 1024, NULL, infoLog);
            throw Error(infoLog);
        }

        id = glCreateProgram();
        glAttachShader(id, computeShader);
        glLinkProgram(id);
        glDeleteShader(computeShader);
    }
};

class Camera {
    float yaw = -90.0f;
    float pitch = 0.0f;
public:
    glm::vec3 position  = { 0.f,  0.f,  0.f };
    glm::vec3 direction = { 0.f,  0.f, -1.f };
    glm::vec3 upvector  = { 0.f,  1.f,  0.f };
    float fov = 60.f;
    float sensitivity = 0.1f;
    float speed = 0.2f;

    bool mouseLocked = false;

    void projMat(float w, float h, float nearPlane, float farPlane, GLuint shaderID, const char* uniform) {
        auto view = glm::mat4(1.f);
        auto proj = glm::mat4(1.f);

        view = glm::lookAt(position, direction + position, { 0.f, 1.f, 0.f });
        proj = glm::perspective(glm::radians(fov), w / h, nearPlane, farPlane);

        glUniformMatrix4fv(glGetUniformLocation(shaderID, uniform), 1, GL_FALSE, glm::value_ptr(proj * view));
    }

    void processInput(std::bitset<6> keys, float dt) {
        glm::vec3 upvec = direction;
        upvec.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch + 90));
        upvec.y = sin(glm::radians(pitch + 90));
        upvec.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch + 90));
        position += glm::vec3({
            speed * dt * (keys[0] && !keys[2] ? 1.f : (keys[2] && !keys[0] ? -1.f : 0.f)) * direction +
            speed * dt * (keys[4] && !keys[5] ? 1.f : (keys[5] && !keys[4] ? -1.f : 0.f)) * upvector +
            speed * dt * (keys[1] && !keys[3] ? 1.f : (keys[3] && !keys[1] ? -1.f : 0.f)) * glm::cross(upvec, direction)
        });
    }

    void rotate(float xoffset, float yoffset) {
        yaw   += xoffset * sensitivity;
        pitch += yoffset * sensitivity;

        if (pitch >  89.0f) pitch =  89.0f;
        if (pitch < -89.0f) pitch = -89.0f;

        direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
        direction.y = sin(glm::radians(pitch));
        direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
        direction = glm::normalize(direction);
    }
};

struct Particle {
    glm::vec3 pos;
    glm::vec3 vel;
    glm::vec3 acc;
    float mass;
    float temp;
    float density;
};

class NBodiment {
    Shader shader;
    ComputeShader cmptshader;
    Camera camera;
    GLuint ssbo;

    GLFWwindow* window;
    GLFWmonitor* monitor;

    glm::ivec2 res;
    glm::ivec2 pos = { 60, 60 };
    std::vector<Particle> pBuffer;
    std::bitset<6> keys{ 0x0 };
    glm::dvec2 prevMousePos{ 0.f, 0.f };
    double lastSpeedChange = -5;
    float timeStep = 0.05f;
    bool wireframe = false;

    glm::vec3 ambientLight = { 1.f, 1.f, 1.f };
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
        glfwWindowHint(GLFW_SAMPLES, 4);

        window = glfwCreateWindow(res.x, res.y, "N-Body Simulation", monitor, NULL);
        if (window == nullptr) {
            throw Error("Failed to create OpenGL context");
        }
        glfwMakeContextCurrent(window);
        // initialize callback functions
        glfwSetFramebufferSizeCallback(window, on_windowResize);
        glfwSetKeyCallback(window, on_keyPress);
        glfwSetScrollCallback(window, on_mouseScroll);
        glfwSetCursorPosCallback(window, on_mouseMove);

        glfwSwapInterval(1);

        glfwSetWindowUserPointer(window, this);

        if (glfwRawMouseMotionSupported()) glfwSetInputMode(window, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);
        glfwGetCursorPos(window, &prevMousePos.x, &prevMousePos.y);

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
        
        std::random_device rd;
        std::mt19937 rng(rd());
        std::uniform_real_distribution<float> pos(-1.f, 1.f);
        std::uniform_real_distribution<float> vel(-1.5f, 1.5f);
        std::uniform_real_distribution<float> mass(1e+5, 1e+9);

        //pBuffer.push_back(Particle({ 0.f, 0.f, -0.1f }, { 0.f, 0.f, 0.f }, { 0.f, 0.f, 0.f }, 1e+10, 0.5, 1e+16));
        for (int i = 0; i < 1000; i++) {
            glm::vec4 p = { pos(rng), pos(rng), pos(rng), 0 };
            glm::vec4 v = { vel(rng), vel(rng), vel(rng), 0 };
            pBuffer.push_back(Particle({
                .pos = p,
                .vel = v,
                .acc = {0.f, 0.f, 0.f},
                .mass = mass(rng),
                .temp = 0.5,
                .density = 1e+16
            }));
        }
        
        //pBuffer.push_back(Particle({ 0.f, 0.f, 0.f }, { 0.f, 0.f, 0.f }, { 0.f, 0.f, 0.f }, 5.972e+24, 0, 1)); // earth
        //pBuffer.push_back(Particle({ 384400.f, 0.f, 0.f }, { 0.f, 0.f, 1022.f }, { 0.f, 0.f, 0.f }, 7.347e+22, 0, 1)); // moon

        glGenBuffers(1, &ssbo);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo);
        glBufferData(GL_SHADER_STORAGE_BUFFER, pBuffer.size() * sizeof(Particle), pBuffer.data(), GL_DYNAMIC_DRAW);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, ssbo);

        cmptshader = ComputeShader();
        cmptshader.create();
        cmptshader.use();
        glShaderStorageBlockBinding(cmptshader.id, glGetProgramResourceIndex(cmptshader.id, GL_SHADER_STORAGE_BLOCK, "vBuffer"), 0);

        shader = Shader();
        shader.create();
        shader.use();
        glShaderStorageBlockBinding(shader.id, glGetProgramResourceIndex(shader.id, GL_SHADER_STORAGE_BLOCK, "vBuffer"), 0);
        glUniform3f(glGetUniformLocation(shader.id, "cameraPos"), camera.position.x, camera.position.y, camera.position.z);
        glUniform3f(glGetUniformLocation(shader.id, "ambientLight"), ambientLight.r, ambientLight.g, ambientLight.b);

        GLuint VAO;
        glGenVertexArrays(1, &VAO);
        glBindVertexArray(VAO);

        unsigned int skyboxID;
        glGenTextures(1, &skyboxID);
        glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxID);


        glEnable(GL_DEPTH_TEST);
        glEnable(GL_MULTISAMPLE);

        camera = Camera();
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
            app->keys |= ((int)(key == GLFW_KEY_W) | (int)(key == GLFW_KEY_A) << 1 | (int)(key == GLFW_KEY_S) << 2 | (int)(key == GLFW_KEY_D) << 3 | (int)(key == GLFW_KEY_SPACE) << 4 | (int)(key == GLFW_KEY_LEFT_SHIFT) << 5);
            glUniform3f(glGetUniformLocation(app->shader.id, "cameraPos"), app->camera.position.x, app->camera.position.y, app->camera.position.z);
            switch (key) {
            case GLFW_KEY_ESCAPE:
                glfwSetWindowShouldClose(window, true);
                break;
            case GLFW_KEY_LEFT_CONTROL:
                app->camera.mouseLocked ^= 1;
                glfwGetCursorPos(window, &app->prevMousePos.x, &app->prevMousePos.y);
                glfwSetInputMode(window, GLFW_CURSOR, app->camera.mouseLocked ? GLFW_CURSOR_DISABLED : GLFW_CURSOR_NORMAL);
                break;
            case GLFW_KEY_F11:
                if (glfwGetWindowMonitor(window) == nullptr) {
                    const GLFWvidmode* mode = glfwGetVideoMode(app->monitor);
                    glfwGetWindowPos(window, &app->pos.x, &app->pos.y);
                    app->res = { mode->width, mode->height };
                    glfwSetWindowMonitor(window, app->monitor, 0, 0, mode->width, mode->height, mode->refreshRate);
                }
                else {
                    app->res = { 600, 600 };
                    glfwSetWindowMonitor(window, nullptr, app->pos.x, app->pos.y, app->res.x, app->res.y, 0);
                }
            }
            break;
        case GLFW_RELEASE:
            app->keys &= ((int)(key != GLFW_KEY_W) | (int)(key != GLFW_KEY_A) << 1 | (int)(key != GLFW_KEY_S) << 2 | (int)(key != GLFW_KEY_D) << 3 | (int)(key != GLFW_KEY_SPACE) << 4 | (int)(key != GLFW_KEY_LEFT_SHIFT) << 5);
        }
    }

    static inline void on_mouseMove(GLFWwindow* window, double x, double y) {
        NBodiment* app = static_cast<NBodiment*>(glfwGetWindowUserPointer(window));
        if (app->camera.mouseLocked) {
            float xoffset = x - app->prevMousePos.x;
            float yoffset = app->prevMousePos.y - y;
            app->prevMousePos = { x, y };

            app->camera.rotate(xoffset, yoffset);
        }
    }

    static inline void on_mouseScroll(GLFWwindow* window, double x, double y) {
        NBodiment* app = static_cast<NBodiment*>(glfwGetWindowUserPointer(window));
        float change = static_cast<float>(y) * (app->camera.speed / 2);
        if (change + app->camera.speed > 0) {
            app->camera.speed += static_cast<float>(y) * (app->camera.speed / 2);
        }
        app->lastSpeedChange = glfwGetTime();
    }

    void mainloop() {
        double lastFrame = 0;
        double fps = 0;
        do {
            double currentTime = glfwGetTime();
            double dt = currentTime - lastFrame;
            fps = 1.0 / dt;

            glfwPollEvents();
            camera.processInput(this->keys, static_cast<float>(dt));
            bool imguiEnable = !camera.mouseLocked || (currentTime - lastSpeedChange < 2.0);
            ImGui_ImplOpenGL3_NewFrame();
            ImGui_ImplGlfw_NewFrame();
            ImGui::NewFrame();
            if (!camera.mouseLocked) {
                ImGui::PushFont(ImGui::font);
                ImGui::SetNextWindowPos({ 10, 10 });
                //ImGui::SetNextWindowCollapsed(true, 1 << 1);
                if (ImGui::Begin("Settings", nullptr,
                    ImGuiWindowFlags_NoScrollbar |
                    ImGuiWindowFlags_NoScrollWithMouse |
                    ImGuiWindowFlags_AlwaysAutoResize |
                    ImGuiWindowFlags_NoMove
                )) {
                    ImGui::Text("FPS: %.3g   Frametime: %.3g ms", fps, 1000.0 * (currentTime - lastFrame));
                    ImGui::SeparatorText("Simulation");
                    ImGui::SliderFloat("Time step", &timeStep, 0.f, 0.1f, "%.9g seconds");
                    ImGui::SeparatorText("Environment");
                    if (ImGui::ColorEdit3("Ambient light", &ambientLight[0]))
                        glUniform3f(glGetUniformLocation(shader.id, "ambientLight"), ambientLight.r, ambientLight.g, ambientLight.b);

                    ImGui::SeparatorText("Camera");
                    ImGui::SliderFloat("FOV", &camera.fov, 1, 100, "%.3g");
                    ImGui::SliderFloat("Sensitivity", &camera.sensitivity, 0.01f, 0.2f, "%.5g");
                    ImGui::SeparatorText("Debugging");
                    if (ImGui::Checkbox("Wireframe mode", &wireframe)) {
                        glPolygonMode(GL_FRONT_AND_BACK, wireframe ? GL_LINE : GL_FILL);
                    }
                }
                ImGui::PopFont();
                ImGui::End();
            }
            if (currentTime - lastSpeedChange < 2.0) {
                ImGui::PushFont(ImGui::font);
                ImGui::Begin("##speed", nullptr,
                    ImGuiWindowFlags_AlwaysAutoResize |
                    ImGuiWindowFlags_NoCollapse |
                    ImGuiWindowFlags_NoTitleBar |
                    ImGuiWindowFlags_NoMove
                );
                ImGui::Text("Speed: %.3g m/s", camera.speed);
                ImGui::SetWindowPos({ res.x / 2.f - ImGui::GetWindowWidth() / 2.f, 30 });
                ImGui::PopFont();
                ImGui::End();
            }
            ImGui::Render();
            glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            camera.projMat(res.x, res.y, 0.0001f, 1e+6, shader.id, "uMatrix");

            cmptshader.use();
            glUniform1f(glGetUniformLocation(cmptshader.id, "uTimeDelta"), timeStep * dt);
            glDispatchCompute(pBuffer.size(), 1, 1);
            glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
            shader.use();
            glUniform1f(glGetUniformLocation(shader.id, "uTimeDelta"), timeStep * dt);
            glPointSize(2);
            glDrawArrays(GL_POINTS, 0, pBuffer.size());
            lastFrame = currentTime;
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