#include <stdio.h>
#include<math.h>
#include "barcode_quality_evaluator.h"

// 计算两点之间距离
double calculate_distance(double x1, double y1, double x2, double y2)
{
	double val = sqrt(pow(x2 - x1, 2) + pow(y2 - y1, 2));
	return val;
}

int get_min_max_coord(int x1, int x2, int x3, int x4, int flag)
{
	int temp12 = 0, temp34 = 0;
	if (flag)
	{
		temp12 = x1 >= x2 ? x1 : x2;
		temp34 = x3 >= x4 ? x3 : x4;
		return temp12 >= temp34 ? temp12 : temp34;
	}
	else
	{
		temp12 = x1 <= x2 ? x1 : x2;
		temp34 = x3 <= x4 ? x3 : x4;
		return temp12 <= temp34 ? temp12 : temp34;
	}
}
/**************************** 1、解码 ***********************************/
/**
 * @description: 判断解码结果等级
 * @param {int} dec_res 0表示失败 非0表示成功
 * @return {Grade_t} GradeA：成功；GRADE_F：失败
 */
Grade_t evaluate_decode_grade(int dec_res)
{
	return dec_res ? GRADE_A : GRADE_F;
}

/************************ 2、字符对比度 *********************************/

int is_point_in_polygon(Point p, Point polygon[], int n) {
	int i, j, c = 0;
	for (i = 0, j = n - 1; i < n; j = i++) {
		if (((polygon[i].y > p.y) != (polygon[j].y > p.y)) &&
			(p.x < (polygon[j].x - polygon[i].x) * (p.y - polygon[i].y) / (polygon[j].y - polygon[i].y) + polygon[i].x)) {
			c++;
		}
	}
	return c % 2 == 1;
}

int get_min_max_gray_value_of_points(CodeLevelInfo* codeLevelInfo, int* sc_rmax, int* sc_rmin)
{
	int x1 = codeLevelInfo->position[0][0], y1 = codeLevelInfo->position[0][1];
	int x2 = codeLevelInfo->position[1][0], y2 = codeLevelInfo->position[1][1];
	int x3 = codeLevelInfo->position[2][0], y3 = codeLevelInfo->position[2][1];
	int x4 = codeLevelInfo->position[3][0], y4 = codeLevelInfo->position[3][1];

	const Point p1 = { x1, y1 };
	const Point p2 = { x2, y2 };
	const Point p3 = { x3, y3 };
	const Point p4 = { x4, y4 };

	Point polygon[] = { p1, p2, p3, p4 };
	int n = sizeof(polygon) / sizeof(polygon[0]);

	int x_start = get_min_max_coord(x1, x2, x3, x4, 0);
	int x_end = get_min_max_coord(x1, x2, x3, x4, 1);
	int y_start = get_min_max_coord(y1, y2, y3, y4, 0);
	int y_end = get_min_max_coord(y1, y2, y3, y4, 1);


	int brightness_max = BRIGHTNESS_MAX;
	int brightness_min = BRIGHTNESS_MIN;

	for (int y = y_start; y <= y_end; y++)
	{
		for (int x = x_start; x <= x_end; x++)
		{
			Point p = { x, y };
			if (!is_point_in_polygon(p, polygon, n))
			{
				continue;
			}
			int index = y * codeLevelInfo->img.cols + x;
			if (index < codeLevelInfo->img.cols * codeLevelInfo->img.rows)
			{
				unsigned char brightness = codeLevelInfo->img.data[index];
				if (brightness > brightness_max) {
					brightness_max = brightness;
				}
				if (brightness < brightness_min) {
					brightness_min = brightness;
				}
			}
		}
	}
	*sc_rmax = brightness_max;
	*sc_rmin = brightness_min;
	return 1;
}

Grade_t evaluate_symbol_contrast_grade(CodeLevelInfo* codeLevelInfo, double* val)
{
	int sc_rmax = 0;
	int sc_rmin = 0;

	get_min_max_gray_value_of_points(codeLevelInfo, &sc_rmax, &sc_rmin);

	if (sc_rmax < sc_rmin)
	{
		return GRADE_ERR;
	}
	*val = (double)(sc_rmax - sc_rmin) / 255;
	if (*val >= 0.70)
	{
		return GRADE_A;
	}
	else if (*val >= 0.55)
	{
		return GRADE_B;
	}
	else if (*val >= 0.40)
	{
		return GRADE_C;
	}
	else if (*val >= 0.20)
	{
		return GRADE_D;
	}
	else
	{
		return GRADE_F;
	}
}

/************************ 3、轴非均一性 **********************************/
int get_modlue_size(CodeLevelInfo* codeLevelInfo, int* x_avg, int* y_avg)
{
	int M = codeLevelInfo->code_vision[0];
	int N = codeLevelInfo->code_vision[1];
	double total_distance_x = 0.0;
	int distance_count_x = 0;
	double total_distance_y = 0.0;
	int distance_count_y = 0;

	for (int i = 0; i < N; i++)
	{
		for (int j = 0; j < M - 1; j++)
		{
			double x1 = codeLevelInfo->module_position[i * M + j].x;
			double y1 = codeLevelInfo->module_position[i * M + j].y;
			double x2 = codeLevelInfo->module_position[i * M + j + 1].x;
			double y2 = codeLevelInfo->module_position[i * M + j + 1].y;
			total_distance_x += calculate_distance(x1, y1, x2, y2);
			distance_count_x++;
		}
	}

	for (int j = 0; j < M; j++)
	{
		for (int i = 1; i < N - 1; i++)
		{
			double x1 = codeLevelInfo->module_position[i * M + j].x;
			double y1 = codeLevelInfo->module_position[i * M + j].y;
			double x2 = codeLevelInfo->module_position[(i + 1) * M + j].x;
			double y2 = codeLevelInfo->module_position[(i + 1) * M + j].y;
			total_distance_y += calculate_distance(x1, y1, x2, y2);
			distance_count_y++;
		}
	}

	*x_avg = distance_count_x > 0 ? total_distance_x / distance_count_x : 0;
	*y_avg = distance_count_y > 0 ? total_distance_y / distance_count_y : 0;
	return 1;
}

