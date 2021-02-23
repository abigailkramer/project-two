//CSCI 5607 HW 2 - Image Conversion Instructor: S. J. Guy <sjguy@umn.edu>
//In this assignment you will load and convert between various image formats.
//Additionally, you will manipulate the stored image data by quantizing, cropping, and supressing channels

#include "image.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <float.h>
#include <time.h>

#define _USE_MATH_DEFINES

#include <fstream>
using namespace std;

/**
 * Image
 **/
Image::Image (int width_, int height_){

    assert(width_ > 0);
    assert(height_ > 0);

    width           = width_;
    height          = height_;
    num_pixels      = width * height;
    sampling_method = IMAGE_SAMPLING_POINT;
    
    data.raw = new uint8_t[num_pixels*4];
		int b = 0; //which byte to write to
		for (int j = 0; j < height; j++){
			for (int i = 0; i < width; i++){
				data.raw[b++] = 0;
				data.raw[b++] = 0;
				data.raw[b++] = 0;
				data.raw[b++] = 0;
			}
		}

    assert(data.raw != NULL);
}

Image::Image (const Image& src){
	width           = src.width;
	height          = src.height;
	num_pixels      = width * height;
	sampling_method = IMAGE_SAMPLING_POINT;
	
	data.raw = new uint8_t[num_pixels*4];
	
	memcpy(data.raw, src.data.raw, num_pixels*sizeof(Pixel));
	// *data.raw = *src.data.raw;
}

Image::Image (char* fname){

   int lastc = strlen(fname);
   int numComponents; //(e.g., Y, YA, RGB, or RGBA)
   data.raw = stbi_load(fname, &width, &height, &numComponents, 4);
	
	if (data.raw == NULL){
		printf("Error loading image: %s", fname);
		exit(-1);
	}
	
	num_pixels = width * height;
	sampling_method = IMAGE_SAMPLING_POINT;
	
}

Image::~Image (){
    delete [] data.raw;
    data.raw = NULL;
}

void Image::Write(char* fname){
	
	int lastc = strlen(fname);

	switch (fname[lastc-1]){
	   case 'g': //jpeg (or jpg) or png
	     if (fname[lastc-2] == 'p' || fname[lastc-2] == 'e') //jpeg or jpg
	        stbi_write_jpg(fname, width, height, 4, data.raw, 95);  //95% jpeg quality
	     else //png
	        stbi_write_png(fname, width, height, 4, data.raw, width*4);
	     break;
	   case 'a': //tga (targa)
	     stbi_write_tga(fname, width, height, 4, data.raw);
	     break;
	   case 'p': //bmp
	   default:
	     stbi_write_bmp(fname, width, height, 4, data.raw);
	}
}


void Image::Brighten (double factor){
	int x,y;
	for (x = 0 ; x < Width() ; x++){
		for (y = 0 ; y < Height() ; y++){
			Pixel p = GetPixel(x, y);
			Pixel scaled_p = p*factor;
			GetPixel(x,y) = scaled_p;
		}
	}
}

void Image::ExtractChannel(int channel){
   if (channel == 0) {           // red
      for (int i = 0; i < Width(); i++) {
         for (int j = 0; j < Height(); j++) {
            Pixel p = GetPixel(i, j);
			Pixel extracted_p = p;
            extracted_p.Set(p.r,0,0);
			GetPixel(i,j) = extracted_p;
         }
      }
   } else if (channel == 1) {    // green
      for (int i = 0; i < Width(); i++) {
         for (int j = 0; j < Height(); j++) {
            Pixel p = GetPixel(i, j);
			Pixel extracted_p = p;
            extracted_p.Set(0,p.g,0);
			GetPixel(i,j) = extracted_p;
         }
      }
   } else if (channel == 2) {    // blue
      for (int i = 0; i < Width(); i++) {
         for (int j = 0; j < Height(); j++) {
            Pixel p = GetPixel(i, j);
			Pixel extracted_p = p;
            extracted_p.Set(0,0,p.b);
			GetPixel(i,j) = extracted_p;
         }
      }
   } else {
      return;
   }
}


