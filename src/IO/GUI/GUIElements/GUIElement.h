#ifndef DATAVIS_GUIELEMENT_H
#define DATAVIS_GUIELEMENT_H

#include <imgui.h>
#include <implot/implot.h>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>

class GUIElement {
public:
    virtual void Update() = 0;
};

#endif //DATAVIS_GUIELEMENT_H
