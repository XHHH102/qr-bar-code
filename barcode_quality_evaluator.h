#ifndef BARCODE_QUALITY_EVALUATOR_H
#define BARCODE_QUALITY_EVALUATOR_H

#ifdef __cplusplus
extern "C" {
#endif

#include "MScanner.h"

#define BRIGHTNESS_MAX 0
#define BRIGHTNESS_MIN 255

#define CODE_TYPE_DM (1 << 0)
#define CODE_TYPE_QR (1 << 1)

#define M_PI 3.14159265358979323846

	typedef struct
	{
		double s0;
		double s1;
	} Point_Gray_t;

	typedef struct
	{
		double x;
		double y;
	} Point;

	typedef struct
	{
		int dec_res; // ������
		int sc_rmax; // �������ֵ
		int sc_rmin; // ��С����ֵ
		int an_xavg; // ����Ԫƽ���ߴ�
		int an_yavg; // ����Ԫƽ���ߴ�
		int gn_hmax; // ���λ����
		int gn_x;    // ��Ԫ�ߴ�
	} InputParameter_t;

	typedef enum
	{
		GRADE_ERR = -1,
		GRADE_F,
		GRADE_D,
		GRADE_C,
		GRADE_B,
		GRADE_A,
	} Grade_t;

	typedef struct
	{
		Grade_t grade_decode; // ����ȼ�
		int val;
	} OutputParameter_t;


	const int qr_finder_pattern[7][7] = {
		{0, 0, 0, 0,0,0,0},
		{0, 1, 1, 1,1,1,0},
		{0, 1, 0, 0,0,1,0},
		{0, 1, 0, 0,0,1,0},
		{0, 1, 0, 0,0,1,0},
		{0, 1, 1, 1,1,1,0},
		{0, 0, 0, 0,0,0,0},
	};


	int evaluate_barcode_quality(CodeLevelInfo* codeLevelInfo);

#ifdef __cplusplus
}
#endif

#endif