void Image::Quantize (int nbits){
   for (int i = 0; i < Width(); i++) {
      for (int j = 0; j < Height(); j++) {
         Pixel p = GetPixel(i, j);
         Pixel quant_p = p;
         GetPixel(i,j) = PixelQuant(quant_p,nbits);
      }
   }
}

Image* Image::Crop(int x, int y, int w, int h){
   Image *img = new Image(w,h);
   for (int i = 0; i < w; i++) {
      for (int j = 0; j < h; j++) {
         Pixel p = GetPixel(x+i, y+j);
         img->SetPixel(i,j,p);
      }
   }
	
	return img;
}


void Image::AddNoise (double factor){
	/* WORK HERE */
	for (int i = 0; i < width; i++) {
		for (int j = 0; j < height; j++) {
			Pixel p = GetPixel(i,j);
			Pixel noise = p;
			double n = (((double)rand()*(factor - (-factor)) / RAND_MAX) - factor);
			noise = (noise + PixelRandom()*n);
			
			noise.SetClamp(noise.r,noise.g,noise.b);
			GetPixel(i,j) = noise;
		}
	}
	return;
}

void Image::ChangeContrast (double factor){
	/* WORK HERE */
	double num_pixels = width*height;
	double avg = 0;
	for (int i = 0; i < width; i++) {
		for (int j = 0; j < height; j++) {
			avg += GetPixel(i,j).Luminance();
		}
	}
	avg /= num_pixels;

	for (int i = 0; i < width; i++) {
		for (int j = 0; j < height; j++) {
			Pixel p = GetPixel(i,j);
			Pixel lum = Pixel(avg,avg,avg);
			Pixel scaled_p = PixelLerp(lum, p, factor);
			GetPixel(i,j) = scaled_p;
		}
	}

	return;
}


void Image::ChangeSaturation(double factor){
	/* WORK HERE */
	for (int i = 0; i < width; i++) {
		for (int j = 0; j < height; j++) {
			Pixel p = GetPixel(i,j);
			Pixel scaled_p = p;
			scaled_p.r = ComponentLerp(scaled_p.Luminance(), scaled_p.r, factor);
			scaled_p.g = ComponentLerp(scaled_p.Luminance(), scaled_p.g, factor);
			scaled_p.b = ComponentLerp(scaled_p.Luminance(), scaled_p.b, factor);
			GetPixel(i,j) = scaled_p;
		}
	}
	return;
}


//For full credit, check that your dithers aren't making the pictures systematically brighter or darker
void Image::RandomDither (int nbits){
	/* WORK HERE */
	for (int i = 0; i < Width(); i++) {
		for (int j = 0; j < Height(); j++) {
			Pixel p = GetPixel(i,j);
			int shift = 8-nbits;
			float mult = 255/float(255 >> shift);
			int new_r, new_g, new_b;
			new_r = (p.r >> shift) + (((double)rand()*(0.5 - (-0.5)) / RAND_MAX) -0.5);
			new_g = (p.g >> shift) + (((double)rand()*(0.5 - (-0.5)) / RAND_MAX) -0.5);
			new_b = (p.b >> shift) + (((double)rand()*(0.5 - (-0.5)) / RAND_MAX) -0.5);

			Pixel ret;
			ret.SetClamp(new_r*mult , new_g*mult , new_b*mult );
			GetPixel(i,j) = ret;
		}
	}

	return;
}

//This bayer method gives the quantization thresholds for an ordered dither.
//This is a 4x4 dither pattern, assumes the values are quantized to 16 levels.
//You can either expand this to a larger bayer pattern. Or (more likely), scale
//the threshold based on the target quantization levels.
static int Bayer4[4][4] ={
    {15,  7, 13,  5},
    { 3, 11,  1,  9},
    {12,  4, 14,  6},
    { 0,  8,  2, 10}
};


