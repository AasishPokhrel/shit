# SuperTux

[![Windows](https://github.com/SuperTux/supertux/actions/workflows/windows.yml/badge.svg?branch=master)](https://github.com/SuperTux/supertux/actions/workflows/windows.yml?branch=master)
[![MacOS](https://github.com/SuperTux/supertux/actions/workflows/macos.yml/badge.svg?branch=master)](https://github.com/SuperTux/supertux/actions/workflows/macos.yml?branch=master)
[![GNU/Linux](https://github.com/SuperTux/supertux/actions/workflows/gnulinux.yml/badge.svg?branch=master)](https://github.com/SuperTux/supertux/actions/workflows/gnulinux.yml?branch=master)
[![Android](https://github.com/SuperTux/supertux/actions/workflows/android.yml/badge.svg?branch=master)](https://github.com/SuperTux/supertux/actions/workflows/android.yml?branch=master)
<!-- BSD disabled, see https://github.com/SuperTux/supertux/pull/2366#issuecomment-1670925526 -->
<!-- [![BSD](https://github.com/SuperTux/supertux/actions/workflows/bsd.yml/badge.svg?branch=master)](https://github.com/SuperTux/supertux/actions/workflows/bsd.yml?branch=master) -->
[![WebAssembly](https://github.com/SuperTux/supertux/actions/workflows/wasm.yml/badge.svg?branch=master)](https://github.com/SuperTux/supertux/actions/workflows/wasm.yml?branch=master)
[![Ubuntu Touch](https://github.com/SuperTux/supertux/actions/workflows/ubuntu-touch.yml/badge.svg?branch=master)](https://github.com/SuperTux/supertux/actions/workflows/ubuntu-touch.yml?branch=master)
[![Github All Releases](https://img.shields.io/github/downloads/supertux/supertux/total.svg?maxAge=2592000)](https://github.com/SuperTux/supertux)

SuperTux is a jump'n'run game with strong inspiration from the
Super Mario Bros. games for the various Nintendo platforms.

Run and jump through multiple worlds, fighting off enemies by jumping
on them, bumping them from below or tossing objects at them, grabbing
power-ups and other stuff on the way.

![Screenshot](https://www.supertux.org/images/0_6_0/0_6_0_3.png)


## Story: Penny gets captured!

Tux and Penny were out having a nice picnic on the ice fields of
Antarctica. Suddenly, a creature jumped from behind an ice bush, there
was a flash, and Tux fell asleep!

When Tux wakes up, he finds that Penny is missing. Where she lay
before now lies a letter:
>Tux, my arch enemy! I have captured your beautiful Penny and have
>taken her to my fortress. The path to my fortress is littered with my
>minions. Give up on the thought of trying to reclaim her, you haven't
>got a chance!
>
>-Nolok

Tux looks and sees Nolok's fortress in the distance. Determined to
save his beloved Penny, he begins his journey.

## Installation

For major platforms, stable releases are built and available for download from
[supertux.org](https://www.supertux.org/download.html) or alternatively directly
from [GitHub](https://github.com/SuperTux/supertux/releases). You should be able
to install these using default tools provided by your platform. On macOS, when
Gatekeeper is enabled (default) it will refuse to open SuperTux. This is due to
the lack of a signature on the application. If you wish to open SuperTux anyway
without disabling the Gatekeeper feature entirely, you can open the application
from the context menu (control click on the icon). macOS will then remember your
choice the next time.

## Documentation

Important documentation for SuperTux is contained in multiple files.
Please see them:

* `INSTALL.md` - Requirements, compiling and installing.
* `README.md` - This file
* `NEWS.md` - Changes since the previous versions of SuperTux.
* `LICENSE.txt` - The GNU General Public License, under whose terms SuperTux is
licensed. (Most of the data subdirectory is also licensed under
CC-by-SA)
* `data/credits.stxt` - Credits for people that contributed to the creation of
SuperTux. (You can view these in the game menu as well.)


## Playing the game

Both keyboards and joysticks/gamepads are supported. You can change
the controls via the Options menu. Basically, the only keys you will
need to use in-game are to do the following actions: jump, duck,
right, left, action and 'P' to pause/unpause the game. There isn't much
to tell about the first few, but the "action" key allows you to pick
up objects and use any powerup you got. For instance, with the fire
flower, you can shoot fireballs, or with the ice flower fire ice pellets.

Other useful keys include the Esc key, which is used to go to the menu
or to go up a level in the menu. The menu can be navigated using the
arrow keys or the mouse.

In the worldmap, the arrow keys are used to navigate and Enter to
enter the current level.

## Community

In case you need help, feel free to reach out using the following means:

* **IRC:** [#supertux](ircs://irc.libera.chat/#supertux) on
  [Libera Chat](https://libera.chat) hosts most of the discussions between
  developers. Also, real-time support can be provided here. If you don't know
  how to use an IRC client, you access the channel using a web-based
  [client](https://kiwiirc.com/nextclient/irc.libera.chat:+6697/?nick=Guest?#supertux).
  Please stay around after asking questions, otherwise you will be disconnected
  and might miss potential answers.
* **Matrix:** [#supertux:matrix.org](https://matrix.to/#/#supertux:matrix.org)
  is bridged to our IRC room.
* **[Forum](https://groups.f-hub.org/supertux):** The SuperTux
  community is also active on the forum, the discussions range from feature
  proposals to support questions. In particular, most community-contributed
  add-ons are published there, so this is worth checking.
* **Mailing Lists:** The
  [supertux-devel](http://lists.lethargik.org/listinfo.cgi/supertux-devel-lethargik.org)
  mailing list is dead. Here is the [archive](https://github.com/supertux-community/supertux-devel-maillist).
* **Social Media:** Mostly on [Twitter](https://twitter.com/supertux_team) at
  the moment.
* **Discord:** Also, you can join our [Discord server](https://discord.com/invite/AcvtHWz) to get in touch with us.

## Development status

As of now, with the release of SuperTux 0.6.3 (December 2021), the Forest World is almost
finished, since the ghost forest section has been included. However, some levels, especially
the Ghostree Level, are considered to be placeholders, because for the next version (0.7.0) a
great overhaul is planned with new features like reworked boss fights, graphics, and worlds.
If you have some Constructive Feedback, Contributions or ideas to share, don't hesitate
to contact us with one of the possibilities given above.
