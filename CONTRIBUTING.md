# Contributing to Qivify

Thanks for your interest in Qivify. This project is a from-scratch Beat Saber
Quest mod; contributions are welcome.

## Getting set up

1. Install the Android NDK (r26 recommended) and point `ndkpath.txt` at it.
2. Install [QPM-RS](https://github.com/QuestPackageManager/QPM.RS) and make sure
   `qpm` is on your `PATH`.
3. Run `qpm restore` to fetch dependencies.
4. Build with `bash scripts/build.sh`.

See [docs/BUILDING.md](docs/BUILDING.md) for details and gotchas.

## Code style

- C++20. Four-space indentation, `camelCase` for functions and `PascalCase` for
  types.
- Match the existing file layout: public headers in `include/qivify/`,
  implementation in `src/`.
- Keep the 42 setting **display names byte-for-byte identical**, including the
  intentional spellings *Mutiplayer* and *Blits*. If you add a setting, add it
  after the 42 (like `EnableAudioLink`).
- Prefer `auto` over `auto*` when handling cordl `UnityW<T>` return values.

## Adding a real code path for a Tier-C setting

The highest-value contribution is wiring one of the "stored only" settings from
[docs/FEATURES.md](docs/FEATURES.md) into an actual runtime behaviour. When you
do:

1. Add the hook or logic in `src/compat.cpp` (or a new `src/*.cpp`).
2. Gate it on the relevant `Settings::Get(Id::...)`.
3. Update the tier tables in `docs/FEATURES.md`.
4. Update `CHANGELOG.md`.

## Honesty policy

This project maintains a strict honesty policy. Do **not** claim a feature works
on hardware unless it has been tested on hardware. Do not fabricate logs, test
results or benchmarks. When you test on a Quest, say so explicitly and include
the log excerpt.

## Pull requests

- Keep PRs focused; one feature or fix per PR.
- Describe what you changed, why, and **how you verified it** (compiled? tested
  on device? which Beat Saber version?).
- Make sure the tree still builds with `bash scripts/build.sh` before opening a
  PR.
