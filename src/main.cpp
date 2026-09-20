#include "cli_options.h"
#include "ncnn_classifier.h"

#include <iomanip>
#include <iostream>
#include <sstream>

namespace {

std::string jsonEscape(const std::string& input)
{
    std::ostringstream output;
    for (std::string::const_iterator it = input.begin(); it != input.end(); ++it) {
        switch (*it) {
        case '\\': output << "\\\\"; break;
        case '"': output << "\\\""; break;
        case '\n': output << "\\n"; break;
        case '\r': output << "\\r"; break;
        case '\t': output << "\\t"; break;
        default: output << *it; break;
        }
    }
    return output.str();
}

void printText(const InferenceReport& report)
{
    std::cout << std::fixed << std::setprecision(3)
              << "image=" << report.sourceWidth << "x" << report.sourceHeight << '\n'
              << "preprocess_ms=" << report.preprocessMs << '\n'
              << "first_inference_ms=" << report.firstInferenceMs << '\n'
              << "average_inference_ms=" << report.averageInferenceMs << '\n'
              << "min_inference_ms=" << report.minInferenceMs << '\n'
              << "max_inference_ms=" << report.maxInferenceMs << '\n'
              << "peak_rss_kb=" << report.peakRssKb << '\n';
    for (std::size_t index = 0; index < report.predictions.size(); ++index) {
        const Prediction& item = report.predictions[index];
        std::cout << "top" << index + 1 << "=" << item.classId << ',' << item.score;
        if (!item.label.empty()) std::cout << ',' << item.label;
        std::cout << '\n';
    }
}

void printJson(const InferenceReport& report)
{
    std::cout << std::fixed << std::setprecision(6)
              << "{\"image_width\":" << report.sourceWidth
              << ",\"image_height\":" << report.sourceHeight
              << ",\"preprocess_ms\":" << report.preprocessMs
              << ",\"first_inference_ms\":" << report.firstInferenceMs
              << ",\"average_inference_ms\":" << report.averageInferenceMs
              << ",\"min_inference_ms\":" << report.minInferenceMs
              << ",\"max_inference_ms\":" << report.maxInferenceMs
              << ",\"peak_rss_kb\":" << report.peakRssKb
              << ",\"predictions\":[";
    for (std::size_t index = 0; index < report.predictions.size(); ++index) {
        if (index) std::cout << ',';
        const Prediction& item = report.predictions[index];
        std::cout << "{\"class_id\":" << item.classId
                  << ",\"score\":" << item.score
                  << ",\"label\":\"" << jsonEscape(item.label) << "\"}";
    }
    std::cout << "]}\n";
}

} // namespace

int main(int argc, char** argv)
{
    CliOptions options;
    std::string error;
    if (!parseCliOptions(argc, argv, options, error)) {
        std::cerr << "error: " << error << "\n\n";
        printUsage(std::cerr, argv[0]);
        return 2;
    }
    if (options.help) {
        printUsage(std::cout, argv[0]);
        return 0;
    }

    InferenceReport report;
    if (!runClassification(options, report, error)) {
        std::cerr << "error: " << error << '\n';
        return 1;
    }
    if (options.json) printJson(report);
    else printText(report);
    return 0;
}

