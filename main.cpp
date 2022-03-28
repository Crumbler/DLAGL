#define UNICODE

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>
#include <windows.h>

#include <iostream>
#include <vector>
#include <string>

#define STBI_WINDOWS_UTF8
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "DLAModel.hpp"
#include "DLABase.hpp"

#include "PointShader.hpp"
#include "DLABaseShader.hpp"
#include "Renderer.hpp"
#include "Loader.hpp"
#include "PointModel.hpp"
#include "Utils.hpp"

constexpr float widthToHeight = 1.4f,
                pointSizeToHeight = 3.0e-3f;

constexpr int minWindowHeight = 520,
              initialWindowHeight = 720;

constexpr auto PopupFlags = ImGuiWindowFlags_AlwaysAutoResize |
                            ImGuiWindowFlags_NoResize |
                            ImGuiWindowFlags_NoMove;

HWND hwndMain;
WNDPROC orgWndProc;
Loader loader;
PointShader pointShader;
DLABaseShader baseShader;
Renderer* renderer;
PointModel pointModel;
RawModel baseModel;
glm::mat4 curProjMat;
bool isGenerating, useHue;
float hueS, hueV, hueMult, pointScale;
int curWidth, curHeight, pointsPerUpdate;

DLAModel curModel;

std::vector<char> selectIndices;

ImGuiIO *io;

void OnChar(GLFWwindow* window, unsigned int codepoint)
{
    if (codepoint < 128)
        io->AddInputCharacter(codepoint);
}

void OnKey(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    const bool isNotDown = action != GLFW_RELEASE;

    switch(key)
    {
    case GLFW_KEY_LEFT_CONTROL:
        io->KeyCtrl = isNotDown;
        break;

    case GLFW_KEY_LEFT_SHIFT:
        io->KeyShift = isNotDown;
        break;

    case GLFW_KEY_LEFT_ALT:
        io->KeyAlt = isNotDown;
        break;

    default:
        io->KeysDown[key] = isNotDown;
    }
}

void OnMouseMove(GLFWwindow* window, double xpos, double ypos)
{
    io->MousePos = ImVec2(xpos, ypos);
}

void OnMouseButton(GLFWwindow* window, int button, int action, int mods)
{
    const bool isPressed = action == GLFW_PRESS;

    switch(button)
    {
    case GLFW_MOUSE_BUTTON_LEFT:
        io->MouseDown[0] = isPressed;
        break;

    case GLFW_MOUSE_BUTTON_RIGHT:
        io->MouseDown[1] = isPressed;
        break;

    case GLFW_MOUSE_BUTTON_MIDDLE:
        io->MouseDown[2] = isPressed;
        break;
    }
}

void OnScroll(GLFWwindow* window, double xoffset, double yoffset)
{
    io->MouseWheel = yoffset;
}

void SaveToImage(GLFWwindow *window)
{
    OPENFILENAME ofn;

    ZeroMemory(&ofn, sizeof(OPENFILENAME));

    std::wstring fName;
    fName.resize(100);

    ofn.lStructSize = sizeof(OPENFILENAME);
    ofn.hwndOwner = hwndMain;
    ofn.lpstrFilter = L"PNG Files (*.png)\0*.png\0All Files (*.*)\0*.*\0";
    ofn.lpstrFile = &fName[0];
    ofn.nMaxFile = 100;
    ofn.Flags = OFN_EXPLORER |
                OFN_ENABLESIZING |
                OFN_CREATEPROMPT |
                OFN_OVERWRITEPROMPT |
                OFN_PATHMUSTEXIST;
    ofn.lpstrDefExt = L"png";

    if (GetSaveFileName(&ofn))
    {
        char fNameBuffer[200];
        stbiw_convert_wchar_to_utf8(fNameBuffer, 200, fName.c_str());

        int width, height;
        glfwGetFramebufferSize(window, &width, &height);
        GLsizei nrChannels = 3;
        GLsizei stride = nrChannels * height;
        stride += (stride % 4) ? (4 - stride % 4) : 0;
        GLsizei bufferSize = stride * height;
        std::vector<char> buffer(bufferSize);
        glPixelStorei(GL_PACK_ALIGNMENT, 4);
        glReadBuffer(GL_FRONT);
        glReadPixels(width - height, 0, height, height, GL_RGB, GL_UNSIGNED_BYTE, buffer.data());
        stbi_flip_vertically_on_write(true);
        stbi_write_png(fNameBuffer, height, height, nrChannels, buffer.data(), stride);
    }
}

