#define VERSION "0.2"

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
#include <functional>

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
        ImGui::SameLine(p.x + 22.f);
        ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 3.f);
        ImGui::Text(name);
        return false;
    }

    static void HelpMarker(const char* desc) {
        ImGui::TextDisabled("(?)");
        if (ImGui::BeginItemTooltip()) {
            ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
            ImGui::TextUnformatted(desc);
            ImGui::PopTextWrapPos();
            ImGui::EndTooltip();
        }
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
        colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.70f);

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
    constexpr glm::dmat3 xyz_to_rgb( // matrix to convert from CIE 1931 XYZ color space to sRGB
         3.2404542, -1.5371385, -0.4985314,
        -0.9692660,  1.8760108,  0.0415560,
         0.0556434, -0.2040259,  1.0572252
    );
    constexpr double h = 6.62607015e+34; // planck's constant
    constexpr double c = 299792458.0; // speed of light
    constexpr double c_1 = 3.741771852e-16; // first radiation constant
    constexpr double c_2 = 1.438776877e-02; // second radiation constant
    constexpr double k_b = 1.380649e-23; // boltzmann constant
    constexpr double b = 2.897771955e-3; // wien's displacement constant

    // https://en.wikipedia.org/wiki/Planckian_locus#The_Planckian_locus_in_the_XYZ_color_space
    static double M(double w, double t) { // black body spectral radiant exitance
        return (2.0 * M_PI * h * pow(c, 2)) / (pow(w, 5) * (exp((h * c / k_b) / (w * t)) - 1));
    }

    // https://en.wikipedia.org/wiki/CIE_1931_color_space#Color_matching_functions
    static double g(double x, double mu, double t_1, double t_2) {
        return exp(-pow((x < mu) ? t_1 : t_2, 2) * pow(x - mu, 2) / 2.0);
    }
    static double x(double w) { return 1.056 * g(w, 599.8, 0.0264, 0.0323) + 0.362 * g(w, 442.0, 0.0624, 0.0374) - 0.065 * g(w, 501.1, 0.0490, 0.0382); }
    static double y(double w) { return 0.821 * g(w, 568.8, 0.0218, 0.0247) + 0.286 * g(w, 530.9, 0.0613, 0.0322); }
    static double z(double w) { return 1.217 * g(w, 437.0, 0.0845, 0.0278) + 0.681 * g(w, 459.0, 0.0385, 0.0725); }

    static double coord(std::function<double(double)> func, double t) {
        double out = 0;
        for (double w = 300.0; w < 700.0; w++) {
            out += func(w) * M(w, t);
        }
        return out;
    }

    static glm::dvec3 temperature_to_color(double t) {
        glm::dvec3 xyz = { coord(x, t), coord(y, t), coord(z, t) };
        return xyz;
    }
}

#include "resource.h"

class Shader {
    GLuint vertexShader;
    GLuint fragmentShader;
    GLuint vao, vbo;
    GLuint textureID;

    float vertices[12] = {
        -1.0f, -1.0f,
        -1.0f,  1.0f,
         1.0f,  1.0f,
        -1.0f, -1.0f,
         1.0f,  1.0f,
         1.0f, -1.0f
    };
    
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

    static inline void load_textures(int tex) {
        int width, height, nrChannels;
        unsigned char* data;
        for (int i = 0; i < 6; i++) {
            std::string path = std::format("assets/{}{}.jpg", tex, i);
            data = stbi_load(path.c_str(), &width, &height, &nrChannels, 0);
            if (!data) throw Error("Skybox not available");
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
            stbi_image_free(data);
        }
    }

    inline virtual void create() {
        char* vertexSource = read_resource(IDR_VRTX);
        char* fragmentSource = read_resource(IDR_FRAG);

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
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), nullptr);

        glActiveTexture(GL_TEXTURE1);
        glGenTextures(1, &textureID);
        glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);
        load_textures(0);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);

        glUniform1i(glGetUniformLocation(id, "skybox"), 0);
    }
    inline virtual void use() const { 
        glUseProgram(id);
        glBindVertexArray(vao);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);
    }
    inline void free() { glDeleteProgram(id); }
};

class PostProcessing : public Shader {
public:
    GLuint fbo;

    GLuint vertexShader;
    GLuint upsampleShader;
    GLuint dwsampleShader;
    GLuint displaytShader;

    GLuint upsampleProgramID;
    GLuint dwsampleProgramID;
    GLuint displaytProgramID;

    GLuint vao, vbo;

    float vertices[24] = {
        -1.0f, -1.0f,  0.0f,  0.0f,
        -1.0f,  1.0f,  0.0f,  1.0f,
         1.0f,  1.0f,  1.0f,  1.0f,
        -1.0f, -1.0f,  0.0f,  0.0f,
         1.0f,  1.0f,  1.0f,  1.0f,
         1.0f, -1.0f,  1.0f,  0.0f
    };

    struct bloomMip {
        glm::vec2 size;
        glm::ivec2 intSize;
        unsigned int texture;
    };
    glm::ivec2 screenSize;
    int mipChainLength;

    std::vector<bloomMip> mMipChain;

    inline PostProcessing(int w, int h, int length) : mipChainLength(length) {
        screenSize = { w, h };
    }
    inline virtual void create() override {
        char* fullquadSource = read_resource(IDR_FULLQUAD);
        char* upsampleSource = read_resource(IDR_UPSAMPLE);
        char* dwsampleSource = read_resource(IDR_DWSAMPLE);
        char* displaytSource = read_resource(IDR_DISPLAYT);

        vertexShader = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vertexShader, 1, &fullquadSource, NULL);
        glCompileShader(vertexShader);
        check_for_errors(vertexShader);

