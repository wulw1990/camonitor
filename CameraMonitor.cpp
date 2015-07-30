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

#include "CameraMonitor.h"

#include <fstream>
#include <iomanip>
#include <time.h>

static string exec(string cmd)
{
	FILE* pipe = popen(cmd.c_str(), "r");
	if (!pipe) return "ERROR";
	char buffer[128];
	string result = "";
	while (!feof(pipe)) {
		if (fgets(buffer, 128, pipe) != NULL)
			result += buffer;
	}
	pclose(pipe);
	return result;
}
static string int2String(int i){
	stringstream ss;
	ss << i;
	return ss.str();
}
static string getCurrentTimeString()
{
    time_t rawtime;
    struct tm * timeinfo;

    time ( &rawtime );
    timeinfo = localtime ( &rawtime );

    stringstream result;
    result << setfill('0');
    result << setw(4) << 1900+timeinfo->tm_year;
    result << "-" << setw(2) << 1+timeinfo->tm_mon;
    result << "-" << setw(2) << timeinfo->tm_mday;
    result << "-" << setw(2) << timeinfo->tm_hour;
    result << "-" << setw(2) << timeinfo->tm_min;
    result << "-" << setw(2) << timeinfo->tm_sec;

    return result.str();
}
static string time2String(
	int year, int mon, int day,
	int hour, int min, int sec)
{
    stringstream result;
    result << setfill('0');
    result << setw(4) << year;
    result << "-" << setw(2) << mon;
    result << "-" << setw(2) << day;
    result << "-" << setw(2) << hour;
    result << "-" << setw(2) << min;
    result << "-" << setw(2) << sec;

    return result.str();
}
static string data2String(
	int year, int mon, int day)
{
    stringstream result;
    result << setfill('0');
    result << setw(4) << year;
    result << "-" << setw(2) << mon;
    result << "-" << setw(2) << day;

    return result.str();
}
static void string2Time(
	string str,
	int& year, int& mon, int& day,
	int& hour, int& min, int& sec)
{
	int index = 0;
	year = atoi(str.substr(index, 4).c_str());
	index += 5;
	mon = atoi(str.substr(index, 2).c_str());
	index += 3;
	day = atoi(str.substr(index, 2).c_str());
	index += 3;
	hour = atoi(str.substr(index, 2).c_str());
	index += 3;
	min = atoi(str.substr(index, 2).c_str());
	index += 3;
	sec = atoi(str.substr(index, 2).c_str());
}

CameraMonitor::Configer::Configer(string config_file)
{
    FileStorage fs(config_file, FileStorage::READ);
    if (!fs.isOpened()){
        cerr << "Could not open the configuration file: \"" << config_file << "\"" << endl;
    	exit(-1);
    }
    fs["DebugFlag" ] >> debug_flag;

    if(debug_flag) cout << "-------------------Configer : ---------------------" << endl;
	if(debug_flag) cout << "DebugFlag : " << debug_flag << endl;

    fs["CameraID" ] >> camera_id;
	if(debug_flag) cout << "CameraID : " << camera_id << endl;

	fs["OutputPath" ] >> output_path;
	if(output_path[output_path.length()-1]!='/') output_path += "/";
	if(debug_flag) cout << "OutputPath : " << output_path << endl;

	fs["IntervalMS" ] >> internal_ms;
	if(debug_flag) cout << "IntervalMS : " << internal_ms << endl;

	fs["MaxGB" ] >> max_gb;
	if(debug_flag) cout << "MaxGB : " << max_gb << endl;

	fs["DownScaleRate" ] >> downscale;
	if(debug_flag) cout << "DownScaleRate : " << downscale << endl;

	fs["ShowVideo" ] >> show_video;
	if(debug_flag) cout << "ShowVideo : " << show_video << endl;

    fs.release(); 
	if(debug_flag) cout << "--------------------------------------------------------" << endl;
}
vector<string> CameraMonitor::Configer::readStringList(FileStorage& fs, string node_name)
{
	vector<string> list;
	FileNode node = fs[node_name];
	FileNodeIterator it = node.begin(), it_end = node.end();
	for( ; it != it_end; ++it )
		list.push_back((string)*it);
	return list;
}
void CameraMonitor::Configer::printList(vector<string>& list, string list_name)
{
	cout << list_name << " :" << endl;
	for(int i=0; i<(int)list.size(); ++i)
		cout << "\t" << list[i] << endl;
}

CameraMonitor::CameraMonitor(string config_file)
{
	m_configer = new Configer(config_file);

	exec((string("mkdir -p ")+m_configer->output_path).c_str());
	exec((string("mkdir -p ")+m_configer->output_path + "data/").c_str());
	ofstream fout;
	fout.open((m_configer->output_path + "log.txt").c_str(), std::fstream::out | std::fstream::app);
	if(!fout.is_open()){
		cerr << "cannot access :" << m_configer->output_path << endl;
		exit(-1);
	}
	fout << getCurrentTimeString() << " : begin" << endl;
	fout.close();
}
CameraMonitor::~CameraMonitor()
{
	delete m_configer;
}
void CameraMonitor::work()
{
	VideoCapture capture;
	capture.open(m_configer->camera_id);
	if(!capture.isOpened()){
		cerr << "can not open camera : " << m_configer->camera_id << endl;
		cerr << "program exit!" << endl;
		exit(-1);
	}

	Mat frame;
	string last_time_str;
	int count;
	while(1){
		bool save_flag = false;
		Mat save_mat;
		for(int i=0; i<m_configer->internal_ms/40; i++){
			capture.read(frame);
			if(judge(frame)){
				save_flag = true;
				save_mat = frame.clone();
			}
			if(m_configer->show_video) imshow("video", frame);
			waitKey(40);
		}
		capture.read(frame);

		if(save_flag){
			string time_str = getCurrentTimeString();
			if(time_str==last_time_str){
				count++;
			}
			else{
				last_time_str = time_str;
				count = 1;
			}
			string name = m_configer->output_path + "data/" + time_str + "_" + int2String(count) + ".jpg";
			imwrite(name, save_mat);
			cout << name << endl;
		}
	}
}
bool CameraMonitor::judge(Mat frame)
{
	Mat gray;
	if(frame.type()==CV_8UC3)
		cvtColor(frame, gray, CV_BGR2GRAY);
	else
		gray = frame.clone();

	if(m_pre_gray.empty()){
		m_pre_gray = gray;
		return true;
	}

	const int rows = gray.rows;
	const int cols = gray.cols;

	vector<Rect> rect;
	const int r_step = rows / 10;
	const int c_step = cols / 10;
	for(int r=0; r+r_step<rows; r+=r_step){
		for(int c=0; c+c_step<cols; c+=c_step){
			rect.push_back(Rect(c, r, c_step, r_step));
		}
	}

	double max_score = -1e6;
	for(int i = 0; i<(int)rect.size(); ++i){
		Mat cur = gray(rect[i]);
		Mat pre = m_pre_gray(rect[i]);
		double score = 0;
		for(int r=0; r<=cur.rows; ++r){
			for(int c=0; c<cur.cols; ++c){
				score += abs(cur.at<unsigned char>(r, c)-pre.at<unsigned char>(r, c));
			}
		}
		score /= cur.rows * cur.cols;
		if(score>max_score)
			max_score = score;
	}
	// cout << max_score << endl;
	m_pre_gray = gray;

	return (max_score>5.0);
}