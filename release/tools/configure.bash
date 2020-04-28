#!/bin/bash

set -e

# Generates 18 WAVE files for testing. The patterns are:
# 
# * 44100 Hz - 1 ch - 16 bit / 44100 Hz - 1 ch - 24 bit / 44100 Hz - 1 ch - 32 bit
# * 44100 Hz - 2 ch - 16 bit / 44100 Hz - 2 ch - 24 bit / 44100 Hz - 2 ch - 32 bit
# * 48000 Hz - 1 ch - 16 bit / 48000 Hz - 1 ch - 24 bit / 48000 Hz - 1 ch - 32 bit
# * 48000 Hz - 2 ch - 16 bit / 48000 Hz - 2 ch - 24 bit / 48000 Hz - 2 ch - 32 bit
# * 96000 Hz - 1 ch - 16 bit / 96000 Hz - 1 ch - 24 bit / 96000 Hz - 1 ch - 32 bit
# * 96000 Hz - 2 ch - 16 bit / 96000 Hz - 2 ch - 24 bit / 96000 Hz - 2 ch - 32 bit
# 
# Required tools
# 
# Bash or equivalant shell.
# - SoX - sound exchange 
# 
# Note
# 
# You need place an input.wav into `audio` directory before run this script.
# An input.wav can be your prefered, but it must have 2 channels at least.

sox ../assets/input.wav -r 44100 -c 1 -b 16 ../assets/44100Hz1ch16bit.wav
sox ../assets/input.wav -r 44100 -c 1 -b 24 ../assets/44100Hz1ch24bit.wav
sox ../assets/input.wav -r 44100 -c 1 -b 32 ../assets/44100Hz1ch32bit.wav

sox ../assets/input.wav -r 44100 -c 2 -b 16 ../assets/44100Hz2ch16bit.wav
sox ../assets/input.wav -r 44100 -c 2 -b 24 ../assets/44100Hz2ch24bit.wav
sox ../assets/input.wav -r 44100 -c 2 -b 32 ../assets/44100Hz2ch32bit.wav

sox ../assets/input.wav -r 48000 -c 1 -b 16 ../assets/48000Hz1ch16bit.wav
sox ../assets/input.wav -r 48000 -c 1 -b 24 ../assets/48000Hz1ch24bit.wav
sox ../assets/input.wav -r 48000 -c 1 -b 32 ../assets/48000Hz1ch32bit.wav

sox ../assets/input.wav -r 48000 -c 2 -b 16 ../assets/48000Hz2ch16bit.wav
sox ../assets/input.wav -r 48000 -c 2 -b 24 ../assets/48000Hz2ch24bit.wav
sox ../assets/input.wav -r 48000 -c 2 -b 32 ../assets/48000Hz2ch32bit.wav

sox ../assets/input.wav -r 96000 -c 1 -b 16 ../assets/96000Hz1ch16bit.wav
sox ../assets/input.wav -r 96000 -c 1 -b 24 ../assets/96000Hz1ch24bit.wav
sox ../assets/input.wav -r 96000 -c 1 -b 32 ../assets/96000Hz1ch32bit.wav

sox ../assets/input.wav -r 96000 -c 2 -b 16 ../assets/96000Hz2ch16bit.wav
sox ../assets/input.wav -r 96000 -c 2 -b 24 ../assets/96000Hz2ch24bit.wav
sox ../assets/input.wav -r 96000 -c 2 -b 32 ../assets/96000Hz2ch32bit.wav
