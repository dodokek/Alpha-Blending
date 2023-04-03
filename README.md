# Alpha Blending

## Overview

The main purpose of this project is implementing alpha-blending algorithm the most efficient way. 

First of all I will implement it the most straightforward way. After that I will speed it up by using SSE instructions.

## Alpha blending in a nutshell 

We want to mix two images together using this formula:

~~~C++
result_pxl = (alpha * rgb_pxl1 + (1 - alpha) * rgb_pxl2)
~~~

Where rgb_pxl1 and rgb_pxl2 are uint32_t color values of individual pixels.

## First approach

To calculate every pixel on the screen we can iterate through each of them. 

To for one cycle of drawing we will make approximately 480000  operations like this (assuming 800x600 img size):

~~~C++

red   = ( backARGB->red   * backARGB->alpha + frontARGB->red   * (255 - frontARGB->alpha) ) >> 8;
green = ( backARGB->green * backARGB->alpha + frontARGB->green * (255 - frontARGB->alpha) ) >> 8;
blue  = ( backARGB->blue  * backARGB->alpha + frontARGB->blue  * (255 - frontARGB->alpha) ) >> 8;

~~~

**Average FPS: 20**

## Optimization idea

To speed up the program we can use SSE instructions, to calculate 4 pixels at once.

** *picture of byte shuffling* **

With optimization flag -O3 FPS rises **up to 400**.

## Performance

Let's test the rise in performance thanks to SSE instructions.

| Version      | Compilation flags | FPS           |
| ------      | :---------------: | :------------: | 
| No SSE      | none              | 20             |  
| No SSE      | -О3               | 60             |  
| No SSE      | -Оfast            | 65             | 
| SSE         | none              | 23             |  
| SSE         | -О3               | 400            | 
| SSE         | -Ofast            | 400            |

As we can see, speed incresed **~8 times**. This confirms the effectivness of SSE instructions.

## Conclusion





