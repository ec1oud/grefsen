Grefsen is a Qt/Wayland compositor providing a minimal desktop environment.

<img src="screenshot.jpg" width="600">

# Requirements

* Qt 5.7: you need to [build it from the git repo](https://wiki.qt.io/Building_Qt_5_from_Git) for now
  * qtbase, qtdeclarative, qtwayland
  * Qt.labs.controls AKA qtquickcontrols2
  * QtQuick.Controls only for the calendar popover
* libQtXdg
  * Arch Linux: install [community/libqtxdg](https://www.archlinux.org/packages/community/x86_64/libqtxdg/)
  * otherwise: build from [github](https://github.com/lxde/libqtxdg) with Qt 5.7 and cmake
* for the Connman network manager popover (optional): [libconnman-qt](https://git.merproject.org/mer-core/libconnman-qt)

# Building

```
qmake
make
```

# Installation

```
mkdir ~/.config/grefsen
cp example-config/*.qml ~/.config/grefsen
```

Then modify to taste.  If you want the wallpaper shown in the screenshot, download
the full-resolution version from
[wikipedia](https://commons.wikimedia.org/wiki/File:Oslo_mot_Grefsentoppen_fra_Ekeberg.jpg)
to your ~/.config/grefsen directory.

# Running

It can run as a window inside an X11 session.

If you want to run it on the Linux console without an X11 session
(which is much more efficient), you can use a startup script like this:

```
#!/bin/sh
export QT_QPA_PLATFORM=eglfs
export QT_AUTO_SCREEN_SCALE_FACTOR=0 # don't embiggen stuff on "high-res" displays
#export QT_QPA_EGLFS_PHYSICAL_WIDTH=480 # in case it's not detected
#export QT_QPA_EGLFS_PHYSICAL_HEIGHT=270 # or you wish to override
# try to restart if it crashes; write a log file
~/src/grefsen/grefsen -r -l /tmp/grefsen.log
```
