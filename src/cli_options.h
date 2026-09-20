#pragma once

#include <array>
#include <iosfwd>
#include <string>

struct CliOptions
{
    std::string paramPath;
    std::string binPath;
    std::string imagePath;
    std::string labelsPath;
    std::string inputBlob = "data";
    std::string outputBlob = "prob";
    std::string pixelOrder = "bgr";
    int inputWidth = 227;
    int inputHeight = 227;
    int topK = 5;
    int warmup = 1;
    int repeat = 5;
    int threads = 1;
    std::array<float, 3> mean{{104.f, 117.f, 123.f}};
    std::array<float, 3> norm{{1.f, 1.f, 1.f}};
    bool json = false;
    bool help = false;
};

bool parseCliOptions(int argc, char** argv, CliOptions& options, std::string& error);
void printUsage(std::ostream& stream, const char* programName);

