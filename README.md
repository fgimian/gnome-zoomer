# GNOME Zoomer

The GNOME zoomer utility updates tho relevant dconf setting to zoom the entire desktop by a chosen
increment.  This may be easily assigned to a keyboard shortcut in GNOME so that the user may zoom
in with shortcuts using their chosen increment.

## Compiling the Binary

```bash
# Install build dependencies
sudo apt install gcc libglib2.0-dev libdconf-dev

# Build the GNOME zoomer binary
gcc -Wall gnome-zoomer.c $(pkg-config --libs --cflags dconf) -o gnome-zoomer

# Install the GNOME zoomer binary
sudo install -o root -g root -m 0755 gnome-zoomer /usr/bin/gnome-zoomer
```

## Assigning GNOME Zoomer to a Keyboard Shortcut

1. Open **Settings** / **Keyboard Shortucts**
2. Scroll to the bottom of the list and click **+**
3. Create an item named **Zoom in increment** and set **Command** to `gnome-zoomer 0.1` (or similar)
   and **Set Shortcut** to **Alt** + **+**
4. Create an item named **Zoom out increment** and set **Command** to `gnome-zoomer -0.1` (or similar)
   and **Set Shortcut** to **Alt** + **-**

## License

GNOME Zoomer is released under the **MIT** license. Please see the
`LICENSE <https://github.com/fgimian/gnome-zoomer/blob/master/LICENSE>`_
file for more details.