Grade_t evaluate_axial_nonuniformity_grade(CodeLevelInfo* codeLevelInfo, double* val)
{
	int x_avg = 0;
	int y_avg = 0;
	get_modlue_size(codeLevelInfo, &x_avg, &y_avg);

	if (x_avg == 0 && y_avg == 0)
	{
		return GRADE_ERR;
	}
	*val = abs(x_avg - y_avg) / ((x_avg + y_avg) / 2);
	if (*val <= 0.06)
	{
		return GRADE_A;
	}
	else if (*val <= 0.08)
	{
		return GRADE_B;
	}
	else if (*val <= 0.10)
	{
		return GRADE_C;
	}
	else if (*val <= 0.12)
	{
		return GRADE_D;
	}
	else
	{
		return GRADE_F;
	}
}

/******************************* 4、非均匀网格 **************************************/

double vector_length(Point p1, Point p2) {
	return sqrt(pow(p2.x - p1.x, 2) + pow(p2.y - p1.y, 2));
}

double get_angle(Point p1, Point p2, Point p3) {
	double a = vector_length(p1, p2);
	double b = vector_length(p2, p3);
	double c = vector_length(p1, p3);
	double val = acos((a * a + b * b - c * c) / (2 * a * b));
	return val * (180.0 / M_PI);
}

Grade_t evaluate_grid_nonuniformity_grade(CodeLevelInfo* codeLevelInfo, double* val)
{
	int x1 = codeLevelInfo->position[0][0], y1 = codeLevelInfo->position[0][1];
	int x2 = codeLevelInfo->position[1][0], y2 = codeLevelInfo->position[1][1];
	int x3 = codeLevelInfo->position[2][0], y3 = codeLevelInfo->position[2][1];
	int x4 = codeLevelInfo->position[3][0], y4 = codeLevelInfo->position[3][1];

	const Point p1 = { x1, y1 };
	const Point p2 = { x2, y2 };
	const Point p3 = { x3, y3 };
	const Point p4 = { x4, y4 };

	double angle1 = get_angle(p1, p2, p3);
	double angle2 = get_angle(p2, p3, p4);
	double angle3 = get_angle(p3, p4, p1);
	double angle4 = get_angle(p4, p1, p2);

	double angles[] = { angle1, angle2, angle3, angle4 };
	double difference_max = 0;
	double x = angles[0];

	for (int i = 0; i < 4; i++) {
		if (abs(angles[i] - 90) > difference_max) {
			difference_max = abs(angles[i] - 90);
		}
	}

	*val = difference_max;
	if (*val <= 10)
	{
		return GRADE_A;
	}
	else if (*val <= 15)
	{
		return GRADE_B;
	}
	else if (*val <= 20)
	{
		return GRADE_C;
	}
	else if (*val <= 25)
	{
		return GRADE_D;
	}
	else
	{
		return GRADE_F;
	}
}

/******************************** 5、调制能力 ******************************************/
int get_image_gt_sc_value(CodeLevelInfo* codeLevelInfo, double* gt, double* sc)
{
	double total_sum0 = 0; // 黑模块
	double total_sum1 = 0; // 白模块
	double ave_sum0; // 黑模块平均灰度
	double ave_sum1; // 白模块平均灰度
	int num_b_count = 0; // 黑模块数量
	int num_w_count = 0; // 白模块数量
	double total_sumb = 0, total_sumw = 0, avg_sumb = 0, avg_sumw = 0;
	// 首先计算黑模块和白模块的灰度值
	for (int m = 0; m < codeLevelInfo->code_vision[1]; m++)
	{
		for (int n = 0; n < codeLevelInfo->code_vision[0]; n++)
		{
			int type = codeLevelInfo->module_position[m * codeLevelInfo->code_vision[0] + n].type;
			int x = codeLevelInfo->module_position[m * codeLevelInfo->code_vision[0] + n].x;
			int y = codeLevelInfo->module_position[m * codeLevelInfo->code_vision[0] + n].y;
			if (!type) {
				total_sum0 += codeLevelInfo->img.data[y * codeLevelInfo->img.cols + x];
				total_sum0 += codeLevelInfo->img.data[y * codeLevelInfo->img.cols + x + 1];
				total_sum0 += codeLevelInfo->img.data[y * codeLevelInfo->img.cols + x - 1];
				total_sum0 += codeLevelInfo->img.data[(y - 1) * codeLevelInfo->img.cols + x];
				total_sum0 += codeLevelInfo->img.data[(y + 1) * codeLevelInfo->img.cols + x];
				num_b_count++;
				total_sumb += codeLevelInfo->img.data[y * codeLevelInfo->img.cols + x];
			}
			else {
				total_sum1 += codeLevelInfo->img.data[y * codeLevelInfo->img.cols + x];
				total_sum1 += codeLevelInfo->img.data[y * codeLevelInfo->img.cols + x + 1];
				total_sum1 += codeLevelInfo->img.data[y * codeLevelInfo->img.cols + x - 1];
				total_sum1 += codeLevelInfo->img.data[(y - 1) * codeLevelInfo->img.cols + x];
				total_sum1 += codeLevelInfo->img.data[(y + 1) * codeLevelInfo->img.cols + x];
				num_w_count++;
				total_sumw += codeLevelInfo->img.data[y * codeLevelInfo->img.cols + x];
			}
		}
	}

	// 取平均值
	ave_sum0 = total_sum0 / (num_b_count * 5.0);
	ave_sum1 = total_sum1 / (num_w_count * 5.0);

	avg_sumb = total_sumb / num_b_count;
	avg_sumw = total_sumw / num_w_count;
	if (ave_sum1 <= ave_sum0) {
		if (avg_sumw <= avg_sumb)
		{
			codeLevelInfo->light_type = 0;
			double temp_sum = 0;
			temp_sum = ave_sum0;
			ave_sum0 = ave_sum1;
			ave_sum1 = temp_sum;
		}
		else
		{
			return 0;
		}
	}
	*gt = (ave_sum0 + ave_sum1) / 2;
	*sc = ave_sum1 - ave_sum0;
	return 1;
}

