## Project 2


### Project Difficulties

**Random Noise:** My only main problem here was the amount of noise to add with an increase in the factor number. 

**Contrast & Saturation:** No significant problems here.
**Sharpen:** Once I got the blur function in place, I had the original image, a blurred image, and interpolated from the blurred to the original.
**Random Dither:** I'm not sure if there was a way to do this using the PixelQuant() function. I ended up modifying that function to add some noise. I did the same sort of process with the **Ordered Dither**.
**Blur:** I played around with this one a little bit, because I felt like I wasn't getting a significant increase in blur when I increased the n. I ended up using an n x n mask, along with using n as the standard deviation. I could visually see the difference between the initial and blurred images. Along with blurred and more blurred.
**Edge Detect:** I used Sobel edge detection, and it went very smoothly when I got the matrix being applied correctly.
**Floyd-Steinberg Dither:** This went pretty well once I understood what the quantization error really was. The code looks kinda ugly, but I needed to visualize what was happening with each pixel.
**Scale:** I encountered some problems with indexing.
**Rotate:** I encountered some problems with indexing.
**Sampling:** I was a little confused with the bilinear sampling. The gaussian went fine - I had completed the Blur function before working on this.

Added: 
- Ordered Dither


1. Numbered
2. List

**Bold** and _Italic_ and `Code` text

[Link](url) and ![Image](src)
