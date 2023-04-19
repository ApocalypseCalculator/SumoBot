# Sumo Bot

1. scan for enemies 
2. charge towards enemy
3. ???
4. profit


#### Optimizations

The final optimized code is in [sumobot_clean.ino](https://github.com/ApocalypseCalculator/SumoBot/blob/master/sumobot_clean.ino).

Several optimizations were made to reduce the latency of the loop: 

1. When compiling the program the GCC compiler option `-Ofast` was set instead of the stock `-Os` (level 2 with file size optimization), which enables level 3 optimizations as well as some non-standard math optimizations. The compiler options were edited in `platform.txt` at `%LocalAppData%\Arduino15\packages\arduino\hardware\avr\1.8.6`. 
2. The denoising/outlier removal of sensor values was removed as a tradeoff for performance
3. Accelerometer was removed entirely
4. The gradual acceleration/deceleration was removed
5. Any non-essential code like Serial and debug LEDs or redundant `digitalWrite()` were removed

Overall the latency of the loop was reduced from 250ms/iteration to roughly 25ms/iteration, **a 10x decrease!**
