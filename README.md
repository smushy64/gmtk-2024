# GMTK 2024

This is the source code to my submission to GMTK Game Jam 2024!

Depends on raylib and the required system libraries for
whatever target platform project is being compiled to.

## Build Requirements
### Windows
- Latest Windows SDK
- Mingw-W64 with clang, make and ar
### Web
- emscripten
- Mingw-W64 with clang, make and ar (Windows only)
### Linux
- glibc
- clang
- make
- ar
- X11 development tools

## Build Instructions

- Clone repository
```sh
git clone https://github.com/smushy64/gmtk-2024.git --depth=1 --recursive-submodules
```

- Build cbuild with clang (Mingw-W64 on Windows)
```sh
clang cbuild.c -o cbuild
```

- Run cbuild:
    - Windows/Linux
    ```sh
    ./cbuild --platform=native
    ```
    - Web
    ```sh
    ./cbuild --platform=web
    ```

- Compiled project will be in `build` directory. Raylib library is in `vendor` directory.

## Editor Configuration

A .editorconfig file is included in this repository
to automatically configure editors that support it.

If you use Neovim + Telescope (like I do),
I've also included a .ripgreprc file that makes
ripgrep ignore directories that are not listed in
.gitignore.

A compile_flags.txt file is also included in `src`
for those using clangd.

## Credits

- Alicia Amarilla (smushy64) : Game Develpment

Copyright (c) alicia amarilla 2024