Grade_t evaluate_modulation_grade(CodeLevelInfo* codeLevelInfo, double* val)
{

	double gt = 0;
	double sc = 0;
	double min_modulation = 1; // 最小模块调制能力
	int rc = 0;
	rc = get_image_gt_sc_value(codeLevelInfo, &gt, &sc);
	if (!rc)
	{
		return GRADE_ERR;
	}

	for (int m = 0; m < codeLevelInfo->code_vision[1]; m++)
	{
		for (int n = 0; n < codeLevelInfo->code_vision[0]; n++)
		{
			int R = codeLevelInfo->module_position[m * codeLevelInfo->code_vision[0] + n].type == 0 ? 0 : 255;
			double modulation = 2.0 * abs(R - gt) / sc;
			if (min_modulation > modulation) {
				min_modulation = modulation;
			}
		}
	}
	*val = min_modulation;
	if (*val >= 0.50)
	{
		return GRADE_A;
	}
	else if (*val >= 0.40)
	{
		return GRADE_B;
	}
	else if (*val >= 0.30)
	{
		return GRADE_C;
	}
	else if (*val >= 0.20)
	{
		return GRADE_D;
	}
	else
	{
		return GRADE_F;
	}
}

/******************************** 6、寻像图案损坏 ***************************************/

int check_qr_finder_pattern(CodeLevelInfo* codeLevelInfo, int x, int y)
{
	int err_num = 0;
	for (int m = x; m < x + 7; m++)
	{
		for (int n = y; n < y + 7; n++)
		{
			int qr_finder_val = qr_finder_pattern[m - x][n - y];
			int gray = codeLevelInfo->module_position[n * codeLevelInfo->code_vision[0] + m].type > 0 ? 1 : 0;
			if (gray != qr_finder_val) {
				err_num++;
			}
		}
	}
	if (!codeLevelInfo->light_type)
	{
		err_num = 49 - err_num;
	}
	return err_num;
}

int get_qr_finder_pattern_err_num(CodeLevelInfo* codeLevelInfo)
{
	// 计算左上角
	int n1 = check_qr_finder_pattern(codeLevelInfo, 0, 0);
	// 计算右上角
	int n2 = check_qr_finder_pattern(codeLevelInfo, codeLevelInfo->code_vision[0] - 7, 0);
	// 计算左下角
	int n3 = check_qr_finder_pattern(codeLevelInfo, 0, codeLevelInfo->code_vision[1] - 7);
	// 计算右下角
	int n4 = check_qr_finder_pattern(codeLevelInfo, codeLevelInfo->code_vision[0] - 7, codeLevelInfo->code_vision[1] - 7);
	//int n_max = (n1 > n2 ? n1 : (n3 > n4 ? n3 : n4));
	int n_max = get_min_max_coord(n1, n2, n3, n4, 1);
	int sum = n1 + n2 + n3 + n4;
	return (sum - n_max);
}

Grade_t evaluate_qr_fixed_pattern_damage(CodeLevelInfo* codeLevelInfo, double* val)
{
	int err_num = get_qr_finder_pattern_err_num(codeLevelInfo);
	*val = err_num;
	if (err_num == 0)
	{
		return GRADE_A;
	}
	else if (err_num == 1)
	{
		return GRADE_B;
	}
	else if (err_num == 2)
	{
		return GRADE_C;
	}
	else if (err_num == 3)
	{
		return GRADE_D;
	}
	else
	{
		return GRADE_F;
	}
}

int get_dm_finder_pattern_err_num(CodeLevelInfo* codeLevelInfo)
{
	int M = codeLevelInfo->code_vision[0]; // 模块行数
	int N = codeLevelInfo->code_vision[1]; // 模块列数
	int num_def_b = (M + N) * 3 / 2 - 3; // 默认黑模块数量
	int num_def_w = (M + N) / 2 - 1; // 默认白模块数量
	int num_b = 0; // 黑模块个数
	int num_w = 0; // 白模块个数
	int num_err = 0; // 模块错误个数
	for (int n = 0; n < N; n++)
	{
		// 提取左侧黑白点数据
		codeLevelInfo->module_position[n * M].type > 0 ? num_w++ : num_b++;
		// 提取右侧黑白点数据
		codeLevelInfo->module_position[n * M + M - 1].type > 0 ? num_w++ : num_b++;
	}

	for (int m = 0; m < M; m++)
	{
		if (m != 0 && m != M - 1)
		{
			// 提取顶侧黑白点数据
			codeLevelInfo->module_position[m].type > 0 ? num_w++ : num_b++;
			// 提取底侧黑白点数据
			codeLevelInfo->module_position[(N - 1) * M + m].type > 0 ? num_w++ : num_b++;
		}
	}
	num_err = abs(num_def_b - num_b) + abs(num_def_w - num_w);
	return num_err;
}

