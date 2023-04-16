#ifndef DATAVIS_CLASSIFIERVIEWER_H
#define DATAVIS_CLASSIFIERVIEWER_H

#include "GUIElement.h"
#include "core/Classification.h"
#include "misc/DataManager.h"

class ClassifierViewer : public GUIElement {
    void Update() final {
        if (!DataManager::HasFeatures())
            return;

        ImGui::Begin("Classifier");

        static int n = 1;
        if (ImGui::InputInt("KNN Neighbour count", &n))
            DataManager::ResetAccuracy();


        if (!DataManager::HasAccuracy()) {
            const auto trainFeatures = DataManager::GetTrainFeatureSeries();
            const auto testFeatures = DataManager::GetTestFeatureSeries();
            std::map<int, int> correct;
            uint totalCorrect = 0;
            uint totalIncorrect = 0;
            for (const auto &seriesSet : testFeatures) {
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
            for (const auto &c : correct)
                classAcc.push_back(((double) c.second / (double) testFeatures.at(c.first).size()) * 100);
            DataManager::SetAccuracy((double) totalCorrect / (double) (totalCorrect + totalIncorrect));
            DataManager::SetClassAccuracy(classAcc);
        }

        ImGui::Text("Total Accuracy: %f", DataManager::GetAccuracy());

        ImPlot::SetNextAxesToFit();
        if (ImPlot::BeginPlot("Class Accuracy")) {
            const auto classAcc = DataManager::GetClassAccuracy();
            ImPlot::PlotBars("", &classAcc[0], classAcc.size());
            ImPlot::EndPlot();
        }

        ImGui::End();
    }
};

#endif //DATAVIS_CLASSIFIERVIEWER_H
