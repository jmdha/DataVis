#include "GUIManager.h"
#include "GUIElements/DataViewer.h"
#include "GUIElements/FeatureViewer.h"
#include "GUIElements/ClassifierViewer.h"

GUIManager::GUIManager() {
    elements.push_back(std::make_unique<DataViewer>(DataViewer()));
    elements.push_back(std::make_unique<FeatureViewer>(FeatureViewer()));
    elements.push_back(std::make_unique<ClassifierViewer>(ClassifierViewer()));
}

void GUIManager::Update() {
    for (const auto &el : elements)
        el->Update();
}