Grade_t evaluate_dm_fixed_pattern_damage(CodeLevelInfo* codeLevelInfo, double* val)
{
	int err_num = get_dm_finder_pattern_err_num(codeLevelInfo);
	*val = err_num;
	if (err_num == 0)
	{
		return GRADE_A;
	}
	else if (err_num == 1)
	{
		return GRADE_B;
	}
	else if (err_num == 2)
	{
		return GRADE_C;
	}
	else if (err_num == 3)
	{
		return GRADE_D;
	}
	else
	{
		return GRADE_F;
	}
}

Grade_t evaluate_fixed_pattern_damage(CodeLevelInfo* codeLevelInfo, double* val)
{
	if (codeLevelInfo->type == CODE_TYPE_QR)
	{
		return evaluate_qr_fixed_pattern_damage(codeLevelInfo, val);
	}
	else if (codeLevelInfo->type == CODE_TYPE_DM)
	{
		return evaluate_dm_fixed_pattern_damage(codeLevelInfo, val);
	}
	return GRADE_ERR;
}

/******************************** 7、未使用的误差校正 ***********************************/
Grade_t evaluate_unused_error_correction_grage(CodeLevelInfo* codeLevelInfo, double* val)
{
	int e = codeLevelInfo->nerases;
	int t = codeLevelInfo->nerrors;
	int d = codeLevelInfo->neccs;
	int p = codeLevelInfo->nmpcs;

	if (d == p)
	{
		return GRADE_ERR;
	}
	*val = 1.0 - ((e + 2 * t) / (d - p));
	if (*val >= 0.62)
	{
		return GRADE_A;
	}
	else if (*val >= 0.50)
	{
		return GRADE_B;
	}
	else if (*val >= 0.37)
	{
		return GRADE_C;
	}
	else if (*val >= 0.25)
	{
		return GRADE_D;
	}
	else
	{
		return GRADE_F;
	}
}

/******************************** 8、打印变化 ***********************************/
int find_most_frequent_gray_in_range(int *histogram, int start, int end, int *max_count)
{
	int most_gray = start;
	for (int i = start; i < end; i++)
	{
		if (histogram[i] > *max_count)
		{
			*max_count = histogram[i];
			most_gray = i;
		}
	}
	return most_gray;

}double calculate_mean_std(int* histogram, int start, int end, int* total_pixels, double* sigma) {
	double sum_of_squared_diff = 0;
	int total_value = 0;
	for (int i = start; i < end; i++)
	{
		*total_pixels += histogram[i];
		total_value += histogram[i] * i;
	}
	double mean = (double)total_value / *total_pixels;
	for (int i = start; i < end; i++)
	{
		if (histogram[i] > 0)
		{
			sum_of_squared_diff += histogram[i] * pow(i - mean, 2);
		}
	}
	double variance = sum_of_squared_diff / *total_pixels;
	*sigma = (int)sqrt(variance);
	if (*sigma == 0) *sigma = 1;
	if (*sigma == 0) *sigma = 1;

	return mean;
}

int count_nearby_pixels(int* histogram, int start, int end, int most_gray) {
	int total_pixels = 0;
	int count = 0;
	double sigma = 0;
	double mean = calculate_mean_std(histogram, start, end, &total_pixels, &sigma);
	int s = (int)(most_gray - sigma * 3);
	int e = (int)(most_gray + sigma * 3);
	s = s > start ? s : start;
	e = e < end ? e : end;
	for (int i = s; i <= e; i++)
	{
		count += histogram[i];
	}

	return count;
}
double calculate_histogram(int* histogram, int* range_count)
{
	int start = 0, end = 256, total_pixels = 0;
	double sigma = 0;
	double mean = calculate_mean_std(histogram, start, end, &total_pixels, &sigma);
	int s = (int)(mean - sigma * 1);
	int e = (int)(mean + sigma * 1);
	s = s > start ? s : start;
	e = e < end ? e : end;
	int temp_sum_mean_gray = 0;
	for (int i = s; i <= e; i++)
	{
		temp_sum_mean_gray += histogram[i];
	}
	double mean_ratio = (double)temp_sum_mean_gray / total_pixels;
	int max_gray = 0;
	if (mean_ratio > 0.5)
	{
		int max_count = 0;
		int most_global_gray = find_most_frequent_gray_in_range(histogram, 0, 256, &max_count);
		*range_count = count_nearby_pixels(histogram, 0, 256, most_global_gray);
		max_gray = most_global_gray;
	}
	else
	{
		int max_count1 = 0, max_count2 = 0;
		int most_left_gray = find_most_frequent_gray_in_range(histogram, 0, (int)mean, &max_count1);
		int most_right_gray = find_most_frequent_gray_in_range(histogram, (int)mean, 256, &max_count2);
		if (max_count1 > max_count2)
		{
			*range_count = count_nearby_pixels(histogram, 0, (int)mean + 1, most_left_gray);
			max_gray = most_left_gray;
		}
		else
		{
			*range_count = count_nearby_pixels(histogram, (int)mean, 256, most_right_gray);
			max_gray = most_right_gray;
		}
		
	}
	return max_gray;
}

