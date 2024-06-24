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
	 *                               ע��
	 * ---------------------------------------------------------------------- *
	 */
	CONFIG_REGISTER_CODE            = 0X37D256B7,       /* [char*] ����ע���� */
	CONFIG_REGISTER_FILE            = 0X517C733D,       /* [char*] ����ע���ļ�����·�� */
	CONFIG_REGISTER_STATUS          = 0X5B651DE5,
	/*
	 * ---------------------------------------------------------------------- *
	 *                               ��������
	 * ---------------------------------------------------------------------- *
	 */
	CONFIG_MEMORY_MODE              = 0X45B8343E,       /* [int = 1] �Ƿ������ʷ����������Ż�����������̣�0.�����䣬1.���� */
	//CONFIG_REPEAT_SEARCH            = 0X681818C5,       /* [int = 0] �Ƿ��ظ�������0.��������1.���� */
	CONFIG_DPM_ENABLE               = 0X744B2D33,       /* [int = 0] �Ƿ���DPM����룬����չ�˸��õ�DPM�����㷨�����Ĭ�Ϲر� */
	//CONFIG_MORPH_PYRAMID            = 0X18B35CF5,       /* [int = 0] ��̬ѧ��������0.�رգ�1.��ʴ��2.���ͣ�3.���� */
	CONFIG_PYRAMID_LAYERS           = 0X6E0CEA4A,       /* [int = 4] �����������Ĳ�����1 ~ 8 */
	/*
	 * ---------------------------------------------------------------------- *
	 *                               DM��������
	 * ---------------------------------------------------------------------- *
	 */
	CONFIG_DM_CODE_NUMBER           = 0X1BB6133E,       /* [int = 0] DM���������0 ~ 100�������0����CONFIG_BARCODE_NUMBER */
	CONFIG_DM_COLOR                 = 0X25385662,       /* [int = 3] DM�����ɫ��1.������ɫ��2.��ɫ��3.�Զ��ж� */
	CONFIG_DM_SAMPLING_METHOD       = 0X7F4FB6D6,       /* [int = 1] DM��Ĳ���������0 ~ 2 */
	CONFIG_DM_FIXED_EDITION         = 0X7D6F26A2,       /* [int = 0x3fffffff] DM��İ汾�ţ�1 ~ 0x3fffffff */
	CONFIG_DM_SHAPE                 = 0X456B9711,       /* [int = 3] DM�����״��1.�����룻2.�����룻3.���� */
	CONFIG_DM_SIZE                  = 0X47862BD4,       /* [int = 8,int = 144] DM�����С�����ߴ磺8 ~ 144 */

	 /*
	  * ---------------------------------------------------------------------- *
	  *                               QR��������
	  * ---------------------------------------------------------------------- *
	  */
	 CONFIG_QR_CODE_NUMBER          = 0X7E75BADE,       /* [int = 0] QR���������0 ~ 100�������0����CONFIG_BARCODE_NUMBER */
	 CONFIG_QR_COLOR                = 0X5381E803,       /* [int = 3] QR�����ɫ��1.������ɫ��2.��ɫ��3.�Զ��ж� */
	 
	 //CONFIG_QR_DECODEING_METHOD = 0x1301,    /* [int = 7] ���뷽����1.�������٣�2.�ǵ���չ����ȱ����4.Ѱ��ƥ�䣻7.���� */
	 //CONFIG_QR_REDECODE_TIMES,				  /* [int = 0] �ظ����������0 ~ 15 */
	 //CONFIG_QR_CONTRAST,					  /* [int = 10] QR����ͶԱȣ�1 ~ 255 */
	 //CONFIG_QR_LINE_SPACE,					  /* [int = 1] QR�������м�ࣺ4 ~ 64 */
	 //CONFIG_QR_MODULE_PPM,					  /* [int = 1] QR��ģ����С���ȣ�1 ~ 255 */
	 //CONFIG_QR_SAMPLING_METHOD,		      /* [int = 2] QR��Ĳ���������0 ~ 2 */
	 //CONFIG_QR_MIN_VERSION,				  /* [int = 1] QR�����С�汾�ţ�1 ~ CONFIG_QR_MAX_VERSION */
	 //CONFIG_QR_MAX_VERSION,				  /* [int = 40] QR������汾�ţ�CONFIG_QR_MIN_VERSION ~ 40 */
	 //CONFIG_QR_VERSION_RANGE,				  /* [int = 1,int = 40] QR��İ汾��Χ��1 ~ 40 */
	 //CONFIG_QR_VERSION_NUM,				  /* [int]<read-only> QR��ѡ���İ汾�� */

	
	/*
	 * ---------------------------------------------------------------------- *
	 *                               Scanner����
	 * �������������scannerָ������ã�������������¼��ͳ�ƽ�����
	 * Ϊ����Ӱ������ٶȣ����ڲ���ģʽ�����ģʽ���ٿ�����������
	 * ---------------------------------------------------------------------- *
	 */

	/* 
	 * CONFIG_RECORD_FILE��CONFIG_RECORD_POINTERѡ����һ�������ü��ɣ�
	 * ��Ҫ���������̨��������CONFIG_RECORD_POINTERΪstdout��
	 * ���齫CONFIG_RECORD_FILE����Ϊcsv�ļ�����CONFIG_RECORD_POINTER����Ϊcsv�ļ�ָ�롣
	 */
	CONFIG_RECORD_FILE              = 0X4F9E6700,       /* [char* = NULL]<write-only> �����ļ���������������¼�������ļ���׺Ϊcsv */
	CONFIG_RECORD_POINTER           = 0X632A5D4E,       /* [FILE* = NULL]<write-only> �����ļ�ָ�룬������������¼ */

	/* 
	 * CONFIG_STAT_FILE��CONFIG_STAT_POINTERѡ����һ�������ü��ɣ�
	 * ��Ҫ���������̨��������CONFIG_STAT_POINTERΪstdout��
	 */
	CONFIG_STAT_FILE                = 0X7B47E890,       /* [char* = NULL]<write-only> �����ļ��������������ͳ�ƽ�� */
	CONFIG_STAT_POINTER             = 0X3C31CBEC,       /* [FILE* = NULL]<write-only> �����ļ�ָ�룬�����������ͳ�ƽ�� */

	/* 
	 * CONFIG_LOG_FILE��CONFIG_LOG_POINTERѡ����һ�������ü��ɣ�
	 * ��Ҫ���������̨��������CONFIG_LOG_POINTERΪstdout��
	 * ������������������������־������Ӱ������ٶȣ����ֻ���ڵ��ԣ�
	 * Ϊ������û������־������������ļ��С�
	 */
	CONFIG_LOG_FILE                 = 0X59CD83B0,       /* [char* = NULL]<write-only> �����ļ��������������LOG��־��������Ӱ������ٶ� */
	CONFIG_LOG_POINTER              = 0X34F4204C,       /* [FILE* = NULL]<write-only> �����ļ�ָ�룬�����������LOG��־��������Ӱ������ٶ� */


};


APUS_DLL void SVS_md5sumImg(const unsigned char* data, int width, int height, char decrypt[33]);
APUS_DLL void SVS_md5sumData(const unsigned char* data, int length, char decrypt[33]);
APUS_DLL void SVS_md5sumFile(const char* filename, char decrypt[33]);

#ifdef __cplusplus
}
#endif

#endif