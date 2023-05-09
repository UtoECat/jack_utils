# WARNING
This rep is no longer maintained and developed.
Use DSSI, CLAP, LADSPA or any other audio plugin API.

# About JackUtils
## jackutils - what is this?
Jackutils is a library and a set of useful utilities for the JACK. The library implements a simple api for managing connected ports using descriptors, connecting and checking whether the jack server is working, setuping and estabilishing connection with session manager, opening a glfw window and gui(powered by nuklear). 
## dependencies
- **GLFW** *(Window and OpenGL context creantion and manipulation)*
- **liblo** *(OSC support)*
- **libfftw** *(Fourie transform implementation)*
- **JACK** *(Jack Audio Connection Kit)* **OR** **pipewire-jack**
# List of utilites
## Normalizer
*(Sound modification utility)* Dynamicly normalizes volume.
![normalizer](/doc/normalizer.png)
## Waveform
*(visualization utility)*    
![waveform](/doc/waveform.png)
## Spectrum
*(visualization utility)*    
![spectrum](/doc/spectrum.png)
# real TODO and contributing
- ~~Include nuklear library!~~ (done)
- Move utilities documentation to separate file!
- ~~Add new session manager support~~ (done)
- C++ is a bullshit! Never tell me to rewrite codebase on C++ or i will take your soul. 
- Any ideas for additional utilities?
- Usage of ports at this moment is **TOTALLY NOT THREADSAFE**.
- I need more tests in real problems, not for my toy needs. If you have any ideas and suggestions, write to Suggestions
- Check is this readme is lexically correct. I hate google translator :D
- Need more optimisation for rendering
