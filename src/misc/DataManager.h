#ifndef DATAVIS_DATAMANAGER_H
#define DATAVIS_DATAMANAGER_H

#include <utility>
#include <mutex>

#include "Constants.h"
#include "utilities/SeriesUtils.h"
#include "types/Feature.h"
#include "utilities/FeatureUtils.h"

class DataManager {
public:
    static void ClearData() {
        rawTrainData.clear();
        rawTestData.clear();
        trainData.clear();
        testData.clear();
        trainMap.clear();
        features.clear();
        trainFeatureSeries.clear();
        testFeatureSeries.clear();
        acc.clear();
        classAcc.clear();
    }
    static void UpdateRawData(const std::vector<LabelledSeries> &train, const std::vector<LabelledSeries> &test) {
        trainData = train;
        rawTrainData = train;
        testData = test;
        rawTestData = test;
        acc.clear();
        classAcc.clear();

        SeriesUtils::MinMaxNormalize(trainData);
        SeriesUtils::MinMaxNormalize(testData);
        SeriesUtils::ForcePositiveRange(trainData);
        SeriesUtils::ForcePositiveRange(testData);

        trainMap = SeriesUtils::ToMap(trainData);
        features.clear();
    }
    static void UpdateRawData(const std::vector<LabelledSeries> &data, double split) {
        const auto trainSplit = SeriesUtils::Split(data, split);
        UpdateRawData(trainSplit.first, trainSplit.second);
    }
    static void UpdateRawData(double split) {
        auto data = rawTrainData;
        data.insert(data.end(), rawTestData.begin(), rawTestData.end());
        UpdateRawData(data, split);
    }
    static bool HasData() { return !rawTrainData.empty() || !rawTestData.empty(); }
    static std::vector<LabelledSeries> GetRawTrainData() { return rawTrainData; }
    static std::vector<LabelledSeries> GetRawTestData() { return rawTestData; }
    static std::vector<LabelledSeries> GetNormTrainData() { return trainData; }
    static std::unordered_map<int, std::vector<Series>> GetNormTrainDataMap() { return trainMap; }
    static std::vector<LabelledSeries> GetNormTestData() { return testData; }
    static void BeginFeatureGeneration() {
        features.clear();
        trainFeatureSeries.clear();
        testFeatureSeries.clear();
        acc.clear();
        classAcc.clear();
        isGenerating = true;
    }
    static void EndFeatureGeneration() {
        isGenerating = false;
    }
    static bool IsGeneratingFeatures() { return isGenerating; }
    static bool HasFeatures() { return !features.empty(); }
    static int FeatureCount() { return features.size(); }
    static Feature GetFeature(int index) { return features[index]; }
    static std::vector<Feature> GetFeatures() { return features; }
    static void AddFeature(const Feature& feature) {
        features.push_back(feature);
        static std::mutex mutex;
        std::unique_lock lock(mutex);
        testFeatureSeries = FeatureUtils::GenerateFeatureSeries(testData, features);
        trainFeatureSeries = FeatureUtils::GenerateFeatureSeries(trainData, features);
    }
    static void ClearFeatures() {
        features.clear();
        acc.clear();
        classAcc.clear();
    }
    static std::unordered_map<int, std::vector<Series>> GetTrainFeatureSeries() { return trainFeatureSeries; }
    static std::unordered_map<int, std::vector<Series>> GetTestFeatureSeries() { return testFeatureSeries; }
    static bool HasAccuracy() { return !acc.empty(); };
    static uint AccuracyCount() { return acc.size(); };
    static std::vector<double> GetAccuracy() { return acc; };
    static std::vector<std::vector<double>> GetClassAccuracy() { return classAcc; };
    static void AddAccuracy(double accuracy) { acc.push_back(accuracy); }
    static void AddClassAccuracy(const std::vector<double>& accuracy) {
        for (uint i = 0; i < accuracy.size(); i++) {
            if (classAcc.size() <= i)
                classAcc.push_back({ accuracy.at(i) });
            else
                classAcc.at(i).push_back(accuracy.at(i));
        }
    }
    static void ResetAccuracy() {
        acc.clear();
        classAcc.clear();
    }
private:
    static inline std::vector<LabelledSeries> rawTrainData;
    static inline std::vector<LabelledSeries> rawTestData;
    static inline std::vector<LabelledSeries> trainData;
    static inline std::vector<LabelledSeries> testData;
    static inline std::unordered_map<int, std::vector<Series>> trainMap;
    static inline std::vector<Feature> features;
    static inline std::unordered_map<int, std::vector<Series>> trainFeatureSeries;
    static inline std::unordered_map<int, std::vector<Series>> testFeatureSeries;
    static inline bool isGenerating = false;
    static inline std::vector<double> acc;
    static inline std::vector<std::vector<double>> classAcc;
};

#endif //DATAVIS_DATAMANAGER_H
