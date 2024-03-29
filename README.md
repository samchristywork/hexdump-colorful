![Banner](https://s-christy.com/sbs/status-banner.svg?icon=action/token&hue=0&title=Hexdump%20Colorful&description=A%20utility%20from%20the%2090s%2C%20now%20in%20color%21)

## Overview

`hexdump_colorful` is a "clone" of hexdump that colorizes the output to make it
easier for users to pick out patterns and identify the character class of
bytes.

## Features

- Clone of hexdump but with color
- Color coding of character classes
- Prints out a color key with `-Y`
- Accepts `stdin` with `-`
- Random color generation
- Handles the `alpha`, `print`, `alnum`, `blank`, `digit`, `graph`, `ascii`, `lower`, `cntrl`, `punct`, `space`, `upper`, and `xdigit` character classes
- Compatible with any terminal which uses standard color codes
- Hex and character panes

## Build

Simply type `make` to build the program. The output will be placed in the
`build/` directory.

## Usage

Invoke the program like you would any other, with a filename as the first
positional argument. You can specify several options as command line
parameters, including highlighting specific character classes and controlling
the color generation. Here is the usage message for reference:

```
Usage: hexdump-colorful [-aAibndglPpsux] [-C] [-S seed] [-o] [-H character] [-X character] filename (- for stdin)

 -C     Allow non-console output.
 -S     Specify the seed to use for random color generation.
 -o     Turn off the random color generation.

 -H character   Highlight a character. Can be specified multiple times.
 -X character   Highlight a character by hex value. Can be specified multiple times.

Highlight character classes by specifying one of the following switches:

 -A     Checks for an alphabetic character.
 -P     Checks for any printable character including space.
 -a     Checks for an alphanumeric character.
 -b     Checks for a blank character; that is, a space or a tab.
 -d     Checks for a digit (0 through 9).
 -g     Checks for any printable character except space.
 -i     Checks whether it is a 7-bit unsigned char value that fits into the ASCII character set.
 -l     Checks for a lowercase character.
 -n     Checks for a control character.
 -p     Checks for any printable character which is not a space or an alphanumeric character.
 -s     Checks for white-space characters.
 -u     Checks for an uppercase letter.
 -x     Checks for hexadecimal digits, that is, one of "0123456789abcdefABCDEF".
 -z     Checks the truth value of the character.
```

Here's an example of some typical output:

<p align="center">
  <img alt="Selecting Character Classes" src="./example/selecting_classes.png">
<p>

## Color Key

The colors that this program outputs have different meanings. You can print out
a key that lists all of the different command line options, their associated
`ctype` function, and their description, in the color that they will show up as.
This output looks like this:

<p align="center">
  <img alt="Color Key" src="./example/color_key.png">
<p>

## Dependencies

```
gcc
make
```

## License

This work is licensed under the GNU General Public License version 3 (GPLv3).

[<img src="https://s-christy.com/status-banner-service/GPLv3_Logo.svg" width="150" />](https://www.gnu.org/licenses/gpl-3.0.en.html)
