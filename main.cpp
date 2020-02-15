#include <iostream>

#include "btree.hpp"
#include "btree_sorted_keys_inserter.hpp"
#include "fstream_wrapper.hpp"
#include "preparer.hpp"
#include "pwned_sonic.hpp"
#include "trie_file_generator.hpp"

#include <fstream>

auto to_sha1(std::string_view sha1_txt)
{
  pwned::sha1_t sha1;

  std::memcpy(&sha1[0], sha1_txt.data(), 40);

  return sha1;
}

int main()
{
  //  std::ofstream{ "/media/stryku/tb_disk/haveibeenpwned/pwned_head.pwned_trie" };

  //  const std::string_view btree_path = "/media/stryku/tb_disk/haveibeenpwned/prepared/btree";
  //  std::ofstream{ btree_path.data() };

  //  pwned::preparer generator{ "/media/stryku/tb_disk/haveibeenpwned/pwned_original.pwned_db",
  //                             "/media/stryku/tb_disk/haveibeenpwned/prepared/" };
  //  generator.prepare();

  pwned::fstream_wrapper wrapper{ "/media/stryku/tb_disk/haveibeenpwned/prepared/btree" };
  pwned::btree<pwned::fstream_wrapper> tree{ wrapper };
  const auto s = pwned::to_sha1("0E115FEEAB9474B9D680E5528024201AF6E7722F");
  const auto result = tree.contains(s);

  //  const std::string_view btree_path =
  //    "/home/stryku/my/programming/pwned_lightning/tmp/btree_test/btree";
  //  std::ofstream{ btree_path.data() };

  //  pwned::btree btree{ btree_path, 2 };
  //
  //  btree.insert_sorted(to_sha1("00AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"));
  //  btree.insert_sorted(to_sha1("10AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"));
  //  btree.insert_sorted(to_sha1("20AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"));
  //  btree.insert_sorted(to_sha1("30AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"));
  //  btree.insert_sorted(to_sha1("40AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"));
  //  btree.insert_sorted(to_sha1("50AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"));
  //  btree.insert_sorted(to_sha1("60AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"));
  //  btree.insert_sorted(to_sha1("70AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"));
  //  btree.insert_sorted(to_sha1("80AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"));
  //  btree.insert_sorted(to_sha1("90AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"));
  //  btree.insert_sorted(to_sha1("A0AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"));
  //  btree.insert_sorted(to_sha1("B0AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"));
  //  btree.insert_sorted(to_sha1("C0AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"));
  //  btree.insert_sorted(to_sha1("D0AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"));
  //  btree.insert_sorted(to_sha1("E0AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"));
  //  btree.insert_sorted(to_sha1("F0AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"));
  //
  //  btree.finalize_inserting();

  //  std::cout << (pwned::exists_splitted_sorted(
  //                  "/media/stryku/tb_disk/haveibeenpwned/prepared_full_text/",
  //                  "895B317C76B8E504C2FB32DBB4420178F60CE321")
  //                  ? "exists"
  //                  : "doesnt exist");

  //  const auto result = generator.prepare();
  //
  //  switch (result) {
  //
  //    case pwned::trie_file_generator::generation_result::input_file_not_found:
  //      std::cout << "input_file_not_found";
  //      break;
  //    case pwned::trie_file_generator::generation_result::cant_open_output_file:
  //      std::cout << "cant_open_output_file";
  //      break;
  //    case pwned::trie_file_generator::generation_result::success:
  //      std::cout << "success";
  //      break;
  //  }
  return 0;
}
