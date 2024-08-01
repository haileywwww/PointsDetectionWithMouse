#include <iostream>
#include <fstream>
#include <opencv2/opencv.hpp>
#include <unistd.h>

 
using namespace std;
using namespace cv;


#define RIGHT 1
#define OBBR_CAMERA 1
int th = 3;
int start_index = 0;
int end_index = 100;
float crop_ratio = 1.0f;
std::string BaseDir = "save0605_calib/right/";
std::string PointsFileName = "save0605_calib/right/obbr_points_0605_right.txt";

 
Point sp(-1, -1);
Point ep(-1, -1);
Mat temp;
int count_corner = 0;
std::vector<cv::Point> corner_coordinate(4);


bool distance_less_than_th(int i1, int j1, int i2, int j2, int th) {
	int i_ = i1 - i2;
	int j_ = j1 - j2;
	return ((i_ * i_ + j_ * j_ - th * th) < 0);
}

static void on_draw(int event, int x, int y, int flags, void* userdata) {
	Mat image = *((Mat*)userdata);
	if (event == EVENT_LBUTTONDOWN) {     //记录鼠标按下的坐标
		sp.x = x;
		sp.y = y; 
	}
	else if (event == EVENT_LBUTTONUP) {			//鼠标抬起
		ep.x = x;							//记录值
		ep.y = y;
		int dx = abs(ep.x - sp.x);				//计算width	 
		int dy = abs(ep.y - sp.y);				// height值		如果你松开的值小于按下的值，那得到的是一个负值，框选大小没有负值(dddd)
		Rect box;
		if (dx > 0 && dy > 0) {					//判断款选
			if ((ep.x - sp.x) > 0 && (ep.y - sp.y) > 0) {			//以左上角为起点，右下角为结点框选
				box.x = sp.x;
				box.y = sp.y;
				box.width = dx;
				box.height = dy;
				cv::rectangle(image, box, Scalar(0, 0, 255), 2, 8, 0);   //绘制框选区域
			}
			else if ((ep.x - sp.x) > 0 && (ep.y - sp.y) < 0) {		//以左下角为起点，右上角为结点框选
				box.x = sp.x;
				box.y = sp.y - dy;
				box.width = dx;
				box.height = dy;
				cv::rectangle(image, box, Scalar(0, 0, 255), 2, 8, 0);
			}
			else if ((ep.x - sp.x) < 0 && (ep.y - sp.y) > 0) {			//以右上角为起点，左下角为结点框选
				box.x = sp.x - dx;
				box.y = sp.y;
				box.width = dx;
				box.height = dy;
				cv::rectangle(image, box, Scalar(0, 0, 255), 2, 8, 0);
			}
			else {												//以右下角为起点，左上角为结点框选
				{
					box.x = sp.x - dx;
					box.y = sp.y - dy;
					box.width = dx;
					box.height = dy;
					cv::rectangle(image, box, Scalar(0, 0, 255), 2, 8, 0);
				}
			}
			temp.copyTo(image);
			namedWindow("鼠标绘制", cv::WINDOW_NORMAL | cv::WINDOW_KEEPRATIO);
			cv::resizeWindow("鼠标绘制", 810, 960);
			cv::moveWindow("鼠标绘制", 1000, 1000);
			imshow("鼠标绘制", image);
			
			cv::Mat image_box = image(box).clone();
			int min_value1 = 1000, index_i1 = -1, index_j1 = -1;
			
			for (int i = 0; i < image_box.rows; i++)
			{
				for (int j = 0; j < image_box.cols; j++)
				{
					if((int)image_box.at<Vec3b>(i, j)[0] < min_value1) {
						min_value1 = (int)image_box.at<Vec3b>(i, j)[0];
						index_i1 = i;
						index_j1 = j;						
					} 
				}
			}
			// std::cout << "value1: " << min_value1 << " " << index_i1 << " " << index_j1 << std::endl;
			// std::cout << "value2: " << min_value2 << " " << index_i2 << " " << index_j2 << std::endl;
			// cv::line(image_box, cv::Point(index_j1, index_i1-2), cv::Point(index_j1, index_i1+2), 255, 1); 
			// cv::line(image_box, cv::Point(index_j1-2, index_i1), cv::Point(index_j1+2, index_i1), 255, 1); 
			// cv::line(image_box, cv::Point(index_j2, index_i2-2), cv::Point(index_j2, index_i2+2), 255, 1); 
			// cv::line(image_box, cv::Point(index_j2-2, index_i2), cv::Point(index_j2+2, index_i2), 255, 1); 


			int outX1 = index_j1 + sp.x, outY1 = index_i1 + sp.y;
			cv::Mat image_clone = image.clone();
			cv::line(image_clone, cv::Point(outX1, outY1-2), cv::Point(outX1, outY1+2), 255, 1); 
			cv::line(image_clone, cv::Point(outX1-2, outY1), cv::Point(outX1+2, outY1), 255, 1); 
			

			// cv::line(image_clone, cv::Point((int)((outX1+outX2)/2), (int)((outY1+outY2)/2)-2), cv::Point((int)((outX1+outX2)/2), (int)((outY1+outY2)/2)+2), 255, 1); 
			// cv::line(image_clone, cv::Point((int)((outX1+outX2)/2)-2, (int)((outY1+outY2)/2)), cv::Point((int)((outX1+outX2)/2)+2, (int)((outY1+outY2)/2)), 255, 1); 

			// imshow("ROI区域", image_box);
			namedWindow("全图", cv::WINDOW_NORMAL | cv::WINDOW_KEEPRATIO);
			cv::resizeWindow("全图", 810, 960);
			imshow("全图", image_clone);



			corner_coordinate[count_corner] = cv::Point(outX1, outY1);
			std::cout << "coord: (" << corner_coordinate[count_corner].x << "," << corner_coordinate[count_corner].y << ")" << std::endl;
			count_corner++;
			if(count_corner > 3) {
				std::vector<cv::Point> point_coordinate(28);
				point_coordinate[0] = corner_coordinate[0];
				point_coordinate[3] = corner_coordinate[1];
				point_coordinate[27] = corner_coordinate[2];
				point_coordinate[24] = corner_coordinate[3];

				point_coordinate[1].x = (int)((point_coordinate[3].x + 2 * point_coordinate[0].x) / 3);
				point_coordinate[1].y = (int)((point_coordinate[3].y + 2 * point_coordinate[0].y) / 3);
				point_coordinate[2].x = (int)((2 * point_coordinate[3].x + point_coordinate[0].x) / 3);
				point_coordinate[2].y = (int)((2 * point_coordinate[3].y + point_coordinate[0].y) / 3);

				point_coordinate[25].x = (int)((point_coordinate[27].x + 2 * point_coordinate[24].x) / 3);
				point_coordinate[25].y = (int)((point_coordinate[27].y + 2 * point_coordinate[24].y) / 3);
				point_coordinate[26].x = (int)((2 * point_coordinate[27].x + point_coordinate[24].x) / 3);
				point_coordinate[26].y = (int)((2 * point_coordinate[27].y + point_coordinate[24].y) / 3);


				for(int j = 0; j <= 3; j++) {
					for(int i = 0; i <= 4; i++) {
						point_coordinate[4 + 4*i + j].x = (int)((i+1) * (point_coordinate[24 + j].x - point_coordinate[0 + j].x) / 6 + point_coordinate[0 + j].x);
						point_coordinate[4 + 4*i + j].y = (int)((i+1) * (point_coordinate[24 + j].y - point_coordinate[0 + j].y) / 6 + point_coordinate[0 + j].y);
					}
				}


				cv::Mat image_clone_allpoints = image.clone();
				// for(int i = 0; i < 28; i++) {
				// 	cv::line(image_clone_allpoints, cv::Point(point_coordinate[i].x, point_coordinate[i].y-2), cv::Point(point_coordinate[i].x, point_coordinate[i].y+2), 255, 1); 
				// 	cv::line(image_clone_allpoints, cv::Point(point_coordinate[i].x-2, point_coordinate[i].y), cv::Point(point_coordinate[i].x+2, point_coordinate[i].y), 255, 1); 
				// }


				int crop_width, crop_height;

				
				std::vector<std::pair<cv::Point, cv::Point>> crop_points(28);
				fstream pintsFile;
				pintsFile.open(PointsFileName.c_str(), ios::app);
				for(int index = 0; index < 28; index++) {
					if(index >= 24) {
						crop_width = (int)(abs(point_coordinate[index].x - point_coordinate[index-4].x) * crop_ratio);
					} else {
						crop_width = (int)(abs(point_coordinate[index].x - point_coordinate[index+4].x) * crop_ratio);
					}
					if(index % 4 == 3) {
						crop_height = (int)(abs(point_coordinate[index].y - point_coordinate[index-1].y) * crop_ratio);
					} else {
						crop_height = (int)(abs(point_coordinate[index].y - point_coordinate[index+1].y) * crop_ratio);
					}

					int x_l_u = (int)(point_coordinate[index].x - crop_width / 2);
					int y_l_u = (int)(point_coordinate[index].y - crop_height / 2);
					int x_r_d = (int)(point_coordinate[index].x + crop_width / 2);
					int y_r_d = (int)(point_coordinate[index].y + crop_height / 2);
					if(x_l_u < 0) x_l_u = 0;
					if(y_l_u < 0) y_l_u = 0;
					if(x_r_d > image_clone_allpoints.cols) x_r_d = image_clone_allpoints.cols;
					if(y_r_d > image_clone_allpoints.rows) y_r_d = image_clone_allpoints.rows;


					int min_value1_allpoints = 1000, index_i1_allpoints = -1, index_j1_allpoints = -1;
			
					for (int i = y_l_u; i < y_r_d; i++)
					{
						for (int j = x_l_u; j < x_r_d; j++)
						{
							if((int)image_clone_allpoints.at<Vec3b>(i, j)[0] < min_value1_allpoints) {
								min_value1_allpoints = (int)image_clone_allpoints.at<Vec3b>(i, j)[0];
								index_i1_allpoints = i;
								index_j1_allpoints = j;						
							} 
						}
					}

					int outX1_allpoints = index_j1_allpoints, outY1_allpoints = index_i1_allpoints;
					if(index == 0) {
						outX1_allpoints = corner_coordinate[0].x;
						outY1_allpoints = corner_coordinate[0].y;
					} else if(index == 3) {
						outX1_allpoints = corner_coordinate[1].x;
						outY1_allpoints = corner_coordinate[1].y;
					} else if(index == 27) {
						outX1_allpoints = corner_coordinate[2].x;
						outY1_allpoints = corner_coordinate[2].y;
					} else if(index == 24) {
						outX1_allpoints = corner_coordinate[3].x;
						outY1_allpoints = corner_coordinate[3].y;
					}
					cv::line(image_clone_allpoints, cv::Point(outX1_allpoints, outY1_allpoints-2), cv::Point(outX1_allpoints, outY1_allpoints+2), 255, 1); 
					cv::line(image_clone_allpoints, cv::Point(outX1_allpoints-2, outY1_allpoints), cv::Point(outX1_allpoints+2, outY1_allpoints), 255, 1); 


					crop_points[index].first = cv::Point(x_l_u, y_l_u);
					crop_points[index].second = cv::Point(x_r_d, y_r_d);


					pintsFile << "(" << (int)outX1_allpoints << "," << (int)outY1_allpoints << ") ";
			

				}
				pintsFile << "\n";
				pintsFile.close();

				for(int i = 0; i < 28; i++) {
					cv::rectangle(image_clone_allpoints, crop_points[i].first, crop_points[i].second, Scalar(0, 0, 255), 1, 8, 0);
				}

				count_corner = 0;
				corner_coordinate.clear();


				// namedWindow("all points", cv::WINDOW_NORMAL | cv::WINDOW_KEEPRATIO);
				// cv::resizeWindow("all points", 810, 960);
				imshow("全图", image_clone_allpoints);

			}



			sp.x = -1;
			sp.y = -1;
		}
	}
	else if (event == EVENT_MOUSEMOVE) {			//鼠标移动 为的是实现款选区域的线看起来是动态的
		if (sp.x > 0 && sp.y > 0) {
			ep.x = x;
			ep.y = y;
			int dx = abs(ep.x - sp.x);
			int dy = abs(ep.y - sp.y);
			Rect box;
			if ((ep.x - sp.x) > 0 && (ep.y - sp.y) > 0) {
				box.x = sp.x;
				box.y = sp.y;
				box.width = dx;
				box.height = dy;
				temp.copyTo(image);		//把temp中的图片复制给image,达到一种清屏的效果	 不明白的可以自行注释这两条代码运行看看就懂了		
				cv::rectangle(image, box, Scalar(0, 0, 255), 2, 8, 0);			//清屏后立刻重新款选
 
			}
			else if ((ep.x - sp.x) > 0 && (ep.y - sp.y) < 0) {
				box.x = sp.x;
				box.y = sp.y - dy;
				box.width = dx;
				box.height = dy;
				temp.copyTo(image);
				cv::rectangle(image, box, Scalar(0, 0, 255), 2, 8, 0);
 
			}
			else if ((ep.x - sp.x) < 0 && (ep.y - sp.y) > 0) {
				box.x = sp.x - dx;
				box.y = sp.y;
				box.width = dx;
				box.height = dy;
				temp.copyTo(image);
				cv::rectangle(image, box, Scalar(0, 0, 255), 2, 8, 0);
 
			}
			else {
				box.x = sp.x - dx;
				box.y = sp.y - dy;
				box.width = dx;
				box.height = dy;
				temp.copyTo(image);
				cv::rectangle(image, box, Scalar(0, 0, 255), 2, 8, 0);
 
			}
			namedWindow("鼠标绘制", cv::WINDOW_NORMAL | cv::WINDOW_KEEPRATIO);
			cv::resizeWindow("鼠标绘制", 810, 960);
			cv::moveWindow("鼠标绘制", 1000, 1000);
			imshow("鼠标绘制", image);
 
 
		}
	} else if (event == EVENT_RBUTTONUP) {
		fstream pintsFile;
		pintsFile.open(PointsFileName.c_str(), ios::app);
		pintsFile << "\n";
		pintsFile.close();
		exit(0);
	}
}
 
