#include "ncnn_classifier.h"
#include "system_metrics.h"

#include <net.h>
#include <simpleocv.h>

#include <algorithm>
#include <chrono>
#include <fstream>
#include <functional>
#include <limits>
#include <numeric>
#include <utility>

namespace {

typedef std::chrono::steady_clock Clock;

double elapsedMs(const Clock::time_point& begin, const Clock::time_point& end)
{
    return std::chrono::duration_cast<std::chrono::duration<double, std::milli> >(end - begin).count();
}

std::vector<std::string> loadLabels(const std::string& path)
{
    std::vector<std::string> labels;
    if (path.empty())
        return labels;
    std::ifstream input(path.c_str());
    std::string line;
    while (std::getline(input, line)) {
        if (!line.empty() && line[line.size() - 1] == '\r')
            line.resize(line.size() - 1);
        labels.push_back(line);
    }
    return labels;
}

bool extractOnce(ncnn::Net& net, const CliOptions& options, const ncnn::Mat& input,
                 ncnn::Mat& output, double& durationMs, std::string& error)
{
    ncnn::Extractor extractor = net.create_extractor();
    extractor.set_light_mode(true);
    extractor.set_num_threads(options.threads);
    const Clock::time_point begin = Clock::now();
    if (extractor.input(options.inputBlob.c_str(), input) != 0) {
        error = "failed to set input blob: " + options.inputBlob;
        return false;
    }
    if (extractor.extract(options.outputBlob.c_str(), output) != 0) {
        error = "failed to extract output blob: " + options.outputBlob;
        return false;
    }
    durationMs = elapsedMs(begin, Clock::now());
    return true;
}

} // namespace

bool runClassification(const CliOptions& options, InferenceReport& report, std::string& error)
{
    ncnn::Net net;
    net.opt.use_vulkan_compute = false;
    net.opt.num_threads = options.threads;

    if (net.load_param(options.paramPath.c_str()) != 0) {
        error = "failed to load param file: " + options.paramPath;
        return false;
    }
    if (net.load_model(options.binPath.c_str()) != 0) {
        error = "failed to load model weights: " + options.binPath;
        return false;
    }

    const cv::Mat image = cv::imread(options.imagePath, 1);
    if (image.empty()) {
        error = "failed to decode image: " + options.imagePath;
        return false;
    }
    report.sourceWidth = image.cols;
    report.sourceHeight = image.rows;

    const Clock::time_point preprocessBegin = Clock::now();
    const int pixelType = options.pixelOrder == "rgb"
        ? ncnn::Mat::PIXEL_BGR2RGB
        : ncnn::Mat::PIXEL_BGR;
    ncnn::Mat input = ncnn::Mat::from_pixels_resize(
        image.data, pixelType, image.cols, image.rows, options.inputWidth, options.inputHeight);
    input.substract_mean_normalize(options.mean.data(), options.norm.data());
    report.preprocessMs = elapsedMs(preprocessBegin, Clock::now());

    ncnn::Mat output;
    double ignoredMs = 0.0;
    for (int index = 0; index < options.warmup; ++index) {
        if (!extractOnce(net, options, input, output, ignoredMs, error))
            return false;
    }

    std::vector<double> times;
    times.reserve(static_cast<std::size_t>(options.repeat));
    for (int index = 0; index < options.repeat; ++index) {
        double durationMs = 0.0;
        if (!extractOnce(net, options, input, output, durationMs, error))
            return false;
        times.push_back(durationMs);
    }
    report.firstInferenceMs = times.front();
    report.minInferenceMs = *std::min_element(times.begin(), times.end());
    report.maxInferenceMs = *std::max_element(times.begin(), times.end());
    report.averageInferenceMs = std::accumulate(times.begin(), times.end(), 0.0) / times.size();
    report.peakRssKb = peakResidentSetKb();

    ncnn::Mat flat = output.reshape(static_cast<int>(output.total()));
    if (flat.empty()) {
        error = "model produced an empty output";
        return false;
    }

    std::vector<std::pair<float, int> > ranked;
    ranked.reserve(static_cast<std::size_t>(flat.w));
    for (int index = 0; index < flat.w; ++index)
        ranked.push_back(std::make_pair(flat[index], index));
    const int count = std::min(options.topK, static_cast<int>(ranked.size()));
    std::partial_sort(ranked.begin(), ranked.begin() + count, ranked.end(),
                      std::greater<std::pair<float, int> >());

    const std::vector<std::string> labels = loadLabels(options.labelsPath);
    for (int index = 0; index < count; ++index) {
        Prediction prediction;
        prediction.score = ranked[index].first;
        prediction.classId = ranked[index].second;
        if (prediction.classId >= 0 && prediction.classId < static_cast<int>(labels.size()))
            prediction.label = labels[prediction.classId];
        report.predictions.push_back(prediction);
    }
    return true;
}
