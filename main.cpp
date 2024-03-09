#define IMGUI_DEFINE_MATH_OPERATORS
#define STB_IMAGE_IMPLEMENTATION
#define _USE_MATH_DEFINES
#define NOMINMAX

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <imgui_internal.h>
#include <stb/stb_image.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtx/vector_angle.hpp>
#include <glm/gtx/string_cast.hpp>

#include <Windows.h>
#include <filesystem>
#include <exception>
#include <string>
#include <vector>
#include <random>
#include <iostream>
#include <math.h>
#include <bitset>
#include <limits>

namespace ImGui {
    ImFont* font;

    static bool ToggleButton(const char* name, bool* v) {
        ImVec2 p = ImGui::GetCursorScreenPos();
        ImDrawList* draw_list = ImGui::GetWindowDrawList();

        float height = ImGui::GetFrameHeight();
        float width = height * 1.8f;
        float radius = height * 0.50f;
        float rounding = 0.4f;

        ImGui::InvisibleButton(name, ImVec2(width, height));
        if (ImGui::IsItemClicked()) {
            *v ^= 1;
            return true;
        }
        ImGuiContext& gg = *GImGui;
        float ANIM_SPEED = 0.055f;
        if (gg.LastActiveId == gg.CurrentWindow->GetID(name))
            float t_anim = ImSaturate(gg.LastActiveIdTimer / ANIM_SPEED);
        if (ImGui::IsItemHovered())
            draw_list->AddRectFilled(p, ImVec2(p.x + width, p.y + height), ImGui::GetColorU32(ImVec4(0.2196f, 0.2196f, 0.2196f, 1.0f)), height * rounding);
        else
            draw_list->AddRectFilled(p, ImVec2(p.x + width, p.y + height), ImGui::GetColorU32(*v ? ImVec4(0.2196f, 0.2196f, 0.2196f, 1.0f) : ImVec4(0.08f, 0.08f, 0.08f, 1.0f)), height * rounding);
        ImVec2 center = ImVec2(radius + (*v ? 1 : 0) * (width - radius * 2.0f), radius);
        draw_list->AddRectFilled(ImVec2((p.x + center.x) - 9.0f, p.y + 1.5f),
            ImVec2((p.x + (width / 2) + center.x) - 9.0f, p.y + height - 1.5f), IM_COL32(255, 255, 255, 255), height * rounding);
        ImGui::SameLine(42.f);
        ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 3.f);
        ImGui::Text(name);
        return false;
    }

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
        exit(1);
    }
};

namespace phys {
    constexpr glm::mat3 xyz_to_rgb( // matrix to convert from CIE 1931 XYZ color space to sRGB
        3.2404542f, -1.5371385f, -0.4985314f,
        -0.9692660f, 1.8760108f, 0.0415560f,
        0.0556434f, -0.2040259f, 1.0572252f
    );
    constexpr float h = 6.62607015e+34; // planck's constant
    constexpr float c = 299792458.f; // speed of light
    constexpr float c_1 = 3.741771852e-16; // first radiation constant
    constexpr float c_2 = 1.438776877e-02; // second radiation constant
    constexpr float k_b = 1.380649e-23; // boltzmann constant
    constexpr float b = 2.897771955e-3; // wien's displacement constant

    float g(float x, float mu, float t_1, float t_2) {
        return exp(-pow((x < mu) ? t_1 : t_2, 2) * pow(x - mu, 2) / 2.f);
    }

    float M(float wavelength, float t) { // black body spectral radiant exitance
        return c_1 / (pow(wavelength, 5) * (exp(c_2 / (wavelength * t)) - 1));
    }

    glm::vec3 temperature_to_color(float t) {

    }
}


#include "resource.h"

class Shader {
    GLuint vertexShader;
    GLuint fragmentShader;
    GLuint vao, vbo;
    GLuint textureID;

    float vertices[108] = {
        -1.0f,  1.0f, -1.0f,
        -1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

        -1.0f,  1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f,  1.0f
    };

    static inline void load_textures() {
        int width, height, nrChannels;
        unsigned char* data;
        for (int i = 0; i < 6; i++) {
            std::string path = std::format("assets/{}.png", i);
            data = stbi_load(path.c_str(), &width, &height, &nrChannels, 0);
            if (!data) throw Error("Skybox not available");
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
            stbi_image_free(data);
        }
    }
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

    static inline void check_for_errors(GLuint shader) {
        int success;
        char infoLog[1024];
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
        if (!success) {
            glGetShaderInfoLog(shader, 1024, NULL, infoLog);
            throw Error(infoLog);
        }
    }
public:
    GLuint id;

    inline virtual void create() {
        char* vertexSource = this->read_resource(IDR_VRTX);
        char* fragmentSource = this->read_resource(IDR_FRAG);

        vertexShader = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vertexShader, 1, &vertexSource, NULL);
        glCompileShader(vertexShader);
        check_for_errors(vertexShader);

        fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fragmentShader, 1, &fragmentSource, NULL);
        glCompileShader(fragmentShader);
        check_for_errors(fragmentShader);

        id = glCreateProgram();
        glAttachShader(id, vertexShader);
        glAttachShader(id, fragmentShader);
        glLinkProgram(id);

        glGenVertexArrays(1, &vao);
        glGenBuffers(1, &vbo);
        glBindVertexArray(vao);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), &vertices, GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), nullptr);

        glGenTextures(1, &textureID);
        glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);
        load_textures();
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);

        glUniform1i(glGetUniformLocation(id, "skybox"), 0);
    }
    inline void use() { 
        glUseProgram(id);
        glBindVertexArray(vao);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);
    }
    inline void free() { glDeleteProgram(id); }
};

class ComputeShader : public Shader {
    GLuint computeShader;
public:
    inline virtual void create() override {
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

class NBodiment;

struct Particle {
    glm::vec3 pos;
    glm::vec3 vel;
    glm::vec3 acc;
    float mass;
    float temp;
    float radius;

    glm::vec3 albedo;
    glm::vec3 emissionColor;
    float emissionStrength;
    float metallicity;
    float roughness;
};

class Camera {
    GLuint shader = NULL;
public:
    glm::vec3 position =  { 0.f,  0.f, 10.f };
    glm::vec3 direction = { 0.f,  0.f, -1.f };
    glm::vec3 upvector =  { 0.f,  1.f,  0.f };

    glm::vec3 localCoords;
    glm::vec3 localUp;
    float proximity = 5;
    Particle* following;
    float theta = -90.f;
    float phi = 0.f;

    float yaw = -90.0f;
    float pitch = 0.0f;

    float fov = 60.f;
    float sensitivity = 0.1f;
    float speed = 10.f;
    glm::vec3 velocity = { 0.f, 0.f, 0.f };
    float farPlane = 1e+6f;
    float nearPlane = 0.0001f;

    bool mouseLocked = false;

    void projMat(float w, float h, const GLuint shaderID) {
        shader = shaderID;
        glm::mat4 view;
        if (following)
            view = glm::lookAt(position, position - localCoords, { 0.f, 1.f, 0.f });
        else
            view = glm::lookAt(position, position + direction, { 0.f, 1.f, 0.f });
        auto proj = glm::perspective(glm::radians(fov), w / h, nearPlane, farPlane);
        
        glUniformMatrix4fv(glGetUniformLocation(shader, "viewMatrix"), 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(glGetUniformLocation(shader, "projMatrix"), 1, GL_FALSE, glm::value_ptr(proj));
        glUniformMatrix4fv(glGetUniformLocation(shader, "invViewMatrix"), 1, GL_FALSE, glm::value_ptr(glm::inverse(view)));
        glUniformMatrix4fv(glGetUniformLocation(shader, "invProjMatrix"), 1, GL_FALSE, glm::value_ptr(glm::inverse(proj)));
    }

    void processInput(std::bitset<6> keys, float dt, Particle* p = nullptr) {
        following = p;

        if (!p) {
            glm::vec3 upvec = direction;
            upvec.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch + 90));
            upvec.y = sin(glm::radians(pitch + 90));
            upvec.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch + 90));

            float ds = pow(0.0001f, dt);
            velocity = glm::vec3(
                (keys[0] && !keys[2] ? speed : (keys[2] && !keys[0] ? -speed : velocity.x * ds)),
                (keys[4] && !keys[5] ? speed : (keys[5] && !keys[4] ? -speed : velocity.y * ds)),
                (keys[1] && !keys[3] ? speed : (keys[3] && !keys[1] ? -speed : velocity.z * ds))
            );
            position += glm::vec3(
                velocity.x * dt * direction +
                velocity.y * dt * upvector +
                velocity.z * dt * glm::cross(upvec, direction)
            );
        }
        else {
            proximity += speed * dt * (keys[0] && !keys[2] ? -1.f : (keys[2] && !keys[0] ? 1.f : 0.f));
            if (proximity < 1.2) proximity = 1.2;
            rotate(0, 0);
            position = following->pos + localCoords;
        }

        glUniform3fv(glGetUniformLocation(shader, "cameraPos"), 1, glm::value_ptr(position));
    }

    void rotate(float xoffset, float yoffset) {
        yaw   += xoffset * sensitivity * (following ? -1.f : 1.f);
        pitch += yoffset * sensitivity;

        if (pitch >  89.0f) pitch =  89.0f;
        if (pitch < -89.0f) pitch = -89.0f;

        direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
        direction.y = sin(glm::radians(pitch));
        direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
        direction = glm::normalize(direction);

        if (following) {
            localCoords.x = cos(glm::radians(yaw + 180)) * cos(glm::radians(pitch));
            localCoords.y = sin(glm::radians(pitch));
            localCoords.z = sin(glm::radians(yaw + 180)) * cos(glm::radians(pitch));
            localCoords = glm::normalize(localCoords) * (following->radius * proximity);
        }
    }
};

