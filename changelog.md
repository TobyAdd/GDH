# v5.0.0-beta.6
- Macro Editor
- Disabled Accuracy Fix for 2P Player due to problems and bugs
- Layout Mode Default Colors of Level
- Replay Engine Recorder: Native Mode (any resolution for overlay mode, 100% quality shader trigger recording)
- Overlay Recording fixes

# v5.0.0-beta.5
- Fixed android64, thanks to whoever broke the bindings

# v5.0.0-beta.4
UI Changes:
- Seach box will be cleared when the menu is closed
- Mobile button now using layouts (Node ID dependency)

Hacks changes:
- Added Process priority
- Added Resume Timer (fixes lag when respawning while TPS Bypass is enabled, adjustable in the Framerate window)
- Fast Complete (Faster animation)
- Fixed Fast Chest open
- Added Auto Deafen
- Added Cheat Indicator
- Fixed "Main Levels Bypass" hack for "The Tower" levels
- Fixed Startpos Swither when you in Practice Mode

Labels changes:
- Added text coloring (CPS Counter, Death Counter will now be colored)
- Added Replay Engine State
- Added FPS Counter
- Added Cheat Indicator
- Added Testmode label
- Fixed Noclip Accuracy deaths
- Fixed Session Time
- "Progress" label have now a more customizable floating point

**Note:** labels may have an old formatting mapping, so they should be re-added

Replay Engine/Recorder changes:
- Fixed Accuracy Fix for 2P
- Audio Sync Recording now works perfectly with Music/SFX triggers
- Overlay Mode for Recorder (capturing overlays like reshade)
- Added option to change the volume percentage when recording audio for the showcase
- Practice Fix is back and reworked
- Audio now has better quality when merged with "Merge" tab

# v5.0.0-beta.3
- armeabi-v7a support (no recorder yet)
- startpos switcher keybinds fix (thanks ery)
- "startpos switcher sort objects x" feature
- removed "Hitbox Fill Color" feature due to problem with hitbox trail
- improved keybinds

# v5.0.0-beta.2
A new major version of GDH!

Global changes:
- Complete rewrite from scratch, redesigned almost all hacks to the hook system, allowing them to be used on both Android and Windows
- Android Support
- Replay Engine v3, P1/P2 macro load/merge support (Beta, merging only for PC)
- A lot of new hacks and features
- Variables (Beta, only for PC)
- Improved Labels (acid thanks)
- Shortcuts (Beta, only for PC)
- Recorder for Android (may be unstable, please report crashes/problems!)

GUI changes:
- More customizable UI size selection
- New keybind system
- Mini theme selection

Hacks changes:
- Fixed Speedhack Audio (works perfectly on SFX/Music triggers)
- Smart Startpos
- Layout Mode
- Pitch Shifter
- Pulse Size

# v4.9.0-alpha.1
- Ported to 2.2074

# 4.8.0
- Redesigned labels (thanks aciddev)
- Improved Recorder that ensure less crashes
- Added “Hide Complete Menu” for the recorder 

# 4.7.8
- fixed conflict between two engines
- replay engine v2 macro playback in editor

# 4.7.7
- fixed recorder aspect ratio mismatch
- lock aspect ratio for recording resoluion input (16:9)

# 4.7.6
- fixed replay system (again)

# 4.7.5
- Fixed input frame conflict that could have broken the macro
- Unlocked showcase video recording from 60 to 240 fps (Engine v2)
- Synchronize Audio on Video Recording (Experimental feature, currently breaking on song triggers)
- Improved Practice Fix
- Fixed recorder presets

# 4.7.4
- New Replay System (Engine v2)
- Improved the accuracy of both engines
- Practice Fix
- Recorder presets save/load
- Fixed pixelization shader on video recording
- Ability to disable popup messages (in Replay Engine settings)
- Hide Attempts

# 4.7.3
- Removed the annoying window if there are Russian characters in the path (now prevents recording a video if the path has Russian characters)
- Save the showcase path in byte format to avoid crashes with Russian characters
- Fixed Recorder when recording level with startpos
- More presets

# 4.7.2
- Fixed Russian characters conflict

# 4.7.1
- Fixed FFmpeg stuff

# v4.7.0
- Recorder
- Сhanged to a more safe method of obtaining the list of macros
- Auto-enter macros name based on level name
- Show Hitboxes on Death

# v4.6.8
- More friendly mod description
- Spambot
- Straight Fly Bot
- Popup windows now appear at the bottom right to avoid conflicts with adjacent windows
- TPS bypass has been moved to the "Framerate" tab (+ fixed problems with bypassing physics when it is off)

# v4.6.7
- More friendly mod description
- Spambot
- Straight Fly Bot
- Popup windows now appear at the bottom right to avoid conflicts with adjacent windows
- TPS bypass has been moved to the "Framerate" tab (+ fixed problems with bypassing physics when it is off)

# v4.6.6
- missed

# v4.6.5
- Fixed keybinds
- Popup message system (for showing if frame advance enabled cuz people say that the level is freezing, and it turns out that they accidentally turned it on through the key)
- Some new hacks

# v4.6.4
- Added more keybinds for hacks (Speedhack, Startpos Switcher, Frame Advance, Replay Engine Playback)
- Random Seed
- Wave Trail Size
- Tint on Death
- Anticrash if save data is broken
- Fixed hitbox trail in editor

# v4.6.3
- Show Hitboxes

# v4.6.2
- Fixed some hints
- Added keybinds

# v4.6.1
- Added Respawn Time
- Added "Open Folder" button when selecting replay

# v4.6
- Geode Support
- Added Auto Pickup Coins
- Fixed Allow Low Volume