#include <string>
#include <iostream>
#include <fstream>
#include <algorithm>
#include "GL/glew.h"
#include "../extra/picopng.h"
#include "image.h"
#include "utils.h"
#include "camera.h"
#include "mesh.h"

Image::Image() {
	width = 0; height = 0;
	pixels = NULL;
}

Image::Image(unsigned int width, unsigned int height)
{
	this->width = width;
	this->height = height;
	pixels = new Color[width*height];
	memset(pixels, 0, width * height * sizeof(Color));
}

// Copy constructor
Image::Image(const Image& c)
{
	pixels = NULL;
	width = c.width;
	height = c.height;
	bytes_per_pixel = c.bytes_per_pixel;
	if(c.pixels)
	{
		pixels = new Color[width*height];
		memcpy(pixels, c.pixels, width*height*bytes_per_pixel);
	}
}

// Assign operator
Image& Image::operator = (const Image& c)
{
	if(pixels) delete[] pixels;
	pixels = NULL;

	width = c.width;
	height = c.height;
	bytes_per_pixel = c.bytes_per_pixel;

	if(c.pixels)
	{
		pixels = new Color[width*height*bytes_per_pixel];
		memcpy(pixels, c.pixels, width*height*bytes_per_pixel);
	}
	return *this;
}

Image::~Image()
{
	if(pixels) 
		delete[] pixels;
}

void Image::Render()
{
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glDrawPixels(width, height, bytes_per_pixel == 3 ? GL_RGB : GL_RGBA, GL_UNSIGNED_BYTE, pixels);
}

// Change image size (the old one will remain in the top-left corner)
void Image::Resize(unsigned int width, unsigned int height)
{
	Color* new_pixels = new Color[width*height];
	unsigned int min_width = this->width > width ? width : this->width;
	unsigned int min_height = this->height > height ? height : this->height;

	for(unsigned int x = 0; x < min_width; ++x)
		for(unsigned int y = 0; y < min_height; ++y)
			new_pixels[ y * width + x ] = GetPixel(x,y);

	delete[] pixels;
	this->width = width;
	this->height = height;
	pixels = new_pixels;
}

// Change image size and scale the content
void Image::Scale(unsigned int width, unsigned int height)
{
	Color* new_pixels = new Color[width*height];

	for(unsigned int x = 0; x < width; ++x)
		for(unsigned int y = 0; y < height; ++y)
			new_pixels[ y * width + x ] = GetPixel((unsigned int)(this->width * (x / (float)width)), (unsigned int)(this->height * (y / (float)height)) );

	delete[] pixels;
	this->width = width;
	this->height = height;
	pixels = new_pixels;
}

Image Image::GetArea(unsigned int start_x, unsigned int start_y, unsigned int width, unsigned int height)
{
	Image result(width, height);
	for(unsigned int x = 0; x < width; ++x)
		for(unsigned int y = 0; y < height; ++y)
		{
			if( (x + start_x) < this->width && (y + start_y) < this->height) 
				result.SetPixelUnsafe( x, y, GetPixel(x + start_x,y + start_y) );
		}
	return result;
}

void Image::FlipY()
{
	int row_size = bytes_per_pixel * width;
	Uint8* temp_row = new Uint8[row_size];
#pragma omp simd
	for (int y = 0; y < height * 0.5; y += 1)
	{
		Uint8* pos = (Uint8*)pixels + y * row_size;
		memcpy(temp_row, pos, row_size);
		Uint8* pos2 = (Uint8*)pixels + (height - y - 1) * row_size;
		memcpy(pos, pos2, row_size);
		memcpy(pos2, temp_row, row_size);
	}
	delete[] temp_row;
}