        upsampleShader = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(upsampleShader, 1, &upsampleSource, NULL);
        glCompileShader(upsampleShader);
        check_for_errors(upsampleShader);

        dwsampleShader = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(dwsampleShader, 1, &dwsampleSource, NULL);
        glCompileShader(dwsampleShader);
        check_for_errors(dwsampleShader);

        displaytShader = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(displaytShader, 1, &displaytSource, NULL);
        glCompileShader(displaytShader);
        check_for_errors(displaytShader);

        upsampleProgramID = glCreateProgram();
        glAttachShader(upsampleProgramID, vertexShader);
        glAttachShader(upsampleProgramID, upsampleShader);
        glLinkProgram(upsampleProgramID);

        dwsampleProgramID = glCreateProgram();
        glAttachShader(dwsampleProgramID, vertexShader);
        glAttachShader(dwsampleProgramID, dwsampleShader);
        glLinkProgram(dwsampleProgramID);

        displaytProgramID = glCreateProgram();
        glAttachShader(displaytProgramID, vertexShader);
        glAttachShader(displaytProgramID, displaytShader);
        glLinkProgram(displaytProgramID);

        glDeleteShader(vertexShader);
        glDeleteShader(upsampleShader);
        glDeleteShader(dwsampleShader);

        glUseProgram(upsampleProgramID);
        glUniform1i(glGetUniformLocation(upsampleProgramID, "srcTexture"), 0);
        glUseProgram(dwsampleProgramID);
        glUniform1i(glGetUniformLocation(dwsampleProgramID, "srcTexture"), 0);

        glGenVertexArrays(1, &vao);
        glGenBuffers(1, &vbo);

        glBindVertexArray(vao);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), nullptr);
        glEnableVertexAttribArray(0);

        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
        glEnableVertexAttribArray(1);

        glBindVertexArray(0);

        glGenFramebuffers(1, &fbo);
        glBindFramebuffer(GL_FRAMEBUFFER, fbo);

        glm::vec2 mipSize = screenSize;
        glm::ivec2 mipIntSize = static_cast<glm::ivec2>(screenSize);

        for (unsigned int i = 0; i < mipChainLength; i++) {
            bloomMip mip;

            mip.size = mipSize;
            mip.intSize = mipIntSize;

            glGenTextures(1, &mip.texture);
            glBindTexture(GL_TEXTURE_2D, mip.texture);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_R11F_G11F_B10F, (int)mipSize.x, (int)mipSize.y, 0, GL_RGBA, GL_FLOAT, nullptr);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

            mipSize *= 0.5f;
            mipIntSize /= 2;

            mMipChain.emplace_back(mip);
        }

        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, mMipChain[0].texture, 0);
        unsigned int attachments[1] = { GL_COLOR_ATTACHMENT0 };
        glDrawBuffers(1, attachments);

        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
            throw Error("Error generating framebuffer for bloom shader");
        }

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    inline void render(bool bloom, GLuint srcTexture, float threshold, float exposure, int accumulationFrameIndex) {
        glBindFramebuffer(GL_FRAMEBUFFER, fbo);
        this->renderDownsamples(srcTexture, threshold, accumulationFrameIndex);
        this->renderUpsamples(exposure, accumulationFrameIndex);

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glViewport(0, 0, screenSize.x, screenSize.y);

        glUseProgram(displaytProgramID);
        glUniform1i(glGetUniformLocation(displaytProgramID, "screenTexture"), 0);
        glUniform2i(glGetUniformLocation(displaytProgramID, "screenSize"), screenSize.x, screenSize.y);
        glBindVertexArray(vao);
        glActiveTexture(GL_TEXTURE0);

        glEnable(GL_BLEND);
        glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);

        glUniform1i(glGetUniformLocation(displaytProgramID, "accumulationFrameIndex"), accumulationFrameIndex);
        glUniform1f(glGetUniformLocation(displaytProgramID, "transparency"), 0.0);
        glBindTexture(GL_TEXTURE_2D, srcTexture);
        glDrawArrays(GL_TRIANGLES, 0, 6);

        if (bloom) {
            glUniform1i(glGetUniformLocation(displaytProgramID, "accumulationFrameIndex"), -1);
            glUniform1f(glGetUniformLocation(displaytProgramID, "transparency"), 1.0);
            glBindTexture(GL_TEXTURE_2D, mMipChain[0].texture);
            glDrawArrays(GL_TRIANGLES, 0, 6);
        }

        glDisable(GL_BLEND);

        glBindVertexArray(0);
    }

    inline void renderDownsamples(GLuint srcTexture, float threshold, int accumulationFrameIndex) {
        glUseProgram(dwsampleProgramID);
        glUniform2f(glGetUniformLocation(dwsampleProgramID, "srcResolution"), screenSize.x, screenSize.y);
        glUniform1f(glGetUniformLocation(dwsampleProgramID, "threshold"), threshold);
        glUniform1i(glGetUniformLocation(dwsampleProgramID, "accumulationFrameIndex"), accumulationFrameIndex);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, srcTexture);

        glBindVertexArray(vao);

        for (int i = 0; i < mMipChain.size(); i++) {
            const bloomMip& mip = mMipChain[i];
            glViewport(0, 0, mip.size.x, mip.size.y);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, mip.texture, 0);
            glUniform1i(glGetUniformLocation(dwsampleProgramID, "depth"), i);

            glDrawArrays(GL_TRIANGLES, 0, 6);

            glUniform2f(glGetUniformLocation(dwsampleProgramID, "srcResolution"), mip.size.x, mip.size.y);
            glBindTexture(GL_TEXTURE_2D, mip.texture);
        }
        glBindVertexArray(0);
        glUseProgram(0);
    }
    inline void renderUpsamples(float exposure, int accumulationFrameIndex) {
        glUseProgram(upsampleProgramID);
        glUniform1f(glGetUniformLocation(upsampleProgramID, "exposure"), exposure);

        glEnable(GL_BLEND);
        glBlendFunc(GL_ONE, GL_ONE);

        glBindVertexArray(vao);

        for (int i = mMipChain.size() - 1; i > 0; i--) {
            const bloomMip& mip = mMipChain[i];
            const bloomMip& nextMip = mMipChain[i - 1];

            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, mip.texture);

            glViewport(0, 0, nextMip.size.x, nextMip.size.y);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, nextMip.texture, 0);
            glUniform1i(glGetUniformLocation(upsampleProgramID, "depth"), i);

            glDrawArrays(GL_TRIANGLES, 0, 6);
        }

        glBindVertexArray(0);
        glDisable(GL_BLEND);
        glUseProgram(0);
    }

    inline void free() {
        for (int i = 0; i < mMipChain.size(); i++) {
            glDeleteTextures(1, &mMipChain[i].texture);
            mMipChain[i].texture = 0;
        }
        glDeleteFramebuffers(1, &fbo);
        glDeleteProgram(upsampleProgramID);
        glDeleteProgram(dwsampleProgramID);
    }
};