void Image::OrderedDither(int nbits){		// lmao rip this is ec
	/* WORK HERE */
	
	// i and j % nbits
	for (int i = 0; i < width; i++) {
		for (int j = 0; j < height; j++) {
         	Pixel e = GetPixel(i, j);

			int x = i % nbits;
			int y = j % nbits;

			e.r = e.r - trunc(e.r);
			e.g = e.g - trunc(e.g);
			e.b = e.b - trunc(e.b);

			if (e.r > (Bayer4[x][y] << nbits)) { // scale threshold
				e.r = ceil(e.r);
			} else { e.r = floor(e.r); }

			if ((e.g > Bayer4[x][y] << nbits)) { // scale threshold
				e.g = ceil(e.g);
			} else { e.g = floor(e.g); }

			if ((e.b > Bayer4[x][y] << nbits)) { // scale threshold
				e.b = ceil(e.b);
			} else { e.r = floor(e.b); }
			
			GetPixel(i,j) = PixelQuant(e,nbits);
		}
	}

}

/* Error-diffusion parameters */
const double
    ALPHA = 7.0 / 16.0,
    BETA  = 3.0 / 16.0,
    GAMMA = 5.0 / 16.0,
    DELTA = 1.0 / 16.0;

void Image::FloydSteinbergDither(int nbits){

	for (int y = 0; y < height-1; y++) {
		for (int x = 1; x < width-1; x++) {
			Pixel p = GetPixel(x,y);
			Pixel quant_p = p;

			quant_p = PixelQuant(p,nbits);
			GetPixel(x,y) = quant_p;

			Pixel error = quant_p;
			error.r = ComponentClamp(p.r-quant_p.r);
			error.g = ComponentClamp(p.g-quant_p.g);
			error.b = ComponentClamp(p.b-quant_p.b);

			Pixel alpha = GetPixel(x+1,y);
			alpha.r = ComponentClamp(alpha.r + error.r*ALPHA);
			alpha.g = ComponentClamp(alpha.g + error.g*ALPHA);
			alpha.b = ComponentClamp(alpha.b + error.b*ALPHA);
			GetPixel(x+1,y) = alpha;

			Pixel beta = GetPixel(x-1,y+1);
			beta.r = ComponentClamp(beta.r + error.r*BETA);
			beta.g = ComponentClamp(beta.g + error.g*BETA);
			beta.b = ComponentClamp(beta.b + error.b*BETA);
			GetPixel(x-1,y+1) = beta;

			Pixel gamma = GetPixel(x,y+1);
			gamma.r = ComponentClamp(gamma.r + error.r*GAMMA);
			gamma.g = ComponentClamp(gamma.g + error.g*GAMMA);
			gamma.b = ComponentClamp(gamma.b + error.b*GAMMA);
			GetPixel(x,y+1) = gamma;	

			Pixel delta = GetPixel(x+1,y+1);
			delta.r = ComponentClamp(delta.r + error.r*DELTA);
			delta.g = ComponentClamp(delta.g + error.g*DELTA);
			delta.b = ComponentClamp(delta.b + error.b*DELTA);
			GetPixel(x+1,y+1) = delta;							

		}
	}
   return;
}

void Image::Blur(int n){

	int size = (2*n) - 1;
	
	float r,g,b;
	Image *img = new Image(*this);

	for (int i = 0; i < Width(); i++) {
		for (int j = 0; j < Height(); j++) {
			r=g=b=0.0;
			float div = 0.0;

			for (int x = -n; x <= n; x++) {
				for (int y = -n; y <= n; y++) {
					int new_x = i+x;
					int new_y = j+y;

					if ((new_y >= 0) && (new_x >= 0) && (new_y+1 < Height()) && (new_x+1 < Width())) {
						Pixel p = GetPixel(i+x,j+y);
						float weight = exp(-(x*x + y*y) / 2);
						r += p.r*weight;
						g += p.g*weight;
						b += p.b*weight;

						div += weight;
					}
				}
			}

			r /= div;
			g /= div;
			b /= div;

			Pixel n = Pixel((int)r,(int)g,(int)b);

			n.r = ComponentClamp(n.r);
			n.g = ComponentClamp(n.g);
			n.b = ComponentClamp(n.b);

			img->GetPixel(i,j) = n;

		}
	}

	this->data.raw = img->data.raw;
	// img->~Image();
	return;
}

