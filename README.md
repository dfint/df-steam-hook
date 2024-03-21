# df-steam-hook

[![XMake](https://github.com/dfint/df-steam-hook/actions/workflows/xmake.yml/badge.svg)](https://github.com/dfint/df-steam-hook/actions/workflows/xmake.yml)
[![clang-format Check](https://github.com/dfint/df-steam-hook/actions/workflows/clang-format-check.yml/badge.svg)](https://github.com/dfint/df-steam-hook/actions/workflows/clang-format-check.yml)
[![Total downloads of all releases](https://img.shields.io/github/downloads/dfint/df-steam-hook/total)](https://github.com/dfint/df-steam-hook/releases)
[![Downloads of the latest release](https://img.shields.io/github/downloads/dfint/df-steam-hook/latest/total)](https://github.com/dfint/df-steam-hook/releases/latest)

> [!Note]
> For Dwarf Fortress versions 50.12 and newer use [localization installer/updater](https://github.com/dfint/installer), which installs new [df-steam-hook-rs](https://github.com/dfint/df-steam-hook-rs).
>
> The current project (df-steam-hook) is going to be suspended.

## Basic feature:

- Intercept text in Dwarf Fortress game and replace according to dfint_dictionary.csv

## Additional features:

- Creates crash reports (`cr_*.txt` files) when the game crashes
- Supports several version of DF; it is possible to add support of new versions with adding a config file to `dfint_data/offset` directory;
- Useful hotkeys:
  - <kbd>Ctrl</kbd>+<kbd>F2</kbd> - reload csv dictionary file;
  - <kbd>Ctrl</kbd>+<kbd>F3</kbd> - switch the translation off;
  - <kbd>Ctrl</kbd>+<kbd>F4</kbd> - switch the translation on.

## How to install:

Download an `dfint-steam-hook-*.zip` archive from the [latest release](https://github.com/dfint/df-steam-hook/releases/latest), unpack it to the game's directory (with replacement of all files).

The package contains an empty translation file `dfint_dictionary.csv`, which you need to replace with a file for your language. There are ready to use files in the https://github.com/dfint/autobuild repository in the `translation_build` directory. To download a file, you need open it (click on the file in the file list), then press "Download raw file" icon button in the top right corner of the file area.

Also you'll probably need to modify `curses_640x300.png` font in the `data/art` directory of the game according to the encoding you've chosen on the to encode csv file. You don't need to redraw the entire font just add characters which is necessary to display text in your language correctly.

To run the game with translation enabled you need to run `dfint_launcher.exe` file (which you unpacked from `dfint-steam-hook-*.zip` archive).

## Supported languages

Keep in mind, that some languages cannot be used with Dwarf Fortress at the moment (at least with our version of **df-steam-hook**): in particular, no languages with rigt-to-left and hieroglyphic wrighting systems are supported.

There are forks of **df-steam-hook** for [Korean](https://github.com/Kheeman/df-steam-hook) and [Chinese](https://github.com/trotsky1997/df-steam-hook), so for these languages try to contact owners of these repositories.

Languages with Latin and Cyrillic writing systems are supported. Other languages with compact alphabets (e.g., Greek) also can be supported, you just need to draw a font for it (and request addition of the language on transifex). Also, text entry and search for alternative alphabets needs additional development. For now, Cyrillic is the only alternative alphabet, for which text entry and search is implemented.
