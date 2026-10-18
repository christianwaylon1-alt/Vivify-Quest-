# Changelog

All notable changes to Qivify are documented here. This project adheres to
[Semantic Versioning](https://semver.org/).

## [1.0.0] - 2025

Initial release.

### Added
- Single in-game Mod Settings category named **Qivify** with all **42** required
  settings, plus an **Enable AudioLink** extension toggle.
- Settings persistence to `files/Qivify.json` with per-setting defaults and a
  reset-to-defaults action.
- Compatibility engine with six runtime hooks:
  - `UnityEngine.Shader.Find`
  - `UnityEngine.Material.set_shader`
  - `UnityEngine.Material.SetTexture`
  - `UnityEngine.AssetBundle.LoadFromFile`
  - `UnityEngine.Camera.get_main`
  - `UnityEngine.Graphics.Blit`
- Camera-box sweep that disables invalid renderers parented to the main camera
  (priority fix, `Disable Extra Camera Boxes Following Screen`, default on).
- Unsupported-shader detection with stand-in shader substitution.
- **Reconvert All AssetBundle Maps** action.
- **Write Asset Bundle Compatibility Report** action.
- Built-in AudioLink bundle loader.
- Full documentation set: README, install, building, features, limitations.

### Notes
- Not yet tested on physical Quest hardware. See `docs/LIMITATIONS.md`.