int get_growth_value(iMat* img, PointInfo p1, PointInfo p2, double* s0, double* s1)
{	
	/*double all_gray = 0, th_gray = 0;
	int count_black = 0, count_white = 0;
	double t, distance = calculate_distance(p1.x, p1.y, p2.x, p2.y) / 2;
	int dx = abs(p1.x - p2.x);
	int dy = abs(p1.y - p2.y);
	int step = dx >= dy ? dx : dy;
	for (int i = 0; i <= step; i++) {
		t = (double)i / step;
		int x = (int)(p1.x + t * (p2.x - p1.x));
		int y = (int)(p1.y + t * (p2.y - p1.y));
		all_gray += img->data[img->cols * y + x];
	}
	th_gray = all_gray / (step + 1);
	for (int i = 0; i <= step; i++) {
		t = (double)i / step;
		int x = (int)(p1.x + t * (p2.x - p1.x));
		int y = (int)(p1.y + t * (p2.y - p1.y));
		if (img->data[img->cols * y + x] <= th_gray) {
			count_black++;
		}
		else {
			count_white++;
		}
	}
	if (count_black == 0 || count_white == 0)return 0;
	double b_w_ratio = (double)count_black / count_white;
	double w_b_ratio = (double)count_white / count_black;
	double ratio = b_w_ratio >= w_b_ratio ? b_w_ratio : w_b_ratio;
	ratio -= 1;
	*s0 = ratio > *s0 ? ratio : *s0;*/
	int distance = (int)(calculate_distance(p1.x, p1.y, p2.x, p2.y) / 3.5);
	int histogram1[257], histogram2[257];
	for (int i = 0; i < 257; i++)
	{
		histogram1[i] = 0;
		histogram2[i] = 0;
	}
	for (int i = -distance; i <= distance; i++)
	{
		for (int j = -distance; j <= distance; j++)
		{
			int new_x = p1.x + i;
			int new_y = p1.y + j;
			int gray_value = img->data[new_y * img->cols + new_x];
			histogram1[gray_value]++;
			int new_x2 = p2.x + i;
			int new_y2 = p2.y + j;
			int gray_value2 = img->data[new_y2 * img->cols + new_x2];
			histogram2[gray_value2]++;
		}
	}
	int black_count = 0, white_count = 0;
	int black_gray = calculate_histogram(histogram1, &black_count);
	int white_gray = calculate_histogram(histogram2, &white_count);
	if ((black_gray > 130 && white_gray > 130) || (black_gray < 130 && white_gray < 130)) return 0;
	double b_w_ratio = (double)black_count / white_count;
	double w_b_ratio = (double)white_count / black_count;
	*s1 = b_w_ratio < w_b_ratio ? b_w_ratio : w_b_ratio;
	*s0 = *s1 < *s0 ? *s1 : *s0;
	return 1;
}

Grade_t evaluate_dm_print_growth_grade(CodeLevelInfo* codeLevelInfo)
{
	//Point_Gray_t gray0 = { 0,0 };
	Point_Gray_t gray1 = { 1,1 };
	//Point_Gray_t gray2 = { 0,0 };
	Point_Gray_t gray3 = { 1,1 };
	int M = codeLevelInfo->code_vision[0]; // 模块行数
	int N = codeLevelInfo->code_vision[1]; // 模块列数
	int rc = 0;
	// 1、计算顶部打印增长率
	//for (int i = 0; i < M - 1; i++) {
	//	PointInfo p1 = *(codeLevelInfo->module_position + i);
	//	PointInfo p2 = *(codeLevelInfo->module_position + i + 1);
	//	rc = get_growth_value(&codeLevelInfo->img, p1, p2, &gray0.s0, &gray0.s1);
	//	if (!rc) return GRADE_ERR;
	//}
	// 2、计算底部打印增长率
	for (int i = 0; i < M - 1; i++) {
		PointInfo p1 = *(codeLevelInfo->module_position + (N - 1) * M + i);
		PointInfo p2 = *(codeLevelInfo->module_position + (N - 1) * M + i + 1);
		rc = get_growth_value(&codeLevelInfo->img, p1, p2, &gray1.s0, &gray1.s1);
		if (!rc) return GRADE_ERR;
	}
	// 3、计算左侧打印增长率
	//for (int n = 0; n < N - 1; n++)
	//{
	//	PointInfo p1 = *(codeLevelInfo->module_position + n * M);
	//	PointInfo p2 = *(codeLevelInfo->module_position + (n + 1) * M);
	//	get_growth_value(&codeLevelInfo->img, p1, p2, &gray2.s0, &gray2.s1);
	//}
	// 4、计算右侧打印增长率
	for (int n = 0; n < N - 1; n++)
	{
		PointInfo p1 = *(codeLevelInfo->module_position + n * M + M - 1);
		PointInfo p2 = *(codeLevelInfo->module_position + (n + 1) * M + M - 1);
		get_growth_value(&codeLevelInfo->img, p1, p2, &gray3.s0, &gray3.s1);
		if (!rc) return GRADE_ERR;
	}
	//double v1 = abs(1 - abs((gray1.s0 + gray1.s1) / 2 - gray1.s0) / abs((gray1.s0 + gray1.s1) / 2 - gray1.s1));
	//double v3 = abs(1 - abs((gray3.s0 + gray3.s1) / 2 - gray3.s0) / abs((gray3.s0 + gray3.s1) / 2 - gray3.s1));
	//double value = v1 > v3 ? v1 : v3;
	double value = gray1.s0 < gray3.s0 ? gray1.s0 : gray3.s0;
	if (value > 0.85) {
		return GRADE_A;
	}
	else if (value > 0.7) {
		return GRADE_B;
	}
	else if (value > 0.6) {
		return GRADE_C;
	}
	else if (value > 0.5) {
		return GRADE_D;
	}
	return GRADE_F;
}