void mouse_drawing_demo(Mat& image) {
	namedWindow("鼠标绘制", cv::WINDOW_NORMAL | cv::WINDOW_KEEPRATIO);
	cv::resizeWindow("鼠标绘制", 810, 960);
	cv::moveWindow("鼠标绘制", 1000, 1000);
	setMouseCallback("鼠标绘制", on_draw, (void*)(&image));
	imshow("鼠标绘制", image);
	temp = image.clone();			//复制image图片
}

void LightUpImage(cv::Mat& src, cv::Mat& dst, float alpha, float beta) {
	for (int row = 0; row < src.rows; row++)
	{
		for (int col = 0; col < src.cols; col++)
		{
			if (src.channels() == 3)
			{
				int b = src.at<Vec3b>(row, col)[0];
				int g = src.at<Vec3b>(row, col)[1];
				int r = src.at<Vec3b>(row, col)[2];

				dst.at<Vec3b>(row, col)[0] = saturate_cast<uchar>(b * alpha + beta);
				dst.at<Vec3b>(row, col)[1] = saturate_cast<uchar>(g * alpha + beta);
				dst.at<Vec3b>(row, col)[2] = saturate_cast<uchar>(r * alpha + beta);

			}
			else if (src.channels() == 1)
			{
				float v = src.at<uchar>(row, col);
				dst.at<uchar>(row, col) = saturate_cast<uchar>(v * alpha + beta);
			}
		}
	}

}
 
