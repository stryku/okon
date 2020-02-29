#include <okon/okon.h>

#include <algorithm>
#include <fstream>
#include <iostream>
#include <iterator>
#include <optional>
#include <unordered_map>
#include <vector>

using parsed_args_t = std::unordered_map<std::string_view, std::string_view>;

std::optional<parsed_args_t> parse_args(const std::vector<std::string_view>& args)
{
  parsed_args_t result;

  struct arg_metadata
  {
    std::string_view name;
    unsigned expected_value_count{ 1u };
  };

  const auto accepted_args = { arg_metadata{ "--path" },    arg_metadata{ "--hash" },
                               arg_metadata{ "--prepare" }, arg_metadata{ "--wd" },
                               arg_metadata{ "--output" },  arg_metadata{ "--help", 0u } };

  const auto find_argument =
    [&accepted_args](std::string_view passed_argument) -> std::optional<arg_metadata> {
    const auto pred = [&passed_argument](const arg_metadata& metadata) {
      return metadata.name == passed_argument;
    };

    const auto found = std::find_if(std::cbegin(accepted_args), std::cend(accepted_args), pred);
    return found == std::cend(accepted_args) ? std::optional<arg_metadata>{} : *found;
  };

  for (auto i = 0u; i < args.size();) {
    const auto metadata = find_argument(args[i]);
    if (!metadata) {
      std::cerr << "unknown argument: " << args[i].data() << '\n';
      return std::nullopt;
    }

    if (i + metadata->expected_value_count >= args.size()) {
      std::cerr << "expected argument after: " << args[i].data() << '\n';
      return std::nullopt;
    }

    result[args[i]] = args[i + metadata->expected_value_count];

    i += metadata->expected_value_count + 1u;
  }

  return result;
}

bool handle_prepare(const parsed_args_t& args)
{
  const auto found_prepare = args.find("--prepare");
  if (found_prepare == std::cend(args)) {
    std::cerr << "expected --prepare argument";
    return false;
  }

  const auto found_wd = args.find("--wd");
  if (found_wd == std::cend(args)) {
    std::cerr << "expected --wd argument";
    return false;
  }

  const auto found_output = args.find("--output");
  if (found_output == std::cend(args)) {
    std::cerr << "expected --output argument";
    return false;
  }

  const auto input_file_path = found_prepare->second;
  const auto working_directory_path = found_wd->second;
  const auto output_file_directory = found_output->second;

  const auto result = okon_prepare(input_file_path.data(), working_directory_path.data(),
                                   output_file_directory.data());
  return !result;
}

bool handle_check(const parsed_args_t& args)
{
  const auto found_path = args.find("--path");
  if (found_path == std::cend(args)) {
    std::cerr << "expected --path argument";
    return false;
  }

  const auto found_hash = args.find("--hash");
  if (found_hash == std::cend(args)) {
    std::cerr << "expected --hash argument";
    return false;
  }

  const auto file_path = found_path->second;
  const auto hash = found_hash->second;
  return okon_exists_text(hash.data(), file_path.data());
}

void print_help()
{
  std::cout << "To prepare a downloaded database:\n"
               "okon-cli --prepare path/to/downloaded/file.txt --wd path/to/working_directory "
               "--output path/to/prepared/file.okon\n"
               "To check whether a hash exists:\n"
               "okon-cli --path path/to/prepared/file.okon --hash "
               "0000000000000000000000000000000000000000\n";
}

int main(int argc, const char* argv[])
{
  std::vector<std::string_view> args;
  args.reserve(argc);

  for (auto i = 1u; i < argc; ++i) {
    args.emplace_back(argv[i]);
  }

  const auto parsed_args = parse_args(args);
  if (!parsed_args) {
    return 2;
  }

  for (std::string_view argument : { "--help" }) {
    if (parsed_args->find(argument) != std::cend(*parsed_args)) {
      print_help();
      return 0;
    }
  }

  for (std::string_view argument : { "--prepare", "--wd", "--output" }) {
    if (parsed_args->find(argument) != std::cend(*parsed_args)) {
      const auto success = handle_prepare(*parsed_args);
      if (!success) {
        return 2;
      }
      return 0;
    }
  }

  for (std::string_view argument : { "--path", "--hash" }) {
    if (parsed_args->find(argument) != std::cend(*parsed_args)) {
      const auto found = handle_check(*parsed_args);
      std::cout << (found ? 1 : 0) << '\n';
      return found ? 1 : 0;
    }
  }

  print_help();

  return 0;
}