void Image::Sharpen(int n){
	/* WORK HERE */
	
	float r,g,b;
	Image *img = new Image(*this);
	img->Blur(n);

	for (int i = 0; i < Width(); i++) {
		for (int j = 0; j < Height(); j++) {
			r=g=b=0.0;
			float div = 0.0;

			for (int x = -n; x <= n; x++) {
				for (int y = -n; y <= n; y++) {
					int new_x = i+x;
					int new_y = j+y;

					if ((x == 0) && (y == 0)) {
						Pixel p = GetPixel(i+x,j+y);
						float weight = exp(-(x*x + y*y) / 2);
						r += p.r*weight;
						g += p.g*weight;
						b += p.b*weight;

						div += weight;
					} else if ((new_y >= 0) && (new_x >= 0) && (new_y+1 < Height()) && (new_x+1 < Width())) {
						Pixel p = GetPixel(i+x,j+y);
						float weight = -exp(-(x*x + y*y) / 2); //(1.0/(sqrt(2*M_PI))) * 
						r += p.r*weight;
						g += p.g*weight;
						b += p.b*weight;

						// div += weight;
					}
				}
			}

			// r /= div;
			// g /= div;
			// b /= div;

			Pixel n = Pixel((int)r,(int)g,(int)b);

			// n.r = ComponentClamp(n.r);
			// n.g = ComponentClamp(n.g);
			// n.b = ComponentClamp(n.b);

			img->GetPixel(i,j) = n;

		}
	}

	this->data.raw = img->data.raw;
	// img->~Image();
	return;
}


void Image::EdgeDetect(){
	/* WORK HERE */

	float r_x,g_x,b_x;
	float r_y, g_y, b_y;
	Image *img = new Image(*this);

	int Gx[3][3] = {{1,0,-1}, {2,0,-2}, {1,0,-1}};
	int Gy[3][3] = {{1,2,1}, {0,0,0}, {-1,-2,-1}};

	for (int i = 0; i < Width(); i++) {
		for (int j = 0; j < Height(); j++) {
			r_x=g_x=b_x=0;
			r_y=g_y=b_y=0;

			if ((i == 0) || (i+1 == Width()) || (j == 0) || (j+1 == Height())) {
				r_x=g_x=b_x=0;
				r_y=g_y=b_y=0;
			} else {
				for (int x = -1; x <= 1; x++) {
					for (int y = -1; y <= 1; y++) {
						Pixel p = GetPixel(x+i,y+j);

						r_x += p.r * Gx[x+1][y+1];
						g_x += p.g * Gx[x+1][y+1];
						b_x += p.b * Gx[x+1][y+1];

						r_y += p.r * Gy[x+1][y+1];
						g_y += p.g * Gy[x+1][y+1];
						b_y += p.b * Gy[x+1][y+1];
					}
				}

				int mag_r = sqrt(r_x*r_x + r_y*r_y);
				int mag_g = sqrt(g_x*g_x + g_y*g_y);
				int mag_b = sqrt(b_x*b_x + b_y*b_y);
				mag_r = ComponentClamp(mag_r);
				mag_g = ComponentClamp(mag_g);
				mag_b = ComponentClamp(mag_b);

				img->GetPixel(i,j) = Pixel(mag_r,mag_g,mag_b);
			}

		}
	}
	this->data.raw = img->data.raw;
	return;
}

Image* Image::Scale(double sx, double sy){
	/* WORK HERE */
	int new_w = (int) width*sx;
	int new_h = (int) height*sy;
	Image *img = new Image(new_w,new_h);
	int x_ratio = width / new_w;
	int y_ratio = height / new_h;

	SetSamplingMethod(0);
	for (int x = 0; x < new_w; x++) {
		for (int y = 0; y < new_h; y++) {
			double u = x*x_ratio;
			double v = y*y_ratio;
			Pixel p = Sample(u,v);
			img->SetPixel(x,y, p);
		}
	}
	return img;
}

