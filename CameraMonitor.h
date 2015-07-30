//Copyright(C) 2015 Liwei Wu
//
//This program is free software : you can redistribute it and / or modify
//it under the terms of the GNU General Public License as published by
//the Free Software Foundation; either version 3 of the License, or
//(at your option) any later version.
//
//This program is distributed in the hope that it will be useful,
//but WITHOUT ANY WARRANTY; without even the implied warranty of
//MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.See the
//GNU General Public License for more details./>
//
//You should have received a copy of the GNU General Public License
//along with this program.If not, see <http://www.gnu.org/licenses>

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