bool Image::LoadPNG(const char* filename, bool flip_y)
{
	std::string sfullPath = absResPath(filename);
	std::ifstream file(sfullPath, std::ios::in | std::ios::binary | std::ios::ate);

	// Get filesize
	std::streamsize size = 0;
	if (file.seekg(0, std::ios::end).good()) size = file.tellg();
	if (file.seekg(0, std::ios::beg).good()) size -= file.tellg();

	if (!size){
		std::cerr << "--- Failed to load file: " << sfullPath.c_str() << std::endl;
		return false;
	}

	std::vector<unsigned char> buffer;

	// Read contents of the file into the vector
	if (size > 0)
	{
		buffer.resize((size_t)size);
		file.read((char*)(&buffer[0]), size);
	}
	else
		buffer.clear();

	std::vector<unsigned char> out_image;

	if (decodePNG(out_image, width, height, buffer.empty() ? 0 : &buffer[0], (unsigned long)buffer.size(), true) != 0){
		std::cerr << "--- Failed to load file: " << sfullPath.c_str() << std::endl;
		return false;
	}

	size_t bufferSize = out_image.size();
	unsigned int originalBytesPerPixel = (unsigned int)bufferSize / (width * height);
	
	// Force 3 channels
	bytes_per_pixel = 3;

	if (originalBytesPerPixel == 3) {
		if (pixels) delete[] pixels;
		pixels = new Color[bufferSize];
		memcpy(pixels, &out_image[0], bufferSize);
	}
	else if (originalBytesPerPixel == 4) {
		if (pixels) delete[] pixels;

		unsigned int newBufferSize = width * height * bytes_per_pixel;
		pixels = new Color[newBufferSize];

		unsigned int k = 0;
		for (unsigned int i = 0; i < bufferSize; i += originalBytesPerPixel) {
			pixels[k] = Color(out_image[i], out_image[i + 1], out_image[i + 2]);
			k++;
		}
	}

	// Flip pixels in Y
	if (flip_y)
		FlipY();

	std::cout << "+++ File loaded: " << sfullPath.c_str() << std::endl;

	return true;
}

// Loads an image from a TGA file
bool Image::LoadTGA(const char* filename, bool flip_y)
{
	unsigned char TGAheader[12] = {0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0};
	unsigned char TGAcompare[12];
	unsigned char header[6];
	unsigned int imageSize;
	unsigned int bytesPerPixel;

    std::string sfullPath = absResPath( filename );

	FILE * file = fopen( sfullPath.c_str(), "rb");
   	if ( file == NULL || fread(TGAcompare, 1, sizeof(TGAcompare), file) != sizeof(TGAcompare) ||
		memcmp(TGAheader, TGAcompare, sizeof(TGAheader)) != 0 ||
		fread(header, 1, sizeof(header), file) != sizeof(header))
	{
		std::cerr << "--- File not found: " << sfullPath.c_str() << std::endl;
		if (file == NULL)
			return NULL;
		else
		{
			fclose(file);
			return NULL;
		}
	}

	TGAInfo* tgainfo = new TGAInfo;
    
	tgainfo->width = header[1] * 256 + header[0];
	tgainfo->height = header[3] * 256 + header[2];
    
	if (tgainfo->width <= 0 || tgainfo->height <= 0 || (header[4] != 24 && header[4] != 32))
	{
		std::cerr << "--- Failed to load file: " << sfullPath.c_str() << std::endl;
		fclose(file);
		delete tgainfo;
		return NULL;
	}
    
	tgainfo->bpp = header[4];
	bytesPerPixel = tgainfo->bpp / 8;
	imageSize = tgainfo->width * tgainfo->height * bytesPerPixel;
    
	tgainfo->data = new unsigned char[imageSize];
    
	if (tgainfo->data == NULL || fread(tgainfo->data, 1, imageSize, file) != imageSize)
	{
		std::cerr << "--- Failed to load file: " << sfullPath.c_str() << std::endl;

		if (tgainfo->data != NULL)
			delete[] tgainfo->data;
            
		fclose(file);
		delete tgainfo;
		return false;
	}

	fclose(file);

	// Save info in image
	if(pixels)
		delete[] pixels;

	width = tgainfo->width;
	height = tgainfo->height;
	pixels = new Color[width*height];

	// Convert to float all pixels
	for (unsigned int y = 0; y < height; ++y) {
		for (unsigned int x = 0; x < width; ++x) {
			unsigned int pos = y * width * bytesPerPixel + x * bytesPerPixel;
			// Make sure we don't access out of memory
			if( (pos < imageSize) && (pos + 1 < imageSize) && (pos + 2 < imageSize))
				SetPixelUnsafe(x, height - y - 1, Color(tgainfo->data[pos + 2], tgainfo->data[pos + 1], tgainfo->data[pos]));
		}
	}

	// Flip pixels in Y
	if (flip_y)
		FlipY();

	delete[] tgainfo->data;
	delete tgainfo;

	if (strcmp(filename, "clear.tga") != 0) {
		std::cout << "+++ File loaded: " << sfullPath.c_str() << std::endl;
		std::cout << filename << std::endl;
	}

	return true;
}