Grade_t evaluate_print_growth_grade(CodeLevelInfo* codeLevelInfo)
{
	if (codeLevelInfo->type == CODE_TYPE_QR)
	{
		return GRADE_A;
	}
	else if (codeLevelInfo->type == CODE_TYPE_DM)
	{
		return evaluate_dm_print_growth_grade(codeLevelInfo);
	}
	return GRADE_ERR;
}

/******************************** 9、静区 ***********************************/
int get_ave_gray_value_of_points(iMat *img, const Point p1, const Point p2, const Point p3, const Point p4, int* ave_gray)
{
	int num = 0;
	int gray_total = 0;
	Point polygon[] = { p1, p2, p3, p4 };
	int n = sizeof(polygon) / sizeof(polygon[0]);

	int x_start = get_min_max_coord(p1.x, p2.x, p3.x, p4.x, 0);
	int x_end = get_min_max_coord(p1.x, p2.x, p3.x, p4.x, 1);
	int y_start = get_min_max_coord(p1.y, p2.y, p3.y, p4.y, 0);
	int y_end = get_min_max_coord(p1.y, p2.y, p3.y, p4.y, 1);

	int brightness_max = BRIGHTNESS_MAX;
	int brightness_min = BRIGHTNESS_MIN;

	for (int y = y_start; y <= y_end; y++)
	{
		for (int x = x_start; x <= x_end; x++)
		{
			Point p = { x, y };
			if (!is_point_in_polygon(p, polygon, n))
			{
				continue;
			}
			int index = y * img->cols + x;
			if (index < img->cols * img->rows)
			{
				num++;
				gray_total += img->data[index];
			}
		}
	}
	return num;
}



double get_module_width(CodeLevelInfo* codeLevelInfo)
{
	double val = 0; // 模块宽度
	int M = codeLevelInfo->code_vision[0]; // 模块行数
	int N = codeLevelInfo->code_vision[1]; // 模块列数
	int x1 = codeLevelInfo->position[0][0], y1 = codeLevelInfo->position[0][1];
	int x2 = codeLevelInfo->position[1][0], y2 = codeLevelInfo->position[1][1];
	int x3 = codeLevelInfo->position[2][0], y3 = codeLevelInfo->position[2][1];
	double l1 = calculate_distance(x1, y1, x2, y2);
	double l2 = calculate_distance(x2, y2, x3, y3);
	val = (l1 + l2) / (M + N);
	return val;
}

int get_ave_gray_value_of_line(iMat* img, Point p1, Point p2, double* ave_gray)
{
	double t, distance = calculate_distance(p1.x, p1.y, p2.x, p2.y) / 2;
	int dx = abs(p1.x - p2.x);
	int dy = abs(p1.y - p2.y);
	int step = dx >= dy ? dx : dy;
	for (int i = 0; i <= step; i++) {
		t = (double)i / step;
		int x = (int)(p1.x + t * (p2.x - p1.x));
		int y = (int)(p1.y + t * (p2.y - p1.y));
		*ave_gray += *(img->data + (img->cols * y + x));
	}
	*ave_gray /= step;
	return 1;
}

int get_margin_target_point(iMat* img, Point p1, Point p2, int scale, int ori/*方向*/, Point* targetP)
{
	double distance = calculate_distance(p1.x, p1.y, p2.x, p2.y);
	double dx = ori ? (p1.x - p2.x) / distance : (p2.x - p1.x) / distance;
	double dy = ori ? (p1.y - p2.y) / distance : (p2.y - p1.y) / distance;
	int x = p1.x + dx * scale;
	int y = p1.y + dy * scale;
	if (x > img->cols || x < 0 || y > img->rows || y < 0) {
		return 0;
	}
	targetP->x = x;
	targetP->y = y;
	return  1;
}



int get_avgage_gray_expand(iMat* img, Point p1, Point p2, Point p3, Point p4, Point p_set[],int n)
{
	int x_start = get_min_max_coord(p1.x, p2.x, p3.x, p4.x, 0);
	int x_end = get_min_max_coord(p1.x, p2.x, p3.x, p4.x, 1);
	int y_start = get_min_max_coord(p1.y, p2.y, p3.y, p4.y, 0);
	int y_end = get_min_max_coord(p1.y, p2.y, p3.y, p4.y, 1);
	int count_num = 0, sum_gray = 0;
	for (int r = y_start; r < y_end; r++)
	{
		for (int c = x_start; c < x_end; c++)
		{
			Point p = { c, r };
			if (!is_point_in_polygon(p, p_set, n)) continue;
			int index = r * img->cols + c;
			if (index < img->cols * img->rows)
			{
				count_num++;
				sum_gray += img->data[index];
			}

		}
	}
	return (double)sum_gray / count_num;
}


