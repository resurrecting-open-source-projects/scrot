# CONTRIBUTING TO SCROT DEVELOPMENT

#### Changes are to be sent as [pull requests on GitHub](https://github.com/resurrecting-open-source-projects/scrot/pulls).

## Setting up the development machine

scrot aims to build with no modifications on all the free unix systems. The
master branch is also kept in working order at all times.

You will want to install the dependencies listed in the `Building` section of
[README.md](README.md). Look for the text `install_script` in the
[.cirrus.yml](.cirrus.yml) CI definitions for example package manager
invocations on many operating systems.

## Style

scrot is primarily written in C99 with
[POSIX 2008](https://pubs.opengroup.org/onlinepubs/9699919799.2008edition/)
extensions. You're also free to make use of any documented function from our
libraries. Make sure to use
[libbsd functions](https://manpages.debian.org/unstable/libbsd-dev/libbsd.7.en.html)
where they're safer, cleaner or more convenient.

scrot follows
[WebKit's code style guidelines](https://webkit.org/code-style-guidelines/)
with additional restrictions:
- No explicit casts.
- Use the identifier and not the type as the argument to the `sizeof` operator.

When in doubt, make use of the `clang-format` source code formatter-it comes
with the WebKit style by default.

## Low-hanging fruit

Ongoing tasks will be written to [TODO.md](TODO.md).

Easy tasks which can be solved by newcomers are tagged with
[good first issue](https://github.com/resurrecting-open-source-projects/scrot/labels/good%20first%20issue).
Issues tagged with [help wanted](https://github.com/resurrecting-open-source-projects/scrot/labels/help%20wanted)
are intermediate/difficult issues where we require help.

Regular contributors can ask to join the scrot project on GitHub.
