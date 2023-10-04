#ifndef CONFIG_H
#define CONFIG_H

namespace Config {

  const std::filesystem::path PATH_OFFSETS{ "./dfint_data/offsets/" };
  const std::filesystem::path PATH_LOG{ "./dfint_data/dfint_log.log" };
  const std::filesystem::path PATH_DICTIONARY{ "./dfint_data/dfint_dictionary.csv" };
  const std::filesystem::path PATH_REPORTS{ "./dfint_data/crash_reports/" };

  inline time_t PETimestamp(const std::string filename) {
    std::ifstream file(filename);

    if (!file.is_open()) {
      logger::error("unable to open PE file {}", filename);
    }
    char buf[64];
    file.get(buf, 64);
    auto dos_header = reinterpret_cast<IMAGE_DOS_HEADER*>(buf);
    file.seekg(dos_header->e_lfanew);
    file.get(buf, 64);
    auto nt_header = reinterpret_cast<IMAGE_NT_HEADERS*>(buf);

    return nt_header->FileHeader.TimeDateStamp;
  }

  inline toml::v3::ex::parse_result GetOffsetsFile(time_t target_checksum) {
    for (const auto& filepath : std::filesystem::recursive_directory_iterator(PATH_OFFSETS)) {
      auto file = toml::parse_file(filepath.path().c_str());
      if (target_checksum == file["metadata"]["checksum"].value_or<time_t>(0)) {
        return file;
      }
    }

    std::string message = std::format("unable to find offsets file for current version of DF (checksum 0x{:x})", target_checksum);

    MessageBoxA(nullptr, message.c_str(), "dfint hook error", MB_ICONERROR);
    exit(2);

    return toml::v3::ex::parse_result{};
  }

  inline auto config = toml::parse_file("./dfint_data/dfint_config.toml");
  inline auto pe_timestamp = Config::PETimestamp("Dwarf Fortress.exe");
  inline auto offsets = Config::GetOffsetsFile(Config::pe_timestamp);

  inline std::vector<uintptr_t> GetStringsOffsetArray() {
    auto strings_patches_nodes =
      Config::offsets["offsets"]["string_patches"] ? Config::offsets["offsets"]["string_patches"].as_array() : nullptr;
    std::vector<uintptr_t> strings_patches{};
    if (strings_patches_nodes) {
      for (toml::node& item : *strings_patches_nodes) {
        strings_patches.push_back(item.value_or<uintptr_t>(0));
      }
    }
    return strings_patches;
  };

  namespace Metadata {

    inline auto name = Config::config["metadata"]["name"].value_or<std::string>("");
    inline auto version = Config::offsets["metadata"]["version"].value_or<std::string>("");
    inline auto checksum = Config::offsets["metadata"]["checksum"].value_or<time_t>(0);
#ifdef HOOK_VERSION
    inline std::string hook_version(HOOK_VERSION);
#else
    inline std::string hook_version("not-defined");
#endif

  } // namespace Metadata

  namespace Setting {

    inline auto log_level = Config::config["settings"]["log_level"].value_or<int>(0);
    inline auto log_file = Config::config["settings"]["log_file"].value_or<std::string>(PATH_LOG.string());
    inline auto crash_report = Config::config["settings"]["crash_report"].value_or<bool>(true);
    inline auto enable_search = Config::config["settings"]["enable_search"].value_or<bool>(true);
    inline auto enable_translation = Config::config["settings"]["enable_translation"].value_or<bool>(true);
    inline auto enable_patches = Config::config["settings"]["enable_patches"].value_or<bool>(true);
    inline auto dictionary = Config::config["settings"]["dictionary"].value_or<std::string>(PATH_DICTIONARY.string());
    inline auto crash_report_dir = Config::config["settings"]["crash_report_dir"].value_or<std::string>(PATH_REPORTS.string());
    inline auto watchdog = Config::config["settings"]["watchdog"].value_or<bool>(true);

  } // namespace Setting

  namespace Keybinding {

    inline auto shift = Config::offsets["offsets"]["keybinding"].value_or<uint32_t>(0);

  } // namespace Keybinding

  namespace Offset {

    inline auto string_copy = Config::offsets["offsets"]["string_copy"].value_or<uintptr_t>(0);
    inline auto string_copy_n = Config::offsets["offsets"]["string_copy_n"].value_or<uintptr_t>(0);
    inline auto string_append = Config::offsets["offsets"]["string_append"].value_or<uintptr_t>(0);
    inline auto string_append_0 = Config::offsets["offsets"]["string_append_0"].value_or<uintptr_t>(0);
    inline auto string_append_n = Config::offsets["offsets"]["string_append_n"].value_or<uintptr_t>(0);
    inline auto convert_ulong_to_string = Config::offsets["offsets"]["convert_ulong_to_string"].value_or<uintptr_t>(0);
    inline auto addst = Config::offsets["offsets"]["addst"].value_or<uintptr_t>(0);
    inline auto addst_top = Config::offsets["offsets"]["addst_top"].value_or<uintptr_t>(0);
    inline auto addcoloredst = Config::offsets["offsets"]["addcoloredst"].value_or<uintptr_t>(0);
    inline auto addst_flag = Config::offsets["offsets"]["addst_flag"].value_or<uintptr_t>(0);
    inline auto addst_template = Config::offsets["offsets"]["addst_template"].value_or<uintptr_t>(0);

    inline auto standardstringentry = Config::offsets["offsets"]["standardstringentry"].value_or<uintptr_t>(0);
    inline auto simplify_string = Config::offsets["offsets"]["simplify_string"].value_or<uintptr_t>(0);
    inline auto upper_case_string = Config::offsets["offsets"]["upper_case_string"].value_or<uintptr_t>(0);
    inline auto lower_case_string = Config::offsets["offsets"]["lower_case_string"].value_or<uintptr_t>(0);
    inline auto capitalize_string_words = Config::offsets["offsets"]["capitalize_string_words"].value_or<uintptr_t>(0);
    inline auto capitalize_string_first_word = Config::offsets["offsets"]["capitalize_string_first_word"].value_or<uintptr_t>(0);

    inline auto addchar = Config::offsets["offsets"]["addchar"].value_or<uintptr_t>(0);
    inline auto addchar_top = Config::offsets["offsets"]["addchar_top"].value_or<uintptr_t>(0);
    inline auto add_texture = Config::offsets["offsets"]["add_texture"].value_or<uintptr_t>(0);
    inline auto gps_allocate = Config::offsets["offsets"]["gps_allocate"].value_or<uintptr_t>(0);
    inline auto cleanup_arrays = Config::offsets["offsets"]["cleanup_arrays"].value_or<uintptr_t>(0);
    inline auto screen_to_texid = Config::offsets["offsets"]["screen_to_texid"].value_or<uintptr_t>(0);
    inline auto screen_to_texid_top = Config::offsets["offsets"]["screen_to_texid_top"].value_or<uintptr_t>(0);

    inline auto loading_world_new_game_loop = Config::offsets["offsets"]["loading_world_new_game_loop"].value_or<uintptr_t>(0);
    inline auto loading_world_continuing_game_loop =
      Config::offsets["offsets"]["loading_world_continuing_game_loop"].value_or<uintptr_t>(0);
    inline auto loading_world_start_new_game_loop = Config::offsets["offsets"]["loading_world_start_new_game_loop"].value_or<uintptr_t>(0);
    inline auto menu_interface_loop = Config::offsets["offsets"]["menu_interface_loop"].value_or<uintptr_t>(0);

    inline auto string_patches = Config::GetStringsOffsetArray();

  } // namespace Offset

} // namespace Config

#endif