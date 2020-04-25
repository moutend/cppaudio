#!/bin/bash

set -e

# Generates 36 WAVE files for testing. The patterns are:
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
# You need place an inputA.wav and inputB.wav into `../assets` directory before run this script.
# These .wav files can be your prefered, but it must have 2 channels and the length must be 5 seconds.

sox ../assets/inputA.wav -r 44100 -c 1 -b 16 ../assets/Test44100Hz1ch16bitA.wav
sox ../assets/inputA.wav -r 44100 -c 1 -b 24 ../assets/Test44100Hz1ch24bitA.wav
sox ../assets/inputA.wav -r 44100 -c 1 -b 32 ../assets/Test44100Hz1ch32bitA.wav

sox ../assets/inputA.wav -r 44100 -c 2 -b 16 ../assets/Test44100Hz2ch16bitA.wav
sox ../assets/inputA.wav -r 44100 -c 2 -b 24 ../assets/Test44100Hz2ch24bitA.wav
sox ../assets/inputA.wav -r 44100 -c 2 -b 32 ../assets/Test44100Hz2ch32bitA.wav

sox ../assets/inputA.wav -r 48000 -c 1 -b 16 ../assets/Test48000Hz1ch16bitA.wav
sox ../assets/inputA.wav -r 48000 -c 1 -b 24 ../assets/Test48000Hz1ch24bitA.wav
sox ../assets/inputA.wav -r 48000 -c 1 -b 32 ../assets/Test48000Hz1ch32bitA.wav

sox ../assets/inputA.wav -r 48000 -c 2 -b 16 ../assets/Test48000Hz2ch16bitA.wav
sox ../assets/inputA.wav -r 48000 -c 2 -b 24 ../assets/Test48000Hz2ch24bitA.wav
sox ../assets/inputA.wav -r 48000 -c 2 -b 32 ../assets/Test48000Hz2ch32bitA.wav

sox ../assets/inputA.wav -r 96000 -c 1 -b 16 ../assets/Test96000Hz1ch16bitA.wav
sox ../assets/inputA.wav -r 96000 -c 1 -b 24 ../assets/Test96000Hz1ch24bitA.wav
sox ../assets/inputA.wav -r 96000 -c 1 -b 32 ../assets/Test96000Hz1ch32bitA.wav

sox ../assets/inputA.wav -r 96000 -c 2 -b 16 ../assets/Test96000Hz2ch16bitA.wav
sox ../assets/inputA.wav -r 96000 -c 2 -b 24 ../assets/Test96000Hz2ch24bitA.wav
sox ../assets/inputA.wav -r 96000 -c 2 -b 32 ../assets/Test96000Hz2ch32bitA.wav

# Generate from inputB.wav

sox ../assets/inputB.wav -r 44100 -c 1 -b 16 ../assets/Test44100Hz1ch16bitB.wav
sox ../assets/inputB.wav -r 44100 -c 1 -b 24 ../assets/Test44100Hz1ch24bitB.wav
sox ../assets/inputB.wav -r 44100 -c 1 -b 32 ../assets/Test44100Hz1ch32bitB.wav

sox ../assets/inputB.wav -r 44100 -c 2 -b 16 ../assets/Test44100Hz2ch16bitB.wav
sox ../assets/inputB.wav -r 44100 -c 2 -b 24 ../assets/Test44100Hz2ch24bitB.wav
sox ../assets/inputB.wav -r 44100 -c 2 -b 32 ../assets/Test44100Hz2ch32bitB.wav

sox ../assets/inputB.wav -r 48000 -c 1 -b 16 ../assets/Test48000Hz1ch16bitB.wav
sox ../assets/inputB.wav -r 48000 -c 1 -b 24 ../assets/Test48000Hz1ch24bitB.wav
sox ../assets/inputB.wav -r 48000 -c 1 -b 32 ../assets/Test48000Hz1ch32bitB.wav

sox ../assets/inputB.wav -r 48000 -c 2 -b 16 ../assets/Test48000Hz2ch16bitB.wav
sox ../assets/inputB.wav -r 48000 -c 2 -b 24 ../assets/Test48000Hz2ch24bitB.wav
sox ../assets/inputB.wav -r 48000 -c 2 -b 32 ../assets/Test48000Hz2ch32bitB.wav

sox ../assets/inputB.wav -r 96000 -c 1 -b 16 ../assets/Test96000Hz1ch16bitB.wav
sox ../assets/inputB.wav -r 96000 -c 1 -b 24 ../assets/Test96000Hz1ch24bitB.wav
sox ../assets/inputB.wav -r 96000 -c 1 -b 32 ../assets/Test96000Hz1ch32bitB.wav

sox ../assets/inputB.wav -r 96000 -c 2 -b 16 ../assets/Test96000Hz2ch16bitB.wav
sox ../assets/inputB.wav -r 96000 -c 2 -b 24 ../assets/Test96000Hz2ch24bitB.wav
sox ../assets/inputB.wav -r 96000 -c 2 -b 32 ../assets/Test96000Hz2ch32bitB.wav
