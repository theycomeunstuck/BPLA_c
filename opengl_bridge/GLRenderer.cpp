// opengl_bridge/GLRenderer.cpp
#include "GLRenderer.h"
#include <GL/glew.h>

GLRenderer::GLRenderer(EventQueue<std::vector<float>>& q)
    : queue_(q)
{}

void GLRenderer::render(int width, int height) {
    // 1) �������� ����� ����������, ���� ��� ����
    std::vector<float> coords;
    if (queue_.try_pop(coords)) {
        lastCoords_ = coords;
    }

    // 2) ����������� viewport
    glViewport(0, 0, width, height);
    glClear(GL_COLOR_BUFFER_BIT);

    // 3) ��������� ��������� �������
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    // 4) �������� ������� / ������� � ����� coords
    if (lastCoords_.size() >= 2) {
        float x = lastCoords_[0];
        float y = lastCoords_[1];
        float z = (lastCoords_.size() >= 3 ? lastCoords_[2] : 0.0f);
        glTranslatef(x, y, z);
    }

    // 5) ������ ������� ����������� ������, �������� ��� ��� �����
    glBegin(GL_POINTS);
    glVertex3f(0.f, 0.f, 0.f);
    glEnd();
}
