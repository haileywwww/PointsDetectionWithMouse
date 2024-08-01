#include <iostream>
#include <fstream>
#include <opencv2/opencv.hpp>
#include <unistd.h>

#ifdef WIN32 //Windows
#include <direct.h>
#include <io.h>
#else  // Linux
#include <sys/io.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>
#endif

 
using namespace std;
using namespace cv;

#define IMAGE_TYPE_RAW 0
int th;
float crop_ratio;
std::string BaseDir;
std::string PointsFileName;



 
Point sp(-1, -1);
Point ep(-1, -1);
Mat temp;
int count_corner = 0;
std::vector<cv::Point> corner_coordinate(4);
std::vector<std::pair<cv::Point, cv::Point>> corner_coordinate_pair(4);



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
		int dx = abs(ep.x - sp.x);				//计算width	 这里为什么用abs()？
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
			int min_value2 = 1000, index_i2 = -1, index_j2 = -1;
			for (int i = 0; i < image_box.rows; i++)
			{
				for (int j = 0; j < image_box.cols; j++)
				{
					if((int)image_box.at<uchar>(i, j) < min_value1) {
						if(distance_less_than_th(i, j, index_i1, index_j1, th)) {
							min_value1 = (int)image_box.at<uchar>(i, j);
							index_i1 = i;
							index_j1 = j;
						} else {
							min_value2 = min_value1;
							index_i2 = index_i1;
							index_j2 = index_j1;
							min_value1 = (int)image_box.at<uchar>(i, j);
							index_i1 = i;
							index_j1 = j;
						}
					} else if((int)image_box.at<uchar>(i, j) < min_value2) {
						if(distance_less_than_th(i, j, index_i1, index_j1, th)) {
							continue;
						} else {
							min_value2 = (int)image_box.at<uchar>(i, j);
							index_i2 = i;
							index_j2 = j;
						}
					}
				}
			}
			// std::cout << "value1: " << min_value1 << " " << index_i1 << " " << index_j1 << std::endl;
			// std::cout << "value2: " << min_value2 << " " << index_i2 << " " << index_j2 << std::endl;
			// cv::line(image_box, cv::Point(index_j1, index_i1-2), cv::Point(index_j1, index_i1+2), 255, 1); 
			// cv::line(image_box, cv::Point(index_j1-2, index_i1), cv::Point(index_j1+2, index_i1), 255, 1); 
			// cv::line(image_box, cv::Point(index_j2, index_i2-2), cv::Point(index_j2, index_i2+2), 255, 1); 
			// cv::line(image_box, cv::Point(index_j2-2, index_i2), cv::Point(index_j2+2, index_i2), 255, 1); 


			int outX1 = index_j1 + sp.x, outY1 = index_i1 + sp.y, outX2 = index_j2 + sp.x, outY2 = index_i2 + sp.y;
			cv::Mat image_clone = image.clone();
			cv::line(image_clone, cv::Point(outX1, outY1-2), cv::Point(outX1, outY1+2), 255, 1); 
			cv::line(image_clone, cv::Point(outX1-2, outY1), cv::Point(outX1+2, outY1), 255, 1); 
			cv::line(image_clone, cv::Point(outX2, outY2-2), cv::Point(outX2, outY2+2), 255, 1); 
			cv::line(image_clone, cv::Point(outX2-2, outY2), cv::Point(outX2+2, outY2), 255, 1); 

			// cv::line(image_clone, cv::Point((int)((outX1+outX2)/2), (int)((outY1+outY2)/2)-2), cv::Point((int)((outX1+outX2)/2), (int)((outY1+outY2)/2)+2), 255, 1); 
			// cv::line(image_clone, cv::Point((int)((outX1+outX2)/2)-2, (int)((outY1+outY2)/2)), cv::Point((int)((outX1+outX2)/2)+2, (int)((outY1+outY2)/2)), 255, 1); 


			// imshow("ROI区域", image_box);
			namedWindow("全图", cv::WINDOW_NORMAL | cv::WINDOW_KEEPRATIO);
			cv::resizeWindow("全图", 810, 960);
			imshow("全图", image_clone);


			corner_coordinate[count_corner] = cv::Point((int)((outX1+outX2)/2), (int)((outY1+outY2)/2));
			std::cout << "coord: (" << corner_coordinate[count_corner].x << "," << corner_coordinate[count_corner].y << ")" << std::endl;
			corner_coordinate_pair[count_corner].first = cv::Point((int)outX1, (int)outY1);
			corner_coordinate_pair[count_corner].second = cv::Point((int)outX2, (int)outY2);
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
					int min_value2_allpoints = 1000, index_i2_allpoints = -1, index_j2_allpoints = -1;
					for (int i = y_l_u; i < y_r_d; i++)
					{
						for (int j = x_l_u; j < x_r_d; j++)
						{
							if((int)image_clone_allpoints.at<uchar>(i, j) < min_value1_allpoints) {
								if(distance_less_than_th(i, j, index_i1_allpoints, index_j1_allpoints, th)) {
									min_value1_allpoints = (int)image_clone_allpoints.at<uchar>(i, j);
									index_i1_allpoints = i;
									index_j1_allpoints = j;
								} else {
									min_value2_allpoints = min_value1_allpoints;
									index_i2_allpoints = index_i1_allpoints;
									index_j2_allpoints = index_j1_allpoints;
									min_value1_allpoints = (int)image_clone_allpoints.at<uchar>(i, j);
									index_i1_allpoints = i;
									index_j1_allpoints = j;
								}
							} else if((int)image_clone_allpoints.at<uchar>(i, j) < min_value2_allpoints) {
								if(distance_less_than_th(i, j, index_i1_allpoints, index_j1_allpoints, th)) {
									continue;
								} else {
									min_value2_allpoints = (int)image_clone_allpoints.at<uchar>(i, j);
									index_i2_allpoints = i;
									index_j2_allpoints = j;
								}
							}
						}
					}

					int outX1_allpoints = index_j1_allpoints, outY1_allpoints = index_i1_allpoints, outX2_allpoints = index_j2_allpoints, outY2_allpoints = index_i2_allpoints;
					if(index == 0) {
						outX1_allpoints = corner_coordinate_pair[0].first.x;
						outY1_allpoints = corner_coordinate_pair[0].first.y;
						outX2_allpoints = corner_coordinate_pair[0].second.x;
						outY2_allpoints = corner_coordinate_pair[0].second.y;
					} else if(index == 3) {
						outX1_allpoints = corner_coordinate_pair[1].first.x;
						outY1_allpoints = corner_coordinate_pair[1].first.y;
						outX2_allpoints = corner_coordinate_pair[1].second.x;
						outY2_allpoints = corner_coordinate_pair[1].second.y;
					} else if(index == 27) {
						outX1_allpoints = corner_coordinate_pair[2].first.x;
						outY1_allpoints = corner_coordinate_pair[2].first.y;
						outX2_allpoints = corner_coordinate_pair[2].second.x;
						outY2_allpoints = corner_coordinate_pair[2].second.y;
					} else if(index == 24) {
						outX1_allpoints = corner_coordinate_pair[3].first.x;
						outY1_allpoints = corner_coordinate_pair[3].first.y;
						outX2_allpoints = corner_coordinate_pair[3].second.x;
						outY2_allpoints = corner_coordinate_pair[3].second.y;
					}
					cv::line(image_clone_allpoints, cv::Point(outX1_allpoints, outY1_allpoints-2), cv::Point(outX1_allpoints, outY1_allpoints+2), 255, 1); 
					cv::line(image_clone_allpoints, cv::Point(outX1_allpoints-2, outY1_allpoints), cv::Point(outX1_allpoints+2, outY1_allpoints), 255, 1); 
					cv::line(image_clone_allpoints, cv::Point(outX2_allpoints, outY2_allpoints-2), cv::Point(outX2_allpoints, outY2_allpoints+2), 255, 1); 
					cv::line(image_clone_allpoints, cv::Point(outX2_allpoints-2, outY2_allpoints), cv::Point(outX2_allpoints+2, outY2_allpoints), 255, 1); 


					crop_points[index].first = cv::Point(x_l_u, y_l_u);
					crop_points[index].second = cv::Point(x_r_d, y_r_d);


					pintsFile << "(" << (int)((outX1_allpoints+outX2_allpoints)/2) << "," << (int)((outY1_allpoints+outY2_allpoints)/2) << ") ";
			

				}
				pintsFile << "\n";
				pintsFile.close();

				for(int i = 0; i < 28; i++) {
					cv::rectangle(image_clone_allpoints, crop_points[i].first, crop_points[i].second, Scalar(0, 0, 255), 1, 8, 0);
				}

				count_corner = 0;
				corner_coordinate.clear();
				corner_coordinate_pair.clear();


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
	setMouseCallback("鼠标绘制", on_draw, (void*)(&image));

	cv::resizeWindow("鼠标绘制", 810, 960);
	cv::moveWindow("鼠标绘制", 1000, 1000);
	imshow("鼠标绘制", image);

	temp = image.clone();			//复制image图片
}
 

