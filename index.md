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

**Original Image:**
![JellyFish](https://user-images.githubusercontent.com/59031606/108917069-c86d2c00-75f4-11eb-837c-1c95ceeba257.jpg)

**Random Noies**
`./image -input JellyFish.jpg -noise 0.5 -output noisy_jelly.jpg`

**Contrast**
`./image -input JellyFish.jpg -noise 0.5 -output noisy_jelly.jpg`

**Saturation**
`./image -input JellyFish.jpg -noise 0.5 -output noisy_jelly.jpg`

**Sharpen**

**Random Dither**

**Blur**

**Edge Detect**
`./image -input JellyFish.jpg -edgeDetect -output edgy_jelly.jpg`
![edgy_jelly](https://user-images.githubusercontent.com/59031606/108917029-b8554c80-75f4-11eb-9681-9d745d560805.jpg)

**Floyd-Steinberg Dither:**
`./image -input JellyFish.jpg -FloydSteinbergDither 1 -output edgy_jelly.jpg`
![floyd_dither_jelly](https://user-images.githubusercontent.com/59031606/108916965-a2e02280-75f4-11eb-8aac-58cafcb4756a.jpg)

**Scale**

**Sample**

**Ordered Dither**


[Link](url) and ![Image](src){:height="200px" width="500px"}
