# Installing Qivify

Qivify is a Scotland2 mod for **Beat Saber 1.40.8** on **Meta Quest** (Quest 1,
2, 3, 3S and Pro). This guide assumes you already have a modded Quest.

## Prerequisites

You need the following already installed on your headset:

1. **A modded Beat Saber 1.40.8.** The easiest route is
   [QuestPatcher](https://github.com/Lauriethefish/QuestPatcher), which patches
   the APK and installs the **Scotland2** modloader.
2. **The Scotland2 core mods.** Qivify declares these as dependencies and
   QuestPatcher will offer to download any that are missing:

   | Dependency | Version range |
   |------------|---------------|
   | `beatsaber-hook` | `^6.4.2` |
   | `custom-types` | `^0.18.4` |
   | `paper2_scotland2` | `^4.8.0` |
   | `bsml` | `^0.4.55` |

## Install with QuestPatcher

1. Connect your Quest to your PC with a USB cable and make sure it is detected.
2. Launch **QuestPatcher** and open your patched Beat Saber.
3. Click **Manage Mods** → **Install Mod** (or simply drag the file in).
4. Select **`Qivify.qmod`**.
5. QuestPatcher will read the manifest, download any missing dependencies, and
   install `libqivify.so` as a `lateModFile`.
6. Let Beat Saber restart.

## Verify the install

1. Launch Beat Saber.
2. Go to **Settings → Mod Settings**.
3. You should see a new category called **Qivify**.
4. Open it — you should see the 42 settings, the **Enable AudioLink** toggle and
   three action buttons.

If the category does not appear, check the log at
`/sdcard/Android/data/com.beatgames.beatsaber/files/logs/` for a line starting
with `Qivify`.

## Enabling AudioLink (optional)

The **Enable AudioLink** toggle loads a bundle from:

```
/sdcard/Android/data/com.beatgames.beatsaber/files/Qivify/audiolink.bundle
```

Copy a compatible AudioLink bundle there, then toggle **Enable AudioLink** on.
If the file is missing, Qivify logs a warning and leaves the toggle in a no-op
state.

## Uninstalling

Use QuestPatcher's **Manage Mods** page to uninstall **Qivify**, or delete
`libqivify.so` from the `Mods` folder on your headset. Your settings are stored
in `files/Qivify.json` and are left in place; delete that file to reset them.
