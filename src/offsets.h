#ifndef OFFSETS_H
#define OFFSETS_H

namespace Offset {
// translation
inline constexpr int string_copy{ 0xB5B0 };
inline constexpr int string_copy_n{ 0xB5D0 };
inline constexpr int string_append_n{ 0xB710 };
inline constexpr int addst{ 0x784C60 };
inline constexpr int addst_top{ 0x784DB0 };
inline constexpr int addcoloredst{ 0x784890 };
inline constexpr int addst_flag{ 0x784970 };
inline constexpr int addst_template{ 0x96E710 };

// search
inline constexpr int standardstringentry{ 0x87ED50 };
inline constexpr int simplify_string{ 0x35C7A0 };
inline constexpr int upper_case_string{ 0x35CAE0 };
inline constexpr int lower_case_string{ 0x35C940 };
inline constexpr int capitalize_string_words{ 0x35CC80 };
inline constexpr int capitalize_string_first_word{ 0x35CF00 };

// ttf inject
inline constexpr int add_texture{ 0xE827F0 };
inline constexpr int addchar{ 0x55D80 };
inline constexpr int addchar_top{ 0xE9D60 };
inline constexpr int gps_allocate{ 0x5C2AB0 };
inline constexpr int cleanup_arrays{ 0x5C28D0 };
inline constexpr int screen_to_texid{ 0x5BAB40 };
inline constexpr int screen_to_texid_top{ 0x5BAD30 };
inline constexpr int reshape{ 0x5C0930 };
inline constexpr int load_multi_pdim{ 0xE82890 };
inline constexpr int load_multi_pdim_2{ 0xE82AD0 };
inline constexpr int upload_textures{ 0xE82020 };
inline constexpr int loading_world_new_game_loop{ 0x9FD2E0 };
inline constexpr int loading_world_continuing_game_loop{ 0x566F40 };
inline constexpr int loading_world_start_new_game_loop{ 0x5652C0 };
inline constexpr int menu_interface_loop{ 0x1678A0 };
}

#endif