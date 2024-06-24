#ifndef __APUS_IMG_PROCESSING_INTERFACE_H__
#define __APUS_IMG_PROCESSING_INTERFACE_H__

#ifndef APUS_DLL
# ifdef _WIN32
#  if defined( BUILD_DLL )
#    define APUS_DLL __declspec(dllexport)
#  elif defined( USE_DLL )
#    define APUS_DLL __declspec(dllimport)
#  else
#    define APUS_DLL
#  endif
# else
#  define APUS_DLL
# endif
#endif

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>

#ifndef iMat_STRUCT
#define iMat_STRUCT
typedef struct iMat {
	unsigned char* data;
	unsigned int rows, cols, channels;
	unsigned int stride;
	unsigned int capacity;
}iMat;
#endif

typedef struct iRoi {
    int xmin, nx;
    int ymin, ny;
}iRoi;

typedef struct iPoint {
    int x, y;
}iPoint;

#define iMat_zero {NULL, 0, 0, 0, 0, 0}

APUS_DLL int iMat_malloc(iMat* mat, int rows, int cols, int channels);
APUS_DLL void iMat_free(iMat* mat);
APUS_DLL iMat* iMat_createZero();
APUS_DLL iMat* iMat_create(int rows, int cols, int channels, unsigned char* data);
APUS_DLL iMat* iMat_destroy(iMat* mat);

APUS_DLL void iMat_limitRoi(const iMat* src, iRoi* roi);
APUS_DLL int iMat_copy(iMat* dst, const iMat* src, iRoi* roi, iPoint* ulp);
APUS_DLL int iMat_changeChannel(iMat* dst, const iMat* src, iRoi* roi, int channels);
APUS_DLL int iMat_getHistogram(int histogram[256], const iMat* src, iRoi* roi);
APUS_DLL int iMat_getCLHistogram(int histogram[256], const iMat* src, iRoi* roi, float cliplimit);
APUS_DLL void iMat_clipLimitHistogram(int histogram[256], int channels, int area, float cliplimit);
APUS_DLL void iMat_getEqualizeHistTable(unsigned char table[256], const int histogram[256], int area);
APUS_DLL int iMat_grayTransformation(iMat* dst, const iMat* src, iRoi* roi, iPoint* ulp, unsigned char table[256]);
APUS_DLL int iMat_avgPooling(iMat* dst, const iMat* src, iRoi* roi, iPoint* ulp);
APUS_DLL int iMat_maxPooling(iMat* dst, const iMat* src, iRoi* roi, iPoint* ulp);
APUS_DLL int iMat_minPooling(iMat* dst, const iMat* src, iRoi* roi, iPoint* ulp);
APUS_DLL int iMat_downsampling(iMat* dst, const iMat* src, iRoi* roi, iPoint* ulp);
APUS_DLL int iMat_gaussianBlur3x3(iMat* dst, const iMat* src, iRoi* roi, iPoint* ulp);
APUS_DLL int iMat_gaussianBlur5x5(iMat* dst, const iMat* src, iRoi* roi, iPoint* ulp);
APUS_DLL int iMat_medianBlur3x3(iMat* dst, const iMat* src, iRoi* roi, iPoint* ulp);
APUS_DLL int iMat_medianBlur5x5(iMat* dst, const iMat* src, iRoi* roi, iPoint* ulp);
APUS_DLL int iMat_getOtusThreshold(const iMat* src, iRoi* roi);
APUS_DLL int iMat_getHistThreshold(const int* histogram, int interval, unsigned char* first_peak, unsigned char* second_peak);
APUS_DLL int iMat_dilate2x2(iMat* dst, const iMat* src, iRoi* roi, iPoint* ulp);
APUS_DLL int iMat_dilate3x3(iMat* dst, const iMat* src, iRoi* roi, iPoint* ulp);
APUS_DLL int iMat_dilate5x5(iMat* dst, const iMat* src, iRoi* roi, iPoint* ulp);
APUS_DLL int iMat_erode2x2(iMat* dst, const iMat* src, iRoi* roi, iPoint* ulp);
APUS_DLL int iMat_erode3x3(iMat* dst, const iMat* src, iRoi* roi, iPoint* ulp);
APUS_DLL int iMat_erode5x5(iMat* dst, const iMat* src, iRoi* roi, iPoint* ulp);

APUS_DLL int iMat_drawRectangle(iMat* src, int rx, int ry, int rw, int rh, unsigned int color, int thickness);
APUS_DLL int iMat_drawLine(iMat* src, int x0, int y0, int x1, int y1, unsigned int color, int thickness);
APUS_DLL int iMat_drawText(iMat* src, const char* text, int x, int y, unsigned int color);

APUS_DLL int iMat_rbSwap(iMat* mat);
APUS_DLL int iMat_gray2bgr(iMat* dst, const iMat* src, iRoi* roi);

APUS_DLL int iMat_readbmp2gray(iMat* dst, const char* file);
APUS_DLL int iMat_writebmp(const char* filename, const iMat* src, iRoi* roi);

APUS_DLL int iMat_readpbm(iMat* dst, const char* filename);
APUS_DLL int iMat_readpgm(iMat* dst, const char* filename);
APUS_DLL int iMat_readppm(iMat* dst, const char* filename);
APUS_DLL int iMat_writepbm(const char* filename, const iMat* src, iRoi* roi, int forceplain);
APUS_DLL int iMat_writepgm(const char* filename, const iMat* src, iRoi* roi, int forceplain);
APUS_DLL int iMat_writeppm(const char* filename, const iMat* src, iRoi* roi, int forceplain);

APUS_DLL int iMat_readImage(iMat* dst, const char* filename);

#ifdef __cplusplus
}
#endif


#endif