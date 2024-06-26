// barcode_quality_evaluator_test.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>
#include "barcode_quality_evaluator.h"	
#include "MScanner.h"

#define MAX_TIME_SLICE (1000 * 60 * 60 * 24)     /* 设置超时时间，单位为毫秒 */

/*********************************************************************
 * 参数表( 用于步骤 2 )
 *********************************************************************/
#define CONFIG_ITEM(c,KEY,VAULE) MOS_set(c, KEY, VAULE);
#define CONFIG_SET(c) \
CONFIG_ITEM(c, CONFIG_RESULT_CALLBACK,     handle_result) \
CONFIG_ITEM(c, CONFIG_PROGRESS_CALLBACK, handle_progress) \
CONFIG_ITEM(c, CONFIG_MODE,              (mNORMAL | mALL  ))  \
CONFIG_ITEM(c, CONFIG_CODE_LEVEL,                    1)   \

int handle_progress(char* s_tag) {
	return 0;
}

void handle_result(char* s_tag, void* result) {
	CodeResult code;
	CodeLevelInfo levelInfo;
	levelInfo.module_position = (PointInfo*)malloc(sizeof(PointInfo) * 177 * 177);
	if (MOS_getResult(result, &code)) {
		std::cout << "    >> √ "
			<< code.string
			<< " ("
			<< (code.type == CODE_TYPE_DM ? "DM" : "QR")
			<< ")    "
			<< std::endl;
	}
	if (MOS_getLevelInfo(result, &levelInfo)) {
		printf("对levelInfo的操作..\n");
		int value = evaluate_barcode_quality(&levelInfo);
		printf("evaluate_barcode_quality grade = %d\n", value);


	}
}

int main()
{
	void* property_page = NULL;
	void* scanner = NULL;
	iMat* img = NULL;
	if ((img = iMat_createZero()) == NULL) {
		printf("iMat_createZero err");
		return -1;
	}
	if (!iMat_readImage(img, "D:/Ccode/barcode_quality_evaluator_test/barcode_quality_evaluator_test/test_tr_4.bmp")) {
		printf("Mat_readImage err");
		return -1;
	}

	if ((property_page = MOS_openPropertyPage()) == NULL) {
		printf("MOS_openPropertyPage err");
		return -1;
	}
	CONFIG_SET(property_page);

	if ((scanner = MOS_openScanner((char*)"scanner", property_page)) == NULL) {
		printf("MOS_openScanner err");
		return -1;
	}

	MOS_setImageSize(scanner, img->cols, img->rows);

	if (!MOS_loadImage(scanner, img->data)) {
		printf("MOS_loadImage err");
		return -1;
	}

	//MOS_selectRegion(scanner, 460, 280, 175, 175);
	//MOS_selectRegion(scanner, 500, 500, 150, 150);
	//MOS_selectRegion(scanner, 660, 205, 150, 150);
	//MOS_selectRegion(scanner, 130, 60, 200, 200);
	MOS_selectRegion(scanner, 680, 0, 120, 120);

	int ncode = MOS_decode(scanner);
	img = iMat_destroy(img);
	property_page = MOS_closePropertyPage(property_page);
	scanner = MOS_closeScanner(scanner);
	printf("ncode = %d\n", ncode);
	return 0;
}