Image* Image::Rotate(double angle){
	/* WORK HERE */
	int new_w = 0; // rotate four corners (0,0) (0,height-1) (width-1,0) (width-1,height-1)
	int new_h = 0;

	// double radians = (angle * M_PI) / 180;  // maybe replace angle w/

	int one_x = (int) (0*cos(angle) - 0*sin(angle));
	int one_y = (int) (0*cos(angle) + 0*sin(angle));

	int two_x = (int) (0*cos(angle) - (height-1)*sin(angle));
	int two_y = (int) ((height-1)*cos(angle) + 0*sin(angle));

	int three_x = (int) ((width-1)*cos(angle) - 0*sin(angle));
	int three_y = (int) (0*cos(angle) + (width-1)*sin(angle));

	int four_x = (int) ((width-1)*cos(angle) - (height-1)*sin(angle));
	int four_y = (int) ((height-1)*cos(angle) + (width-1)*sin(angle));

	int minx = min(min(one_x,two_x), min(three_x,four_x));
	int maxx = max(max(one_x,two_x), max(three_x,four_x));
	int miny = min(min(one_y,two_y), min(three_y,four_y));
	int maxy = max(max(one_y,two_y), max(three_y,four_y));

	printf("min x: %d\n",minx);
	printf("max x: %d\n",maxx);
	printf("min y: %d\n",miny);
	printf("max y: %d\n",maxy);

	printf("P1 (%d,%d)\n",one_x,one_y);
	printf("P2 (%d,%d)\n",two_x,two_y);
	printf("P3 (%d,%d)\n",three_x,three_y);
	printf("P4 (%d,%d)\n",four_x,four_y);

	new_w = maxx - minx;
	new_h = maxy - miny;

	Image *img = new Image(new_w,new_h);
	double x,y;
	SetSamplingMethod(0);
	for (int i = 0; i < new_w; i++) {
		for (int j = 0; j < new_h; j++) {
			x = i*cos(angle) - j*sin(angle);
			y = i*sin(angle) + j*cos(angle);
			img->Sample(x,y) = GetPixel(i,j);
			// img->SetPixel(i,j,Sample(x,y));
		} // ^ (x,y) of the new image = GetPixel(i,j) ??
	}
	return img;
}

void Image::Fun(){
	/* WORK HERE */
}

/**
 * Image Sample
 **/
void Image::SetSamplingMethod(int method){
   assert((method >= 0) && (method < IMAGE_N_SAMPLING_METHODS));
   sampling_method = method;
}


Pixel Image::Sample (double u, double v){
   /* WORK HERE */
   if (sampling_method == IMAGE_SAMPLING_POINT) {
	   if ((ValidCoord((int)u*width, (int)v*height)) == 0) {
		   printf("\npoint (%d,%d), width=%d  height=%d\n\n",((int)u*width), ((int)v*height), width, height);
	   }
	   return GetPixel((int)u*width, (int)v*height);
   } else if (sampling_method == IMAGE_SAMPLING_BILINEAR) {
	   // Get 4 nearest pixels
	   // return the bilinear average
	   
   } else if (sampling_method == IMAGE_SAMPLING_GAUSSIAN) {
	   int x = (int) u;
	   int y = (int) v;
	   double r,g,b = 0.0;
	   double sum = 0.0;

	   for (int i = -1; i <= 1; i++) {
		   for (int j = -1; j <= 1; j++) {
				if ((x+i < 0) || (x+i+1 >= Width()) || (y+j < 0) || (y+j+1 >= Height())) {
					r += 0;
					g += 0;
					b += 0;
				} else {
					if ((ValidCoord(x+i,y+j)) == 0) {
						printf("\npoint (%d,%d), width=%d  height=%d\n\n",x+i,y+j,width,height);
					}
					Pixel cur = GetPixel(x+i, y+j);
					float weight = (1/sqrt(2*M_PI))*exp(i/2);
			   		r += weight * cur.r;
			   		g += weight * cur.g;
			   		b += weight * cur.b;
				}
		   }
	   }

	   r /= sum;
	   g /= sum;
	   b /= sum;

	   r = ComponentClamp(r);
	   g = ComponentClamp(g);
	   b = ComponentClamp(b);

	   return Pixel(r,g,b);
   }

   return Pixel();
}