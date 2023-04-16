#ifndef DATAVIS_GUIMANAGER_H
#define DATAVIS_GUIMANAGER_H

#include <vector>
#include <memory>
#include "GUIElements/GUIElement.h"

class GUIManager {
public:
    GUIManager();
    void Update();
private:
    std::vector<std::unique_ptr<GUIElement>> elements;
};

#endif //DATAVIS_GUIMANAGER_H