void GUI_Menu(GLFWwindow *window)
{
    if (ImGui::BeginMenuBar())
    {
        if (ImGui::BeginMenu("File"))
        {
            if (ImGui::MenuItem("Save image"))
                SaveToImage(window);
            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("Options"))
        {
            if (ImGui::SliderFloat("Point scale", &pointScale, 0.2f, 5.0f, nullptr, ImGuiSliderFlags_AlwaysClamp))
                glPointSize(pointSizeToHeight * pointScale * curHeight);

            ImGui::SliderInt("Points per update", &pointsPerUpdate, 1, 250, nullptr, ImGuiSliderFlags_AlwaysClamp);

            if (ImGui::BeginMenu("Hue"))
            {
                if (ImGui::Checkbox("Use hue", &useHue))
                {
                    pointShader.Start();
                    pointShader.setUseHue(useHue);

                    baseShader.Start();
                    baseShader.setUseHue(useHue);
                }

                if (ImGui::SliderFloat("Multiplier", &hueMult, 0.0f, 10.0f, nullptr, ImGuiSliderFlags_AlwaysClamp))
                {
                    pointShader.Start();
                    pointShader.setHueMult(hueMult);
                }

                if (ImGui::SliderFloat("S", &hueS, 0.0f, 1.0f, nullptr, ImGuiSliderFlags_AlwaysClamp))
                {
                    pointShader.Start();
                    pointShader.setHueS(hueS);

                    baseShader.Start();
                    baseShader.setHueS(hueS);
                }

                if (ImGui::SliderFloat("V", &hueV, 0.0f, 1.0f, nullptr, ImGuiSliderFlags_AlwaysClamp))
                {
                    pointShader.Start();
                    pointShader.setHueV(hueV);

                    baseShader.Start();
                    baseShader.setHueV(hueV);
                }

                ImGui::EndMenu();
            }

            ImGui::EndMenu();
        }

        ImGui::EndMenuBar();
    }
}

