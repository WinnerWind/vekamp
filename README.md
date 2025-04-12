# VekAmp - basstest-linux
<p align="center">
  <img alt="vekamp logo" width="45%" src="assets/branding/wordmark.svg" />
</p>

## About
VekAmp is intended to be a cross platform music player and library management tool making use of the [BASS audio library](https://www.un4seen.com/bass.html). 

The main motivation for this project is to replace my currently used music player (which is Windows exclusive) with one that can be used across several platforms, so that I can get what I'm looking for out of a music player on any platform I may use. I also intend to add a plethora of powerful library management systems to make sure your library is organised and stored in the way you want it to.
>[!NOTE]
>This project is still in it's infancy, If there are any suggestions or feedback you would like to provide, please [submit an issue](https://github.com/vektor451/vekamp/issues/new).

## Current Plans
Currently, the goal for the project's development, and this branch, is to get the audio library to play music files. The progress for this goal is as follows:
- [ ] Make it all work on Linux.

## Platforms
This branch intends to make the project cross-platform compatible across Windows and Linux.

## Compiling
Currently the project uses CMake with MSVC to compile on Windows, and GCC on Linux instead of MSVC. 

Before compilation, it is important to place the necessary library files in their directories. Check the folders in `vendor` for more information. 
