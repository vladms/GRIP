/*	The first two paramers are the image width & height
	These functions only work correctly for width, height powers of 2
	and >=4.
	Parameters 3 and 4 are the input real and imaginary parts arrays
	of width*height values. Acceptable value types are unsigned char (BYTE),
	float and double. The imaginary part is optional (T*)0 can be provided instead,
	and the input will be assumed as consisting of values of 0.
	Parameters  5 and 6 are the output real and imaginary parts.Acceptable value types 
	are unsigned char (BYTE), float and double. The imaginary part is optional (T*)0 can be 
	provided instead. The imaginary part of the output will be discarded in this case.
*/

/*perform FFT on image rows*/
template<class T> void fftrows(int width, int height, const T *ix, const T *iy, double *ox, double *oy);

/*perform IFFT on image rows*/
template<class T> void ifftrows(int width, int height, const double *ix, const double *iy, T *ox, T *oy);

/*perform FFT on image cols*/
template<class T> void fftcols(int width, int height, const T *ix, const T *iy, double *ox, double *oy);

/*perform FFT on image cols*/
template<class T> void ifftcols(int width, int height, const double *ix, const double *iy, T *ox, T *oy);

/*perform FFT on image*/
template<class T> void fftimage(int width, int height, const T *inpx, const T* inpy, double *ox, double *oy);

/*perform IFFT on image*/
template<class T> void ifftimage(int width, int height, const double *ix, const double *iy, T *outpx, T *outpy);