void GUI_Editing()
{
    ImGui::SetNextWindowPos(ImVec2(curWidth / 2.0f, curHeight / 2.0f), ImGuiCond_Always);

    if (ImGui::BeginPopupModal("No base", nullptr, PopupFlags))
    {
        ImGui::Text("There's no base to grow off of!");

        ImGui::Separator();

        if (ImGui::Button("OK"))
            ImGui::CloseCurrentPopup();

        ImGui::EndPopup();
    }

    ImGui::SetNextWindowPos(ImVec2(curWidth / 2.0f, curHeight / 2.0f), ImGuiCond_Always);

    if (ImGui::BeginPopupModal("Max items", nullptr, PopupFlags))
    {
        ImGui::Text("There can be no more than 10 items per type.");

        ImGui::Separator();

        if (ImGui::Button("OK"))
            ImGui::CloseCurrentPopup();

        ImGui::EndPopup();
    }

    if (ImGui::Button("Start generation"))
    {
        if (curModel.base.IsEmpty())
        {
            ImGui::OpenPopup("No base");
        }
        else
        {
            isGenerating = true;
            curModel.Reset();
            pointModel.resetPoints();
            pointModel.updatePoints(curModel);
        }
    }

    auto& curBase = curModel.base;
    auto& points = curBase.points;
    auto& lines = curBase.lines;
    auto& circles = curBase.circles;
    auto& boxes = curBase.boxes;
    auto& beziers = curBase.beziers;

    const std::string pointsHeader = std::string("Points(") +
                                     std::to_string(points.size()) +
                                     std::string(")###PHeader");

    ImGui::Indent(10.0f);

    if (ImGui::CollapsingHeader(pointsHeader.c_str()))
    {
        bool updateNeeded = false;

        if (ImGui::Button("Add point"))
        {
            if (points.size() < 10)
            {
                points.push_back(glm::vec2());
                updateNeeded = true;
            }
            else
                ImGui::OpenPopup("Max items");
        }

        ImGui::Indent(10.0f);

        std::string pointName;

        const float windowWidth = ImGui::GetWindowPos().x + ImGui::GetWindowContentRegionMax().x;
        ImGuiStyle& style = ImGui::GetStyle();

        int delInd = -1;

        ImGui::PushItemWidth(200.0f);

        for (size_t i = 0; i < points.size(); ++i)
        {
            ImGui::PushID(i);

            ImGui::BeginGroup();

            pointName = std::string("Point ") + std::to_string(i + 1);
            if (ImGui::Button(pointName.c_str()))
                delInd = i;

            updateNeeded = updateNeeded |
                           ImGui::SliderFloat2("", (float*)&points[i], -100.0f, 100.0f, nullptr, ImGuiSliderFlags_AlwaysClamp);

            ImGui::EndGroup();

            const float nextX = ImGui::GetItemRectMax().x + style.ItemSpacing.x + 200.0f; // Expected position if next button was on same line
            if (i + 1 < points.size() && nextX < windowWidth)
                ImGui::SameLine();

            ImGui::PopID();
        }

        ImGui::PopItemWidth();

        ImGui::Unindent(10.0f);

        if (delInd != -1)
        {
            points.erase(points.begin() + delInd);
            updateNeeded = true;
        }

        if (updateNeeded)
        {
            curModel.Reset();
            pointModel.resetPoints();
            pointModel.updatePoints(curModel);
        }
    }


    const std::string linesHeader = std::string("Lines(") +
                                    std::to_string(lines.size()) +
                                    std::string(")###LHeader");

    if (ImGui::CollapsingHeader(linesHeader.c_str()))
    {
        bool updateNeeded = false;

        if (ImGui::Button("Add line"))
        {
            if (lines.size() < 10)
            {
                lines.push_back(Line());
                updateNeeded = true;
            }
            else
                ImGui::OpenPopup("Max items");
        }

        ImGui::Indent(10.0f);

        std::string lineName;

        const float windowWidth = ImGui::GetWindowPos().x + ImGui::GetWindowContentRegionMax().x;
        ImGuiStyle& style = ImGui::GetStyle();

        int delInd = -1;

        ImGui::PushItemWidth(200.0f);

        for (size_t i = 0; i < lines.size(); ++i)
        {
            ImGui::PushID(i + 20);

            ImGui::BeginGroup();

            lineName = std::string("Line ") + std::to_string(i + 1);
            if (ImGui::Button(lineName.c_str()))
                delInd = i;

            updateNeeded |= ImGui::SliderFloat2("", (float*)&lines[i], -100.0f, 100.0f, nullptr, ImGuiSliderFlags_AlwaysClamp);

            ImGui::PushID(1596756);

            updateNeeded |= ImGui::SliderFloat2("", (float*)&lines[i] + 2, -100.0f, 100.0f, nullptr, ImGuiSliderFlags_AlwaysClamp);

            ImGui::PopID();

            ImGui::EndGroup();

            const float nextX = ImGui::GetItemRectMax().x + style.ItemSpacing.x + 200.0f;
            if (i + 1 < lines.size() && nextX < windowWidth)
                ImGui::SameLine();

            ImGui::PopID();
        }

        ImGui::PopItemWidth();

        ImGui::Unindent(10.0f);

        if (delInd != -1)
        {
            lines.erase(lines.begin() + delInd);
            updateNeeded = true;
        }

        if (updateNeeded)
        {
            baseShader.Start();
            baseShader.setLines(lines);
            baseShader.Stop();

            curModel.Reset();
            pointModel.resetPoints();
            pointModel.updatePoints(curModel);
        }
    }

    const std::string circlesHeader = std::string("Circles(") +
                                    std::to_string(circles.size()) +
                                    std::string(")###CHeader");

    if (ImGui::CollapsingHeader(circlesHeader.c_str()))
    {
        bool updateNeeded = false;

        if (ImGui::Button("Add circle"))
        {
            if (circles.size() < 10)
            {
                circles.push_back(Circle());
                updateNeeded = true;
            }
            else
                ImGui::OpenPopup("Max items");
        }

        ImGui::Indent(10.0f);

        std::string circleName;

        const float windowWidth = ImGui::GetWindowPos().x + ImGui::GetWindowContentRegionMax().x;
        ImGuiStyle& style = ImGui::GetStyle();

        int delInd = -1;

        ImGui::PushItemWidth(200.0f);

        bool rv;

        for (size_t i = 0; i < circles.size(); ++i)
        {
            ImGui::PushID(i + 30);

            ImGui::BeginGroup();

            circleName = std::string("Circle ") + std::to_string(i + 1);
            if (ImGui::Button(circleName.c_str()))
                delInd = i;

            updateNeeded |= ImGui::SliderFloat2("", (float*)&circles[i].c, -100.0f, 100.0f, nullptr, ImGuiSliderFlags_AlwaysClamp);

            ImGui::PushID("Radius");

            updateNeeded |= ImGui::SliderFloat("", &circles[i].r, 0.0f, 100.0f, nullptr, ImGuiSliderFlags_AlwaysClamp);

            ImGui::PopID();

            rv = circles[i].rev == 1.0f;
            if (ImGui::Checkbox("Reverse", &rv))
            {
                circles[i].rev = (rv ? 1.0f : 0.0f);
                updateNeeded = true;
            }

            ImGui::EndGroup();

            const float nextX = ImGui::GetItemRectMax().x + style.ItemSpacing.x + 200.0f;
            if (i + 1 < circles.size() && nextX < windowWidth)
                ImGui::SameLine();

            ImGui::PopID();
        }

        ImGui::PopItemWidth();

        ImGui::Unindent(10.0f);

        if (delInd != -1)
        {
            circles.erase(circles.begin() + delInd);
            updateNeeded = true;
        }

        if (updateNeeded)
        {
            baseShader.Start();
            baseShader.setCircles(circles);
            baseShader.Stop();

            curModel.Reset();
            pointModel.resetPoints();
            pointModel.updatePoints(curModel);
        }
    }

    const std::string boxesHeader = std::string("Boxes(") +
                                    std::to_string(boxes.size()) +
                                    std::string(")###BHeader");

    if (ImGui::CollapsingHeader(boxesHeader.c_str()))
    {
        bool updateNeeded = false;

        if (ImGui::Button("Add box"))
        {
            if (boxes.size() < 10)
            {
                boxes.push_back(Box());
                updateNeeded = true;
            }
            else
                ImGui::OpenPopup("Max items");
        }

        ImGui::Indent(10.0f);

        std::string boxName;

        const float windowWidth = ImGui::GetWindowPos().x + ImGui::GetWindowContentRegionMax().x;
        ImGuiStyle& style = ImGui::GetStyle();

        int delInd = -1;

        ImGui::PushItemWidth(200.0f);

        for (size_t i = 0; i < boxes.size(); ++i)
        {
            ImGui::PushID(i + 40);

            ImGui::BeginGroup();

            boxName = std::string("Box ") + std::to_string(i + 1);
            if (ImGui::Button(boxName.c_str()))
                delInd = i;

            updateNeeded |= ImGui::SliderFloat2("", (float*)&boxes[i], -100.0f, 100.0f, nullptr, ImGuiSliderFlags_AlwaysClamp);

            ImGui::PushID("B vector");

            updateNeeded |= ImGui::SliderFloat2("", (float*)&boxes[i] + 2, -100.0f, 100.0f, nullptr, ImGuiSliderFlags_AlwaysClamp);

            ImGui::PopID();

            ImGui::PushID("B width");
            ImGui::SetNextItemWidth(ImGui::CalcItemWidth() - ImGui::CalcTextSize("Width").x);

            updateNeeded |= ImGui::SliderFloat("Width", (float*)&boxes[i] + 4, 1.0f, 200.0f, nullptr, ImGuiSliderFlags_AlwaysClamp);

            ImGui::PopID();

            ImGui::EndGroup();

            const float nextX = ImGui::GetItemRectMax().x + style.ItemSpacing.x + 200.0f;
            if (i + 1 < boxes.size() && nextX < windowWidth)
                ImGui::SameLine();

            ImGui::PopID();
        }

        ImGui::PopItemWidth();

        ImGui::Unindent(10.0f);

        if (delInd != -1)
        {
            boxes.erase(boxes.begin() + delInd);
            updateNeeded = true;
        }

        if (updateNeeded)
        {
            baseShader.Start();
            baseShader.setBoxes(boxes);
            baseShader.Stop();

            curModel.Reset();
            pointModel.resetPoints();
            pointModel.updatePoints(curModel);
        }
    }

    const std::string beziersHeader = std::string("Beziers(") +
                                    std::to_string(beziers.size()) +
                                    std::string(")###BezierHeader");

    if (ImGui::CollapsingHeader(beziersHeader.c_str()))
    {
        bool updateNeeded = false;

        if (ImGui::Button("Add bezier"))
        {
            if (beziers.size() < 10)
            {
                beziers.push_back(Bezier());
                updateNeeded = true;
            }
            else
                ImGui::OpenPopup("Max items");
        }

        ImGui::Indent(10.0f);

        std::string bezierName;

        const float windowWidth = ImGui::GetWindowPos().x + ImGui::GetWindowContentRegionMax().x;
        ImGuiStyle& style = ImGui::GetStyle();

        int delInd = -1;

        ImGui::PushItemWidth(200.0f);

        for (size_t i = 0; i < beziers.size(); ++i)
        {
            ImGui::PushID(i + 50);

            ImGui::BeginGroup();

            bezierName = std::string("Bezier ") + std::to_string(i + 1);
            if (ImGui::Button(bezierName.c_str()))
                delInd = i;

            updateNeeded |= ImGui::SliderFloat2("", (float*)&beziers[i], -100.0f, 100.0f, nullptr, ImGuiSliderFlags_AlwaysClamp);

            ImGui::PushID("B vector");

            updateNeeded |= ImGui::SliderFloat2("", (float*)&beziers[i] + 2, -100.0f, 100.0f, nullptr, ImGuiSliderFlags_AlwaysClamp);

            ImGui::PopID();

            ImGui::PushID("C vector");

            updateNeeded |= ImGui::SliderFloat2("", (float*)&beziers[i] + 4, -100.0f, 100.0f, nullptr, ImGuiSliderFlags_AlwaysClamp);

            ImGui::PopID();

            ImGui::EndGroup();

            const float nextX = ImGui::GetItemRectMax().x + style.ItemSpacing.x + 200.0f;
            if (i + 1 < beziers.size() && nextX < windowWidth)
                ImGui::SameLine();

            ImGui::PopID();
        }

        ImGui::PopItemWidth();

        ImGui::Unindent(10.0f);

        if (delInd != -1)
        {
            beziers.erase(beziers.begin() + delInd);
            updateNeeded = true;
        }

        if (updateNeeded)
        {
            baseShader.Start();
            baseShader.setBeziers(beziers);
            baseShader.Stop();

            curModel.Reset();
            pointModel.resetPoints();
            pointModel.updatePoints(curModel);
        }
    }

    ImGui::Unindent(10.0f);
}

