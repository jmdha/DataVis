#ifndef DATAVIS_CLASSIFIERVIEWER_H
#define DATAVIS_CLASSIFIERVIEWER_H

#include "GUIElement.h"
#include "core/Classification.h"
#include "misc/DataManager.h"

class ClassifierViewer : public GUIElement {
    void Update() final {
        ImGui::SetNextWindowPos(ImVec2(0, 364), ImGuiCond_Appearing);
        ImGui::SetNextWindowSize(ImVec2(700, 370), ImGuiCond_Appearing);
        ImGui::Begin("Classifier", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar);

        if (!DataManager::HasFeatures()) {
            const auto windowSize = ImGui::GetWindowSize();
            const auto textSize = ImGui::CalcTextSize("Please generate features.");

            ImGui::SetCursorPosX((windowSize.x - textSize.x) * 0.5f);
            ImGui::SetCursorPosY((windowSize.y - textSize.y) * 0.5f);
            ImGui::Text("Please generate features.");
        } else {
            static int n = 1;
            if (ImGui::InputInt("KNN Neighbour count", &n))
                DataManager::ResetAccuracy();


            if (DataManager::AccuracyCount() < DataManager::FeatureCount()) {
                const auto trainFeatures = DataManager::GetTrainFeatureSeries();
                const auto testFeatures = DataManager::GetTestFeatureSeries();
                std::map<int, int> correct;
                uint totalCorrect = 0;
                uint totalIncorrect = 0;
                for (const auto &seriesSet: testFeatures) {
                    correct[seriesSet.first] = 0;
                    for (const auto &series: seriesSet.second) {
                        const auto guess = Classification::KNN::Classify(trainFeatures, series, n);
                        if (guess == seriesSet.first) {
                            correct[seriesSet.first]++;
                            totalCorrect++;
                        } else
                            totalIncorrect++;
                    }
                }
                std::vector<double> classAcc;
                for (const auto &c: correct)
                    classAcc.push_back(((double) c.second / (double) testFeatures.at(c.first).size()) * 100);
                DataManager::AddAccuracy(((double) totalCorrect / (double) (totalCorrect + totalIncorrect)) * 100);
                DataManager::AddClassAccuracy(classAcc);
            }

            ImGui::Text("Total Accuracy: %f", DataManager::GetAccuracy().back());

            ImPlot::SetNextAxesToFit();
            if (ImPlot::BeginPlot("Class Accuracy")) {
                const auto classAcc = DataManager::GetClassAccuracy();
                for (uint i = 0; i < classAcc.size(); i++)
                    ImPlot::PlotLine(std::to_string(i).c_str(), &classAcc[i][0], classAcc[i].size());
                auto avgAcc = DataManager::GetAccuracy();
                ImPlot::PlotLine("Average", &avgAcc[0], avgAcc.size());
                ImPlot::EndPlot();
            }
        }
        ImGui::End();
    }
};

#endif //DATAVIS_CLASSIFIERVIEWER_H