// Saves the image to a TGA file
bool Image::SaveTGA(const char* filename)
{
	unsigned char TGAheader[12] = {0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0};

	std::string fullPath = absResPath(filename);
	FILE *file = fopen(fullPath.c_str(), "wb");
	if ( file == NULL )
	{
		std::cerr << "--- Failed to save file: " << fullPath.c_str() << std::endl;
		return false;
	}

	unsigned short header_short[3];
	header_short[0] = width;
	header_short[1] = height;
	unsigned char* header = (unsigned char*)header_short;
	header[4] = 24;
	header[5] = 0;

	fwrite(TGAheader, 1, sizeof(TGAheader), file);
	fwrite(header, 1, 6, file);

	// Convert pixels to unsigned char
    /*
	unsigned char* bytes = new unsigned char[width*height*3];
	for(unsigned int y = 0; y < height; ++y)
    */
	// don't save the menu bar
    unsigned char* bytes = new unsigned char[width * height * 3];
    for (unsigned int y = 0; y < 50; ++y) {
        for (unsigned int x = 0; x < width; ++x) {
            unsigned int pos = (y * width + x) * 3;
            bytes[pos + 2] = 0;
            bytes[pos + 1] = 0;
            bytes[pos] = 0;
        }
    }
    for(unsigned int y = 50; y < height; ++y)
		for(unsigned int x = 0; x < width; ++x)
		{
			Color c = pixels[y*width+x];
			unsigned int pos = (y*width+x)*3;
			bytes[pos+2] = c.r;
			bytes[pos+1] = c.g;
			bytes[pos] = c.b;
		}

	fwrite(bytes, 1, width*height*3, file);
	fclose(file);

	delete[] bytes;

	if (strcmp(filename, "clear.tga") != 0) {
		std::cout << "+++ File saved: " << fullPath.c_str() << std::endl;
	}

	return true;
}

// OUR FUNCTIONS

void Image::DrawLineDDA(int x0, int y0, int x1, int y1, const Color& c) {
    float dx = x1-x0;
    float dy = y1-y0;
    int d = std::max(abs(dx), abs(dy));
    
    Vector2 v0(x0, y0);
    Vector2 v(dx/d, dy/d);
    
    for(int i=0; i<d; i++) {
        for(int j=-defBorderWidth; j<=defBorderWidth;j++){
            for(int k=-defBorderWidth; k<=defBorderWidth;k++) {
                SetPixel(v0.x+j, v0.y+k, c);
            }
        }
        v0 += v;
    }
}

// given a position determine if it has to be filled or not
bool Image::toBeFilled(int i, int j, int x, int y, int w, int h, int borderWidth) {
    if((x+borderWidth <= i) && (y+borderWidth <= j) && (i < x+w-borderWidth ) && (j < y+h-borderWidth)) {
        return true;
    }
    return false;
}

void Image::DrawRect(int x, int y, int w, int h, const Color& borderColor, int borderWidth, bool isFilled, const Color& fillColor) {
    if((w <= 2*borderWidth) || (h <= 2*borderWidth)) {    // incorrect passed parameters
        borderWidth = std::min(w/2, h/2) - 1;
    }
    for(int i=x; i<x+w; i++) {
        for(int j=y; j<y+h; j++) {
            if(toBeFilled(i, j, x, y, w, h, borderWidth)){
                if(isFilled) {
                    SetPixel(i, j, fillColor);
                }
            } else {
                SetPixel(i, j, borderColor);
            }
        }
    }
}