class ComputeShader : public Shader {
    GLuint computeShader;
public:
    inline virtual void create() override {
        int success;
        char infoLog[1024];
        char* computeSource = read_resource(IDR_CMPT);

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

    inline virtual void use() const override {
        glUseProgram(id);
    }
};

struct Particle {
    glm::vec3 pos;
    glm::vec3 vel;
    glm::vec3 acc;
    float mass;
    float temp;
    float radius;

    glm::vec3 albedo;
    glm::vec3 emissionColor;
    glm::vec3 absorptionColor;
    float luminosity;
    float specularity;
    float metallicity;
    float translucency;
    float refractive_index;
    float blurriness;
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
            localCoords.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
            localCoords.y = sin(glm::radians(pitch));
            localCoords.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
            localCoords = glm::normalize(localCoords) * (following->radius * proximity);
        }
    }
};

struct Scene {
    int num_particles = 100;

    float min_distance = 2.5f;
    float max_distance = 5.f;
    float min_mass = 1e+6f;
    float max_mass = 1e+7f;
    float min_density = 10e+8f;
    float max_density = 10e+9f;
    float min_temperature = 0.f;
    float max_temperature = 0.f;
    float min_velocity = 0.f;
    float max_velocity = 1.f;

    bool orbital_velocity = true;
    bool disk_only = false;

    bool central_particle = true;
    float central_mass = 1e+10f;
    float central_density = 1e+10f;
    float central_temperature = 3e+3;
    float central_luminosity = 20.f;
};

struct Controls {
    GLenum forward   = GLFW_KEY_W;
    GLenum backward  = GLFW_KEY_S;
    GLenum leftward  = GLFW_KEY_A;
    GLenum rightward = GLFW_KEY_D;
    GLenum upward    = GLFW_KEY_SPACE;
    GLenum downward  = GLFW_KEY_LEFT_SHIFT;
    GLenum del       = GLFW_KEY_DELETE;
    GLenum camctrl   = GLFW_KEY_LEFT_CONTROL;
    GLenum pause     = GLFW_KEY_Q;
    GLenum reverse   = GLFW_KEY_E;
    GLenum speedup   = GLFW_KEY_R;
    GLenum speeddown = GLFW_KEY_F;
    GLenum follow    = GLFW_KEY_X;
    GLenum toggleGI  = GLFW_KEY_G;
};

class NBodiment {
public:
    Shader* shader;
    ComputeShader* cmptshader;
    PostProcessing* pprocshader;
    Camera camera;
    Scene scene;
    Controls controls;
    GLuint ssbo;
    GLuint fbo;
    GLuint screenTexture;

    GLFWwindow* window;
    GLFWmonitor* monitor;
    GLFWcursor* arrow;
    GLFWcursor* hand;

    std::vector<Particle> original;
    std::vector<Particle> pBuffer;

    glm::ivec2 res;
    glm::ivec2 pos = { 60, 60 };
    std::bitset<6> keys{ 0x0 };
    glm::dvec2 prevMousePos{ 0.f, 0.f };
    double lastSpeedChange = -5.0;
    float timeStep = 1.0f;
    bool paused = false;
    bool reverse = false;
    int collisionType = 0;

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
    bool bloom = true;
    float bloomThreshold = 1.f;
    float exposure = 0.05f;
    int accumulationFrameIndex = 0;
    int skyboxImage = 0;

    glm::vec3 ambientLight = { 0.1f, 0.1f, 0.1f };

