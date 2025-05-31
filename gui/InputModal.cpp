// gui/InputModal.cpp
// -*- coding: utf-8 -*-
#pragma execution_character_set("utf-8")

#include "InputModal.h"
#include <imgui.h>
#include <vector>



void InputModal::reset(int mode) {
    // обнуляем только те поля, которые будут показаны
    switch (mode) {
    case 0: valX_ = valY_ = valZ_ = 0.0f; break;
    case 1: valR_ = valPhi_ = 0.0f;       break;
    case 2: valR_ = valPhi_ = valZ_ = 0.0f; break;
    case 3: valR_ = valTheta_ = valPhi_ = 0.0f; break;
    }
    justOpened_ = true;
}

InputModal::InputModal(EventQueue<std::vector<float>>& q)
    : queue_(q)
{}

// mode: 0=XYZ, 1=Полярные, 2=Цилиндрические, 3=Сферические
void InputModal::draw(bool& open, int mode) {
    if (!open) return;
    if (justOpened_) {
        reset(mode);
        justOpened_ = false;
    }

    ImGui::OpenPopup("Ввод координат");
    if (!ImGui::BeginPopupModal("Ввод координат", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
        return;

    // Разные поля для разных режимов
    switch (mode) {
    case 0: // XYZ
        ImGui::InputFloat("X", &valX_);
        ImGui::InputFloat("Y", &valY_);
        ImGui::InputFloat("Z", &valZ_);
        break;
    case 1: // Полярные
        ImGui::InputFloat("r", &valR_);
        ImGui::InputFloat("φ (rad)", &valPhi_);
        break;
    case 2: // Цилиндрические
        ImGui::InputFloat("ρ", &valR_);
        ImGui::InputFloat("φ (rad)", &valPhi_);
        ImGui::InputFloat("z", &valZ_);
        break;
    case 3: // Сферические
        ImGui::InputFloat("r", &valR_);
        ImGui::InputFloat("θ (rad)", &valTheta_);
        ImGui::InputFloat("φ (rad)", &valPhi_);
        break;
    default:
        ImGui::Text("Неподдерживаемый режим!");
    }

    // Кнопки
    if (ImGui::Button("OK")) {
        std::vector<float> coords;
        switch (mode) {
        case 0: coords = { valX_, valY_, valZ_ };               break;
        case 1: coords = { valR_, valPhi_ };                    break;
        case 2: coords = { valR_, valPhi_, valZ_ };             break;
        case 3: coords = { valR_, valTheta_, valPhi_ };         break;
        }
        queue_.push(coords);
        open = false;
        ImGui::CloseCurrentPopup();
    }
    ImGui::SameLine();
    if (ImGui::Button("Отмена")) {
        open = false;
        ImGui::CloseCurrentPopup();
    }

    ImGui::EndPopup();
}