int main(void)
{

	int num_processed = 0;
	for(int i = start_index; i < end_index; i++) {
		std::string ImageName = BaseDir + "Ir_" + to_string(i) + "_0.png";
		if (access(ImageName.c_str(), F_OK) == 0) {
			//Process camera1 IR
			std::cout << "Process " << ImageName << std::endl;

			fstream pintsFile;
			pintsFile.open(PointsFileName.c_str(), ios::app);
			pintsFile << ImageName << " ";
			pintsFile.close();


			cv::Mat img = cv::imread(ImageName.c_str());

#if OBBR_CAMERA
			cv::resize(img, img, cv::Size(1706, 1280), 0, 0, INTER_AREA);
#if RIGHT
			cv::rotate(img, img, cv::ROTATE_90_COUNTERCLOCKWISE);
			cv::resize(img, img, cv::Size(1080, 1440), 0, 0, cv::INTER_AREA);
			cv::Rect roi(0, 80, 1080, 1280);
			img = img(roi);
			cv::flip(img, img, 1);
#else
			cv::Rect roi(313, 0, 1080, 1280);
			img = img(roi);
			cv::flip(img, img, 1);

#endif
#else
			cv::Rect roi1(32, 0, 576, 576);
			img = img(roi1);
			cv::resize(img, img, cv::Size(1280, 1280), 0, 0, INTER_AREA);
			cv::Rect roi2(100, 0, 1080, 1280);
			img = img(roi2);
#endif

			cv::Mat img_light;
			img_light = Mat::zeros(img.size(), img.type());
			LightUpImage(img, img_light, 1.5, 30);

			std::string ImageCalibName = BaseDir + "Ir_" + to_string(i) + "_calib.png";
			cv::imwrite(ImageCalibName.c_str(), img_light);
			
			mouse_drawing_demo(img_light);
			waitKey(0);
		}
	}



	destroyAllWindows();
 
	std::cout << "end\n";
	return 0;
 
}
