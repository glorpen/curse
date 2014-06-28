=====
Curse
=====

An lightweight Curse based addon manager for World of Warcraft. Supports linux & windows 32bit/64bit.

Help
====

.. sourcecode:: txt

   $ ./curse -h
   
   Curse - an addon uddater for World of Warcraft.
   
   Usage:
      -h, --help                   this text
      -v, --verbose                  make output more verbose (can be used multiple times)
      -s, --set <name> <version>     sets addon version in database
      -r, --remove <name>            removes addon from database
      -c, --clear                    clears (zeroes) versions of all addons
      -u[name], --update=[name]      checks for updates for all addons or one given by name
      -u,--update=[name] -f|--force  reinstalls addon
      -d, --dir <dir>                WoW instalation folder, default: current directory

Usage
=====

Copy *curse* binary to *World of Warcraft* installation directory.

To add new addon just run `./curse -usome-addon` on Linux or `curse.exe -usome-addon` on Windows.

Addons name are taken from Curse site, eg: `http://www.curse.com/addons/wow/deadly-boss-mods` means addon name will be `deadly-boss-mods`.

Updating addons
***************

Just run `curse.exe --update` on Windows or `./curse --update` on linux
