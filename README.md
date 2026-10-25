# YaPang

A matching decompilation of the **PangYa** client (`ProjectG_ReleaseQA.exe`,
build of 2011-10-21) and its Direct3D renderer (`wangreal_ReleaseQA.dll`).

The goal is C++ source that, compiled with the original toolchain
(Visual C++ .NET 2003 SP1, cl 13.10.6030) and the original flags, produces
the same machine code as the shipped binary, function by function. Every
function is checked against the original with objdiff; a function counts as
done only at a 100% match.

`src/` mirrors the original source tree: each translation unit lives at its
original path (`src/client/ProjectG/...`, `src/client/Fresh/...`,
`src/client/Wangreal/...`, `src/shared/...`).

One exception: `src/jp/` is not matching. It is the JP 983 client's game
code around its Refresh UI, decompiled for a port and compiled by clang only
(`src/jp/README.md`).

Third-party code linked into the game (the C++ library, ATL, Lua, FreeType,
libpng, zlib, nProtect GameGuard, Miles) is not decompiled.

## Future direction

The project may later be upgraded to work with the latest PangYa game data.
That work would mean changing the code, so the project could move beyond
being a strict matching decompilation of the 2011 QA build.

## Credits

The files used to set up this decompilation were retrieved from
[pangbox/rebang](https://github.com/pangbox/rebang/tree/master), which is
the source of the ProjectG QA build this project targets.

## Copyright

PangYa, its client, its assets and every trademark attached to them are the
property of **Ntreev Soft** and its licensors. This project is not affiliated
with, endorsed by or sponsored by them.

This repository contains only source code written by its contributors by
studying the binary. It contains no game binaries, assets or data files, and
it is not playable on its own: building or running anything requires your
own legally obtained copy of the game.

The contributors' own work is released under CC0 1.0 (see `LICENSE`). That
dedication covers only what the contributors wrote; it grants no rights to
the original game.
