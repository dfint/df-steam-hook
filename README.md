# df-steam-hook-kr

[![XMake](https://github.com/dfint/df-steam-hook/actions/workflows/xmake.yml/badge.svg)](https://github.com/dfint/df-steam-hook/actions/workflows/xmake.yml)
[![clang-format Check](https://github.com/dfint/df-steam-hook/actions/workflows/clang-format-check.yml/badge.svg)](https://github.com/dfint/df-steam-hook/actions/workflows/clang-format-check.yml)

## 기본 기능:

- 드워프 포트리스 글자를 가로채서 dfint_dictionary.csv 와 kr_regex.txt를 참고해서 글자를 그림으로 생성 후에 화면에 뿌립니다.

## 추가 기능:

- 게임 충돌 할 때 (`cr_*.txt` files) 파일 생성합니다
- 적용된 번역을 상황을 보기 위해서 (dfint_log.log) 파일 생성합니다. !뭔가 잘못 오류가 생겼을때 무지막지한 용량이 됩니다 확인하세요! 
- DF의 버전 별로 동시에 지원할 수 있으며, `dfint_data/offset` 디렉토리에 구성 파일을 추가하여 새 버전을 추가할 수 있습니다.;
- 단축키:
  - <kbd>Ctrl</kbd>+<kbd>F2</kbd> - csv 파일 다시 읽시;
  - <kbd>Ctrl</kbd>+<kbd>F3</kbd> - 번역 끄기;
  - <kbd>Ctrl</kbd>+<kbd>F4</kbd> - 번역 켜기.
- dfint_config.toml파일에서 폰트, 사이즈 조절 가능
## 추가 사항:

- 아직 많이 부족합니다만 번역에 조금이라도 도움이 되고자 업로드 합니다.
- 능력자분들 코드 개선에 많은 도움 좀 주세요~! 

