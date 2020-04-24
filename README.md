cppaudio
========

Utility for reading and writing PCM wave audio.

## Usage

WIP

## Develop

### Prerequisites

- Common tools
  - [cmake (v3.12 or later)](https://cmake.org/download/)
  - [git](https://git-scm.com/downloads)
  - clang-format
- Windows
  - VisualStudio 2017 or later
- Mac OSX
  - Xcode 10.0 or later

### Build

```console
git clone https://github.com/moutend/cppaudio
cd cppaudio/release
mkdir build
cd build
cmake ..

make all            # Linux
msbuild all.vcxproj # Windows
xcodebuild          # Mac OSX
```

### Test

```console
mkdir build
cd build
cmake -DBUILD_TESTS ..
make all
ctest --verbose
```

## LICENSE

MIT

## Author

[Yoshiyuki Koyanagi <moutend@gmail.com>](https://github.com/moutend)