// fill AET table
void Image::ScanLineDDA(int x0, int y0, int x1, int y1, std::vector<Cell>& table) {
    float dx = x1-x0;
    float dy = y1-y0;
    int d = std::max(abs(dx), abs(dy));
    
    Vector2 v0(x0, y0);
    Vector2 v(dx/d, dy/d);
    
    for(int i=0; i<=d; i++) {
        if(table[v0.y].minx == -1) {
            table[v0.y].minx = v0.x;
            table[v0.y].maxx = v0.x;
        } else if(table[v0.y].minx > v0.x) {
            table[v0.y].minx = v0.x;
        } else if(table[v0.y].maxx < v0.x) {
            table[v0.y].maxx = v0.x;
        }
        v0 += v;
    }
}

void Image::DrawTriangle(const Vector2& p0, const Vector2& p1, const Vector2& p2, const Color& borderColor, bool isFilled, const Color& fillColor) {
    // Avoiding out-of-bounds positions when resizing window
    Vector2* in0 = makeInside(p0);
    Vector2* in1 = makeInside(p1);
    Vector2* in2 = makeInside(p2);
    const Vector2* use0;
    const Vector2* use1;
    const Vector2* use2;
    
    if(in0 == NULL) {
        use0 = &p0;
    } else {
        use0 = in0;
    }
    if(in1 == NULL) {
        use1 = &p1;
    } else {
        use1 = in1;
    }
    if(in2 == NULL) {
        use2 = &p2;
    } else {
        use2 = in2;
    }
    
    std::vector<Cell> table;
    int tableSize = height;
    table.resize(tableSize);
    
    ScanLineDDA(use0->x, use0->y, use1->x, use1->y, table);
    ScanLineDDA(use0->x, use0->y, use2->x, use2->y, table);
    ScanLineDDA(use1->x, use1->y, use2->x, use2->y, table);
    
    if(isFilled) {
        for(int i=0; i<tableSize; i++) {
            if(table[i].minx != -1) {
                for(int j=table[i].minx; j<=table[i].maxx; j++) {
                    if((0 < j) && (j < width) && (0 < i) && (i < height)) {
                        SetPixel(j, i, fillColor);
                    }
                }
            }
        }
    }
    
    // Drawing borders of Triangle
    DrawLineDDA(use0->x, use0->y, use1->x, use1->y, borderColor);
    DrawLineDDA(use0->x, use0->y, use2->x, use2->y, borderColor);
    DrawLineDDA(use1->x, use1->y, use2->x, use2->y, borderColor);
}

void Image::DrawImage(const Image& image, int x, int y) {
    for(int i=0; i<image.width; i++) {
        for(int j=0; j<image.height; j++) {
            SetPixel(x+i, y+j, image.GetPixel(i, j));
        }
    }
}

// make a point inside the image, if it is already inside return NULL
Vector2* Image::makeInside(const Vector2 &p) {
    Vector2 *in = new Vector2(p.x, p.y);
    bool modified = false;
    if(p.x < 0) {
        in->x = 0;
        modified = true;
    }
    if(p.x > width) {
        in->x = width-1;
        modified = true;
    }
    if(p.y < 0) {
        in->y = 0;
        modified = true;
    }
    if(p.y > height) {
        in->y = height-1;
        modified = true;
    }
    if(!modified){
        delete in;
        return NULL;
    } else {
        return in;
    }
}

