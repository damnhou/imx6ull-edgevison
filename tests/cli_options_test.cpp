#include "cli_options.h"

#include <cassert>
#include <cmath>
#include <string>
#include <vector>

namespace {

bool parse(const std::vector<std::string>& input, CliOptions& options, std::string& error)
{
    std::vector<char*> argv;
    for (std::size_t index = 0; index < input.size(); ++index)
        argv.push_back(const_cast<char*>(input[index].c_str()));
    return parseCliOptions(static_cast<int>(argv.size()), argv.data(), options, error);
}

} // namespace

int main()
{
    {
        CliOptions options;
        std::string error;
        const std::vector<std::string> args{
            "edgevision_cli", "--param", "m.param", "--bin", "m.bin", "--image", "a.jpg",
            "--pixel", "rgb", "--mean", "1,2,3", "--norm", "0.1,0.2,0.3",
            "--width", "224", "--repeat", "3", "--json"
        };
        assert(parse(args, options, error));
        assert(options.pixelOrder == "rgb");
        assert(options.inputWidth == 224);
        assert(options.repeat == 3);
        assert(options.json);
        assert(std::fabs(options.norm[2] - 0.3f) < 0.0001f);
    }
    {
        CliOptions options;
        std::string error;
        const std::vector<std::string> args{"edgevision_cli", "--param", "m.param"};
        assert(!parse(args, options, error));
        assert(error.find("required") != std::string::npos);
    }
    {
        CliOptions options;
        std::string error;
        const std::vector<std::string> args{
            "edgevision_cli", "--param", "m.param", "--bin", "m.bin", "--image", "a.jpg",
            "--mean", "1,2"
        };
        assert(!parse(args, options, error));
    }
    {
        CliOptions options;
        std::string error;
        const std::vector<std::string> args{
            "edgevision_cli", "--param", "m.param", "--bin", "m.bin", "--image", "a.jpg",
            "--warmup", "0"
        };
        assert(parse(args, options, error));
        assert(options.warmup == 0);
    }
    {
        CliOptions options;
        std::string error;
        const std::vector<std::string> args{
            "edgevision_cli", "--param", "m.param", "--bin", "m.bin", "--image", "a.jpg",
            "--mean", "1,,3"
        };
        assert(!parse(args, options, error));
    }
    return 0;
}
