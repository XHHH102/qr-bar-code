#ifndef __APUS_PRIVATE_INTERFACE_H__
#define __APUS_PRIVATE_INTERFACE_H__


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

enum PRIVATE_CONFIG_ITEM {
	/*
	 * ---------------------------------------------------------------------- *
	 *                               注册
	 * ---------------------------------------------------------------------- *
	 */
	CONFIG_REGISTER_CODE            = 0X37D256B7,       /* [char*] 设置注册码 */
	CONFIG_REGISTER_FILE            = 0X517C733D,       /* [char*] 设置注册文件所在路径 */
	CONFIG_REGISTER_STATUS          = 0X5B651DE5,
	/*
	 * ---------------------------------------------------------------------- *
	 *                               解码设置
	 * ---------------------------------------------------------------------- *
	 */
	CONFIG_MEMORY_MODE              = 0X45B8343E,       /* [int = 1] 是否记忆历史解码情况以优化后续解码过程：0.不记忆，1.记忆 */
	//CONFIG_REPEAT_SEARCH            = 0X681818C5,       /* [int = 0] 是否重复搜索：0.不搜索；1.搜索 */
	CONFIG_DPM_ENABLE               = 0X744B2D33,       /* [int = 0] 是否开启DPM码解码，因拓展了更好的DPM解码算法，因此默认关闭 */
	//CONFIG_MORPH_PYRAMID            = 0X18B35CF5,       /* [int = 0] 形态学金字塔：0.关闭；1.腐蚀；2.膨胀；3.所有 */
	CONFIG_PYRAMID_LAYERS           = 0X6E0CEA4A,       /* [int = 4] 搜索金字塔的层数：1 ~ 8 */
	/*
	 * ---------------------------------------------------------------------- *
	 *                               DM码配置项
	 * ---------------------------------------------------------------------- *
	 */
	CONFIG_DM_CODE_NUMBER           = 0X1BB6133E,       /* [int = 0] DM码的数量：0 ~ 100，如果是0则共用CONFIG_BARCODE_NUMBER */
	CONFIG_DM_COLOR                 = 0X25385662,       /* [int = 3] DM码的颜色：1.正常颜色；2.反色；3.自动判断 */
	CONFIG_DM_SAMPLING_METHOD       = 0X7F4FB6D6,       /* [int = 1] DM码的采样方法：0 ~ 2 */
	CONFIG_DM_FIXED_EDITION         = 0X7D6F26A2,       /* [int = 0x3fffffff] DM码的版本号：1 ~ 0x3fffffff */
	CONFIG_DM_SHAPE                 = 0X456B9711,       /* [int = 3] DM码的形状：1.方形码；2.矩形码；3.所有 */
	CONFIG_DM_SIZE                  = 0X47862BD4,       /* [int = 8,int = 144] DM码的最小和最大尺寸：8 ~ 144 */

	 /*
	  * ---------------------------------------------------------------------- *
	  *                               QR码配置项
	  * ---------------------------------------------------------------------- *
	  */
	 CONFIG_QR_CODE_NUMBER          = 0X7E75BADE,       /* [int = 0] QR码的数量：0 ~ 100，如果是0则共用CONFIG_BARCODE_NUMBER */
	 CONFIG_QR_COLOR                = 0X5381E803,       /* [int = 3] QR码的颜色：1.正常颜色；2.反色；3.自动判断 */
	 
	 //CONFIG_QR_DECODEING_METHOD = 0x1301,    /* [int = 7] 解码方法：1.轮廓跟踪；2.角点拓展（暂缺）；4.寻像匹配；7.所有 */
	 //CONFIG_QR_REDECODE_TIMES,				  /* [int = 0] 重复解码次数：0 ~ 15 */
	 //CONFIG_QR_CONTRAST,					  /* [int = 10] QR码最低对比：1 ~ 255 */
	 //CONFIG_QR_LINE_SPACE,					  /* [int = 1] QR码搜索行间距：4 ~ 64 */
	 //CONFIG_QR_MODULE_PPM,					  /* [int = 1] QR码模块最小长度：1 ~ 255 */
	 //CONFIG_QR_SAMPLING_METHOD,		      /* [int = 2] QR码的采样方法：0 ~ 2 */
	 //CONFIG_QR_MIN_VERSION,				  /* [int = 1] QR码的最小版本号：1 ~ CONFIG_QR_MAX_VERSION */
	 //CONFIG_QR_MAX_VERSION,				  /* [int = 40] QR码的最大版本号：CONFIG_QR_MIN_VERSION ~ 40 */
	 //CONFIG_QR_VERSION_RANGE,				  /* [int = 1,int = 40] QR码的版本范围：1 ~ 40 */
	 //CONFIG_QR_VERSION_NUM,				  /* [int]<read-only> QR码选定的版本数 */

	
	/*
	 * ---------------------------------------------------------------------- *
	 *                               Scanner设置
	 * 以下设置是针对scanner指针的设置，用于输出解码记录，统计解码结果
	 * 为避免影响解码速度，请在测试模式或调试模式下再开启下列配置
	 * ---------------------------------------------------------------------- *
	 */

	/* 
	 * CONFIG_RECORD_FILE与CONFIG_RECORD_POINTER选择其一进行配置即可，
	 * 若要输出到控制台，则设置CONFIG_RECORD_POINTER为stdout，
	 * 建议将CONFIG_RECORD_FILE设置为csv文件，或将CONFIG_RECORD_POINTER设置为csv文件指针。
	 */
	CONFIG_RECORD_FILE              = 0X4F9E6700,       /* [char* = NULL]<write-only> 设置文件，用以输出解码记录，建议文件后缀为csv */
	CONFIG_RECORD_POINTER           = 0X632A5D4E,       /* [FILE* = NULL]<write-only> 设置文件指针，用以输出解码记录 */

	/* 
	 * CONFIG_STAT_FILE与CONFIG_STAT_POINTER选择其一进行配置即可，
	 * 若要输出到控制台，则设置CONFIG_STAT_POINTER为stdout。
	 */
	CONFIG_STAT_FILE                = 0X7B47E890,       /* [char* = NULL]<write-only> 设置文件，用以输出解码统计结果 */
	CONFIG_STAT_POINTER             = 0X3C31CBEC,       /* [FILE* = NULL]<write-only> 设置文件指针，用以输出解码统计结果 */

	/* 
	 * CONFIG_LOG_FILE与CONFIG_LOG_POINTER选择其一进行配置即可，
	 * 若要输出到控制台，则设置CONFIG_LOG_POINTER为stdout，
	 * 配置这两项，将输出大量解码日志，严重影响解码速度，务必只用于调试，
	 * 为避免淹没其他日志，建议输出到文件中。
	 */
	CONFIG_LOG_FILE                 = 0X59CD83B0,       /* [char* = NULL]<write-only> 设置文件，用以输出解码LOG日志，将极大影响解码速度 */
	CONFIG_LOG_POINTER              = 0X34F4204C,       /* [FILE* = NULL]<write-only> 设置文件指针，用以输出解码LOG日志，将极大影响解码速度 */


};


APUS_DLL void SVS_md5sumImg(const unsigned char* data, int width, int height, char decrypt[33]);
APUS_DLL void SVS_md5sumData(const unsigned char* data, int length, char decrypt[33]);
APUS_DLL void SVS_md5sumFile(const char* filename, char decrypt[33]);

#ifdef __cplusplus
}
#endif

#endif