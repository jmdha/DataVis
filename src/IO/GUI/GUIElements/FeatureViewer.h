#ifndef DATAVIS_FEATUREVIEWER_H
#define DATAVIS_FEATUREVIEWER_H

#include <future>
#include <thread>
#include "GUIElement.h"
#include "misc/DataManager.h"
#include "core/feature_finding/FeatureFinding.h"
#include "core/attributes/MinDist.h"
#include "core/attributes/OccPos.h"
#include "core/attributes/MaxDist.h"
#include "core/attributes/Frequency.h"

class FeatureViewer : public GUIElement {
    void Update() final {
        ImGui::SetNextWindowPos(ImVec2(700, 0), ImGuiCond_Appearing);
        ImGui::SetNextWindowSize(ImVec2(580, 734), ImGuiCond_Appearing);
        ImGui::Begin("Features", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar);

        if (!DataManager::HasData()) {
            const auto windowSize = ImGui::GetWindowSize();
            const auto textSize = ImGui::CalcTextSize("Please load data.");

            ImGui::SetCursorPosX((windowSize.x - textSize.x) * 0.5f);
            ImGui::SetCursorPosY((windowSize.y - textSize.y) * 0.5f);
            ImGui::Text("Please load data.");
        } else {
            bool isDisabled = false;
            if (DataManager::IsGeneratingFeatures()) {
                ImGui::BeginDisabled();
                isDisabled = true;
            }

            bool hasChanged = false;
            static int minWindowSize = 2;
            static int maxWindowSize = 128;
            static int minSampleSize = 0;
            static int maxSampleSize = 5;
            static int featureCount = 256;
            ImGui::PushItemWidth(96);
            hasChanged |= ImGui::InputInt("Min Window Size", &minWindowSize);
            ImGui::SameLine();
            hasChanged |= ImGui::InputInt("Max Window Size", &maxWindowSize);
            hasChanged |= ImGui::InputInt("Min Sample Size", &minSampleSize);
            ImGui::SameLine();
            hasChanged |= ImGui::InputInt("Max Sample Size", &maxSampleSize);
            hasChanged |= ImGui::InputInt("Feature Count", &featureCount);
            ImGui::PopItemWidth();

            if (hasChanged)
                DataManager::ClearFeatures();

            auto result = ImGui::Button("Generate");
            static std::optional<std::thread> resultThread;
            if (result && !DataManager::IsGeneratingFeatures()) {
                if (resultThread.has_value() && resultThread.value().joinable())
                    resultThread.value().join();
                else if (resultThread.has_value())
                    resultThread.value().detach();
                resultThread = std::thread([&] {
                    FeatureFinding::GenerateFeaturesFromSamples(DataManager::GetNormTrainDataMap(),
                                                                minWindowSize, maxWindowSize, minSampleSize,
                                                                maxSampleSize, featureCount,
                                                                {
                                                                        std::make_unique<MinDist>(MinDist()),
                                                                        std::make_unique<MaxDist>(MaxDist())
                                                                });
                });
                /*FeatureFinding::GenerateFeaturesFromSamples(DataManager::GetNormTrainDataMap(),
                                                            minWindowSize, maxWindowSize, minSampleSize, maxSampleSize, featureCount,
                                                            { std::make_unique<MinDist>(MinDist()) });*/
            }
            if (isDisabled)
                ImGui::EndDisabled();
            ImGui::SameLine();

            if (DataManager::IsGeneratingFeatures() && !DataManager::HasFeatures())
                ImGui::Text("Please wait...");
            else if (DataManager::HasFeatures()) {
                static int featureIndex = 0;
                ImGui::SliderInt("Feature", &featureIndex, 0, DataManager::FeatureCount() - 1);

                const auto feature = DataManager::GetFeature(featureIndex);

                ImGui::Text("Gain:");
                ImGui::SameLine();
                if (feature.gain > 1.25)
                    ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(0, 255, 0, 255));
                else if (feature.gain > 0.75)
                    ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(255, 255, 0, 255));
                else
                    ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(255, 0, 0, 255));
                ImGui::Text("%s", std::to_string(feature.gain).c_str());
                ImGui::PopStyleColor();
                ImGui::SameLine();
                std::string attributeText = "Attribute: " + feature.attribute->Name();
                ImGui::Text("%s", attributeText.c_str());

                const auto shapelet = feature.shapelet;

                ImPlot::SetNextAxesToFit();
                if (ImPlot::BeginPlot("Shapelet")) {
                    for (int i = 0; i < shapelet.size(); i++) {
                        ImPlot::PlotLine("", &shapelet[0], shapelet.size());
                    }

                    ImPlot::EndPlot();
                }

                const auto data = DataManager::GetNormTrainDataMap();
                std::map<int, std::vector<double>> classValues;
                for (const auto &seriesSet: data)
                    for (const auto &series: seriesSet.second)
                        classValues[seriesSet.first].push_back(
                                feature.attribute->GenerateValue(series, feature.shapelet));

                std::vector<double> avgs;
                for (const auto &valueSet: classValues)
                    avgs.push_back(std::accumulate(valueSet.second.begin(), valueSet.second.end(), (double) 0) /
                                   (double) valueSet.second.size());

                ImPlot::SetNextAxesToFit();
                if (ImPlot::BeginPlot("Class Avg")) {
                    ImPlot::PlotBars("", &avgs[0], avgs.size());
                    ImPlot::EndPlot();
                }
            }
        }
        ImGui::End();
    }
};


#endif //DATAVIS_FEATUREVIEWER_H
