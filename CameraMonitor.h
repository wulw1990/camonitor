#ifndef _CAMERA_MONITOR_H_
#define _CAMERA_MONITOR_H_

#include <iostream>
#include <string>
using namespace std;

#include <opencv2/opencv.hpp>
using namespace cv;

class CameraMonitor
{
private:
	class Configer
	{
	public:
		Configer(string config_file);
		int debug_flag;
		int camera_id;
		string output_path;
		int internal_ms;
		double max_gb;
		double downscale;
		int show_video;
	private:
		vector<string> readStringList(FileStorage& fs, string node_name);
		void printList(vector<string>& list, string list_name);

	};
	Configer* m_configer;
	Mat m_pre_gray;
	bool judge(Mat frame);

public:
	CameraMonitor(string config_file);
	~CameraMonitor();

	void work();
};

#endif