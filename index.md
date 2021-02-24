## Project 2


### Project Difficulties

**Random Noise:** My only main problem here was the amount of noise to add with an increase in the factor number. 

**Contrast & Saturation:** No significant problems here.

**Sharpen:** Once I got the blur function in place, I had the original image, a blurred image, and interpolated from the blurred to the original.

**Random Dither:** I'm not sure if there was a way to do this using the PixelQuant() function. I ended up modifying that function to add some noise. I did the same sort of process with the **Ordered Dither**.

**Blur:** I played around with this one a little bit, because I felt like I wasn't getting a significant increase in blur when I increased the n. I ended up using an n x n mask, along with using n as the standard deviation. I could visually see the difference between the initial and blurred images. Along with blurred and more blurred.

**Edge Detect:** I used Sobel edge detection, and it went very smoothly when I got the matrix being applied correctly.

**Floyd-Steinberg Dither:** This went pretty well once I understood what the quantization error really was. The code looks kinda ugly, but I needed to visualize what was happening with each pixel.

**Scale:** I encountered some problems with indexing. When I was calculating the x and y indices to Sample the original image with, I was dividing by both widths, so my doubles were rounding down to 0, giving me a solid colored image. I needed to just divide by the new width, and adjust the point sampling function to clamp at width - 1.  (height was the same way with everything as well).

**Rotate:** I encountered some problems with indexing.

**Sampling:** I was a little confused with the bilinear sampling. The gaussian went fine - I had completed the Blur function before working on this.

Added: 
- **Ordered Dither**

**Original Image:**
![JellyFish](https://user-images.githubusercontent.com/59031606/108917069-c86d2c00-75f4-11eb-837c-1c95ceeba257.jpg)

**Random Noise**

`./image -input JellyFish.jpg -noise 0.5 -output noisy_jelly.jpg`

![noisy_jelly](https://user-images.githubusercontent.com/59031606/108917808-f2731e00-75f5-11eb-981b-37003d7165e7.jpg)

**Contrast**

`./image -input JellyFish.jpg -contrast 5 -output contrasted_jelly.jpg`

![contrast_jelly](https://user-images.githubusercontent.com/59031606/108917904-1898be00-75f6-11eb-890e-85d84afd35d6.jpg)

**Saturation**

`./image -input JellyFish.jpg -saturation 0.5 -output saturated_jelly.jpg`

![saturate_jelly](https://user-images.githubusercontent.com/59031606/108918022-4b42b680-75f6-11eb-9359-5d4721539e0c.jpg)

**Sharpen**

`./image -input JellyFish.jpg -sharpen 6 -output sharp_jelly.jpg`

![sharp_jelly](https://user-images.githubusercontent.com/59031606/108917354-387bb200-75f5-11eb-8376-5ca2719396ba.jpg)

**Random Dither**

`./image -input JellyFish.jpg -randomDither 3 -output rand_jelly.jpg`

![rand_jelly](https://user-images.githubusercontent.com/59031606/108917451-5c3ef800-75f5-11eb-9594-da93a8b15bd7.jpg)

**Blur**

`./image -input JellyFish.jpg -blur 6 -output blurry_jelly.jpg`

![blurry_jelly](https://user-images.githubusercontent.com/59031606/108917164-ee92cc00-75f4-11eb-9897-69493d5316e5.jpg)

**Edge Detect**

`./image -input JellyFish.jpg -edgeDetect -output edgy_jelly.jpg`

![edgy_jelly](https://user-images.githubusercontent.com/59031606/108917029-b8554c80-75f4-11eb-9681-9d745d560805.jpg)

**Floyd-Steinberg Dither:**

`./image -input JellyFish.jpg -FloydSteinbergDither 1 -output floyd_dither_jelly.jpg`

![floyd_dither_jelly](https://user-images.githubusercontent.com/59031606/108916965-a2e02280-75f4-11eb-8aac-58cafcb4756a.jpg)

**Scale**

`./image -input JellyFish.jpg -scale 0.5 0.5 -output scaled_jelly.jpg`

![baby_jelly](https://user-images.githubusercontent.com/59031606/108935624-ab942100-7613-11eb-92ca-84f43bd4d9d3.png)

**Rotate**

`./image -input JellyFish.jpg -FloydSteinbergDither 1 -output rotate_jelly.jpg`

**Ordered Dither**

`./image -input JellyFish.jpg -orderedDither 5 -output ordered_jelly.jpg`

![ordered_jelly](https://user-images.githubusercontent.com/59031606/108917646-b0e27300-75f5-11eb-9b7d-7e8186082a97.jpg)