class NBodiment {
public:
    Shader shader;
    ComputeShader cmptshader;
    Camera camera;
    GLuint ssbo;
    GLuint ms_ssbo;

    GLFWwindow* window;
    GLFWmonitor* monitor;

    std::vector<Particle> pBuffer;

    glm::ivec2 res;
    glm::ivec2 pos = { 60, 60 };
    std::bitset<6> keys{ 0x0 };
    glm::dvec2 prevMousePos{ 0.f, 0.f };
    double lastSpeedChange = -5.0;
    double doubleClickInterval = 0.4;
    glm::dvec2 lastPresses = { -doubleClickInterval, 0.0 };
    float timeStep = 1.0f;
    int collisionType = 0;

    bool showMilkyway = true;
    int hovering;
    int selected;
    int following;
    bool hoveringParticle = false;
    bool selectedParticle = false;
    bool lockedToParticle = false;
    glm::dvec2 mousePos;
    int numRaysPerPixel = 10;
    bool globalIllumination = false;
    bool shadows = true;

    int num_particles = 100;

    float min_distance = 4.f;
    float max_distance = 5.f;
    float min_mass = 1e+6f;
    float max_mass = 1e+8f;
    float min_density = 10e+8f;
    float max_density = 10e+9f;
    float min_temperature = 0.f;
    float max_temperature = 1e+4f;
    float min_velocity = 0.f;
    float max_velocity = 1.f;

    bool orbital_velocity = true;
    bool disk_only = false;

    float central_mass = 1e+10f;
    float central_density = 1e+9f;
    float central_temperature = 3e+3;

    glm::vec3 ambientLight = { 0.1f, 0.1f, 0.1f };

