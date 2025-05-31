// main.cpp
// 
// -*- coding: utf-8 -*-
#pragma execution_character_set("utf-8")
#define WIN32_LEAN_AND_MEAN   // чтобы windows.h не тянул всё подряд
#include <windows.h>
#include <GL/glew.h>
#include <imgui.h>
#include <string>
#include <vector>

#include "core/EventQueue.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_opengl3.h"
#include "gui/InputModal.h"
#include "opengl_bridge/GLRenderer.h"

// Прототип из бэкенда Win32
extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

// Оконная процедура Win32
LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    // Сначала отдадим сообщения ImGui
    if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
        return TRUE;
    switch (msg) {
    case WM_SIZE:
        if (wParam != SIZE_MINIMIZED) {
            RECT r;
            GetClientRect(hWnd, &r);
            glViewport(0, 0, r.right, r.bottom);
        }
        return 0;
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    }
    return DefWindowProc(hWnd, msg, wParam, lParam);
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int nCmdShow) {
    //==========================================================================
    // 1) Регистрация класса окна
    WNDCLASSEX wc = { sizeof(WNDCLASSEX), CS_CLASSDC, WndProc, 0, 0,
                      hInstance, NULL, NULL, NULL, NULL,
                      L"MyImGuiWindowClass", NULL };
    RegisterClassEx(&wc);

    // Создание окна
    HWND hwnd = CreateWindow(wc.lpszClassName, L"Симулятор Миссии",
        WS_OVERLAPPEDWINDOW, 100, 100, 1280, 800,
        NULL, NULL, wc.hInstance, NULL);

    //==========================================================================
    // 2) Создание OpenGL-контекста
    HDC hdc = GetDC(hwnd);

    PIXELFORMATDESCRIPTOR pfd = {
        sizeof(pfd), 1,
        PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER,
        PFD_TYPE_RGBA, 32,
        0,0,0,0,0,0, 0,0,0,0,0,
        24, 8, 0,
        PFD_MAIN_PLANE, 0, 0,0,0
    };
    int pf = ChoosePixelFormat(hdc, &pfd);
    SetPixelFormat(hdc, pf, &pfd);

    HGLRC gl_ctx = wglCreateContext(hdc);
    wglMakeCurrent(hdc, gl_ctx);

    //==========================================================================
    // 3) Инициализация GLEW
    if (glewInit() != GLEW_OK) {
        MessageBox(hwnd, L"Не удалось инициализировать GLEW", L"Ошибка", MB_OK | MB_ICONERROR);
        return 1;
    }

    //==========================================================================
    // 4) Инициализация ImGui
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    (void)io;

    ImFontConfig font_cfg;
    // Собираем нужные диапазоны
    ImFontGlyphRangesBuilder builder;
    // 1) добавляем дефолтные (ASCII + латиницу, знаки)
    builder.AddRanges(io.Fonts->GetGlyphRangesDefault());
    // 2) добавляем кириллицу
    builder.AddRanges(io.Fonts->GetGlyphRangesCyrillic());
    // 3) добавляем греческие символы U+0370…U+03FF
    static const ImWchar greek_ranges[] = { 0x0370, 0x03FF, 0 };
    builder.AddRanges(greek_ranges);
    // (опционально) гарантируем, что конкретные буквы есть
    builder.AddText("φρθ");

    // Собираем всё в итоговый массив
    ImVector<ImWchar> glyph_ranges;
    builder.BuildRanges(&glyph_ranges);

    // Наконец — грузим шрифт с этими диапазонами
    io.Fonts->AddFontFromFileTTF(
        "C:\\Windows\\Fonts\\tahoma.ttf",
        18.0f,
        &font_cfg,
        glyph_ranges.Data
    );


    ImGui::StyleColorsDark();
    ImGui_ImplWin32_Init(hwnd);
    ImGui_ImplOpenGL3_Init("#version 130");



    //==========================================================================
    // 5) Создаём слои и очереди
    // очередь для выбранного режима
    EventQueue<int> modeQueue;
    EventQueue<std::vector<float>> coordQueue;


    // Массив названий режимов:
    static const char* modeNames[] = {
        "Система координат XYZ",
        "Полярные координаты",
        "Цилиндрические координаты",
        "Сферические координаты",
        "Косоугольная система координат"
    };

    // Количество элементов:
    constexpr int modeCount = sizeof(modeNames) / sizeof(modeNames[0]);


    // -----------------------

    InputModal  inputModal(coordQueue);
    GLRenderer  renderer(coordQueue);

    bool showInputModal = false;
    int  currentMode = 0;

    // Показываем окно
    ShowWindow(hwnd, nCmdShow);
    UpdateWindow(hwnd);

    //==========================================================================
    // 6) Главный цикл
    MSG msg;
    bool done = false;
    while (!done) {
        // 1) Обработка Win32-сообщений
        MSG msg;
        while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
            if (msg.message == WM_QUIT) { done = true; break; }
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        if (done) break;

        // 2) Начало одного кадра ImGui
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplWin32_NewFrame();
        ImGui::NewFrame();

        // 3) Ваш GUI
        ImGui::SetNextWindowSize(ImVec2(500, 250), ImGuiCond_FirstUseEver);
        ImGui::SetNextWindowSizeConstraints(ImVec2(300, 0), ImVec2(FLT_MAX, FLT_MAX));
        ImGui::Begin("Выбор режима работы");
        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(20, 10));
        float btnW = ImGui::GetContentRegionAvail().x;
        for (int i = 0; i < modeCount; ++i) {
            if (ImGui::Button(modeNames[i], ImVec2(btnW, 0)))
                modeQueue.push(i);
            if (i == modeCount - 1) ImGui::Dummy(ImVec2(0, 20));
        }
        ImGui::PopStyleVar();
        ImGui::End();

        // Отображение последних координат

        // Показать окно с последними введёнными координатами
        ImGui::Begin("Последние координаты");  // <-- вот здесь задаётся заголовок окна

        const auto& lc = renderer.getLastCoords();
        if (!lc.empty()) {
            if (lc.size() == 3) {
                ImGui::Text("X = %.3f   Y = %.3f   Z = %.3f", lc[0], lc[1], lc[2]);
            }
            else if (lc.size() == 2) {
                ImGui::Text("r = %.3f   φ = %.3f", lc[0], lc[1]);
            }
            else if (lc.size() == 4) {
                ImGui::Text("r = %.3f   θ = %.3f   φ = %.3f  z = %.3f",
                    lc[0], lc[1], lc[2], lc[3]);
            }
            else {
                ImGui::Text("coords size = %d", (int)lc.size());
            }
        }
        else {
            ImGui::Text("Координаты ещё не заданы");
        }

        ImGui::End();

        if (!showInputModal && modeQueue.try_pop(currentMode))
            showInputModal = true;
        inputModal.draw(showInputModal, currentMode);

        // 4) Завершаем кадр ImGui
        ImGui::Render();

        // 5) Рендерим OpenGL и ImGui
        RECT r; GetClientRect(hwnd, &r);
        renderer.render(r.right, r.bottom);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        SwapBuffers(hdc);
    }

    //==========================================================================
    // 7) Очистка
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();

    wglMakeCurrent(NULL, NULL);
    wglDeleteContext(gl_ctx);
    ReleaseDC(hwnd, hdc);
    DestroyWindow(hwnd);
    UnregisterClass(wc.lpszClassName, hInstance);

    return 0;
}
