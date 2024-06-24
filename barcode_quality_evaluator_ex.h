#ifndef BARCODE_QUALITY_EVALUATOR_EX_H
#define BARCODE_QUALITY_EVALUATOR_EX_H

#ifdef __cplusplus
extern "C" {
#endif

#define BRIGHTNESS_MAX 0
#define BRIGHTNESS_MIN 255
#define CODE_TYPE_DM (1 << 0)
#define CODE_TYPE_QR (1 << 1)

#define M_PI 3.14159265358979323846


	typedef struct
	{
		double x;
		double y;
	} Point;

	typedef struct iMat {
		unsigned char* data;
		unsigned int rows, cols;
	}iMat;
	typedef struct CodeLevelInfo {
		int type;								/* 码制 */
		iMat img;								/* 操作图像 */
		int position[4][2];						/* 二维码四个角点的位置 */
	}CodeLevelInfo;

	typedef enum
	{
		GRADE_ERR = -1,
		GRADE_F,
		GRADE_D,
		GRADE_C,
		GRADE_B,
		GRADE_A,
	} Grade_t;

	int evaluate_barcode_quality(CodeLevelInfo *codeLevelInfo);

#ifdef __cplusplus
}
#endif

#endif