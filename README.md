# Alpha Blending

## Overview


The goal of this project is implementing Alpha Blending algorithm the most efficient way.

To maximize performance we are going to use SSE instructions.

Example:

<img src="https://user-images.githubusercontent.com/57039216/230920021-e88b8c77-fc0d-4bdc-b3f9-7a30d160c97f.png" width="500px">


## Alpha blending in a nutshell 

We want to mix two images together. Each pixel will be calculated according to this formula:

$ColorNew_{red} = ColorBack_{red} * \alpha + ColorFront_{red} * (1 - \alpha)$ , where $\alpha$ varies between 0 and 1 

The same goes for Green and Blue channels.
## First approach

We can calculate each pixel iteratively.
This means that for one cycle of drawing we will make approximately **480000  operations** like this:

~~~C++

red   = ( red_1   * alpha + red_2   * (255 - alpha) ) >> 8;
green = ( green_1 * alpha + green_2 * (255 - alpha) ) >> 8;
blue  = ( blue_1  * alpha + blue_2  * (255 - alpha) ) >> 8;

~~~

I will add one internal cycle, to calculate one point 1000 times. This is going to *lower the impact* of graphic library.

> Assuming 800x600 foreground image size. 
> System info: Core i5, 9th gen.

**Average FPS: 9.6 +- 0.9**

## Optimizing with SSE instructions

Each pixel is calculated independently. This means that we can calculate 4 of them at once with SSE instrucions.

The method I used to implement this algorithm includes a vast variety of bit manipulations. Because of them my code becomes much more complicated and bigger in size.

However, we can see the **FPS counter jumps to 36.6 +- 1.6**

>Lines of code increased *from 5 to 22*.

## Performance

Let's test the rise in performance.

| Version      | Compilation flags | FPS           | Speed Growth |
| ------      | :---------------: | :------------: | :----------: |
| No SSE      | none              | 4             |    0.4      |
| No SSE      | -О3               | 9             |     0.9     |
| No SSE      | -Оfast            | 10            |     1     |
| SSE         | none              | 3             |      0.3    |
| SSE         | -О3               | 36            |      3.6    |
| SSE         | -Ofast            | 38            |      3.8    |

As we can see, speed incresed **~4 times**. This confirms the effectivness of SSE instructions.

Also there is negative growth in speed if we don't use optimization flags, because of uneffecient usage of SIMD registers. 

## Conclusion

SSE instructions allowed us to speed up the calculations *4 times*. However the amount of code became *4 times* bigger too.

Once again great language C proved that **the more** code you write - **the faster** your program becomes.