    void load_texture(const char* path, unsigned int binding, GLenum dimension) {
        int width, height, nrChannels;
        unsigned char* data = stbi_load(path, &width, &height, &nrChannels, 0);
        if (!data) throw Error("texture not found in assets");
        GLuint texture;

        glGenTextures(1, &texture);
        glActiveTexture(GL_TEXTURE0 + binding);
        glBindTexture(dimension, texture);
        switch (dimension) {
        case GL_TEXTURE_1D:
            glTexImage1D(GL_TEXTURE_1D, 0, GL_RGB, width, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
            break;
        case GL_TEXTURE_2D:
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        }
        glTexParameteri(dimension, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(dimension, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(dimension, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(dimension, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        stbi_image_free(data);
    }

    NBodiment() {
        std::setlocale(LC_CTYPE, ".UTF8");

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
        glfwSetMouseButtonCallback(window, on_mousePress);
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
        ImGui::font = io.Fonts->AddFontFromFileTTF("C:\\Windows\\Fonts\\consola.ttf", 11.f);
        IM_ASSERT(ImGui::font != NULL);
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
        io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
        ImGui::load_theme();
        ImGui_ImplGlfw_InitForOpenGL(window, true);
        ImGui_ImplOpenGL3_Init("#version 430");

        if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
            throw Error("Failed to load GLAD");
        }

        glGenBuffers(1, &ssbo);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, ssbo);

        generate_scene();

        cmptshader = ComputeShader();
        cmptshader.create();
        cmptshader.use();
        glShaderStorageBlockBinding(cmptshader.id, glGetProgramResourceIndex(cmptshader.id, GL_SHADER_STORAGE_BLOCK, "vBuffer"), 0);
        glUniform1i(glGetUniformLocation(cmptshader.id, "collisionType"), collisionType);

        shader = Shader();
        shader.create();
        shader.use();
        glShaderStorageBlockBinding(shader.id, glGetProgramResourceIndex(shader.id, GL_SHADER_STORAGE_BLOCK, "vBuffer"), 0);
        glUniform3f(glGetUniformLocation(shader.id, "cameraPos"), camera.position.x, camera.position.y, camera.position.z);
        glUniform3f(glGetUniformLocation(shader.id, "ambientLight"), ambientLight.r, ambientLight.g, ambientLight.b);
        glUniform1i(glGetUniformLocation(shader.id, "numParticles"), pBuffer.size());
        glUniform1i(glGetUniformLocation(shader.id, "numRaysPerPixel"), numRaysPerPixel);
        glUniform1i(glGetUniformLocation(shader.id, "globalIllumination"), globalIllumination);
        glUniform1i(glGetUniformLocation(shader.id, "shadows"), shadows);

        load_texture("assets/8k_earth_daymap.jpg", 0, GL_TEXTURE_2D);
        load_texture("assets/8k_earth_clouds.jpg", 1, GL_TEXTURE_2D);
        load_texture("assets/8k_sun.jpg", 2, GL_TEXTURE_2D);
        load_texture("assets/temperature.jpg", 3, GL_TEXTURE_1D);

        glEnable(GL_MULTISAMPLE);

        on_windowResize(window, res.x, res.y);

        camera = Camera();
    }

    static inline void on_windowResize(GLFWwindow* window, int w, int h) {
        NBodiment* app = static_cast<NBodiment*>(glfwGetWindowUserPointer(window));
        app->res = { w, h };
        glViewport(0, 0, app->res.x, app->res.y);
        glUniform2f(glGetUniformLocation(app->shader.id, "screenSize"), w, h);
    }

    static inline void on_keyPress(GLFWwindow* window, int key, int scancode, int action, int mods) {
        NBodiment* app = static_cast<NBodiment*>(glfwGetWindowUserPointer(window));
        switch (action) {
        case GLFW_PRESS:
            app->keys |= ((int)(key == GLFW_KEY_W) | (int)(key == GLFW_KEY_A) << 1 | (int)(key == GLFW_KEY_S) << 2 | (int)(key == GLFW_KEY_D) << 3 | (int)(key == GLFW_KEY_SPACE) << 4 | (int)(key == GLFW_KEY_LEFT_SHIFT) << 5);
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
                break;
            case GLFW_KEY_DELETE:
                if (app->lockedToParticle && app->following == app->selected) return;
                Particle p = app->pBuffer[app->selected];
                p.mass = 0.f;
                p.radius = 0.f;
                app->selectedParticle = false;
                glBindBuffer(GL_SHADER_STORAGE_BUFFER, app->ssbo);
                glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
                glBufferSubData(GL_SHADER_STORAGE_BUFFER, static_cast<GLintptr>(app->selected * sizeof(Particle)), sizeof(Particle), reinterpret_cast<float*>(&p));
            }
            break;
        case GLFW_RELEASE:
            app->keys &= ((int)(key != GLFW_KEY_W) | (int)(key != GLFW_KEY_A) << 1 | (int)(key != GLFW_KEY_S) << 2 | (int)(key != GLFW_KEY_D) << 3 | (int)(key != GLFW_KEY_SPACE) << 4 | (int)(key != GLFW_KEY_LEFT_SHIFT) << 5);
        }
    }

    static inline void on_mouseMove(GLFWwindow* window, double x, double y) {
        NBodiment* app = static_cast<NBodiment*>(glfwGetWindowUserPointer(window));
        if (app->camera.mouseLocked || (app->lockedToParticle && glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS)) {
            float xoffset = x - app->prevMousePos.x;
            float yoffset = app->prevMousePos.y - y;
            app->prevMousePos = { x, y };

            app->camera.rotate(xoffset, yoffset);
        }
        else if (app->lockedToParticle) {
            double x, y;
            glfwGetCursorPos(window, &x, &y);
            app->prevMousePos = { x, y };
        }
    }

    static inline void on_mouseScroll(GLFWwindow* window, double x, double y) {
        NBodiment* app = static_cast<NBodiment*>(glfwGetWindowUserPointer(window));
        float change = static_cast<float>(y) * (app->camera.speed / 2);
        if (change + app->camera.speed > 0) {
            app->camera.speed += change;
        }
        app->lastSpeedChange = glfwGetTime();
    }

    static inline void on_mousePress(GLFWwindow* window, int button, int action, int mods) {
        NBodiment* app = static_cast<NBodiment*>(glfwGetWindowUserPointer(window));
        if (ImGui::GetIO().WantCaptureMouse) return;
        switch (button) {
        case GLFW_MOUSE_BUTTON_LEFT:
            if (action == GLFW_PRESS) {
                if (app->hovering == app->selected) {
                    app->lastPresses.x = app->lastPresses.y;
                    app->lastPresses.y = glfwGetTime();
                }
                app->selectedParticle = app->hoveringParticle;
                app->selected = app->hovering;
                if (app->lockedToParticle && !app->selectedParticle) {
                    app->selected = app->following;
                }
            }
            else if (app->hoveringParticle && app->lastPresses.y - app->lastPresses.x < app->doubleClickInterval && app->selected == app->hovering) {
                app->lockedToParticle = true;
                app->following = app->selected;
                app->lastPresses.x = app->lastPresses.y - app->doubleClickInterval - 1.f;
            }
            break;
        case GLFW_MOUSE_BUTTON_RIGHT:
            if (!app->lockedToParticle) return;
            if (action == GLFW_PRESS) {
                double x, y;
                glfwGetCursorPos(window, &x, &y);
                glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
                app->prevMousePos = { x, y };
            }
            else {
                glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
                glfwSetCursorPos(window, app->prevMousePos.x, app->prevMousePos.y);
            }
        }
    }

    void generate_scene() {
        std::random_device rd;
        std::mt19937 rng(rd());

        std::uniform_real_distribution<float> unit_vec(-1.f, 1.f);
        std::uniform_real_distribution<float> pos(min_distance, max_distance);
        std::uniform_real_distribution<float> vel(min_velocity, max_velocity);
        std::uniform_real_distribution<float> mass(min_mass, max_mass);
        std::uniform_real_distribution<float> density(min_density, max_density);
        std::uniform_real_distribution<float> temp(min_temperature, max_temperature);
        std::uniform_real_distribution<float> col(0.f, 1.f);
        
        pBuffer.clear();


        pBuffer.push_back(Particle({
            .pos = glm::vec3(0.f),
            .vel = glm::vec3(0.f),
            .acc = glm::vec3(0.f),
            .mass = 1e+10,
            .temp = central_temperature,
            .radius = cbrt((3.f * (1e+10f / 10e+9f)) / (4.f * (float)(M_PI))),

            .albedo = glm::vec3(0.f, 0.f, 0.f),
            .emissionColor = glm::vec3(1.f, 1.f, 1.f),
            .emissionStrength = 50.f,
            .metallicity = 0.f,
            .roughness = 0.5f
        }));
        int tries = 0;
        for (int i = 0; i < num_particles; i++) {
            float m = mass(rng);
            float d = density(rng);
            float r = cbrt((3.f * (m / d)) / (4.f * (float)(M_PI)));

            glm::vec3 p = { unit_vec(rng), !disk_only * unit_vec(rng), unit_vec(rng) };
            p = glm::normalize(p) * pos(rng);

            // check if overlapping with any of the previous particles
            bool abort_flag = false;
            for (int j = 0; j <= i; j++) if (glm::distance(pBuffer[j].pos, p) < pBuffer[j].radius + r) abort_flag = true;
            if (abort_flag && tries < 100) {
                i -= 1;
                tries += 1;
                continue;
            }
            tries = 0;

            glm::vec3 v{};
            if (orbital_velocity) v = glm::normalize(glm::cross(p, p + glm::vec3(0.f, 1.f, 0.f))) * sqrt(6.67430e-11f * (1e+10f + m) / glm::length(p));
            else v = glm::normalize(glm::vec3(unit_vec(rng), unit_vec(rng), unit_vec(rng))) * vel(rng);

            glm::vec3 c = { col(rng), col(rng), col(rng) };
            
            pBuffer.push_back(Particle({
                .pos = p,
                .vel = v,
                .acc = { 0.f, 0.f, 0.f },
                .mass = m,
                .temp = temp(rng),
                .radius = r,

                .albedo = c,
                .emissionColor = c,
                .emissionStrength = 0.f,
                .metallicity = 0.f,
                .roughness = 1.f,
            }));
        }

        glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo);
        glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
        glBufferData(GL_SHADER_STORAGE_BUFFER, pBuffer.size() * sizeof(Particle), reinterpret_cast<float*>(pBuffer.data()), GL_DYNAMIC_DRAW);
        glUniform1i(glGetUniformLocation(shader.id, "numParticles"), pBuffer.size());
    }

    void mainloop() {
        double lastFrame = 0;
        double fps = 0;

        do {
            double currentTime = glfwGetTime();
            double dt = currentTime - lastFrame;
            fps = 1.0 / dt;

            glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo);
            glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
            GLvoid* p = glMapBuffer(GL_SHADER_STORAGE_BUFFER, GL_READ_ONLY);
            memcpy(reinterpret_cast<float*>(pBuffer.data()), p, pBuffer.size() * sizeof(Particle));
            glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);
            glfwGetCursorPos(window, &mousePos.x, &mousePos.y);
            glm::vec2 coord = (glm::vec2({ mousePos.x, res.y - mousePos.y })) / glm::vec2(res);
            glm::mat4 view;
            if (lockedToParticle) view = glm::lookAt(camera.position, camera.position - camera.localCoords, { 0.f, 1.f, 0.f });
            else view = glm::lookAt(camera.position, camera.position + camera.direction, { 0.f, 1.f, 0.f });
            glm::mat4 proj = glm::perspective(glm::radians(camera.fov), static_cast<float>(res.x) / static_cast<float>(res.y), camera.nearPlane, camera.farPlane);
            glm::mat4 invView = glm::inverse(view);
            glm::mat4 invProj = glm::inverse(proj);
            glm::vec3 direction = glm::vec3(invView * glm::vec4(glm::normalize(glm::vec3(invProj * glm::vec4(2.f * coord - 1.f, 1.f, 1.f))), 0));
            int closest = -1;
            float minT = std::numeric_limits<float>::max();
            for (int i = 0; i < pBuffer.size(); i++) {
                if (lockedToParticle && i == following) continue;
                Particle p = pBuffer[i];

                glm::vec3 origin = camera.position - p.pos;
                float a = glm::dot(direction, direction);
                float b = 2.f * glm::dot(origin, direction);
                float c = glm::dot(origin, origin) - p.radius * p.radius;

                float d = b * b - 4.f * a * c;
                if (d <= 0.f) continue;
                float t = (-b - sqrt(d)) / (2.f * a);
                if (t < minT && t >= 0) {
                    minT = t;
                    closest = i;
                }
            }
            hoveringParticle = closest != -1.f;
            if (hoveringParticle) hovering = closest;

            glfwPollEvents();
            camera.processInput(this->keys, static_cast<float>(dt), lockedToParticle ? pBuffer.data() + following : nullptr);
            bool imguiEnable = !camera.mouseLocked || (currentTime - lastSpeedChange < 2.0 || hoveringParticle);
            ImGui_ImplOpenGL3_NewFrame();
            ImGui_ImplGlfw_NewFrame();
            ImGui::NewFrame();
            ImVec2 settingsSize;
            ImVec2 sceneGenSize;
            if (!camera.mouseLocked) {
                ImGui::PushFont(ImGui::font);
                ImGui::SetNextWindowPos({ 10.f, 10.f });
                if (ImGui::Begin("Settings", nullptr,
                    ImGuiWindowFlags_NoScrollbar |
                    ImGuiWindowFlags_NoScrollWithMouse |
                    ImGuiWindowFlags_AlwaysAutoResize |
                    ImGuiWindowFlags_NoMove
                )) {
                    ImGui::Text("FPS: %.3g   Frametime: %.3g ms", fps, 1e+3 * (currentTime - lastFrame));
                    ImGui::SeparatorText("Simulation");
                    ImGui::SliderFloat("Time step", &timeStep, 0.f, 100.f, "%.9g seconds", ImGuiSliderFlags_Logarithmic | ImGuiSliderFlags_NoRoundToFormat);

                    const char* types[] = { "Elastic", "Inelastic", "No collision"};
                    const char* preview = types[collisionType];

                    if (ImGui::BeginCombo("Collision type", preview)) {
                        for (int n = 0; n < 3; n++) {
                            const bool is_selected = (collisionType == n);
                            if (ImGui::Selectable(types[n], is_selected)) {
                                collisionType = n;
                            }
                            if (is_selected) ImGui::SetItemDefaultFocus();
                        }
                        ImGui::EndCombo();
                    }

                    ImGui::SeparatorText("Environment");
                    ImGui::Checkbox("Milky way background", &showMilkyway);
                    if (ImGui::ColorEdit3("Ambient light", &ambientLight[0]))
                        glUniform3f(glGetUniformLocation(shader.id, "ambientLight"), ambientLight.r, ambientLight.g, ambientLight.b);
                    ImGui::SeparatorText("Graphics");
                    if (ImGui::ToggleButton("Global Illumination", &globalIllumination)) {
                        glUniform1i(glGetUniformLocation(shader.id, "globalIllumination"), globalIllumination);
                    }
                    if (globalIllumination) {
                        if (ImGui::SliderInt("Samples per pixel", &numRaysPerPixel, 1, 5000, "%d", ImGuiSliderFlags_Logarithmic | ImGuiSliderFlags_NoRoundToFormat))
                            glUniform1i(glGetUniformLocation(shader.id, "numRaysPerPixel"), numRaysPerPixel);
                    }
                    ImGui::Separator();
                    if (!globalIllumination) {
                        if (ImGui::ToggleButton("Shadows", &shadows))
                            glUniform1i(glGetUniformLocation(shader.id, "shadows"), shadows);
                    }
                    ImGui::SeparatorText("Camera");
                    ImGui::DragFloat3("Position", glm::value_ptr(camera.position), 0.1f, -FLT_MAX, FLT_MAX);
                    ImGui::SliderFloat("FOV", &camera.fov, 1, 100, "%.3g");
                    ImGui::SliderFloat("Sensitivity", &camera.sensitivity, 0.01f, 0.2f, "%.5g");
                }
                ImGui::PopFont();
                settingsSize = ImGui::GetWindowSize();
                ImGui::End();

                ImGui::PushFont(ImGui::font);
                ImGui::SetNextWindowPos({ 10.f, settingsSize.y + 20.f });
                ImGui::SetNextWindowSize(ImVec2(settingsSize.x, 0.f));
                if (ImGui::Begin("Scene Generator", nullptr,
                    ImGuiWindowFlags_NoScrollbar |
                    ImGuiWindowFlags_NoScrollWithMouse |
                    ImGuiWindowFlags_AlwaysAutoResize |
                    ImGuiWindowFlags_NoMove
                )) {
                    ImGui::SliderInt("# Particles", &num_particles, 0, 1e+4, "%d", ImGuiSliderFlags_AlwaysClamp | ImGuiSliderFlags_Logarithmic);
                    ImGui::PushItemWidth(100);
                    ImGui::SeparatorText("Distance");
                    ImGui::DragFloat("Min##distance", &min_distance, min_distance / 20.f, FLT_MIN, std::min(FLT_MAX, max_distance), "%.9g m", ImGuiSliderFlags_AlwaysClamp); ImGui::SameLine();
                    ImGui::DragFloat("Max##distance", &max_distance, max_distance / 20.f, std::max(FLT_MIN, min_distance), FLT_MAX, "%.9g m", ImGuiSliderFlags_AlwaysClamp);
                    ImGui::SeparatorText("Mass");
                    ImGui::DragFloat("Min##mass", &min_mass, min_mass / 20.f, FLT_MIN, std::min(FLT_MAX, max_mass), "%.9g kg", ImGuiSliderFlags_AlwaysClamp); ImGui::SameLine();
                    ImGui::DragFloat("Max##mass", &max_mass, max_mass / 20.f, std::max(FLT_MIN, min_mass), FLT_MAX, "%.9g kg", ImGuiSliderFlags_AlwaysClamp);
                    ImGui::SeparatorText("Density");
                    ImGui::DragFloat("Min##density", &min_density, min_density / 20.f, FLT_MIN, std::min(FLT_MAX, max_density), "%.9g kg/m^3", ImGuiSliderFlags_AlwaysClamp); ImGui::SameLine();
                    ImGui::DragFloat("Max##density", &max_density, max_density / 20.f, std::max(FLT_MIN, min_density), FLT_MAX, "%.9g kg/m^3", ImGuiSliderFlags_AlwaysClamp);
                    ImGui::SeparatorText("Temperature");
                    ImGui::DragFloat("Min##temperature", &min_temperature, min_temperature / 20.f, 0.f, std::min(FLT_MAX, max_temperature), "%.9g K", ImGuiSliderFlags_AlwaysClamp); ImGui::SameLine();
                    ImGui::DragFloat("Max##temperature", &max_temperature, max_temperature / 20.f, std::max(0.f, min_temperature), FLT_MAX, "%.9g K", ImGuiSliderFlags_AlwaysClamp);
                    ImGui::SeparatorText("Velocity");
                    ImGui::Checkbox("Automatic Orbital Velocity", &orbital_velocity);
                    if (!orbital_velocity) {
                        ImGui::DragFloat("Min##velocity", &min_velocity, min_velocity / 20.f, 0.f, std::min(FLT_MAX, max_velocity), "%.9g m/s", ImGuiSliderFlags_AlwaysClamp); ImGui::SameLine();
                        ImGui::DragFloat("Max##velocity", &max_velocity, max_velocity / 20.f, std::max(0.f, min_velocity), FLT_MAX, "%.9g m/s", ImGuiSliderFlags_AlwaysClamp);
                    }
                    ImGui::Checkbox("No vertical component", &disk_only);
                    ImGui::PopItemWidth();
                    if (ImGui::Button("Load", ImVec2(100, 0))) generate_scene();
                }
                ImGui::PopFont();
                sceneGenSize = ImGui::GetWindowSize();
                ImGui::End();

                if (selectedParticle) {
                    ImGui::PushFont(ImGui::font);
                    ImGui::SetNextWindowPos({ 10.f, sceneGenSize.y + settingsSize.y + 30.f });
                    ImGui::SetNextWindowSize(ImVec2(sceneGenSize.x, 0.f));
                    ImGui::Begin("Selected particle", nullptr,
                        ImGuiWindowFlags_AlwaysAutoResize |
                        ImGuiWindowFlags_NoMove
                    );
                    Particle p = pBuffer[selected];
                    bool update = false;
                    float velocity = glm::length(p.vel);
                    update |= ImGui::DragFloat3("Position", glm::value_ptr(p.pos), 0.02f);
                    if (update |= ImGui::DragFloat("Velocity", &velocity, (velocity / 10), FLT_MIN, FLT_MAX, "%.3g m/s", ImGuiSliderFlags_AlwaysClamp)) {
                        p.vel = glm::normalize(p.vel) * velocity;
                    }
                    update |= ImGui::DragFloat("Mass", &p.mass, (p.mass / 10.f), FLT_MIN, FLT_MAX, "%.9g kg", ImGuiSliderFlags_AlwaysClamp);
                    update |= ImGui::DragFloat("Temperature", &p.temp, (p.temp / 10.f), FLT_MIN, FLT_MAX, "%.9g K", ImGuiSliderFlags_AlwaysClamp);
                    update |= ImGui::DragFloat("Radius", &p.radius, (p.radius / 10.f), FLT_MIN, FLT_MAX, "%.9g m", ImGuiSliderFlags_AlwaysClamp);
                    ImGui::SeparatorText("Appearance");
                    update |= ImGui::ColorEdit3("Albedo", glm::value_ptr(p.albedo));
                    update |= ImGui::ColorEdit3("Emission Color", glm::value_ptr(p.emissionColor));
                    update |= ImGui::DragFloat("Luminosity", &p.emissionStrength, 0.5f, FLT_MIN, FLT_MAX);
                    update |= ImGui::SliderFloat("Roughness", &p.roughness, 0.f, 1.f);
                    ImGui::SeparatorText("Actions");
                    if (!(lockedToParticle && following == selected) && ImGui::Button("Follow")) {
                        lockedToParticle = true;
                        following = selected;
                    }
                    else if (lockedToParticle && following == selected && ImGui::Button("Unfollow")) {
                        lockedToParticle = false;
                        camera.direction = -camera.localCoords;
                        camera.yaw -= 180;
                        camera.pitch = -camera.pitch;
                    }

                    ImGui::SameLine();
                    if (ImGui::Button("Delete")) {
                        p.mass = 0.f;
                        p.radius = 0.f;
                        if (lockedToParticle && following == selected) {
                            lockedToParticle = false;
                            camera.direction = -camera.localCoords;
                            //camera.yaw -= 180;
                            camera.pitch -= 180;
                        }
                        selectedParticle = false;
                        update = true;
                    }
                    if (update) {
                        glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo);
                        glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
                        glBufferSubData(GL_SHADER_STORAGE_BUFFER, static_cast<GLintptr>(selected * sizeof(Particle)), sizeof(Particle), reinterpret_cast<float*>(&p));
                    }
                    ImGui::SetWindowPos({ res.x / 2.f - ImGui::GetWindowWidth() / 2.f, 30 });
                    ImGui::PopFont();
                    ImGui::End();
                }
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
            if (hoveringParticle && !camera.mouseLocked && !ImGui::GetIO().WantCaptureMouse && glfwGetInputMode(window, GLFW_CURSOR) != GLFW_CURSOR_DISABLED) {
                ImGui::PushFont(ImGui::font);
                double x, y;
                glfwGetCursorPos(window, &x, &y);
                ImGui::SetNextWindowPos({ static_cast<float>(x + 10.f), static_cast<float>(y) });
                ImGui::Begin("##pInfo", nullptr,
                    ImGuiWindowFlags_AlwaysAutoResize |
                    ImGuiWindowFlags_NoCollapse |
                    ImGuiWindowFlags_NoTitleBar |
                    ImGuiWindowFlags_NoMove
                );
                Particle p = pBuffer[hovering];
                ImGui::Text("X:% 06f", p.pos.x); ImGui::SameLine();
                ImGui::Text("Y:% 06f", p.pos.y); ImGui::SameLine();
                ImGui::Text("Z:% 06f", p.pos.z);
                ImGui::Text("Velocity: %.3g m/s", glm::length(p.vel));
                ImGui::Text("Acceleration: %.3g m/s^2", glm::length(p.acc));
                ImGui::Text("Mass: %.9g kg", p.mass);
                ImGui::Text("Temperature: %.9g K", p.temp);
                ImGui::Text("Radius: %.9g m", p.radius);
                ImGui::Text("Density: %.9g kg/m^3", p.mass / (4.f * M_PI * pow(p.radius, 3) / 3.f));
                ImGui::SetWindowPos({ res.x / 2.f - ImGui::GetWindowWidth() / 2.f, 30 });
                ImGui::PopFont();
                ImGui::End();
            }
            ImGui::Render();
            glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            camera.projMat(res.x, res.y, shader.id);
            glUniform1f(glGetUniformLocation(shader.id, "uTimeDelta"), timeStep * dt);
            glUniform1f(glGetUniformLocation(shader.id, "uTime"), currentTime);
            glDrawArrays(GL_TRIANGLES, 0, 36);

            cmptshader.use();
            glUniform1f(glGetUniformLocation(cmptshader.id, "uTimeDelta"), timeStep* dt);
            glUniform1i(glGetUniformLocation(cmptshader.id, "collisionType"), collisionType);
            glDispatchCompute(pBuffer.size(), 1, 1);
            glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
            shader.use();

            lastFrame = currentTime;
            ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
            glfwSwapBuffers(window);
        } while (!glfwWindowShouldClose(this->window));

        glfwDestroyWindow(window);
        glfwTerminate();
    }
};

int main() {
    NBodiment app = NBodiment();
    app.mainloop();
    return 0;
}