Grade_t evaluate_margin_grade(CodeLevelInfo* codeLevelInfo, double* val)
{
	double gt = 0;
	double sc = 0;
	int rc = 0;
	double module_w = 0;
	rc = get_image_gt_sc_value(codeLevelInfo, &gt, &sc);
	if (!rc)
	{
		return GRADE_ERR;
	}
	double avg_black = 0, avg_white = 0, avg_gray = 0;
	avg_black = gt - sc / 2;
	avg_white = gt + sc / 2;
	double compare_ratio = 0, compare_gray = 0;
	module_w = get_module_width(codeLevelInfo);
	int scale = 4;
	Point p1 = { codeLevelInfo->position[0][0] , codeLevelInfo->position[0][1] };
	Point p2 = { codeLevelInfo->position[1][0] , codeLevelInfo->position[1][1] };
	Point p3 = { codeLevelInfo->position[2][0] , codeLevelInfo->position[2][1] };
	Point p4 = { codeLevelInfo->position[3][0] , codeLevelInfo->position[3][1] };

	Point p12 = { 0, 0 }, p21 = { 0, 0 }, p23 = { 0, 0 }, p34 = { 0, 0 }, p43 = { 0, 0 };
	int gc = 1, ori = 1;
	gc *= get_margin_target_point(&codeLevelInfo->img, p1, p2, (int)module_w * scale, ori, &p12);
	gc *= get_margin_target_point(&codeLevelInfo->img, p2, p1, (int)module_w * scale, ori, &p21);
	gc *= get_margin_target_point(&codeLevelInfo->img, p4, p3, (int)module_w * scale, ori, &p43);
	gc *= get_margin_target_point(&codeLevelInfo->img, p2, p3, (int)module_w * scale, ori, &p23);
	gc *= get_margin_target_point(&codeLevelInfo->img, p3, p4, (int)module_w * scale, ori, &p34);
	if (!gc)return GRADE_ERR;
	if (codeLevelInfo->type == CODE_TYPE_DM)
	{
		Point type_p4 = { 0, 0 };
		gc = get_margin_target_point(&codeLevelInfo->img, p12, p43, (int)module_w * scale, ori, &type_p4);
		if (!gc)return GRADE_ERR;
		Point p_set[] = {p43, p4, p1, p2, p23, type_p4};
		int n = sizeof(p_set) / sizeof(p_set[0]);
		avg_gray = get_avgage_gray_expand(&codeLevelInfo->img, p43, p3, p23, type_p4, p_set, n);
	}
	else if(codeLevelInfo->type == CODE_TYPE_QR)
	{
		Point type_p1 = { 0, 0 }, type_p2 = { 0, 0 }, type_p3 = { 0, 0 }, type_p4 = { 0, 0 };
		gc *= get_margin_target_point(&codeLevelInfo->img, p12, p43, (int)module_w * scale, ori, &type_p1);
		gc *= get_margin_target_point(&codeLevelInfo->img, p21, p34, (int)module_w * scale, ori, &type_p2);
		gc *= get_margin_target_point(&codeLevelInfo->img, p34, p21, (int)module_w * scale, ori, &type_p3);
		gc *= get_margin_target_point(&codeLevelInfo->img, p43, p12, (int)module_w * scale, ori, &type_p4);
		if (!gc)return GRADE_ERR;
		Point p_set[] = { type_p1, type_p2, type_p3, type_p4, p1, p2, p3, p4 };
		int n = sizeof(p_set) / sizeof(p_set[0]);
		avg_gray = get_avgage_gray_expand(&codeLevelInfo->img, type_p1, type_p2, type_p3, type_p4, p_set, n);
	}
	if (codeLevelInfo->light_type)
	{
		compare_gray = avg_gray - avg_black;
	}
	else
	{
		compare_gray = avg_white - avg_gray;
	}
	if (compare_gray < 0) return GRADE_ERR;
	compare_ratio = compare_gray / sc;
	if (compare_ratio > 0.8)
	{
		return GRADE_A;
	}
	else if (compare_ratio > 0.6)
	{
		return GRADE_B;
	}
	else if (compare_ratio > 0.45)
	{
		return GRADE_C;
	}
	else if (compare_ratio > 0.3)
	{
		return GRADE_D;
	}
	else
	{
		return GRADE_ERR;
	}



	return GRADE_ERR;
}
/******************************** 10、条码尺寸 ***********************************/

Grade_t evaluate_qr_dimension_grade(CodeLevelInfo* codeLevelInfo, double* val)
{
	int x1 = codeLevelInfo->position[0][0], y1 = codeLevelInfo->position[0][1];
	int x2 = codeLevelInfo->position[1][0], y2 = codeLevelInfo->position[1][1];
	int x3 = codeLevelInfo->position[2][0], y3 = codeLevelInfo->position[2][1];
	int x4 = codeLevelInfo->position[3][0], y4 = codeLevelInfo->position[3][1];

	double l1 = calculate_distance(x1, y1, x2, y2);
	double l2 = calculate_distance(x2, y2, x3, y3);
	double l3 = calculate_distance(x3, y3, x4, y4);
	double l4 = calculate_distance(x4, y4, x1, y1);
	double a1 = fabs((l1 / l2) - 1);
	double a2 = fabs((l2 / l3) - 1);
	double a3 = fabs((l3 / l4) - 1);
	double a4 = fabs((l4 / l1) - 1);
	double max_diff = a1;
	if (a2 > max_diff) max_diff = a2;
	if (a3 > max_diff) max_diff = a3;
	if (a4 > max_diff) max_diff = a4;
	if (max_diff <= 0.1)
	{
		return GRADE_A;
	}
	else if (max_diff <= 0.15)
	{
		return GRADE_B;
	}
	else if (max_diff <= 0.2)
	{
		return GRADE_C;
	}
	else if (max_diff <= 0.25)
	{
		return GRADE_C;
	}
	return GRADE_F;
}

