Grefsen is a Qt/Wayland compositor providing a minimal desktop environment.

# Requirements

* Qt 5.7
* Qt.labs.controls AKA QtQuick Controls 2
* libQtXdg
  * Arch Linux: install community/libqtxdg
  * otherwise: build from [github](https://github.com/lxde/libqtxdg) with Qt 5.7
* for the Connman network manager popover (non-fatal): libconnman-qt

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

Then modify to taste.

# Running

You might need the following env variables:

`export QT_AUTO_SCREEN_SCALE_FACTOR=0`
  (attempt to avoid hidpi scaling)

Append `-platform eglfs` to run it on the Linux console.
Otherwise it can run as a window inside an X11 session.

