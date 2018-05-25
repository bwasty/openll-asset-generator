# openll-asset-generator
Font Asset Generator based on OpenLL Specifications

[![GitHub contributors](https://img.shields.io/github/contributors/hpicgs/openll-asset-generator.svg)](https://GitHub.com/hpicgs/openll-asset-generator/graphs/contributors/)
[![MIT license](https://img.shields.io/badge/License-MIT-blue.svg)](https://github.com/hpicgs/openll-asset-generator/blob/master/LICENSE)

**What is openll-asset-generator?** (*llassetgen*)

The openll-asset-generator, or in short *llassetgen*, is a cross-platform C++ generator for font assets. Font assets are needed for rendering fonts in various realtime-applications.
With *llassetgen*, the user can adapt the parameters via CLI, GUI or can directly use the core lib. The GUI offers a pre-render using the generated font asset.

**What is a font asset?**

A font asset consists of the data needed to render a font.
* The glyph atlas is a visual representation of the characters supported by the font. It can be a bitmap, a signed distance field, vector graphics, etc. The *llassetgen* creates signed distance fields as documented by Chris Green of Valve in the SIGGRAPH 2007 paper [Improved Alpha-Tested Magniﬁcation for Vector Textures and Special Effects.](http://www.valvesoftware.com/publications/2007/SIGGRAPH2007_AlphaTestedMagnification.pdf).
* The font description file contains the typesetting information that is needed to position each glyph correctly: the position in the glyph atlas, the position on the baseline (*advance*, *height*, ...) and the kerning data.

**Main Workflow**
1. Load the font file as it is found on your machine. Then generate a high-resolution bitmap image containing the glyphs.
2. Pack all glyphs into a glyph atlas for lower space consumption.
3. Render the distance field of every glyph. Input: high resolution bitmap image. Output: lower resolution distance field.
4. Export the texture atlas (i.e. the glyph atlas as distance field) and the font description file (i.e. information needed to use that atlas and to typeset the glyphs).

## Project Health

| Service | System | Compiler | Status |
| :------ | ------ | -------- | -----: |
| [Travis-CI](https://travis-ci.org/hpicgs/openll-asset-generator) | Ubuntu 14.04, macOS | GCC 4.8, Clang 3.9 <br> AppleClang 8.1 | [![Travis Build Status](https://img.shields.io/travis/hpicgs/openll-asset-generator.svg)](https://travis-ci.org/hpicgs/openll-asset-generator)|
| [AppVeyor](https://ci.appveyor.com/project/anne-gropler/openll-asset-generator-5cjbt) | Windows | MSVC 2015<br>MSVC 2017 | [![AppVeyor Build Status](https://img.shields.io/appveyor/ci/anne-gropler/openll-asset-generator-5cjbt.svg)](https://ci.appveyor.com/project/anne-gropler/openll-asset-generator-5cjbt)|


## Installation

### Windows

The core library and its applications can be installed either by downloading an installer, e.g., the latest x64 installer for Microsoft Visual Studio 2017, or downloading and extracting one of the precompiled archives, e.g. runtime, examples, and dev. Alternatively, download the source code and commence building from source. See [latest release](https://github.com/hpicgs/openll-asset-generator/releases/latest).

### Ubuntu

We plan to provide the Font Asset Generator using PPAs. Until then, download the source code and commence building from source.

### OS X

We plan to provide the Font Asset Generator via homebrew package manager. Until then, download the source code and commence building from source.

## Build Instructions

### Prerequisites and Dependencies

Necessary for the core library (*llassetgen*) and the CLI application (*llassetgen-cmd*)
* [CMake](https://cmake.org/) 3.4 or higher for building from source
* [Freetype](https://www.freetype.org/) to load and render fonts
* [libpng](http://www.libpng.org/pub/png/libpng.html) to import and export PNGs images.
* [fontconfig](https://www.freedesktop.org/wiki/Software/fontconfig/) on Unix systems. 

Additionally necessary to build the rendering application (*llassetgen-rendering*):
* [GLM](https://github.com/g-truc/glm) for OpenGL math and data structures
* [glbinding](https://github.com/cginternals/glbinding) as OpenGL API binding
* [globjects](https://github.com/cginternals/globjects) to wrap OpenGL API objects
* [Qt5](http://www.qt.io/developers/) 5.0 or higher for GUI elements

### Compile Instructions

For compilation, a C++11 compliant compiler, e.g., GCC 4.8, Clang 3.9, AppleClang 8.1, MSVC 2015, is required.

First, download the source code [as archive](https://github.com/hpicgs/openll-asset-generator/releases). or via git:
```shell
> git clone https://github.com/hpicgs/openll-asset-generator.git
> cd openll-asset-generator
```

Then, depending on the version of globjects you want to build, choose the appropriate tag or branch, e.g., for the 1.0 release:

```shell
> git fetch --tags
> git checkout v1.0-master
```

The actual compilation can be done using CMake and your favorite compiler and IDE.

## Usage

**ToDo** Overview
* how to link and use lib
* using CLI
* using rendering / GUI (what's WIP)
* explaining parameters and their consequences (Distance Transform, Packing, etc...)
* algorithms: link paper or explain how they work

### Miscellaneous

<dl>
  <dt>Font Name</dt><dd>The font is loaded from the local machine using the given font name, e.g. Arial, Verdana</dd>
  <dt>(Original) Font Size</dt><dd>This size is used to pre-render the glyphs before the Distance Transform is applied. A higher value results in smoother fonts (high resolution font), but the Distance Transform performs slower. We encourage larger font size, as the Distance Field is only generated once.</dd>
  <dt>Padding</dt><dd>Space that is added around the glyphs. Gives more space for dynamic range of distance transformed glyphs. That means, the glyphs get 'larger' and thus need more space to not be cut off.</dd>
  <dt>Downsampling</dt><dd>Distance Fields are meant to be downsampled. We offer different downsampling types and also different scaling factors.</dd>
  <dt>Dynamic Range</dt><dd>The Distance Transform calculates values, that need to be clamped in order to generate a PNG. Choose the min and max values.</dd>
</dl>

### Packing

Our implemented packing algorithms are based on the publication by Jukka Jylänki: *A thousand ways to pack the bin -- a practical approach to two-dimensional rectangle bin packing* (2010), except that for now, we don't use multiple bins (i.e. textures).

The *Shelf Bin Packing* (O(n log(n))) performs faster, but there are cases where *Max Rects Packing* (O(n^5)) gives better results.

Parameters: All glyph sizes, downsampled.

### Distance Transform

Algorithms: Dead-Reckoning, Parabola Envelope

Needs a result from Packing to know the glyph sizes

ToDo: parameters for dynamic range are applied for the png to export, but it also influences the rendering outcome of the distance field???

Parameters: 

### Rendering
application *llassetgen-rendering*
using fragment-shader as in [OpenLL](http://openll.org/) (Super Sampling)

<dl>
  <dt>Super Sampling</dt><dd>Choose the type of Super Sampling.</dd>
  <dt>Threshold</dt><dd>The default value is 0.5, since the Distance Transform considers 0.5 as the contour. Choosing a smaller value renders the glyphs bold; a larger value renders them thinner, until they completely disappear. A smaller value than 0.3 is clamped to 0.3, because such fragments are dicarded in fragment shader for performance reasons.</dd>
  <dt>Switch Rendering</dt><dd>You can toggle between rendering the calculated distance field or rendering the resulting glyphs when using the calculated distance field.</dd>
</dl>