Grade_t evaluate_dimension_grade(CodeLevelInfo* codeLevelInfo, double* val)
{
	if (codeLevelInfo->type == CODE_TYPE_DM)
	{
		return GRADE_A;
	}
	else if (codeLevelInfo->type == CODE_TYPE_QR)
	{
		return evaluate_qr_dimension_grade(codeLevelInfo, val);
	}
	return GRADE_ERR;
}

/******************************** 11、循环解码次数 ***********************************/
Grade_t evaluate_decode_cycle_grade(CodeLevelInfo* codeLevelInfo, double* val)
{
	if (codeLevelInfo->cycle_decode == 1) {
		return GRADE_A;
	}
	return GRADE_ERR;
}

/******************************** 12、逆向处理 ***********************************/
Grade_t get_inverse_decode_grade(CodeLevelInfo* codeLevelInfo)
{
	return codeLevelInfo->decoder_type == 2 ? GRADE_F : GRADE_A;
}

int evaluate_barcode_quality(CodeLevelInfo* codeLevelInfo)
{
	double val = 0;
	int grade = 0;
	int cur_min_gradle = GRADE_A;
	codeLevelInfo->light_type = 1;
	// 1、解码等级
	grade = evaluate_decode_grade(codeLevelInfo->length);
	printf("1、evaluate_decode_grade grade = %d \n", grade);
	if (grade == GRADE_F) {
		return grade;
	}
	if (grade < cur_min_gradle) {
		cur_min_gradle = grade;
	}
	// 12、逆向处理
	grade = get_inverse_decode_grade(codeLevelInfo);
	printf("12、get_inverse_decode_grade grade = %d \n", grade);
	if (grade == GRADE_F) {
		return grade;
	}
	if (grade < cur_min_gradle) {
		cur_min_gradle = grade;
	}
	// 2、字符对比度等级
	grade = evaluate_symbol_contrast_grade(codeLevelInfo, &val);
	printf("2、evaluate_symbol_contrast_grade grade = %d,value = %f \n", grade, val);
	if (grade == GRADE_F) {
		return grade;
	}
	if (grade < cur_min_gradle) {
		cur_min_gradle = grade;
	}
	// 3、轴非均一性
	grade = evaluate_axial_nonuniformity_grade(codeLevelInfo, &val);
	printf("3、evaluate_axial_nonuniformity_grade grade = %d,value = %f \n", grade, val);
	if (grade == GRADE_F) {
		return grade;
	}
	if (grade < cur_min_gradle) {
		cur_min_gradle = grade;
	}
	// 4、非均匀网格
	grade = evaluate_grid_nonuniformity_grade(codeLevelInfo, &val);
	printf("4、evaluate_grid_nonuniformity_grade grade = %d,value = %f \n", grade, val);
	if (grade == GRADE_F) {
		return grade;
	}
	if (grade < cur_min_gradle) {
		cur_min_gradle = grade;
	}
	// 5、调制能力
	grade = evaluate_modulation_grade(codeLevelInfo, &val);
	printf("5、evaluate_modulation_grade grade = %d,value = %f \n", grade, val);
	if (grade == GRADE_F) {
		return grade;
	}
	if (grade < cur_min_gradle) {
		cur_min_gradle = grade;
	}
	// 6、寻像图案损坏
	grade = evaluate_fixed_pattern_damage(codeLevelInfo, &val);
	printf("6、evaluate_fixed_pattern_damage grade = %d,value = %f \n", grade, val);
	if (grade == GRADE_F) {
		return grade;
	}
	if (grade < cur_min_gradle) {
		cur_min_gradle = grade;
	}
	// 7、未使用的误差校正
	grade = evaluate_unused_error_correction_grage(codeLevelInfo, &val);
	printf("7、evaluate_unused_error_correction_grage grade = %d,value = %f \n", grade, val);
	if (grade == GRADE_F) {
		return grade;
	}
	if (grade < cur_min_gradle) {
		cur_min_gradle = grade;
	}
	// 8、打印变化
	grade = evaluate_print_growth_grade(codeLevelInfo);
	printf("8、evaluate_print_growth_grade grade = %d,value = %f \n", grade, val);
	if (grade == GRADE_F) {
		return grade;
	}
	if (grade < cur_min_gradle) {
		cur_min_gradle = grade;
	}
	// 9、静区
	grade = evaluate_margin_grade(codeLevelInfo, &val);
	printf("9、evaluate_margin_grade grade = %d,value = %f \n", grade, val);
	if (grade == GRADE_F) {
		return grade;
	}
	if (grade < cur_min_gradle) {
		cur_min_gradle = grade;
	}
	// 10、条码尺寸
	grade = evaluate_dimension_grade(codeLevelInfo, &val);
	printf("10、evaluate_dimension_grade grade = %d,value = %f \n", grade, val);
	if (grade == GRADE_F) {
		return grade;
	}
	if (grade < cur_min_gradle) {
		cur_min_gradle = grade;
	}
	// 11、循环解码次数
	grade = evaluate_decode_cycle_grade(codeLevelInfo, &val);
	printf("11、evaluate_decode_cycle_grade grade = %d,value = %f \n", grade, val);
	if (grade == GRADE_F) {
		return grade;
	}
	if (grade < cur_min_gradle) {
		cur_min_gradle = grade;
	}
	return cur_min_gradle;
}

//int main()
//{

	// Grade_t val = evaluate_symbol_contrast_grade(100, 52);

	/*  Write C code in this online editor and run it. */
  //  printf("Hello, World!   11\n");
  //  printf("Hello, World!   22\n");

  //  return 0;
//}