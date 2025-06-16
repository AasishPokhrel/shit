SuperTux Release and Development Notes
======================================

SuperTux 0.6.3 (2021-12-22)
---------------------------
The SuperTux team is excited to announce the release of SuperTux 0.6.3 after approximately 1.5 years of development.  This release introduces many new features; perhaps the most new features in a long time!

Some of the most significant changes include:
- WASM compilation! SuperTux nightlies can now be played directly in the browser (thanks to Semphris)
- Added swimming (thanks to Daniel and Zwatotem)
- Added walljumping (thanks to Daniel)
- Autotiles (thanks to Semphris)
- Updated animations (thanks to Alzter, Daniel and RustyBox)
- Refreshed many contrib worldmaps (thanks to Servalot)
- Many updates to paths, with easing, bezier curves, path sharing between objects, and more (thanks to Semphris)
- Remade the crystal tileset (thanks to Alzter and RustyBox)
- Many new snow tiles (thanks to Daniel)
- Many new objects, like the sideways bumper and falling blocks (thanks to Daniel)
- Custom particles (thanks to Semphris)
- A new rublight object (thanks to HybridDog)
- Official binaries for FreeBSD, Linux 32-bit and Ubuntu Touch (thanks to Semphris)
- Added in-game progress statistics (thanks to Semphris)
- New color picker based on OKLab (thanks to HybridDog)
- Add-on creator, to easily create add-on packages with your world (thanks to Semphris)
- Rework of the Revenge in Redmond worldmap (thanks to RustyBox)
- Added timeshift ambience in the worldmap (thanks to Semphris)
- Skippable cutscenes (thanks to Semphris)
- Editor auto-saves at regular intervals (thanks to Semphris)
- Optional integration with Discord (thanks to Semphris)
- Updated translations, of course (thanks to translators)

And these are just the most significant changes; there are plenty of other small features and bugfixes for you all to explore :)

SuperTux 0.6.2 (2020-05-14)
---------------------------
The SuperTux Team is excited to announce the availability of SuperTux 0.6.2. This release fixes a number of bugs that were reported after 0.6.1. It also features reworked graphics and levels. 

Other changes include a power-up counter in the hud, new sprites for the door, a new tileset and several new badguys. But overall, this release is supossed to celebrate our anniversary (May 2000) with a little surprise Add-On, which can be found in the contrib levels!

Changes:
  - A new worldmap, "Revenge In Redmond" celebrating SuperTux's 20th anniversary, which includes new enemies and sprites !!!
  - New and improved backgrounds and sprites
  - Improvements to many levels in the Icy Island and forest world
  - Speed improvements for levels using a huge amount of lava tiles
  - An issue causing the bridge in the forest world to not be shown in certain cases was fixed

SuperTux 0.6.1 (2019-12-15)
---------------------------
The SuperTux Team is excited to announce the availability of SuperTux 0.6.1. This is first and foremost a bugfix release that fixes reported issues after the release of 0.6.0 a year ago, however, we also introduced a lot of other changes, such as new graphics, levels and other game content.

Changes:
* Rework of the first 3 Bonus Worlds
* Added 3 new bonus worlds to the core game
* Improvements to the story mode
* Addition of Ghost Forest to the Story Mode
* New Backgrounds and Music by BlasterMaster
* Tiles and Sprites Improvements by Alzter, weluvgoatz and RustyBox
* New enemy: The Ghoul
* Fixes and optimizations (Fixing buggy controls, game speed, etc.)
* Level Editor Improvements, Fixes and Optimisations
* Revamp of the Credits Menu
* Blocks (and other objects behaving like blocks, such as lanterns) no longer jitter when stacked on top of one another
* Trampolines as bonus block contents no longer hurt Tux
* An issue causing music not to get saved in the level editor was fixed

SuperTux 0.6.0 (2018-12-23)
---------------------------

The SuperTux team is excited to announce the availability of the first alpha for the upcoming stable release 0.6.0 after almost two years of development.

Changes:
* Complete redesign of the icy world and forest world
* Complete revamp of our rendering engine, the game should be much faster than it was previously
* We now support OpenGL 3.3 Core as well as OpenGL ES 2.0, thus allowing SuperTux to be run on the Raspberry Pi, and potentially WebGL.
* A few graphics have been updated, and effects have been added
    * The save bell was reworked (Thanks to Raghavendra "raghukamath" Kamath!)
    * Improved big Tux graphics (Thanks to Alzter)
    * Various effects and shaders (Thanks to Grumbel)
* Support for right-to-left languages through vector fonts. This will also fix a few non-ASCII characters, which often caused problems before in translations
* Forest worldmap redesigned with new levels and other redesigned levels (Thank you, RustyBox and Serano)
* A lot of other under-the-hood changes and bugfixes
* Official Linux binaries

