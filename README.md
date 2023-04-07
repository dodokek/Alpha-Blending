# Alpha Blending

## Overview


The goal of this project is implementing Alpha Blending algorithm. set the most effecient way.

To maximize performance we are going to use SSE instructions.

Here is the example of what we want to get as a result of drawing.

## Alpha blending in a nutshell 

We want to mix two images together. Each pixel will be calculated according to this formula:

$NewColor_{red} = Color1_{red} * alpha + Color2_{red} * (alpha - 1)$ , where *alpha* varies between 0 and 1 

The same goes for Green and Blue channels.
## First approach

We can calculate each pixel iteratively.
This means that for one cycle of drawing we will make approximately **480000  operations** like this:

~~~C++

red   = ( red_1   * alpha + red_2   * (255 - alpha) ) >> 8;
green = ( green_1 * alpha + green_2 * (255 - alpha) ) >> 8;
blue  = ( blue_1  * alpha + blue_2  * (255 - alpha) ) >> 8;

~~~
> Assuming 800x600 foreground image size
> System info: Core i5, 9th gen.

**Average FPS: 20**

## Optimizing with SSE instructions

Each pixel is calculated independently. This means that we can calculate 4 of them at once with SSE instrucions.

The method I used to implement this algorithm includes a vast variaty of bit manipulations. Because of that the code becomes much more complicated and bigger in size.

However we can see the **FPS counter jumps to 76**


## Performance

Let's test the rise in performance.

| Version      | Compilation flags | FPS           | Speed Growth |
| ------      | :---------------: | :------------: | :----------: |
| No SSE      | none              | 20             |    0.3      |
| No SSE      | -О3               | 60             |     1     |
| No SSE      | -Оfast            | 65             |     1.08     |
| SSE         | none              | 23             |      0.3    |
| SSE         | -О3               | 400            |      6.5    |
| SSE         | -Ofast            | 400            |      6.5    |

As we can see, speed incresed **~4 times**. This confirms the effectivness of SSE instructions.

## Conclusion

SSE instructions allowed us to speed up the calculations *4 times*. However the amount of code became *4 times* bigger too.

Once again great language C proved that **the more** code you write - **the faster** your programm becomes.







