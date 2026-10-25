# jp: the JP 983 client's game side of Refresh (not matching)

This folder is **not** part of the matching decompilation. It holds the
game code of the JP 983 client (`ProjectG_Editavel_983.exe`) that drives
its Refresh UI library (`src/client/Refresh`, which is matching): the
`RFInterface` / `RFRenderer` / `RFTexture` implementations
(`RefreshInterface`, `RefreshRenderer`, `RefreshTexture`,
`RefreshInputHandler`), the `RF*Manager` singletons (`RFSceneManager`,
`RFTextManager`, `RFTextureManager`), the lobby desktop (`RFLobbyDesktop`)
and the JP screens and their glue (`Jp*`).

## What it is

- **Decompiled from the JP exe, functionally.** A `// 0x<va>` comment
  before a definition (or trailing on the line that starts one) gives the
  JP function the body was written from; `JP +0x<off>` comments give the
  JP layout of a field. The JP exe has no PDB: the names are the JP RTTI's
  where it has them, ours elsewhere.
- **Not matching.** No file here is a unit: nothing compiles it with the
  original compiler or scores it with objdiff. It is compiled by clang only,
  as the `pg_refresh_game` library of the macOS JP port, with `JP_PAKS`,
  `PG_PAK_REGION_JP` and `PG_7XX` defined.
- **Written against the KR classes.** The code uses the KR 2011 game
  classes of `src/client/ProjectG`, `Fresh` and `Wangreal` as they are. The
  KR files reach this code only inside `#ifdef JP_PAKS`, so the matching
  build never sees it.
- **Adapters are declared here, defined by each port.** What the JP code
  needs from game state the KR classes lay out differently (a JP field
  mapped onto the KR member it is proven to be, JP-only state) goes through
  the functions `JpLobbyGlue.h`, `JpItemGlue.h`, `JpFreshGlue.h`,
  `JpShopGlue.h`, `JpPetFrameGlue.h`, `JpPointShopData.h` and `JpFrForms.h`
  declare, the mastery glue `JpMasterySlot.h` declares, and the lobby
  pop-up menu handlers `RFLobbyDesktop.cpp` declares. Their bodies are not
  in this folder: a port that builds this code provides them (projectg
  keeps its own in `port/refresh/game/`). A link error on one of those
  names means the port has not written that adapter yet.
- **Port bodies.** Some functions keep a safe stand-in body (`NULL`,
  `false`, `0`, empty) until the JP body is written. Those carry their VA on
  the declaration only, never a `// 0x<va>` marker before the body: the
  marker means the body is the JP function's.

## Building it

The KR game sources must be compiled with `JP_PAKS` too (the hooks in
`ProjectG.cpp`, `TaskManager.cpp`, `TaskMain.cpp`, `LobbyMain.cpp`,
`Avatar_Main.cpp`, `LogoutDlg.cpp`, `SharedDoc.cpp` include headers from
here). The include order the port uses, which keeps a Refresh header from
shadowing a game one on a case-insensitive file system:

- `pg_refresh_game` (this folder's `*.cpp`): the Win32 shim, the Refresh
  headers (`client/Refresh/include`, `client/Refresh/src`), `jp/`, then
  the game's (`client/ProjectG`, `client/Fresh/include`,
  `client/Wangreal/include`, `shared`, `shared/luasystem`, `src`), WTL 7.5
  and Lua 5.1.
- The hook sources above: their usual include list, then `jp/` and the
  Refresh headers searched last (`-idirafter`).

The Win32 shim and the platform layer are not here: each port brings its
own. The reference build is projectg's `port/CMakeLists.txt` (the
`JP_PAKS` block).