**Note:** If you've previously used torches in your levels and their positioning is off, please re-position them. We had to re-align their bounding boxes in order to fix bugs with their flame.

SuperTux 0.5.1 (2016-11-05)
---------------------------

This is a bugfix release fixing some smaller problems that were reported after
the release of 0.5.0. It mostly features changes to the behavior of the level
editor, and adds some options that were missing but should have been included in
the stable release 0.5.0.

Changes:

* Editor: Tilemap: Add an option to change the draw target
* Editor: Add an option to snap objects to the grid (this will make it easier to
  add objects to a nice location in levels)
* Editor: Camera: Remove autoscroll option, as it is deprecated and should not
  be used anymore
* Editor: Fix an issue where some areas in the editor would be excessively large
  relative to the window/screen size

SuperTux 0.5.0 (2016-09-25)
---------------------------

The SuperTux team is excited to announce the availability of the stable release 0.5.0 after less than a year of development. The most prominent change for this release is a new in-game level editor which allows you to create levels and worldmaps on-the-fly from within SuperTux itself.

Changes:

* In-game level editor
* Improved levels in Antarctica and Forest Island
* Language packs are fixed
* Engine performance improvements
* Extended the scripting API: gradients are now scriptable
* Added a few more tiles and music
* New console commands and command line options (related to the editor)
* Various other bugfixes of issues reported since the v0.4.0 release
* And more (minor) improvements and changes

Changes from 0.5.0-rc.1 (2016-08-05):

* Fixed build system in order to correctly support SemVer pre-release tags
* Fixed miniswig compilation
* Editor: correctly join paths (using FileSystem::join())
* Editor: LevelDot: add a NULL check to prevent a crash when editing worldmaps
* Editor: correctly save BadGuy and Dispenser direction
* Editor: reorder layer objects when their object menu is closed
* Editor: allow setting z-pos of decals
* Editor: display a notile when the tile ID is invalid
* Editor: make it possible to switch tilesets

Changes from 0.5.0-rc.2 (2016-08-10):

