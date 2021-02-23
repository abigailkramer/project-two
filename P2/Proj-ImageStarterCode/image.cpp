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

			// double n = (((double)rand()*(0.5 - (-0.5)) / RAND_MAX) -0.5);

			noise = noise + PixelRandom()*factor;

			// noise.r = noise.r + ComponentRandom()*factor;
			// noise.g = noise.g + ComponentRandom()*factor;
			// noise.b = noise.b + ComponentRandom()*factor;
			
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


void Image::OrderedDither(int nbits){
	/* WORK HERE */
	for (int i = 0; i < Width(); i++) {
		for (int j = 0; j < Height(); j++) {
			int x = i % 4;
			int y = j % 4;
			Pixel p = GetPixel(i,j);
			int shift = 8-nbits;
			float mult = 255/float(255 >> shift);
			int new_r, new_g, new_b;
			new_r = ((p.r >> shift) + Bayer4[x][y]-0.5 / 15.0);
			new_g = ((p.g >> shift) + Bayer4[x][y]-0.5 / 15.0);
			new_b = ((p.b >> shift) + Bayer4[x][y]-0.5 / 15.0);

			Pixel ret;
			ret.SetClamp(new_r*mult , new_g*mult , new_b*mult );
			GetPixel(i,j) = ret;
		}
	}

	return;

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
						float weight = exp(-(x*x + y*y) / n);
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
			Pixel p = GetPixel(i,j);
			Pixel q = img->GetPixel(i,j);
			Pixel sharp = PixelLerp(q,p,n);
			GetPixel(i,j) = sharp;
		}
	}

	img->~Image();
	return;
}


void Image::EdgeDetect(){
	/* WORK HERE */

	Image *img = new Image(*this);
	int Gx[3][3] = {{1,0,-1}, {2,0,-2}, {1,0,-1}};
	int Gy[3][3] = {{1,2,1}, {0,0,0}, {-1,-2,-1}};

	int max = 20;
	float mag_x, mag_y;

	for (int i = 0; i < Width(); i++) {
		for (int j = 0; j < Height(); j++) {
			mag_x=mag_y=0;

			if (!(i == 0) && !(i+1 == Width()) && !(j == 0) && !(j+1 == Height())) {
				for (int x = -1; x <= 1; x++) {
					for (int y = -1; y <= 1; y++) {

						Pixel p = GetPixel(x+i,y+j);
						double r = p.r;
						double g = p.g;
						double b = p.b;

						double intensity = r+g+b;
						mag_x += intensity*Gx[x+1][y+1];
						mag_y += intensity*Gy[x+1][y+1];
					}
				}				
			}

			float mag = sqrt(mag_x*mag_x + mag_y*mag_y);
			(mag > max) ? 255 : 0;
			img->GetPixel(i,j) = Pixel(mag,mag,mag);
		}
	}
	this->data.raw = img->data.raw;
	// img->~Image();
	return;
}

Image* Image::Scale(double sx, double sy){
	/* WORK HERE */
	int new_w = Width()*sx;
	int new_h = Height()*sy;
	Image *img = new Image(new_w,new_h);
	double r = max(1/sx,1/sy);

	for (int x = 0; x < new_w; x++) {
		for (int y = 0; y < new_h; y++) {

			// double u = ((double)x / sx);
			// double v = ((double)y / sy);
			// Pixel p = GetPixel((int)u,(int)v);

			double u = ((double)x * sx) / (double)width;
			double v = ((double)x * sy) / (double)height;
			Pixel p = Sample(u,v);	// return GetPixel((int)u*width, (int)v*height);


			img->GetPixel(x,y) = p;
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

	for (int i = 0; i < Width(); i++) {
		for (int j = 0; j < Height(); j++) {
			x = i*cos(angle) + j*sin(angle);
			y = j*cos(angle) - i*sin(angle);
			img->Sample(x,y) = GetPixel(i,j);
			// Pixel p = img->Sample(x,y);
		}
	}

	// for (int i = 0; i < new_w; i++) {
	// 	for (int j = 0; j < new_h; j++) {
	// 		x = i*cos(-angle) - j*sin(-angle);	// negative angle?
	// 		y = i*sin(-angle) + j*cos(-angle);
	// 		img->GetPixel(i,j) = GetPixel((int)x,(int)y);
	// 		// img->GetPixel(i,j) = Sample(x,y);
	// 		// img->SetPixel(i,j,Sample(x,y));
	// 	} // ^ (x,y) of the new image = GetPixel(i,j) ??
	// }

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
	   int x = (int) u;
	   int y = (int) v;
		Pixel p1,p2,p3,p4;

	   Pixel p1 = GetPixel(x,y);

	   if (x+1 < Width()) {
		   Pixel p2 = GetPixel(x+1, y);
	   } else {
		   Pixel p2 = GetPixel(x-1, y);
	   }

	   if (y+1 < Height()) {
		   Pixel p3 = GetPixel(x, y+1);
	   } else {
		   Pixel p3 = GetPixel(x, y-1);
	   }

	   if ((x+1 < Width()) && (y+1 < Height())) {
	   	   Pixel p4 = GetPixel(x+1, y+1);
	   } else {
		   Pixel p4 = GetPixel(x-1,y-1);
	   }

	   double bi_x = u - x;
	   double bi_y = v - y;

	   Pixel p = PixelLerp(PixelLerp(p1,p2,bi_x), PixelLerp(p3,p4,bi_x),bi_y);
	   return p;
	   
	   // Get 4 nearest pixels
	   // return the bilinear average
	   
   } else if (sampling_method == IMAGE_SAMPLING_GAUSSIAN) {
	   int x = (int) u;
	   int y = (int) v;
	   double r,g,b = 0.0;
	   double sum = 0.0;

	   for (int i = -1; i <= 1; i++) {
		   for (int j = -1; j <= 1; j++) {
				if ((x+i >= 0) || (x+i+1 < Width()) || (y+j >= 0) || (y+j+1 < Height())) {
					Pixel cur = GetPixel(x+i, y+j);
					float weight = exp(-(x*x + y*y) / 2);
			   		r += weight * cur.r;
			   		g += weight * cur.g;
			   		b += weight * cur.b;

					sum += weight;
				}
		   }
	   }

	   r /= sum;
	   g /= sum;
	   b /= sum;

	   Pixel n = ((int)r, (int)g, (int)b);

	   n.r = ComponentClamp(r);
	   n.g = ComponentClamp(g);
	   n.b = ComponentClamp(b);

	   return n;
   }

   return Pixel();
}