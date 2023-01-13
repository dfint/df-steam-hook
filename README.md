# df-steam-hook

[![XMake](https://github.com/dfint/df-steam-hook/actions/workflows/xmake.yml/badge.svg)](https://github.com/dfint/df-steam-hook/actions/workflows/xmake.yml)
[![clang-format Check](https://github.com/dfint/df-steam-hook/actions/workflows/clang-format-check.yml/badge.svg)](https://github.com/dfint/df-steam-hook/actions/workflows/clang-format-check.yml)

## Basic feature:

- Intercept text in Dwarf Fortress game and replace according to dfint_dictionary.csv

## Additional features:

- Creates crash reports (`cr_*.txt` files) when the game crashes
- Can support several features of DF at the same time, support of new versions is possible to add with adding a config file to `dfint_data/offset` directory;
- Useful hotkeys:
  - <kbd>Ctrl</kbd>+<kbd>F2</kbd> - reload csv dictionary file;
  - <kbd>Ctrl</kbd>+<kbd>F3</kbd> - switch the translation off;
  - <kbd>Ctrl</kbd>+<kbd>F4</kbd> - switch the translation on.

## How to install:

Unpack `dfint-steam-hook-*.zip` to the game's directory and run `dfint_launcher.exe` file. It will run the game with translation applied.

For your language you'll need to prepare csv file from the [hardcoded_steam resource on transifex](https://www.transifex.com/dwarf-fortress-translation/dwarf-fortress-steam/hardcoded_steam/). The package contains an empty csv file as a placeholder, so you'll not see any translation before you generate actual csv file for your language.

To do this, you need to use tools from [df-gettext-toolkit](https://github.com/dfint/df-gettext-toolkit) repository, see the first example in "Usage examples". The last parameter must be an encoding which is relevant for your language. E.g. for Esperanto can be used `latin3`.

Also you'll probably need to modify `curses_640x300.png` font in the `data/art` directory of the game according to the encoding you've chosen on the to encode csv file. You don't need to redraw the entire font just add characters which is necessary to display text in your language correctly.

Later we'll add posibility to create csv file using [df-translation-client](https://github.com/dfint/df-translation-client).
