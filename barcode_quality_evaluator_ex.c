//#include <stdio.h>
//#include<math.h>
//#include "barcode_quality_evaluator_ex.h"
//
//
//// 计算两点之间距离
//double calculate_distance(double x1, double y1, double x2, double y2)
//{
//	double val = sqrt(pow(x2 - x1, 2) + pow(y2 - y1, 2));
//	return val;
//}
//
//int get_min_max_coord(int x1, int x2, int x3, int x4, int flag)
//{
//	int temp12 = 0, temp34 = 0;
//	if (flag)
//	{
//		temp12 = x1 >= x2 ? x1 : x2;
//		temp34 = x3 >= x4 ? x3 : x4;
//		return temp12 >= temp34 ? temp12 : temp34;
//	}
//	else
//	{
//		temp12 = x1 <= x2 ? x1 : x2;
//		temp34 = x3 <= x4 ? x3 : x4;
//		return temp12 <= temp34 ? temp12 : temp34;
//	}
//}
//
//
///************************ 1、字符对比度 *********************************/
//
//int is_point_in_polygon(Point p, Point polygon[], int n) {
//	int i, j, c = 0;
//	for (i = 0, j = n - 1; i < n; j = i++) {
//		if (((polygon[i].y > p.y) != (polygon[j].y > p.y)) &&
//			(p.x < (polygon[j].x - polygon[i].x) * (p.y - polygon[i].y) / (polygon[j].y - polygon[i].y) + polygon[i].x)) {
//			c++;
//		}
//	}
//	return c % 2 == 1;
//}
//
//int get_min_max_gray_value_of_points(CodeLevelInfo* codeLevelInfo, int* sc_rmax, int* sc_rmin)
//{
//	int x1 = codeLevelInfo->position[0][0], y1 = codeLevelInfo->position[0][1];
//	int x2 = codeLevelInfo->position[1][0], y2 = codeLevelInfo->position[1][1];
//	int x3 = codeLevelInfo->position[2][0], y3 = codeLevelInfo->position[2][1];
//	int x4 = codeLevelInfo->position[3][0], y4 = codeLevelInfo->position[3][1];
//
//	const Point p1 = { x1, y1 };
//	const Point p2 = { x2, y2 };
//	const Point p3 = { x3, y3 };
//	const Point p4 = { x4, y4 };
//
//	Point polygon[] = { p1, p2, p3, p4 };
//	int n = sizeof(polygon) / sizeof(polygon[0]);
//
//	int x_start = get_min_max_coord(x1, x2, x3, x4, 0);
//	int x_end = get_min_max_coord(x1, x2, x3, x4, 1);
//	int y_start = get_min_max_coord(y1, y2, y3, y4, 0);
//	int y_end = get_min_max_coord(y1, y2, y3, y4, 1);
//
//
//	int brightness_max = BRIGHTNESS_MAX;
//	int brightness_min = BRIGHTNESS_MIN;
//
//	for (int y = y_start; y <= y_end; y++)
//	{
//		for (int x = x_start; x <= x_end; x++)
//		{
//			Point p = { x, y };
//			if (!is_point_in_polygon(p, polygon, n))
//			{
//				continue;
//			}
//			int index = y * codeLevelInfo->img.cols + x;
//			if (index < codeLevelInfo->img.cols * codeLevelInfo->img.rows)
//			{
//				unsigned char brightness = codeLevelInfo->img.data[index];
//				if (brightness > brightness_max) {
//					brightness_max = brightness;
//				}
//				if (brightness < brightness_min) {
//					brightness_min = brightness;
//				}
//			}
//		}
//	}
//	*sc_rmax = brightness_max;
//	*sc_rmin = brightness_min;
//	return 1;
//}
//
//Grade_t evaluate_symbol_contrast_grade(CodeLevelInfo* codeLevelInfo, double* val)
//{
//	int sc_rmax = 0;
//	int sc_rmin = 0;
//
//	get_min_max_gray_value_of_points(codeLevelInfo, &sc_rmax, &sc_rmin);
//
//	if (sc_rmax < sc_rmin)
//	{
//		return GRADE_ERR;
//	}
//	*val = (double)(sc_rmax - sc_rmin) / 255;
//	if (*val >= 0.70)
//	{
//		return GRADE_A;
//	}
//	else if (*val >= 0.55)
//	{
//		return GRADE_B;
//	}
//	else if (*val >= 0.40)
//	{
//		return GRADE_C;
//	}
//	else if (*val >= 0.20)
//	{
//		return GRADE_D;
//	}
//	else
//	{
//		return GRADE_F;
//	}
//}
//
/////************************ 2、轴非均一性 **********************************/
//Grade_t evaluate_qr_axial_nonuniformity_grade(CodeLevelInfo* codeLevelInfo, double* val)
//{
//	int x1 = codeLevelInfo->position[0][0], y1 = codeLevelInfo->position[0][1];
//	int x2 = codeLevelInfo->position[1][0], y2 = codeLevelInfo->position[1][1];
//	int x3 = codeLevelInfo->position[2][0], y3 = codeLevelInfo->position[2][1];
//	int x4 = codeLevelInfo->position[3][0], y4 = codeLevelInfo->position[3][1];
//
//	double l1 = calculate_distance(x1, y1, x2, y2);
//	double l2 = calculate_distance(x2, y2, x3, y3);
//	double l3 = calculate_distance(x3, y3, x4, y4);
//	double l4 = calculate_distance(x4, y4, x1, y1);
//	double a1 = fabs((l1 / l2) - 1);
//	double a2 = fabs((l2 / l3) - 1);
//	double a3 = fabs((l3 / l4) - 1);
//	double a4 = fabs((l4 / l1) - 1);
//	double max_diff = a1;
//	if (a2 > max_diff) max_diff = a2;
//	if (a3 > max_diff) max_diff = a3;
//	if (a4 > max_diff) max_diff = a4;
//	if (max_diff <= 0.1)
//	{
//		return GRADE_A;
//	}
//	else if (max_diff <= 0.15)
//	{
//		return GRADE_B;
//	}
//	else if (max_diff <= 0.2)
//	{
//		return GRADE_C;
//	}
//	else if (max_diff <= 0.25)
//	{
//		return GRADE_C;
//	}
//	return GRADE_F;
//}
//
//Grade_t evaluate_axial_nonuniformity_grade(CodeLevelInfo* codeLevelInfo, double* val)
//{
//	if (codeLevelInfo->type == CODE_TYPE_DM)
//	{
//		return GRADE_A;
//	}
//	else if (codeLevelInfo->type == CODE_TYPE_QR)
//	{
//		return evaluate_qr_axial_nonuniformity_grade(codeLevelInfo, val);
//	}
//	return GRADE_ERR;
//}
//
///******************************* 3、非均匀网格 **************************************/
//
//double vector_length(Point p1, Point p2) {
//	return sqrt(pow(p2.x - p1.x, 2) + pow(p2.y - p1.y, 2));
//}
//
//double get_angle(Point p1, Point p2, Point p3) {
//	double a = vector_length(p1, p2);
//	double b = vector_length(p2, p3);
//	double c = vector_length(p1, p3);
//	double val = acos((a * a + b * b - c * c) / (2 * a * b));
//	return val * (180.0 / M_PI);
//}
//
//Grade_t evaluate_grid_nonuniformity_grade(CodeLevelInfo* codeLevelInfo, double* val)
//{
//	int x1 = codeLevelInfo->position[0][0], y1 = codeLevelInfo->position[0][1];
//	int x2 = codeLevelInfo->position[1][0], y2 = codeLevelInfo->position[1][1];
//	int x3 = codeLevelInfo->position[2][0], y3 = codeLevelInfo->position[2][1];
//	int x4 = codeLevelInfo->position[3][0], y4 = codeLevelInfo->position[3][1];
//
//	const Point p1 = { x1, y1 };
//	const Point p2 = { x2, y2 };
//	const Point p3 = { x3, y3 };
//	const Point p4 = { x4, y4 };
//
//	double angle1 = get_angle(p1, p2, p3);
//	double angle2 = get_angle(p2, p3, p4);
//	double angle3 = get_angle(p3, p4, p1);
//	double angle4 = get_angle(p4, p1, p2);
//
//	double angles[] = { angle1, angle2, angle3, angle4 };
//	double difference_max = 0;
//	double x = angles[0];
//
//	for (int i = 0; i < 4; i++) {
//		if (abs(angles[i] - 90) > difference_max) {
//			difference_max = abs(angles[i] - 90);
//		}
//	}
//
//	*val = difference_max;
//	if (*val <= 10)
//	{
//		return GRADE_A;
//	}
//	else if (*val <= 15)
//	{
//		return GRADE_B;
//	}
//	else if (*val <= 20)
//	{
//		return GRADE_C;
//	}
//	else if (*val <= 25)
//	{
//		return GRADE_D;
//	}
//	else
//	{
//		return GRADE_F;
//	}
//}
//int evaluate_barcode_quality(CodeLevelInfo * codeLevelInfo)
//{
//	double val = 0;
//	int grade = 0;
//	int cur_min_gradle = GRADE_A;
//
//	// 1、字符对比度等级
//	grade = evaluate_symbol_contrast_grade(codeLevelInfo, &val);
//	printf("1、evaluate_symbol_contrast_grade grade = %d,value = %f \n", grade, val);
//	if (grade == GRADE_F) {
//		return grade;
//	}
//	if (grade < cur_min_gradle) {
//		cur_min_gradle = grade;
//	}
//	// 2、轴非均一性
//	grade = evaluate_axial_nonuniformity_grade(codeLevelInfo, &val);
//	printf("2、evaluate_axial_nonuniformity_grade grade = %d,value = %f \n", grade, val);
//	if (grade == GRADE_F) {
//		return grade;
//	}
//	if (grade < cur_min_gradle) {
//		cur_min_gradle = grade;
//	}
//	// 3、非均匀网格
//	grade = evaluate_grid_nonuniformity_grade(codeLevelInfo, &val);
//	printf("3、evaluate_grid_nonuniformity_grade grade = %d,value = %f \n", grade, val);
//	if (grade == GRADE_F) {
//		return grade;
//	}
//	if (grade < cur_min_gradle) {
//		cur_min_gradle = grade;
//	}
//	return cur_min_gradle;
//}