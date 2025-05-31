// gui/InputModal.h
#pragma once
#include "../core/EventQueue.h"
#include <vector>

// ����� ������ �������� � �������� float, � �� pair
class InputModal {
public:
    // ����������� ��������� EventQueue<std::vector<float>>&
    InputModal(EventQueue<std::vector<float>>& q);

    // draw �������� ���� �������� � ����� ������
    void draw(bool& open, int mode);
    void reset(int mode);
    bool justOpened_ = false;
private:
    EventQueue<std::vector<float>>& queue_;

    // ��������� ��� ������ ������ ���������
    float valX_{ 0 }, valY_{ 0 }, valZ_{ 0 };
    float valR_{ 0 }, valPhi_{ 0 }, valTheta_{ 0 };
};