void GUI_Generating()
{
    if (ImGui::Button("Stop generation"))
        isGenerating = false;
}

void GUI_Main(GLFWwindow *window)
{
    ImGui::SetNextWindowPos(ImVec2(), ImGuiCond_Always);
    ImGui::SetNextWindowSize(ImVec2(curWidth - curHeight, curHeight), ImGuiCond_Always);

    ImGui::Begin("MainWindow", nullptr,
                 ImGuiWindowFlags_NoMove |
                 ImGuiWindowFlags_NoResize |
                 ImGuiWindowFlags_NoCollapse |
                 ImGuiWindowFlags_NoTitleBar |
                 ImGuiWindowFlags_MenuBar);

    GUI_Menu(window);

    if (isGenerating)
        GUI_Generating();
    else
        GUI_Editing();

    ImGui::End();
}

void OnUpdate(GLFWwindow *window)
{
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();

    ImGui::NewFrame();

    GUI_Main(window);

    if (isGenerating)
    {
        isGenerating = curModel.AddParticles(pointsPerUpdate);
        pointModel.updatePoints(curModel);
    }
}

void OnInit()
{
    useHue = true;
    isGenerating = false;
    pointScale = 1.0f;

    pointsPerUpdate = 50;

    hueS = hueV = hueMult = 1.0f;

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

    curHeight = initialWindowHeight;
    curWidth = curHeight * widthToHeight;

    curModel.Reset();

    glViewport(curWidth - curHeight, 0, initialWindowHeight, initialWindowHeight);

    curProjMat = glm::scale(glm::mat4(1.0f), glm::vec3(1.0f / 100.0f));

    pointModel = loader.loadPointsEmpty();
    pointModel.updatePoints(curModel);
    pointShader.Init(useHue, hueS, hueV, hueMult);

    const std::vector<float> pos{-100.0f, 100.0f,
                                 100.0f, 100.0f,
                                 -100.0f, -100.0f,
                                 100.0f, -100.0f};

    baseModel = loader.loadDLABase(pos, pos);
    baseShader.Init(useHue, hueS, hueV);

    renderer = new Renderer();

    renderer->updateProjectionMatrix(pointShader, curProjMat);
    renderer->updateProjectionMatrix(baseShader, curProjMat);

    glPointSize(initialWindowHeight * pointSizeToHeight * pointScale);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    io->ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io->IniFilename = nullptr;
}

