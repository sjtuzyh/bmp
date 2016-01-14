// Test Bitmap Plane Generator
// compile with --std=c++11
// License: You can use this code under either the MIT, ISC, or MPL 2.0 licenses.

#include <stdio.h>
#include <stdint.h>
#include <math.h>

#include <vector>
#include <string>

using namespace std;

uint8_t fix(float capme)
{
	capme *= 255;
    capme += 0.5;
    if(capme < 0)
        return 0;
    if(capme > 255)
        return 255;
    return (int8_t)capme;
}

uint8_t crop(float capme)
{
    capme += 0.5;
    if(capme < 0)
        return 0;
    if(capme > 255)
        return 255;
    return (int8_t)capme;
}

float tosrgb(float linear)
{
	if(linear > 0.04045)
		return pow((linear + 0.055)/1.055, 2.4);
	else
		return linear/12.92;
}
float tolinear(float srgb)
{
	if(srgb > 0.0031308)
		return 1.055*pow(srgb, 1/2.4) - 0.055;
	else
		return srgb*1292;
}

struct triad
{
	float r;
	float g;
	float b;
	
	triad()
	{
		r = 1.0;
		g = 1.0;
		b = 1.0;
	}
};

struct image
{
	unsigned short width;
	unsigned short height;
	vector<triad> data;
	
	bool linear;
	
	image()
	{
		width = 1;
		height = 1;
		data = {triad()};
		
		linear = false;
	}
	
	triad& operator()(unsigned short x, unsigned short y)
	{
		return data[x+width*y];
	}
	
	void dimensions(unsigned short arg_width, unsigned short arg_height)
	{
		width = arg_width;
		height = arg_height;
		data = vector<triad>();
		data.resize(width*height);
	}
	
	void makelinear()
	{
		if(linear) return;
		
		for(auto& t : data)
		{
			t.r = tolinear(t.r);
			t.g = tolinear(t.g);
			t.b = tolinear(t.b);
		}
		linear = true;
	}
	void makesrgb()
	{
		if(!linear) return;
		
		for(auto& t : data)
		{
			t.r = tosrgb(t.r);
			t.g = tosrgb(t.g);
			t.b = tosrgb(t.b);
		}
		linear = false;
	}
	void applygamma(float g)
	{
		for(auto& t : data)
		{
			t.r = pow(t.r, g);
			t.g = pow(t.g, g);
			t.b = pow(t.b, g);
		}
	}
	
	void writefile(const char * filename)
	{
		string temp(filename);
		if(temp.substr(temp.length()-4) != ".ppm")
		{
			puts("fixing filename");
			temp += ".ppm";
			filename = temp.data();
		}
		
		FILE* file = fopen(filename, "wb");
		printf("writing file %s\n", filename);
		if(file != NULL)
		{
			bool waslinear = linear;
			makesrgb();
			
			printf("w h : %d %d\n", width, height);
			fprintf(file, "P6 %d %d 255\n", width, height);
			for(auto& t : data)
			{
				fputc(fix(t.r), file);
				fputc(fix(t.g), file);
				fputc(fix(t.b), file);
			}
			
			if(waslinear) makelinear();
		
			fclose(file);
		}
		else
			puts("Error opening file.");
	}
};

void makeYCgCoPlane(image& myimage, float luma)
{
	myimage.dimensions(1020,1020);
    for(int x = 0; x < 1020; x++)
    for(int y = 0; y < 1020; y++)
    {
        float cy = luma*1020;
        float cg = x-510;
        float co = y-510;
        
        float temp = cy - (cg);
        float r = (temp + co)/4;
        float g = (cy + cg)/4;
        float b = (temp - co)/4;
        
        if(r > 255 or g > 255 or b > 255 or
           r < 0 or g < 0 or b < 0)
        {
            int out = -1;
            if(r > 255 or g > 255 or b > 255)
                out = 255;
            if(r < 0 or g < 0 or b < 0)
                out += 1;
            if(out == 256)
                out = 128;
            myimage(x, y).r =
            myimage(x, y).g =
            myimage(x, y).b = out/255.0;
        }
        else
        {
            myimage(x, y).r = crop(r)/255.0;
            myimage(x, y).g = crop(g)/255.0;
            myimage(x, y).b = crop(b)/255.0;
        }
    }
}

int main() 
{
	image myimage;
	myimage.dimensions(256,256);
	for(auto x = 0; x < myimage.width; x++)
	{
		for(auto y = 0; y < myimage.height; y++)
		{
			myimage(x, y).r = x/255.0;
			myimage(x, y).g = y/255.0;
			myimage(x, y).b = (255-max(x,y))/255.0;
		}
	}
	myimage.writefile("srgb std test.ppm");
	myimage.makelinear();
	myimage.linear = false;
	myimage.writefile("linear std test.ppm");
	myimage.applygamma(3.0);
	myimage.writefile("visual std test.ppm");
	
	
	
	for(int c = 1; c < 8; c++)
	{
		makeYCgCoPlane(myimage, c/8.0);
		
		std::string filename;
		filename = "YCgCo " + to_string(c) + "／8 Y, srgb.ppm";
		myimage.writefile(filename.data());
		myimage.makelinear();
		myimage.linear = false;
		filename = "YCgCo " + to_string(c) + "／8 Y, linear.ppm";
		myimage.writefile(filename.data());
		myimage.applygamma(3.0);
		filename = "YCgCo " + to_string(c) + "／8 Y, visual.ppm";
		myimage.writefile(filename.data());
	}
}
