# turtile 🐢
A simple and customizable tiling Wayland compositor based on [wlroots](https://gitlab.freedesktop.org/wlroots/wlroots)

# Overview
turtile was started as a passion and learning project, with the objective of getting a usable compositor that provides a customizable experience. It was written in C using [tinywl](https://gitlab.freedesktop.org/wlroots/wlroots/tinywl) as a base.

# Features
- Flexible CLI system for interactig with the compositor
- Easy to parse JSON output for the shell script enjoyers
- Readable config file making use of [libconfig](https://github.com/hyperrealm/libconfig)
- Support for vitually infinite desktops (haven't tried infinite, maybe stick with 50 max...)
- User defined keybinds that can run any shell command

# Getting started
Take a look at our [wiki](https://github.com/migueldeoleiros/turtile/wiki) page for detailed explanation on build, configuration and usage

# Contributing
turtile is a free and open source project and we wlcome contributions from the community.
If you find a bug or have a suggestion feel free to file an issue or create a new [discussion](https://github.com/migueldeoleiros/turtile/discussions).
If you are interested in contributing in any other way, please see our [CONTRIBUTING.md](https://github.com/migueldeoleiros/turtile/blob/master/CONTRIBUTING.md) file for more information.

# License
turtile is licensed under [GPL v3](https://github.com/migueldeoleiros/turtile/blob/master/LICENSE) !

# Acknowledgments
turtile was heavily inspired by other projects like:
- [tinywl](https://gitlab.freedesktop.org/wlroots/wlroots/tinywl)
- [sway](https://github.com/swaywm/sway)
- [hyprland](https://github.com/hyprwm/Hyprland)
- [dwl](https://codeberg.org/dwl/dwl)
- [vivarium](https://github.com/inclement/vivarium)
- [bspwm](https://github.com/baskerville/bspwm)

Special thanks to [Perry Werneck](https://github.com/PerryWerneck) for his help with the build system!
