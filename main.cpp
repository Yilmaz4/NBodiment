#define IMGUI_DEFINE_MATH_OPERATORS
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <imgui_internal.h>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include <Windows.h>
#include <exception>
#include <string>

class Error : public std::exception {
    wchar_t msg[512];
public:
	Error(const wchar_t* message) {
        wcsncpy_s(msg, message, 18);
		this->display();
	}
    Error(const char* message) {
        mbstowcs_s(nullptr, msg, message, 512);
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
        char infoLog[512];

        const char* vertexSource = this->read_resource(IDR_VRTX);
        const char* fragmentSource = this->read_resource(IDR_FRAG);

        vertexShader = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vertexShader, 1, &vertexSource, NULL);
        glCompileShader(vertexShader);
        glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
        if (!success) {
            glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
            throw Error(infoLog);
        }

        fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fragmentShader, 1, &fragmentSource, NULL);
        glCompileShader(fragmentShader);
        glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
        if (!success) {
            glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
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

class NBodiment {
    Shader shader;
    GLFWwindow* window;
    GLFWmonitor* monitor;
	
public:
    NBodiment() {
		glfwInit();
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

		monitor = glfwGetPrimaryMonitor();
		const GLFWvidmode* mode = glfwGetVideoMode(monitor);

		glfwWindowHint(GLFW_RED_BITS, mode->redBits);
		glfwWindowHint(GLFW_GREEN_BITS, mode->greenBits);
		glfwWindowHint(GLFW_BLUE_BITS, mode->blueBits);
		glfwWindowHint(GLFW_REFRESH_RATE, mode->refreshRate);

		window = glfwCreateWindow(mode->width, mode->height, "N-Body Simulation", monitor, NULL);
		if (window == nullptr) {
			throw Error("Failed to create OpenGL context");
		}
		// initialize callback functions
		glfwSetFramebufferSizeCallback(window, on_windowResize);
        glfwSetWindowUserPointer(window, this);
		glfwMakeContextCurrent(window);
		glfwSwapInterval(1);

        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO(); (void)io;
        io.Fonts->AddFontDefault();
        ImFont* font_title = io.Fonts->AddFontFromFileTTF("C:\\Windows\\Fonts\\consola.ttf", 12.f);
        IM_ASSERT(font_title != NULL);
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
        io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

        ImGui_ImplGlfw_InitForOpenGL(window, true);
        ImGui_ImplOpenGL3_Init("#version 430");

        if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
            throw Error("Failed to load GLAD");
        }

		shader = Shader();
        shader.create();

        this->mainloop();
	}

    static inline void on_windowResize(GLFWwindow* window, int w, int h) {
        NBodiment* app = static_cast<NBodiment*>(glfwGetWindowUserPointer(window));
    }

    void mainloop() {
        while (!glfwWindowShouldClose(this->window)) {
            glfwPollEvents();
            glfwSwapBuffers(window);
        }
        glfwDestroyWindow(window);
        glfwTerminate();
    }
};

int main() {
    NBodiment proj = NBodiment();
	return 0;
}