* Editor: Torch: make it possible to change the sprite (#541)
* Don't turn Tux into stone above ground when using earth bonus (#537)
* WillOWisp: get name property in order to fix a scripting issue (#529)
* Editor: don't play WillOWisp sound
* Editor: move moving objects to the center of the mouse pointer when adding
* Editor: Switch: make sprite option visible again
* Make use of auto pointers and modern iterators where appropriate
* README: various updates related to download count
* Save and load visibility of SpriteChange tiles in squirrel table (#342)
* Editor: Fix an issue where the name of a scripted object wouldn't be saved (#557)
* Editor: add an alignment option for background (#547)
* Editor: Better approach for layer removal (#510)
* Add a launcher script for Windows builds (#443)

Changes from 0.5.0-rc.3 (2016-08-26):

* Fixes for the build system and updates with regard to the automatic deployment
  of releases
* More code style fixes
* Editor: verify some level properties have been set
* Add the possibility to add sawblade/hurting platforms using the editor
* Jump with Up: mark as deprecated in KeyboardMenu and JoystickMenu (addresses
  #499)
* Fix an issue with odd behavior of music playback (#583)
* Update some parts of the (user) documentation
* Editor: multiple fixes to levelset and worldmap creation, including UX
  improvements
* Editor: Angle option for spotlight
* Unify string spelling for consistency
* String freeze for translators is now in effect
* Editor: sort layer in ascending order
* Scripting: deduplicate code using the ExposedObject template class
* Update the translations to reflect the translation progress as of 2016-09-12

Changes from 0.5.0-rc.4 (2016-09-12):

* Use PhysFS file system abstraction for is\_directory checks
* Prevent statistics text from overlapping
* Fix a bug with Level.edit() in squirrel (#207)

Changes from 0.5.0-rc.5 (2016-09-23):

* Stop looping sounds when dead
* Final translation update

SuperTux 0.4.0 (2015-12-20)
----------------------------

Compared to 0.1.3, this release features:

* a nearly completely rewritten game engine based on OpenGL, OpenAL, SDL2, ...
* support for translations
* in-game manager for downloadable add-ons and translations
* Bonus Island III, a for now unfinished Forest Island and the development levels in Incubator Island
* a final boss in Icy Island
* new and improved soundtracks and sound effects
* new badguys, bonuses and power-ups (air-, earth- and ice-flower)
* a halloween tilemap
* new graphic effects (glowing objects, particles, ...)
* levels and worldmaps are scriptable using [squirrel](http://squirrel-lang.org/)
* much more game objects: trampolines, switches, portable stones, wind, moving platforms, ... - most of them have scripting APIs
* improved statistics
* many invisible changes, like unit tests, efficiency improvements and more bugfixes
* much more...

Compared to the latest beta, 0.3.5a, this release features:

* updated translations
* reworked add-on manager to support downloadable and updatable localization packs
* many bugfixes
* added iced-graphics to some more badguys
* melting animation for some badguys
* updated more levels (fix playability, ...)
* updated build system, featuring unit tests, libraries as git submodules and improved Travis-CI builds to support nightly builds
* more levels in Incubator Island
* support for horizontal and vertical gradients that span the length / height of a sector. Add `(direction "horizontal_sector / vertical_sector")` to your gradient.
* much more...

SuperTux 0.3.5a (2015-05-01)
----------------------------

A bugfix release is sneaking in. 3 weeks after 0.3.5 was released it's time to
fix some bugs. This affects mostly the windows installer where some libraries
were missing and add-ons couldn't be downloaded. Luckily we managed to get all
of these nasty bugs away.
One more patch affects linux packagers: The AppData file from our repository is
now contained in the source package.
Thanks to the activity of some translators there are also new translations in
this release.

If you want to help us and improve SuperTux, join our IRC channel #supertux on
chat.freenode.net or write to the mailing list!


SuperTux 0.3.5 (2015-04-11)
---------------------------

In celebration of SuperTux's 15 year anniversary, we are releasing SuperTux 0.3.5.

This release includes the switch to SDL2 for graphics rendering, a few performance
improvements, tons of bugfixes, new tiles and badguys amongst other things.

NOTE: Due to the planned shutdown of Google Code, the SuperTux source code has
moved to GitHub. You can find all the repositories here:
https://github.com/SuperTux/supertux

A more detailed changelog for this release is available at
http://supertux.lethargik.org/wiki/Changelog_0.3.5

Major changes in this release:

* move to SDL2 for graphics rendering
* glow effects
* new badguys: iceflame, ghostflame, livefire, goldbomb, smartblock
* new bonuses: coinrain, coinexplode
* statistics improved
* icy island levels tweaked
* new sounds
* massive improvements to localization
* efficiency tweaks
* menus reworked
* addon manager improved
* new tilemap: halloween
* new powerups: air- and earth-flower
* support for horizontal gradients in levels (add
(direction "vertical|horizontal") to your level)


SuperTux 0.3.4 (2013-07)
------------------------

It's been more than three years since the last development snapshot for
Milestone 2 of SuperTux, making this a bit overdue. Once again it is hard to
say what the most notable changes are, but one thing players should notice is
a greater wealth of levels to play through an expanded avalibility of add-ons.

Additionally, SuperTux development has moved to GitHub. Check it out at:
https://github.com/SuperTux/supertux/

If you are intersted in contribuiting to SuperTux, please do so. The more
people working on the project, the faster development can continue. The hope
is that this release will generate more interest which will lead to more
frequent releases. You can refer to the GitHub wiki page on "Contributing" to get
started.

And of course, enjoy the game and have fun!


SuperTux 0.3.3 (2010-02-26)
---------------------------

After more than three years of development the SuperTux development team has
agreed to package a development snapshot of Milestone 2 of our jump and run
game.

Unfortunatly, after such a long time it's hard to say what the most notable
changes are. We hope that the game engine is more stable (or at least more
advanced ;) and that the game is more fun to play overall.


We hope you enjoy SuperTux. Now off you go, rescue Penny! ;)


SuperTux 0.3.2-SVN (2007-12-20)
-------------------------------

Just in time for Christmas, we bring you SuperTux 0.3.2-SVN, a preview
of SuperTux Milestone 2.

This build doesn't contain all levels planned for Milestone 2 and
its story is by no means complete, but the game is already fun to
play.

SuperTux 0.3.2-SVN features:

* new, OpenGL- and OpenAL-based engine
* final boss for Icy Island
* brand new Forest World with new badguys and new game objects
* new and improved soundtrack, immersive sound effects
* much more...

The changes in more detail: The SuperTux 0.1 engine was nearly
completely rewritten. The game is translatable now, a new camera
algorithm allows scrolling in all four directions. Collision
detection supports slopes and moving objects now. We have scripting
support for dynamic level events and animations. New game objects
include trampolines, switches, portable stones, upside-down
levels, wind, moving platforms and particle effects. The sound
system is now OpenAL-based with an improved soundtrack now played
from Ogg/Vorbis-files, which were pre-rendered on a high-class
synthesizer.

This is also a call for artists and coders: If you like the game in
its current state, join us and help us finish Milestone 2!