// given two diagonal vertices of rectangle, compute (x, y, width, height)
int* Image::CompRect(Vector2 v1, Vector2 v2) {
    int* data = new int[4];
    Vector2 v3(v1.x, v2.y);
    Vector2 v4(v2.x, v1.y);
    Vector2 vs[4] = {v1, v2, v3, v4};
    Vector2 temp;
    
    // ordering increasing vertices (x,y values)
    for(int i=0; i<4; i++) {
        for(int j=i+1; j<4; j++) {
            if((vs[j].x<=vs[i].x)&&(vs[j].y<=vs[i].y)) {
                temp = vs[i];
                vs[i] = vs[j];
                vs[j] = temp;
            }
        }
    }
    data[0] = vs[0].x;
    data[1] = vs[0].y;
    data[2] = vs[3].x-vs[0].x;
    data[3] = vs[3].y-vs[0].y;
    // x, y, width, height
    return data;
}

Vector2 Image::GetScreenCoordinates(Vector3 v) {
    // Convert from clip space (-1 to 1) to screen space (0 to width/height)
    float x = (v.x * 0.5f + 0.5f) * width;
    float y = (v.y * 0.5f + 0.5f) * height;
    return Vector2(x, y);
}

bool Image::SetZInterpolated(const Vector2& p, const Vector3& p0, const Vector3& p1, const Vector3& p2, FloatImage* zbuffer) {
    Vector2 p0_2 = Vector2(p0.x, p0.y);
    Vector2 p1_2 = Vector2(p1.x, p1.y);
    Vector2 p2_2 = Vector2(p2.x, p2.y);
    
    Vector2 pp0 = p0_2 - p;
    Vector2 pp1 = p1_2 - p;
    Vector2 pp2 = p2_2 - p;
    Vector2 p0p1 = p1_2 - p0_2;
    Vector2 p0p2 = p2_2 - p0_2;
    
    Vector3 pp0_3 = Vector3(pp0.x, pp0.y, 0);
    Vector3 pp1_3 = Vector3(pp1.x, pp1.y, 0);
    Vector3 pp2_3 = Vector3(pp2.x, pp2.y, 0);
    Vector3 p0p1_3 = Vector3(p0p1.x, p0p1.y, 0);
    Vector3 p0p2_3 = Vector3(p0p2.x, p0p2.y, 0);
    
    float a0 = ((pp1_3.Cross(pp2_3)).Length())/2;
    float a1 = ((pp2_3.Cross(pp0_3)).Length())/2;
    float a2 = ((pp0_3.Cross(pp1_3)).Length())/2;
    float a012 = ((p0p1_3.Cross(p0p2_3)).Length())/2;
    
    float alpha = a0/a012;
    float beta = a1/a012;
    float gamma = a2/a012;
    float sum = alpha + beta + gamma;
    
    alpha /= sum;
    beta /= sum;
    gamma /= sum;
    
    float pz = (alpha*p0.z) + (beta*p1.z) + (gamma*p2.z);
    
    if(zbuffer->GetPixel(p.x, p.y) > pz) {
        zbuffer->SetPixel(p.x, p.y, pz);
        return true;
    } else {
        return false;
    }
}

void Image::SetPixelInterpolated(const Vector2& p, const Vector2& p0, const Vector2& p1, const Vector2& p2, const Color& c0, const Color& c1, const Color& c2) {
    Vector2 pp0 = p0 - p;
    Vector2 pp1 = p1 - p;
    Vector2 pp2 = p2 - p;
    Vector2 p0p1 = p1 - p0;
    Vector2 p0p2 = p2 - p0;
    
    Vector3 pp0_3 = Vector3(pp0.x, pp0.y, 0);
    Vector3 pp1_3 = Vector3(pp1.x, pp1.y, 0);
    Vector3 pp2_3 = Vector3(pp2.x, pp2.y, 0);
    Vector3 p0p1_3 = Vector3(p0p1.x, p0p1.y, 0);
    Vector3 p0p2_3 = Vector3(p0p2.x, p0p2.y, 0);
    
    float a0 = ((pp1_3.Cross(pp2_3)).Length())/2;
    float a1 = ((pp2_3.Cross(pp0_3)).Length())/2;
    float a2 = ((pp0_3.Cross(pp1_3)).Length())/2;
    float a012 = ((p0p1_3.Cross(p0p2_3)).Length())/2;
    
    float alpha = a0/a012;
    float beta = a1/a012;
    float gamma = a2/a012;
    float sum = alpha + beta + gamma;
    
    alpha /= sum;
    beta /= sum;
    gamma /= sum;
    
    Color c = (alpha*c0) + (beta*c1) + (gamma*c2);
    //Vector2 p_screen = GetScreenCoordinates(Vector3(p.x, p.y, 0));
    SetPixel(p.x, p.y, c);
}

