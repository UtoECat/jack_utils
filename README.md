# jackutils - what is this?
Jackutils is a library and a set of useful utilities for the JACK. The library implements a simple api for managing connected ports using descriptors, connecting and checking whether the jack server is working, opening a glfw window and primitive drawing functions (for visualizations, optional)

# list of utilites
- *Some simple utilities* **TODO**
- **Generator** *(increased during time waveform generator)*
- **Waveform** *(visualization utility)*
- **Spectrum** *(visualization utility)*

# real TODO and contributing
- C++ is a bullshit! Never tell me to rewrite codebase on C++ or i will take your soul. 
- Any ideas for additional utilities?
- Usage of ports at this moment is **TOTALLY NOT THREADSAFE** and thats a problem. Any ideas how to lock them threadsafely and fast please.
- I need more tests in real problems, not for my toy needs. If you have any ideas and suggestions, write to Suggestions
- Check is this readme is lexically correct. I hate google translator :D
- ju_settings api is not done yet, need more tests and code.
- autoclose timeout for clients... ?
