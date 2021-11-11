# StereoToAmbi

> A plugin for converting stereo audio to ambisonic

![StereoToAmbi Plugin](https://github.com/HaydonCardew/StereoToAmbi/blob/master/Images/ScreenShot.png?raw=true "StereoToAmbi running inside Reaper")

A plugin which performs blind source extraction on a left/right input, estimates the azimuth of each source and remaps them into an ambisonic sound field. Optionally, the input can be passed through a reverb extraction engine.



## Parameters
**Reverb Extraction**: Toggle the reverb extraction on/off.

**Threshold**: The threshold for which correlation between left and right is deemed reverb instead of a direct signal.

**Sustain**: How suddenly a given signal will change between reverb and direct.

**Width**: A dial to control where the centre of the sound field should be. A slider to control the width of the sound field.

## Usage
This plugin should be placed on a track with at least 16 channels. Input channels 1 & 2 will be assumed to be stereo (left/right) and output channels 1-16 will be 3rd order ambisonic in AmbiX format.

![Sound Source Distribution](https://github.com/HaydonCardew/StereoToAmbi/blob/master/Images/Width_Direction_Example.png?raw=true "How Sound Sources Are Distributed")

## Contributing
If you experience any problems please open an issue. Also, feel free to add any functionality and submit a pull request.

## Building
This project is built via JUCE. Please download the Projucer application and open StereoToAmbi.jucer, then export to your chosen IDE.

## Pre-built Downloads
Compiled versions of this plugin can be found here: ***  
Versions available: ***

## Graphic Design
A big thank you to my sister Hazel for providing GUI images. Check out her portfolio here: https://cardewdesign.wordpress.com

## License

???
