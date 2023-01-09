#ifndef CONFIG_H
#define CONFIG_H

namespace Config {

  inline auto config = toml::parse_file("dfint_config.toml");

  namespace Metadata {

    inline auto name = Config::config["metadata"]["name"].value_or<std::string>("");
    inline auto version = Config::config["metadata"]["version"].value_or<std::string>("");
    inline auto checksum = Config::config["metadata"]["checksum"].value_or<time_t>(0);

  }

  namespace Setting {

    inline auto log_level = Config::config["settings"]["log_level"].value_or<int>(0);
    inline auto enable_search = Config::config["settings"]["enable_search"].value_or<bool>(true);

  }

  namespace Offset {

    inline int string_copy = Config::config["offsets"]["string_copy"].value_or<uintptr_t>(0);
    inline int string_copy_n = Config::config["offsets"]["string_copy_n"].value_or<uintptr_t>(0);
    inline int string_append_n = Config::config["offsets"]["string_append_n"].value_or<uintptr_t>(0);
    inline int addst = Config::config["offsets"]["addst"].value_or<uintptr_t>(0);
    inline int addst_top = Config::config["offsets"]["addst_top"].value_or<uintptr_t>(0);
    inline int addcoloredst = Config::config["offsets"]["addcoloredst"].value_or<uintptr_t>(0);
    inline int addst_flag = Config::config["offsets"]["addst_flag"].value_or<uintptr_t>(0);
    inline int addst_template = Config::config["offsets"]["addst_template"].value_or<uintptr_t>(0);

    inline int standardstringentry = Config::config["offsets"]["standardstringentry"].value_or<uintptr_t>(0);
    inline int simplify_string = Config::config["offsets"]["simplify_string"].value_or<uintptr_t>(0);
    inline int upper_case_string = Config::config["offsets"]["upper_case_string"].value_or<uintptr_t>(0);
    inline int lower_case_string = Config::config["offsets"]["lower_case_string"].value_or<uintptr_t>(0);
    inline int capitalize_string_words = Config::config["offsets"]["capitalize_string_words"].value_or<uintptr_t>(0);
    inline int capitalize_string_first_word =
      Config::config["offsets"]["capitalize_string_first_word"].value_or<uintptr_t>(0);

    inline int addchar = Config::config["offsets"]["addchar"].value_or<uintptr_t>(0);
    inline int addchar_top = Config::config["offsets"]["addchar_top"].value_or<uintptr_t>(0);
    inline int add_texture = Config::config["offsets"]["add_texture"].value_or<uintptr_t>(0);
    inline int gps_allocate = Config::config["offsets"]["gps_allocate"].value_or<uintptr_t>(0);
    inline int cleanup_arrays = Config::config["offsets"]["cleanup_arrays"].value_or<uintptr_t>(0);
    inline int screen_to_texid = Config::config["offsets"]["screen_to_texid"].value_or<uintptr_t>(0);
    inline int screen_to_texid_top = Config::config["offsets"]["screen_to_texid_top"].value_or<uintptr_t>(0);

    inline int loading_world_new_game_loop =
      Config::config["offsets"]["loading_world_new_game_loop"].value_or<uintptr_t>(0);
    inline int loading_world_continuing_game_loop =
      Config::config["offsets"]["loading_world_continuing_game_loop"].value_or<uintptr_t>(0);
    inline int loading_world_start_new_game_loop =
      Config::config["offsets"]["loading_world_start_new_game_loop"].value_or<uintptr_t>(0);
    inline int menu_interface_loop = Config::config["offsets"]["menu_interface_loop"].value_or<uintptr_t>(0);

  }
}

#endif