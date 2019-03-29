# grand_piano

Console application which reads a simple music "score" from a .txt, then generates and saves the melody as a .wav file. The music's .txt file is of the following format, where each number is a frequency in Hz:
```
207.7
207.7
220
246.9
```
Where there are two notes of the same frequency, the program treats them as a single longer note, so varied note lengths can be achieved. There are 3 instrument presets: trumpet, toypiano, and sine (which is the default if none/other specified).

*2019 Project 1 for ENGR101 @ VUW.*
*Note:* Will not run without wav.hpp file, property of VUW.
