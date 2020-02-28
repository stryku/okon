#include <iostream>

#include "btree.hpp"
#include "btree_sorted_keys_inserter.hpp"
#include "fstream_wrapper.hpp"
#include "preparer.hpp"
#include "pwned_sonic.hpp"

#include <okon/okon.h>

#include <algorithm>
#include <fstream>
#include <iterator>
#include <optional>
#include <unordered_map>

auto to_sha1(std::string_view sha1_txt)
{
  okon::sha1_t sha1;

  std::memcpy(&sha1[0], sha1_txt.data(), 40);

  return sha1;
}

using parsed_args_t = std::unordered_map<std::string_view, std::string_view>;

std::optional<parsed_args_t> parse_args(const std::vector<std::string_view>& args)
{
  parsed_args_t result;

  const auto accepted_args = { std::string_view{ "--path" }, std::string_view{ "--hash" },
                               std::string_view{ "--prepare" }, std::string_view{ "--wd" },
                               std::string_view{ "--output" } };

  for (auto i = 0u; i < args.size(); i += 2) {
    if (std::find(std::cbegin(accepted_args), std::cend(accepted_args), args[i]) ==
        std::cend(accepted_args)) {
      std::cerr << "unknown argument: " << args[i].data() << '\n';
      return std::nullopt;
    }

    if (i + 1 >= args.size()) {
      std::cerr << "expected argument after: " << args[i].data() << '\n';
      return std::nullopt;
    }

    result[args[i]] = args[i + 1];
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

  for (std::string_view argument : { "--prepare", "--wd", "--output" }) {
    if (parsed_args->find(argument) != std::cend(*parsed_args)) {
      const auto success = handle_prepare(*parsed_args);
      if (!success) {
        return 2;
      }
      break;
    }
  }

  for (std::string_view argument : { "--path", "--hash" }) {
    if (parsed_args->find(argument) != std::cend(*parsed_args)) {
      const auto found = handle_check(*parsed_args);
      std::cout << (found ? 1 : 0) << '\n';
      return found ? 1 : 0;
    }
  }

  return 0;
}
