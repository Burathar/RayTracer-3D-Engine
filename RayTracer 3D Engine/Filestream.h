#ifndef _FILESTREAM_H
#define _FILESTREAM_H

#include <string.h>
#include <stdio.h>

struct rgb_type {
	double r;
	double g;
	double b;
};

class filestream {
public:

	filestream() {};

	void set_header_filesize(const int s, unsigned char bmpfileheader[14])
	{
		const int filesize = 54 + s;
		bmpfileheader[2] = static_cast<unsigned char>(filesize);
		bmpfileheader[3] = static_cast<unsigned char>(filesize >> 8);
		bmpfileheader[4] = static_cast<unsigned char>(filesize >> 16);
		bmpfileheader[5] = static_cast<unsigned char>(filesize >> 24);
	}

	void set_header_resolution(int w, int h, unsigned char bmpinfoheader[40])
	{
		bmpinfoheader[4] = static_cast<unsigned char>(w);
		bmpinfoheader[5] = static_cast<unsigned char>(w >> 8);
		bmpinfoheader[6] = static_cast<unsigned char>(w >> 16);
		bmpinfoheader[7] = static_cast<unsigned char>(w >> 24);

		bmpinfoheader[8] = static_cast<unsigned char>(h);
		bmpinfoheader[9] = static_cast<unsigned char>(h >> 8);
		bmpinfoheader[10] = static_cast<unsigned char>(h >> 16);
		bmpinfoheader[11] = static_cast<unsigned char>(h >> 24);
	}

	void set_header_size(const int s, unsigned char bmpinfoheader[40])
	{
		bmpinfoheader[21] = static_cast<unsigned char>(s);
		bmpinfoheader[22] = static_cast<unsigned char>(s >> 8);
		bmpinfoheader[23] = static_cast<unsigned char>(s >> 16);
		bmpinfoheader[24] = static_cast<unsigned char>(s >> 24);
	}

	void set_header_ppm(int dpi, unsigned char bmpinfoheader[40])
	{
		const double factor = 39.375;
		const int m = static_cast<int>(factor);

		const int ppm = dpi*m;
		bmpinfoheader[25] = static_cast<unsigned char>(ppm);
		bmpinfoheader[26] = static_cast<unsigned char>(ppm >> 8);
		bmpinfoheader[27] = static_cast<unsigned char>(ppm >> 16);
		bmpinfoheader[28] = static_cast<unsigned char>(ppm >> 24);

		bmpinfoheader[29] = static_cast<unsigned char>(ppm);
		bmpinfoheader[30] = static_cast<unsigned char>(ppm >> 8);
		bmpinfoheader[31] = static_cast<unsigned char>(ppm >> 16);
		bmpinfoheader[32] = static_cast<unsigned char>(ppm >> 24);
	}

	bool open_file(const char* filename, FILE* file, errno_t err)
	{
		if ((err = fopen_s(&file, filename, "wb")) != 0)
		{
			char buffer[256];
			strerror_s(buffer, sizeof buffer, err);
			fprintf_s(stderr, "cannot open file '%s': %s\n",
				filename, buffer);
			return false;
		}
		return true;
	}

	void write_pixels(rgb_type* data, const int k, FILE* file)
	{
		for (int i = 0; i < k; i++) {
			const double red = (data[i].r) * 255;
			const double green = (data[i].g) * 255;
			const double blue = (data[i].b) * 255;

			unsigned char color[3] = { static_cast<unsigned char>(static_cast<int>(floor(blue))),
				static_cast<unsigned char>(static_cast<int>(floor(green))),
				static_cast<unsigned char>(static_cast<int>(floor(red)))
			};

			fwrite(color, 1, 3, file);
		}
	}

	bool write_data(const char* filename, rgb_type* data, const int k, unsigned char bmpfileheader[14], unsigned char bmpinfoheader[40])
	{
		FILE* file = nullptr;
		errno_t err = NULL;
		bool value;
		if (!open_file(filename, file, err)) return false;

		fwrite(bmpfileheader, 1, 14, file);
		fwrite(bmpinfoheader, 1, 40, file);//file is NULL?

		write_pixels(data, k, file);

		fclose(file);
		return true;
	}

	bool savembp(const char *filename, int w, int h, int dpi, rgb_type *data) {
		const int k = w*h;
		const int s = 4 * k;

		unsigned char bmpfileheader[14] = { 'B','M', 0,0,0,0, 0,0,0,0, 54,0,0,0 };
		unsigned char bmpinfoheader[40] = { 40,0,0,0, 0,0,0,0, 0,0,0,0, 1,0,24,0 };

		set_header_filesize(s, bmpfileheader);
		set_header_resolution(w, h, bmpinfoheader);
		set_header_size(s, bmpinfoheader);
		set_header_ppm(dpi, bmpinfoheader);

		return write_data(filename, data, k, bmpfileheader, bmpinfoheader);
	}
};

#endif