// void MkDir(std::string SaveDir) {
// 	if (access(SaveDir.c_str(), 0) == -1) { //判断该文件夹是否存在
// #ifdef WIN32
//         int flag = mkdir(SaveDir.c_str());  //Windows创建文件夹
// #else
//         int flag = mkdir(SaveDir.c_str(), S_IRWXU);  //Linux创建文件夹
// #endif
//         if (flag == 0) {  //创建成功
//             std::cout << "Create directory successfully." << std::endl;
//         } else { //创建失败
//             std::cout << "Fail to create directory." << std::endl;
//             throw std::exception();
//         }
//     }
// }


int main(int argc, char **argv)
{
	// MkDir(BaseDir + "data/");
	// MkDir(BaseDir + "data/camera1/");
	// MkDir(BaseDir + "data/camera2/");
	// MkDir(BaseDir + "data/camera1/board_depth/");
	// MkDir(BaseDir + "data/camera1/board_ir/");
	// MkDir(BaseDir + "data/camera2/board_ir/");

	// int num_processed = 0;
	// for(int i = 0; i < 100; i++) {
	// 	std::string IRName = BaseDir + "Ir_" + to_string(i) + ".png";
	// 	if (access(IRName.c_str(), F_OK) == 0) {
	// 		//Process camera1 IR
	// 		std::cout << "Process " << IRName << std::endl;
	// 		cv::Mat img_ir = cv::imread(IRName.c_str());
	// 		cv::resize(img_ir, img_ir, cv::Size(1706, 1280), 0, 0, INTER_AREA);
	// 		cv::Rect roi(313, 0, 1080, 1280);
	// 		img_ir = img_ir(roi);
	// 		cv::flip(img_ir, img_ir, 1);

	// 		cv::Mat img_light;
	// 		img_light = Mat::zeros(img_ir.size(), img_ir.type());
	// 		LightUpImage(img_ir, img_light, 1.5, 30);

	// 		std::string IRSaveName = BaseDir + "data/camera1/board_ir/" + to_string(num_processed + 1) + ".png";
	// 		std::cout << "Save to " << IRSaveName << std::endl;
	// 		cv::imwrite(IRSaveName.c_str(), img_light);



	// 		//Process camera2 wavue image
	// 		std::string ImageName = BaseDir + to_string(i) + ".raw";
	// 		if (access(ImageName.c_str(), F_OK) == 0) {
	// 			std::cout << "Process " << ImageName << std::endl;

	// 			unsigned char* temp = new unsigned char[1280*1080];
	// 			FILE* fp = fopen(ImageName.c_str(), "rb");
	// 			fread(temp, 1280*1080, 1, fp);

	// 			cv::Mat img_wavue(1280, 1080, CV_8UC1, temp);

	// 			std::string ImageSaveName = BaseDir + "data/camera2/board_ir/" + to_string(num_processed + 1) + ".png";
	// 			std::cout << "Save to " << ImageSaveName << std::endl;
	// 			cv::imwrite(ImageSaveName.c_str(), img_wavue);
	// 		}


	// 		//Process camera1 depth
			
	// 	}
	// }



    printf("argc = %d\n", argc);
	if(argc != 5) {
		std::cout << "usage: ./test_mouse_auto \"wavue_points_0510.txt\" 6 0.8 \"save0510/45\"" << std::endl;
		return 0;
	}

    for (int i = 0; i < argc; i++)
        printf("%s\n", argv[i]);

	th = atoi(argv[2]);
	crop_ratio = atof(argv[3]);
	BaseDir = argv[4];
	PointsFileName = argv[1];

	// return 0;
	
#if IMAGE_TYPE_RAW
	std::string ImageName = BaseDir + ".raw";
	std::cout << ImageName << std::endl;

	unsigned char* temp = new unsigned char[1280*1080];
	FILE* fp = fopen(ImageName.c_str(), "rb");
	fread(temp, 1280*1080, 1, fp);

	cv::Mat img(1280, 1080, CV_8UC1, temp);
	// cv::resize(img, img, cv::Size(540, 640), 0, 0, INTER_AREA);

#else
	std::string ImageName = BaseDir + ".png";
	std::cout << ImageName << std::endl;
	cv::Mat Image = cv::imread(ImageName);

	cv::Mat img(1280, 1080, CV_8UC1);

	for (int i = 0; i < img.rows; i++) {
		for (int j = 0; j < img.cols; j++) {
			img.at<uchar>(i, j) = Image.at<cv::Vec3b>(i, j)[0];
		}
	}


#endif


	fstream pintsFile;
	pintsFile.open(PointsFileName.c_str(), ios::app);
	pintsFile << ImageName << " ";
	pintsFile.close();

	mouse_drawing_demo(img);
	waitKey(0);


	destroyAllWindows();
 
	std::cout << "end\n";
	return 0;
 
}