    void load_texture(const char* path, unsigned int binding) {
        int width, height, nrChannels;
        unsigned char* data = stbi_load(path, &width, &height, &nrChannels, 0);
        if (!data) throw Error("texture not found in assets");
        GLuint texture;

        glGenTextures(1, &texture);
        glActiveTexture(GL_TEXTURE0 + binding);
        glBindTexture(GL_TEXTURE_2D, texture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
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
        if (window == nullptr) throw Error("Failed to create OpenGL context");
        glfwMakeContextCurrent(window);
        glfwSwapInterval(0);
        glfwSetWindowUserPointer(window, this);

        if (glfwRawMouseMotionSupported()) glfwSetInputMode(window, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);
        glfwGetCursorPos(window, &prevMousePos.x, &prevMousePos.y);

        if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress))) throw Error("Failed to load GLAD");

        glGenBuffers(1, &ssbo);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, ssbo);

        cmptshader = new ComputeShader();
        cmptshader->create();
        cmptshader->use();
        glShaderStorageBlockBinding(cmptshader->id, glGetProgramResourceIndex(cmptshader->id, GL_SHADER_STORAGE_BLOCK, "vBuffer"), 0);
        glUniform1i(glGetUniformLocation(cmptshader->id, "collisionType"), collisionType);

        pprocshader = new PostProcessing(res.x, res.y, 8);
        pprocshader->create();

        shader = new Shader();
        shader->create();
        generate_scene();
        shader->use();
        glShaderStorageBlockBinding(shader->id, glGetProgramResourceIndex(shader->id, GL_SHADER_STORAGE_BLOCK, "vBuffer"), 0);
        glUniform3f(glGetUniformLocation(shader->id, "cameraPos"), camera.position.x, camera.position.y, camera.position.z);
        glUniform3f(glGetUniformLocation(shader->id, "ambientLight"), ambientLight.r, ambientLight.g, ambientLight.b);
        glUniform1i(glGetUniformLocation(shader->id, "numParticles"), pBuffer.size());
        glUniform1i(glGetUniformLocation(shader->id, "spp"), numRaysPerPixel);
        glUniform1i(glGetUniformLocation(shader->id, "globalIllumination"), globalIllumination);
        glUniform1i(glGetUniformLocation(shader->id, "shadows"), shadows);

        load_texture("assets/8k_earth_daymap.jpg", 2);
        load_texture("assets/8k_earth_clouds.jpg", 3);
        load_texture("assets/8k_sun.jpg", 4);
        load_texture("assets/temperature.jpg", 5);

        glEnable(GL_MULTISAMPLE);

        glGenTextures(1, &screenTexture);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, screenTexture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, res.x, res.y, 0, GL_RGBA, GL_FLOAT, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        
        glGenFramebuffers(1, &fbo);
        glBindFramebuffer(GL_FRAMEBUFFER, fbo);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, screenTexture, 0);

        glfwSetFramebufferSizeCallback(window, on_windowResize);
        glfwSetMouseButtonCallback(window, on_mousePress);
        glfwSetKeyCallback(window, on_keyPress);
        glfwSetScrollCallback(window, on_mouseScroll);
        glfwSetCursorPosCallback(window, on_mouseMove);

        arrow = glfwCreateStandardCursor(GLFW_ARROW_CURSOR);
        hand = glfwCreateStandardCursor(GLFW_HAND_CURSOR);

        on_windowResize(window, res.x, res.y);

        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO(); (void)io;
        io.Fonts->AddFontDefault();
        ImGui::font = io.Fonts->AddFontFromFileTTF("C:\\Windows\\Fonts\\consola.ttf", 11.f);
        IM_ASSERT(ImGui::font != NULL);
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
        io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
        io.IniFilename = NULL;
        io.LogFilename = NULL;
        ImGui::load_theme();
        ImGui_ImplGlfw_InitForOpenGL(window, true);
        ImGui_ImplOpenGL3_Init("#version 430");

        camera = Camera();
    }

    static inline void on_windowResize(GLFWwindow* window, int w, int h) {
        NBodiment* app = static_cast<NBodiment*>(glfwGetWindowUserPointer(window));
        app->res = { w, h };
        glViewport(0, 0, app->res.x, app->res.y);
        glUniform2f(glGetUniformLocation(app->shader->id, "screenSize"), w, h);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, app->screenTexture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, app->res.x, app->res.y, 0, GL_RGBA, GL_FLOAT, NULL);
        
        app->pprocshader->screenSize = app->res;
        glm::vec2 mipSize = app->res;
        glm::ivec2 mipIntSize = static_cast<glm::ivec2>(app->res);
        for (PostProcessing::bloomMip& mip : app->pprocshader->mMipChain) {
            mip.size = mipSize;
            mip.intSize = mipIntSize;
            glBindTexture(GL_TEXTURE_2D, mip.texture);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_R11F_G11F_B10F, (int)mipSize.x, (int)mipSize.y, 0, GL_RGBA, GL_FLOAT, nullptr);
            mipSize *= 0.5f;
            mipIntSize /= 2;
        }

        app->accumulationFrameIndex = 0;
    }

    static inline void on_keyPress(GLFWwindow* window, int key, int scancode, int action, int mods) {
        NBodiment* app = static_cast<NBodiment*>(glfwGetWindowUserPointer(window));
        switch (action) {
        case GLFW_PRESS:
            app->keys |= ((int)(key == GLFW_KEY_W) | (int)(key == GLFW_KEY_A) << 1 | (int)(key == GLFW_KEY_S) << 2 | (int)(key == GLFW_KEY_D) << 3 | (int)(key == GLFW_KEY_SPACE) << 4 | (int)(key == GLFW_KEY_LEFT_SHIFT) << 5);
            if (key == GLFW_KEY_ESCAPE) {
                glfwSetWindowShouldClose(window, true);
            } if (key == GLFW_KEY_F11) {
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
            } if (key == app->controls.camctrl) {
                app->camera.mouseLocked ^= 1;
                glfwGetCursorPos(window, &app->prevMousePos.x, &app->prevMousePos.y);
                glfwSetInputMode(window, GLFW_CURSOR, app->camera.mouseLocked ? GLFW_CURSOR_DISABLED : GLFW_CURSOR_NORMAL);
            } if (key == app->controls.del) {
                if (app->lockedToParticle && app->following == app->selected) return;
                Particle p = app->pBuffer[app->selected];
                p.mass = 0.f;
                p.radius = 0.f;
                app->selectedParticle = false;
                glBindBuffer(GL_SHADER_STORAGE_BUFFER, app->ssbo);
                glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
                glBufferSubData(GL_SHADER_STORAGE_BUFFER, static_cast<GLintptr>(app->selected * sizeof(Particle)), sizeof(Particle), reinterpret_cast<float*>(&p));
            } if (key == app->controls.pause) {
                app->paused ^= 1;
            } if (key == app->controls.reverse) {
                app->reverse ^= 1;
            } if (key == app->controls.speedup) {
                app->timeStep *= 2;
            } if (key == app->controls.speeddown) {
                app->timeStep /= 2;
            } if (key == app->controls.follow) {
                if (app->lockedToParticle) {
                    app->lockedToParticle = false;
                    app->camera.direction = -app->camera.localCoords;
                    app->camera.yaw -= 180;
                    app->camera.pitch = -app->camera.pitch;
                }
                else if (app->selectedParticle) {
                    app->lockedToParticle = true;
                    app->following = app->selected;
                }
            } if (key == app->controls.toggleGI) {
                app->globalIllumination ^= 1;
                glUniform1i(glGetUniformLocation(app->shader->id, "globalIllumination"), app->globalIllumination);
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
            app->accumulationFrameIndex = 0;
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
                if (app->hoveringParticle && app->selected == app->hovering) {
                    app->lockedToParticle = true;
                    app->following = app->selected;
                    app->accumulationFrameIndex = 0;
                }
                app->selectedParticle = app->hoveringParticle;
                app->selected = app->hovering;
                if (app->lockedToParticle && !app->selectedParticle) {
                    app->selected = app->following;
                    app->selectedParticle = true;
                }
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
        std::uniform_real_distribution<float> pos(scene.min_distance, scene.max_distance);
        std::uniform_real_distribution<float> vel(scene.min_velocity, scene.max_velocity);
        std::uniform_real_distribution<float> mass(scene.min_mass, scene.max_mass);
        std::uniform_real_distribution<float> density(scene.min_density, scene.max_density);
        std::uniform_real_distribution<float> temp(scene.min_temperature, scene.max_temperature);
        std::uniform_real_distribution<float> col(0.f, 1.f);
        
        pBuffer.clear();

        pBuffer.push_back(Particle({
            .pos = glm::vec3(0.f),
            .vel = glm::vec3(0.f),
            .acc = glm::vec3(0.f),
            .mass = scene.central_mass,
            .temp = scene.central_temperature,
            .radius = cbrt((3.f * (scene.central_mass / scene.central_density)) / (4.f * (float)(M_PI))),

            .albedo = glm::vec3(0.f, 0.f, 0.f),
            .emissionColor = glm::vec3(1.f, 1.f, 1.f),
            .luminosity = scene.central_luminosity,
            .specularity = 0.f,
            .metallicity = 1.f,
            .translucency = 0.f,
            .refractive_index = 1.f,
            .blurriness = 0.f
        }));
        int tries = 0;
        for (int i = 0; i < scene.num_particles; i++) {
            float m = mass(rng);
            float d = density(rng);
            float r = cbrt((3.f * (m / d)) / (4.f * (float)(M_PI)));

            glm::vec3 p = { unit_vec(rng), !scene.disk_only * unit_vec(rng), unit_vec(rng) };
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
            if (scene.orbital_velocity) v = glm::cross(glm::normalize(p), glm::vec3(0.f, 1.f, 0.f)) * sqrt(6.67430e-11f * (scene.central_mass + m) / glm::length(p));
            else v = glm::normalize(glm::vec3(unit_vec(rng), !scene.disk_only * unit_vec(rng), unit_vec(rng))) * vel(rng);

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
                .luminosity = 0.f,
                .specularity = 0.f,
                .metallicity = 1.f,
                .translucency = 0.f,
                .refractive_index = 1.f,
                .blurriness = 0.f
            }));
        }

        original = pBuffer;

        glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo);
        glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
        glBufferData(GL_SHADER_STORAGE_BUFFER, pBuffer.size() * sizeof(Particle), reinterpret_cast<float*>(pBuffer.data()), GL_DYNAMIC_DRAW);
        glUniform1i(glGetUniformLocation(shader->id, "numParticles"), pBuffer.size());
        accumulationFrameIndex = 0;
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
            
            if (hoveringParticle) {
                hovering = closest;
                if (!ImGui::GetIO().WantCaptureMouse) {
                    ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);
                    glfwSetCursor(window, hand);
                }
            }
            else {
                glfwSetCursor(window, arrow);
                ImGui::SetMouseCursor(ImGuiMouseCursor_Arrow);
            }

            if (lockedToParticle && pBuffer[following].mass == 0) {
                camera.direction = -camera.localCoords;
                camera.yaw -= 180;
                camera.pitch = -camera.pitch;
                lockedToParticle = false;
                if (selected == following) {
                    selectedParticle = false;
                }
            }

            glfwPollEvents();
            camera.processInput(this->keys, static_cast<float>(dt), lockedToParticle ? pBuffer.data() + following : nullptr);
            if (glm::length(camera.velocity) > 0.01f || keys.any()) accumulationFrameIndex = 0;
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
                    if (ImGui::Button("About"))
                        ImGui::OpenPopup("About NBodiment");
                    if (ImGui::BeginPopupModal("About NBodiment", nullptr, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoMove)) {
                        ImGui::Text("Version v" VERSION " (Build date: " __DATE__ " " __TIME__ ")\n\nNBodiment is a gravitational N-body simulator with\nray-tracing and rigid body physics.");
                        ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(150, 150, 150, 255));
                        ImGui::Text(u8"Copyright © 2017-2024 Yilmaz Alpaslan");
                        ImGui::PopStyleColor();
                        if (ImGui::Button("Open GitHub Page"))
                            ShellExecuteW(0, 0, L"https://github.com/Yilmaz4/NBodiment", 0, 0, SW_SHOW);
                        ImGui::SameLine();
                        if (ImGui::Button("Close"))
                            ImGui::CloseCurrentPopup();
                        ImGui::EndPopup();
                    }
                    ImGui::SameLine();
                    if (ImGui::Button("Check for updates")) {
                        // not yet implemented
                    }
                    ImGui::SeparatorText("Simulation");
                    if (ImGui::Button(paused ? "Resume" : "Pause"))
                        paused ^= 1;
                    ImGui::SameLine();
                    if (ImGui::Button("Reverse"))
                        reverse ^= 1;
                    ImGui::SliderFloat("Time step", &timeStep, FLT_MIN, 100.f, "%.9g seconds", ImGuiSliderFlags_Logarithmic | ImGuiSliderFlags_NoRoundToFormat);
                    ImGui::SameLine();
                    ImGui::HelpMarker("Higher values speed up the simulation while decreasing accuracy.");

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
                    ImGui::SameLine();
                    ImGui::HelpMarker("In elastic collisions, particles bounce off each other while conserving momentum. In inelastic collisions, masses are merged and momentum is not conserved.");

                    ImGui::SeparatorText("Environment");
                    if (ImGui::ColorEdit3("Ambient light", &ambientLight[0])) {
                        glUniform3f(glGetUniformLocation(shader->id, "ambientLight"), ambientLight.r, ambientLight.g, ambientLight.b);
                        accumulationFrameIndex = 0;
                    }
                    const char* images[] = { "Milky Way", "Sorsele" };
                    preview = images[skyboxImage];

                    if (ImGui::BeginCombo("Skybox", preview)) {
                        for (int n = 0; n < 2; n++) {
                            const bool is_selected = (skyboxImage == n);
                            if (ImGui::Selectable(images[n], is_selected)) {
                                skyboxImage = n;
                                shader->load_textures(n);
                                accumulationFrameIndex = 0;
                            }
                            if (is_selected) ImGui::SetItemDefaultFocus();
                        }
                        ImGui::EndCombo();
                    }
                    ImGui::SeparatorText("Graphics");
                    if (ImGui::ToggleButton("Global Illumination", &globalIllumination)) {
                        glUniform1i(glGetUniformLocation(shader->id, "globalIllumination"), globalIllumination);
                    }
                    if (globalIllumination) {
                        ImGui::SameLine();
                        if (ImGui::Button("Reset accumulation"))
                            accumulationFrameIndex = 0;
                        if (ImGui::SliderInt("Samples per pixel", &numRaysPerPixel, 1, 5000, "%d", ImGuiSliderFlags_Logarithmic | ImGuiSliderFlags_NoRoundToFormat))
                            glUniform1i(glGetUniformLocation(shader->id, "spp"), numRaysPerPixel);
                    }
                    else {
                        ImGui::SameLine();
                        ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 3.f);
                        ImGui::HelpMarker("Global illumination, or path tracing, is a computer algorithm to produce physically based and realistic-looking 3D scenes by simulating the very nature of light, which causes optical effects like reflection, refraction and soft shadows to naturally emerge instead of having to be explicitly implemented.\n\nPause the simulation to start accumulating samples in order to address noise.");
                    }
                    ImGui::ToggleButton("Bloom", &bloom);
                    if (!globalIllumination) {
                        ImGui::SameLine();
                        if (ImGui::ToggleButton("Shadows", &shadows))
                            glUniform1i(glGetUniformLocation(shader->id, "shadows"), shadows);
                    }
                    if (bloom) {
                        ImGui::DragFloat("Bloom threshold", &bloomThreshold, std::max(bloomThreshold / 20, 1e-2f), 0.f, FLT_MAX, "%.9g", ImGuiSliderFlags_NoRoundToFormat);
                        ImGui::DragFloat("Exposure", &exposure, exposure / 20, FLT_MIN, FLT_MAX, "%.3g", ImGuiSliderFlags_NoRoundToFormat);
                    }
                    
                    ImGui::SeparatorText("Camera");
                    if (ImGui::DragFloat3("Position", glm::value_ptr(camera.position), 0.1f, -FLT_MAX, FLT_MAX))
                        accumulationFrameIndex = 0;
                    if (ImGui::SliderFloat("FOV", &camera.fov, 1, 100, "%.3g"))
                        accumulationFrameIndex = 0;
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
                    ImGui::SliderInt("# Particles", &scene.num_particles, 0, 1e+4, "%d", ImGuiSliderFlags_AlwaysClamp | ImGuiSliderFlags_Logarithmic);
                    ImGui::PushItemWidth(100);
                    ImGui::SeparatorText("Distance");
                    ImGui::DragFloat("Min##distance", &scene.min_distance, scene.min_distance / 20.f, FLT_MIN, std::min(FLT_MAX, scene.max_distance), "%.9g m", ImGuiSliderFlags_AlwaysClamp); ImGui::SameLine();
                    ImGui::DragFloat("Max##distance", &scene.max_distance, scene.max_distance / 20.f, std::max(FLT_MIN, scene.min_distance), FLT_MAX, "%.9g m", ImGuiSliderFlags_AlwaysClamp);
                    ImGui::SeparatorText("Mass");
                    ImGui::DragFloat("Min##mass", &scene.min_mass, scene.min_mass / 20.f, FLT_MIN, std::min(FLT_MAX, scene.max_mass), "%.9g kg", ImGuiSliderFlags_AlwaysClamp); ImGui::SameLine();
                    ImGui::DragFloat("Max##mass", &scene.max_mass, scene.max_mass / 20.f, std::max(FLT_MIN, scene.min_mass), FLT_MAX, "%.9g kg", ImGuiSliderFlags_AlwaysClamp);
                    ImGui::SeparatorText("Density");
                    ImGui::DragFloat("Min##density", &scene.min_density, scene.min_density / 20.f, FLT_MIN, std::min(FLT_MAX, scene.max_density), "%.9g kg/m^3", ImGuiSliderFlags_AlwaysClamp); ImGui::SameLine();
                    ImGui::DragFloat("Max##density", &scene.max_density, scene.max_density / 20.f, std::max(FLT_MIN, scene.min_density), FLT_MAX, "%.9g kg/m^3", ImGuiSliderFlags_AlwaysClamp);
                    ImGui::SeparatorText("Velocity");
                    ImGui::Checkbox("Automatic Orbital Velocity", &scene.orbital_velocity);
                    if (!scene.orbital_velocity) {
                        ImGui::DragFloat("Min##velocity", &scene.min_velocity, std::max(scene.min_velocity / 20.f, 1e-9f), 0.f, std::min(FLT_MAX, scene.max_velocity), "%.9g m/s", ImGuiSliderFlags_AlwaysClamp); ImGui::SameLine();
                        ImGui::DragFloat("Max##velocity", &scene.max_velocity, std::max(scene.max_velocity / 20.f, 1e-9f), std::max(0.f, scene.min_velocity), FLT_MAX, "%.9g m/s", ImGuiSliderFlags_AlwaysClamp);
                    }
                    ImGui::Checkbox("No vertical component", &scene.disk_only);
                    ImGui::SameLine();
                    ImGui::HelpMarker("Particles will be placed on a horizontal plane only.");
                    ImGui::PopItemWidth();
                    ImGui::SeparatorText("Central body");
                    ImGui::DragFloat("Mass", &scene.central_mass, scene.central_mass / 20.f, FLT_MIN, FLT_MAX, "%.9g kg", ImGuiSliderFlags_AlwaysClamp);
                    ImGui::DragFloat("Density", &scene.central_density, scene.central_density / 20.f, FLT_MIN, FLT_MAX, "%.9g kg", ImGuiSliderFlags_AlwaysClamp);
                    ImGui::DragFloat("Emission strength", &scene.central_luminosity, std::max(scene.central_luminosity / 20.f, 1e-2f), 0.f, FLT_MAX, "%.9g", ImGuiSliderFlags_AlwaysClamp);
                    ImGui::SeparatorEx(ImGuiSeparatorFlags_Horizontal, 3.f);
                    
                    if (ImGui::Button("Generate", ImVec2(100, 0))) generate_scene();
                }
                ImGui::PopFont();
                sceneGenSize = ImGui::GetWindowSize();
                ImGui::End();

                auto particleConfig = [](NBodiment* app, int idx, ImVec2 size, ImVec2 pos, const char* title) {
                    ImGui::PushFont(ImGui::font);
                    ImGui::SetNextWindowPos(pos);
                    ImGui::SetNextWindowSize(size);
                    ImGui::Begin(title, nullptr,
                        ImGuiWindowFlags_AlwaysAutoResize |
                        ImGuiWindowFlags_NoMove
                    );
                    Particle p = app->pBuffer[idx];
                    bool update = false;
                    float velocity = glm::length(p.vel);
                    update |= ImGui::DragFloat3("Position", glm::value_ptr(p.pos), 0.01f);
                    if (update |= ImGui::DragFloat("Velocity", &velocity, (velocity / 10), FLT_MIN, FLT_MAX, "%.3g m/s", ImGuiSliderFlags_AlwaysClamp)) {
                        p.vel = glm::normalize(p.vel) * velocity;
                    }
                    update |= ImGui::DragFloat("Mass", &p.mass, (p.mass / 10.f), FLT_MIN, FLT_MAX, "%.9g kg", ImGuiSliderFlags_AlwaysClamp);
                    update |= ImGui::DragFloat("Temperature", &p.temp, (p.temp / 10.f), FLT_MIN, FLT_MAX, "%.9g K", ImGuiSliderFlags_AlwaysClamp);
                    update |= ImGui::DragFloat("Radius", &p.radius, (p.radius / 10.f), FLT_MIN, FLT_MAX, "%.9g m", ImGuiSliderFlags_AlwaysClamp);
                    ImGui::SeparatorText("Appearance");
                    update |= ImGui::ColorEdit3("Albedo", glm::value_ptr(p.albedo));
                    ImGui::SameLine(); ImGui::HelpMarker("Proportion of the incident light that is reflected by the surface.");
                    update |= ImGui::ColorEdit3("Emission Color", glm::value_ptr(p.emissionColor));
                    update |= ImGui::DragFloat("Luminosity", &p.luminosity, 0.5f, FLT_MIN, FLT_MAX);
                    update |= ImGui::SliderFloat("Specularity", &p.specularity, 0.f, 1.f);
                    ImGui::SameLine(); ImGui::HelpMarker("Measure of how microscopically smooth the surface is, which affects how uniformly it reflects incident light.");
                    if ((update |= ImGui::SliderFloat("Metallicity", &p.metallicity, 0.f, 1.f)) && p.metallicity > 1.f - p.translucency)
                        p.metallicity = 1.f - p.translucency;
                    ImGui::SameLine(); ImGui::HelpMarker("Measure of how much of incoming light is reflected.");
                    if ((update |= ImGui::SliderFloat("Translucency", &p.translucency, 0.f, 1.f)) && p.translucency > 1.f - p.metallicity)
                        p.translucency = 1.f - p.metallicity;
                    update |= ImGui::ColorEdit3("Absorption Color", glm::value_ptr(p.absorptionColor));
                    update |= ImGui::SliderFloat("Refractive index", &p.refractive_index, 1.f, 10.f, "%.3f", ImGuiSliderFlags_Logarithmic | ImGuiSliderFlags_NoRoundToFormat);
                    ImGui::SameLine(); ImGui::HelpMarker("Ratio of the speed of light in a vacuum to the speed of light inside the material.\nAir: 1.0003, Water: 1.33, Glass: 1.52, Diamond: 2.41");
                    update |= ImGui::SliderFloat("Blurriness", &p.blurriness, 0.f, 1.f);
                    ImGui::SeparatorText("Actions");
                    if (!(app->lockedToParticle && app->following == idx) && ImGui::Button("Follow")) {
                        app->lockedToParticle = true;
                        app->following = idx;
                        app->accumulationFrameIndex = 0;
                    }
                    else if (app->lockedToParticle && app->following == idx && ImGui::Button("Unfollow")) {
                        app->lockedToParticle = false;
                        app->camera.direction = -app->camera.localCoords;
                        app->camera.yaw -= 180;
                        app->camera.pitch = -app->camera.pitch;
                    }

                    ImGui::SameLine();
                    if (ImGui::Button("Delete")) {
                        p.mass = 0.f;
                        p.radius = 0.f;
                        if (app->lockedToParticle && app->following == idx) {
                            app->lockedToParticle = false;
                            app->camera.direction = -app->camera.localCoords;
                            app->camera.yaw -= 180;
                            app->camera.pitch = -app->camera.pitch;
                        }
                        app->selectedParticle = false;
                        update = true;
                    }

                    ImGui::SameLine();
                    if (ImGui::Button("Reset")) {
                        Particle q = app->original[idx];
                        q.pos = p.pos;
                        q.vel = p.vel;
                        q.acc = p.acc;
                        p = q;
                        update = true;
                    }

                    if (update) {
                        glBindBuffer(GL_SHADER_STORAGE_BUFFER, app->ssbo);
                        glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
                        glBufferSubData(GL_SHADER_STORAGE_BUFFER, static_cast<GLintptr>(idx * sizeof(Particle)), sizeof(Particle), reinterpret_cast<float*>(&p));
                        app->accumulationFrameIndex = 0;
                    }
                    ImGui::SetWindowPos({ app->res.x / 2.f - ImGui::GetWindowWidth() / 2.f, 30 });
                    ImGui::PopFont();
                    float height = ImGui::GetWindowSize().y;
                    ImGui::End();
                    return height;
                };

                float fpconfigH = 0;
                if (lockedToParticle) {
                    fpconfigH = particleConfig(this, following, ImVec2(settingsSize.x, 0.f), ImVec2(res.x - settingsSize.x - 10.f, 10.f), "Following particle");
                }
                if (selectedParticle && (!lockedToParticle || following != selected)) {
                    particleConfig(this, selected, ImVec2(settingsSize.x, 0.f), ImVec2(res.x - settingsSize.x - 10.f, 10.f + fpconfigH + (fpconfigH ? 10.f : 0.f)), "Selected particle");
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
                ImGui::SetNextWindowPos({ static_cast<float>(x + 15.f), static_cast<float>(y) });
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
            glClearColor(0.f, 0.f, 0.f, 1.f);
            glClear(GL_COLOR_BUFFER_BIT);

            ImGui::Render();

            shader->use();
            glBindFramebuffer(GL_FRAMEBUFFER, fbo);

            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, screenTexture);

            camera.projMat(res.x, res.y, shader->id);
            glUniform1f(glGetUniformLocation(shader->id, "timeDelta"), timeStep * dt * int(!paused) * (reverse ? -1 : 1));
            glUniform1f(glGetUniformLocation(shader->id, "uTime"), currentTime);
            glUniform1i(glGetUniformLocation(shader->id, "accumulationFrameIndex"), accumulationFrameIndex);
            if (globalIllumination && paused) accumulationFrameIndex++;
            else accumulationFrameIndex = 0;
            glDrawArrays(GL_TRIANGLES, 0, 36);

            pprocshader->use();
            pprocshader->render(bloom, screenTexture, bloomThreshold, exposure, accumulationFrameIndex);

            cmptshader->use();
            glUniform1f(glGetUniformLocation(cmptshader->id, "timeDelta"), timeStep * dt * int(!paused) * (reverse ? -1 : 1));
            glUniform1i(glGetUniformLocation(cmptshader->id, "collisionType"), collisionType);
            glDispatchCompute(pBuffer.size(), 1, 1);
            glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
            shader->use();

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