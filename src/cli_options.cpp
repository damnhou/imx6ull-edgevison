#include "cli_options.h"

#include <cstdlib>
#include <sstream>

namespace {

bool parsePositiveInt(const std::string& text, int& value)
{
    char* end = 0;
    const long parsed = std::strtol(text.c_str(), &end, 10);
    if (!end || *end != '\0' || parsed <= 0 || parsed > 100000)
        return false;
    value = static_cast<int>(parsed);
    return true;
}

bool parseNonNegativeInt(const std::string& text, int& value)
{
    char* end = 0;
    const long parsed = std::strtol(text.c_str(), &end, 10);
    if (!end || end == text.c_str() || *end != '\0' || parsed < 0 || parsed > 100000)
        return false;
    value = static_cast<int>(parsed);
    return true;
}

bool parseTriplet(const std::string& text, std::array<float, 3>& value)
{
    std::stringstream input(text);
    std::string part;
    for (int index = 0; index < 3; ++index) {
        if (!std::getline(input, part, ','))
            return false;
        char* end = 0;
        value[index] = std::strtof(part.c_str(), &end);
        if (!end || end == part.c_str() || *end != '\0')
            return false;
    }
    return !std::getline(input, part, ',');
}

bool takeValue(int argc, char** argv, int& index, std::string& value, std::string& error)
{
    if (index + 1 >= argc) {
        error = std::string("missing value after ") + argv[index];
        return false;
    }
    value = argv[++index];
    return true;
}

} // namespace

bool parseCliOptions(int argc, char** argv, CliOptions& options, std::string& error)
{
    for (int index = 1; index < argc; ++index) {
        const std::string argument(argv[index]);
        std::string value;
        if (argument == "--help" || argument == "-h") {
            options.help = true;
        } else if (argument == "--json") {
            options.json = true;
        } else if (argument == "--param") {
            if (!takeValue(argc, argv, index, options.paramPath, error)) return false;
        } else if (argument == "--bin") {
            if (!takeValue(argc, argv, index, options.binPath, error)) return false;
        } else if (argument == "--image") {
            if (!takeValue(argc, argv, index, options.imagePath, error)) return false;
        } else if (argument == "--labels") {
            if (!takeValue(argc, argv, index, options.labelsPath, error)) return false;
        } else if (argument == "--input") {
            if (!takeValue(argc, argv, index, options.inputBlob, error)) return false;
        } else if (argument == "--output") {
            if (!takeValue(argc, argv, index, options.outputBlob, error)) return false;
        } else if (argument == "--pixel") {
            if (!takeValue(argc, argv, index, options.pixelOrder, error)) return false;
            if (options.pixelOrder != "rgb" && options.pixelOrder != "bgr") {
                error = "--pixel must be rgb or bgr";
                return false;
            }
        } else if (argument == "--warmup") {
            if (!takeValue(argc, argv, index, value, error)) return false;
            if (!parseNonNegativeInt(value, options.warmup)) {
                error = "--warmup must be a non-negative integer";
                return false;
            }
        } else if (argument == "--width" || argument == "--height" || argument == "--topk"
                   || argument == "--repeat" || argument == "--threads") {
            if (!takeValue(argc, argv, index, value, error)) return false;
            int parsed = 0;
            if (!parsePositiveInt(value, parsed)) {
                error = argument + " must be a positive integer";
                return false;
            }
            if (argument == "--width") options.inputWidth = parsed;
            else if (argument == "--height") options.inputHeight = parsed;
            else if (argument == "--topk") options.topK = parsed;
            else if (argument == "--repeat") options.repeat = parsed;
            else options.threads = parsed;
        } else if (argument == "--mean" || argument == "--norm") {
            if (!takeValue(argc, argv, index, value, error)) return false;
            std::array<float, 3>& target = argument == "--mean" ? options.mean : options.norm;
            if (!parseTriplet(value, target)) {
                error = argument + " must contain three comma-separated numbers";
                return false;
            }
        } else {
            error = "unknown argument: " + argument;
            return false;
        }
    }

    if (options.help)
        return true;
    if (options.paramPath.empty() || options.binPath.empty() || options.imagePath.empty()) {
        error = "--param, --bin and --image are required";
        return false;
    }
    return true;
}

void printUsage(std::ostream& stream, const char* programName)
{
    stream
        << "Usage: " << programName << " --param MODEL.param --bin MODEL.bin --image IMAGE [options]\n"
        << "\nRequired:\n"
        << "  --param PATH       ncnn network structure\n"
        << "  --bin PATH         ncnn model weights\n"
        << "  --image PATH       local JPEG/PNG/BMP image\n"
        << "\nModel options (defaults match ncnn SqueezeNet 1.1):\n"
        << "  --labels PATH      one label per line\n"
        << "  --input NAME       input blob name (data)\n"
        << "  --output NAME      output blob name (prob)\n"
        << "  --width N          model input width (227)\n"
        << "  --height N         model input height (227)\n"
        << "  --pixel rgb|bgr    channel order expected by model (bgr)\n"
        << "  --mean A,B,C       per-channel mean (104,117,123)\n"
        << "  --norm A,B,C       per-channel scale (1,1,1)\n"
        << "  --topk N           number of classes to print (5)\n"
        << "  --warmup N         warm-up runs, 0 disables warm-up (1)\n"
        << "  --repeat N         measured runs (5)\n"
        << "  --threads N        ncnn inference threads (1 for i.MX6ULL)\n"
        << "  --json             emit machine-readable JSON\n";
}
