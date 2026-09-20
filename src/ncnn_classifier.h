#pragma once

#include "cli_options.h"

#include <string>
#include <vector>

struct Prediction
{
    int classId = -1;
    float score = 0.f;
    std::string label;
};

struct InferenceReport
{
    int sourceWidth = 0;
    int sourceHeight = 0;
    double preprocessMs = 0.0;
    double firstInferenceMs = 0.0;
    double averageInferenceMs = 0.0;
    double minInferenceMs = 0.0;
    double maxInferenceMs = 0.0;
    long peakRssKb = 0;
    std::vector<Prediction> predictions;
};

bool runClassification(const CliOptions& options, InferenceReport& report, std::string& error);

