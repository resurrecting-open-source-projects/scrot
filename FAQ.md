
# Frequently Asked Questions about scrot


### Can I change the start of the selection area? ###

Yes. Using the arrow keys on your keyboard.

### When I use the `--select` and `--pointer` options, how can I capture the mouse cursor if the cursor is being used for selection? ###

Use the `--delay` option which will give you time to move the cursor
into the selection area you have created.

### How to avoid canceling the selection when pressing any key on the keyboard? ###

Use the `--ignorekeyboard` option. This option prevents exit by
ignoring any keyboard event, except the ESC key.


### The --select option leaves rectangle trails of the selection rectangle. How do I avoid this? ###

There are 2 ways to solve this:

Try first with: `scrot --select --freeze`

Or using another selection mode: `scrot --select --line mode=edge`

But this last one does not behave correctly with some
CWM (Composite Window Manager)


### My CWM allows me to override shading on some windows by adding the class name, does scrot have a class name? ###

Yes. The class name is "scrot" and is useful only when using the
`--line mode=edge`.

For example for the CWM picom:
```
shadow-exclude = [
    "class_g = 'scrot'",
];
```

### Why isn't the --freeze option enabled by default? ###

The `--freeze` option generates a call to XGrabServer and, according to
the X11 documentation, says the following:

"[..] disables processing of requests and close downs on all other
connections [...] You should not grab the X server any more than is
absolutely necessary."

We must not have a default behavior that is not recommended by X11.

Also the `--freeze` option may cause some video players to cause a lag
between video and audio or other unforeseen problems.
Some users are using this option to freeze a video frame and thus
capture it, although it works, keep in mind that this was not the
purpose of the option, but rather to provide another alternative to
the `--select` option to solve other problems.


### What image file formats can I save my screenshot? ###

All formats supported by the Imlib2 library (present and future).


### What is the image format that is saved by default? ###

The default format is PNG, even if it does not have a .png file
extension.


### Does it support redirection of the screenshot to the standard output? ###

Yes. For example:
```console
$ scrot -
$ scrot -> myfile.png
$ scrot - | pngquant -> myfile.png
```


### Is there a default image format in the redirection to the standard output? ###

Yes. The default format is PNG. Use `--format` flag to change it.


### What is the default line mode for the `--select` option? ###

The default line mode is `--line mode=auto`


### Do all sub-options of the `--line` option work for both line modes (edge, classic)? ###

No. For example the opacity sub-option is only for the edge line mode.
To know more consult the man page.


### When I use the `--select` option and click on the area of a window that is below another, this window does not rise, how can I solve this? ###

The way to force a window to have focus and rise is to add the
`--border` option. However, this does not work on all WMs. So you may
have to manually raise the window beforehand.


### How do I change the opacity of the option `--select=hole` or `--select=hide?` ###

Using `--line opacity=35`


### How do I change the color of the option `--select=hole` or `--select=hide?`###

Using `--line color='Forest Green'`


### I am using an image (instead of just color) to hide a selection area with: `--select=hide,stamp.png`, can I change the opacity of this image? ###

Yes. Using the opacity sub-option of the --line option:
--line opacity=35


### I am using an image to hide a selection area with: `--select=hide,stamp.png --line opacity=35`, but the image is always opaque, how do I fix this? ###

The image you are using must have an alpha channel (RGBA).
You will find out if you do the following:

```console
$ file stamp.png
```
Shows the following "8-bit/color RGBA", it has alpha channel.

```console
$ file stamp.png
```
Shows the following "8-bit colormap", does not have alpha channel.


### Why can't I use floating point numbers anymore? ###

Until version v1.7 it would silently convert a floating point number
to a zero(0). Which is an error since it allowed the user to believe
that the value given to the option was correct, when it is not.
Since version v1.8, the entered value is indicated with an
error 'is not an integer'

For example:
```console
$ scrot --delay 0.5
$ scrot: option --delay: '0.5' is not an integer
```