void Image::DrawTriangleInterpolated(const Vector3& p0, const Vector3& p1, const Vector3& p2, const Color& c0, const Color& c1, const Color& c2, FloatImage* zbuffer) {
    // ...
    Vector2 use0 = GetScreenCoordinates(p0);
    Vector2 use1 = GetScreenCoordinates(p1);
    Vector2 use2 = GetScreenCoordinates(p2);
    /*
    Vector3 z0 = Vector3(use0.x, use0.y, p0.z);
    Vector3 z1 = Vector3(use1.x, use1.y, p1.z);
    Vector3 z2 = Vector3(use2.x, use2.y, p2.z);
    */
    std::vector<Cell> table;
    int tableSize = height;
    table.resize(tableSize);
    
    ScanLineDDA(use0.x, use0.y, use1.x, use1.y, table);
    ScanLineDDA(use0.x, use0.y, use2.x, use2.y, table);
    ScanLineDDA(use1.x, use1.y, use2.x, use2.y, table);
    
    for(int i=0; i<tableSize; i++) {
        if(table[i].minx != -1) {
            for(int j=table[i].minx; j<=table[i].maxx; j++) {
                if((0 < j) && (j < width) && (0 < i) && (i < height)) {
                    Vector2 use = Vector2(j, i);
                    if(SetZInterpolated(use, p0, p1, p2, zbuffer)) {
                        SetPixelInterpolated(use, use0, use1, use2, c0, c1, c2);
                    }
                    //SetPixel(j, i, fillColor);
                }
            }
        }
    }
}
 
#ifndef IGNORE_LAMBDAS

// You can apply and algorithm for two images and store the result in the first one
// ForEachPixel( img, img2, [](Color a, Color b) { return a + b; } );
template <typename F>
void ForEachPixel(Image& img, const Image& img2, F f) {
	for(unsigned int pos = 0; pos < img.width * img.height; ++pos)
		img.pixels[pos] = f( img.pixels[pos], img2.pixels[pos] );
}

#endif

FloatImage::FloatImage(unsigned int width, unsigned int height)
{
	this->width = width;
	this->height = height;
	pixels = new float[width * height];
	memset(pixels, 0, width * height * sizeof(float));
}

// Copy constructor
FloatImage::FloatImage(const FloatImage& c) {
	pixels = NULL;

	width = c.width;
	height = c.height;
	if (c.pixels)
	{
		pixels = new float[width * height];
		memcpy(pixels, c.pixels, width * height * sizeof(float));
	}
}

// Assign operator
FloatImage& FloatImage::operator = (const FloatImage& c)
{
	if (pixels) delete[] pixels;
	pixels = NULL;

	width = c.width;
	height = c.height;
	if (c.pixels)
	{
		pixels = new float[width * height * sizeof(float)];
		memcpy(pixels, c.pixels, width * height * sizeof(float));
	}
	return *this;
}

FloatImage::~FloatImage()
{
	if (pixels)
		delete[] pixels;
}

// Change image size (the old one will remain in the top-left corner)
void FloatImage::Resize(unsigned int width, unsigned int height)
{
	float* new_pixels = new float[width * height];
	unsigned int min_width = this->width > width ? width : this->width;
	unsigned int min_height = this->height > height ? height : this->height;

	for (unsigned int x = 0; x < min_width; ++x)
		for (unsigned int y = 0; y < min_height; ++y)
			new_pixels[y * width + x] = GetPixel(x, y);

	delete[] pixels;
	this->width = width;
	this->height = height;
	pixels = new_pixels;
}