void OnExit()
{
    pointShader.CleanUp();
    baseShader.CleanUp();

    loader.CleanUp();
}

void OnDisplay(GLFWwindow* window)
{
    renderer->Prepare();

    baseShader.Start();
    renderer->RenderDLABase(baseModel);

    pointShader.Start();
    renderer->RenderPoints(pointModel);
    pointShader.Stop();

    ImGui::Render();

    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    glfwSwapBuffers(window);
}

void OnSize(GLFWwindow* window, int width, int height)
{
    glViewport(width - height, 0, height, height);
    curWidth = width;
    curHeight = height;
    glPointSize(pointSizeToHeight * pointScale * height);
}

void OnFramebufferSize(GLFWwindow* window, int width, int height)
{
    OnUpdate(window);
    OnDisplay(window);
}

LRESULT CALLBACK SubWndProc(HWND hwd, UINT uMsg, WPARAM wParam, LPARAM lParam);

int main(void)
{
    GLFWwindow* window;

    if (!glfwInit())
        return -1;

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    window = glfwCreateWindow(initialWindowHeight * widthToHeight, initialWindowHeight, "DLAGL", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);

    hwndMain = glfwGetWin32Window(window);
    orgWndProc = (WNDPROC)SetWindowLongPtr(hwndMain, GWLP_WNDPROC, (LONG_PTR)SubWndProc);


    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD\n";
        return -1;
    }

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    io = &ImGui::GetIO();

    ImGui::StyleColorsDark();

    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");


    glfwSetKeyCallback(window, OnKey);
    glfwSetCharCallback(window, OnChar);
    glfwSetCursorPosCallback(window, OnMouseMove);
    glfwSetMouseButtonCallback(window, OnMouseButton);
    glfwSetScrollCallback(window, OnScroll);
    glfwSetWindowSizeCallback(window, OnSize);
    glfwSetFramebufferSizeCallback(window, OnFramebufferSize);

    OnInit();

    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();

        OnUpdate(window);

        OnDisplay(window);
    }

    OnExit();

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}

void OnSizing(HWND hwnd, RECT* rectPtr)
{
    int width = rectPtr->right - rectPtr->left,
        height = rectPtr->bottom - rectPtr->top;

    if (height < minWindowHeight)
    {
        height = minWindowHeight;
        rectPtr->bottom = rectPtr->top + height;
    }

    if (width < int(height * widthToHeight))
        rectPtr->right = rectPtr->left + int(height * widthToHeight);
}

LRESULT CALLBACK SubWndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    if (uMsg == WM_SIZING)
        OnSizing(hwnd, (RECT*)lParam);

    return CallWindowProc(orgWndProc, hwnd, uMsg, wParam, lParam);
}
