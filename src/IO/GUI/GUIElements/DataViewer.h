#ifndef DATAVIS_DATAVIEWER_H
#define DATAVIS_DATAVIEWER_H

#include "GUIElement.h"
#include "IO/FileHanding.h"
#include "misc/DataManager.h"
#include "ImGuiFileDialog/ImGuiFileDialog.h"

class DataViewer : public GUIElement {
    void Update() final {
        ImGui::Begin("Raw Data");

        if (ImGui::Button("Select"))
            ImGuiFileDialog::Instance()->OpenDialog("ChooseFileDlgKey", "Select Files", ".*", ".", 0);

        ImGui::SameLine();
        static bool shotBased = true;
        if (ImGui::Checkbox("Shot based", &shotBased))
            DataManager::ClearData();

        bool loadedData = false;
        if (ImGuiFileDialog::Instance()->Display("ChooseFileDlgKey")) {
            if (ImGuiFileDialog::Instance()->IsOk()) {
                DataManager::ClearData();
                if (shotBased) {
                    std::vector<std::string> paths;
                    for (const auto &sel: ImGuiFileDialog::Instance()->GetSelection())
                        paths.push_back(sel.second);
                    DataManager::UpdateRawData(FileHanding::ReadCSV(paths, "\t"), 0);
                    loadedData = true;
                } else {
                    std::vector<LabelledSeries> trainData;
                    std::vector<LabelledSeries> testData;
                    for (const auto &sel: ImGuiFileDialog::Instance()->GetSelection()) {
                        const auto d = FileHanding::ReadCSV(sel.second, "\t");
                        if (sel.second.find("TEST") != std::string::npos)
                            testData.insert(testData.end(), d.begin(), d.end());
                        else
                            trainData.insert(trainData.end(), d.begin(), d.end());
                    }
                    DataManager::UpdateRawData(trainData, testData);
                    loadedData = true;
                }
            }
            ImGuiFileDialog::Instance()->Close();
        }

        if (shotBased && DataManager::HasData()) {
            static int split = 6;
            static int priorSplit = split;
            static int maxCount = 10;
            ImGui::SameLine();
            ImGui::SliderInt("n-shot", &split, 1, maxCount);
            if (loadedData || split != priorSplit) {
                DataManager::UpdateRawData(split);
                priorSplit = split;
            }
        }

        if (DataManager::HasData()) {
            static bool showNormalized = true;
            ImGui::Checkbox("Normalized Data", &showNormalized);
            static bool includeTrainData = true;
            ImGui::SameLine();
            ImGui::Checkbox("Include Train Data", &includeTrainData);
            static bool includeTestData = true;
            ImGui::SameLine();
            ImGui::Checkbox("Include Test Data", &includeTestData);
            std::vector<LabelledSeries> data;
            if (includeTrainData) {
                const auto tempData = showNormalized ? DataManager::GetNormTrainData() : DataManager::GetRawTrainData();
                data.insert(data.end(), tempData.begin(), tempData.end());
            }
            if (includeTestData) {
                const auto tempData = showNormalized ? DataManager::GetNormTestData() : DataManager::GetRawTestData();
                data.insert(data.end(), tempData.begin(), tempData.end());
            }
            ImPlot::SetNextAxesToFit();
            if (ImPlot::BeginPlot("Data")) {
                for (int i = 0; i < data.size(); i++) {
                    ImPlot::SetNextLineStyle(ImVec4{1 / ((float) data[i].label + 2), 1 / ((float) data[i].label + 2), 0.5f, 1});
                    ImPlot::PlotLine(std::to_string(data[i].label).c_str(), &data[i].series[0], data[i].series.size());
                }

                ImPlot::EndPlot();
            }
        }
        ImGui::End();
    }
};

#endif //DATAVIS_DATAVIEWER_H
