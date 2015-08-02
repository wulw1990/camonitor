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

#include <string>
#include <vector>
#include <opencv2/opencv.hpp>
using namespace std;
using namespace cv;

string exec(string cmd)
{
#ifdef __linux__ 
#define _popen popen
#define _pclose pclose
#endif
    FILE* pipe = _popen(cmd.c_str(), "r");
    if (!pipe) return "ERROR";
    char buffer[128];
    string result = "";
    while (!feof(pipe)) {
        if (fgets(buffer, 128, pipe) != NULL)
            result += buffer;
    }
    _pclose(pipe);
    return result;
}
vector<string> getFileListInDir(string dir)
{
    vector<string> list;
#ifdef __linux__ 
    string result = exec(string("ls ") + dir);
#else
    string result = exec(string("dir /B ") + dir);
#endif

    stringstream ss;
    ss << result;
    string str;
    while (ss >> str)
        list.push_back(str);
    return list;
}
static void help()
{
    cout << "two parameters needed:" << endl;
    cout << "\t1) image dir" << endl;
    cout << "\t2) video path" << endl;
}
static void assertWithInfo(bool flag, string info)
{
    if(!flag){
        cerr << info << endl;
        assert(flag);
    }
}
int main(int argc, char** argv)
{
    if(argc!=3){
        help();
        exit(-1);
    }
    string input_path(argv[1]);
    string output_path(argv[2]);

    if(input_path[input_path.length()-1]!='/')
        input_path+="/";
    if(output_path[output_path.length()-1]!='/')
        output_path+="/";

    vector<string> list = getFileListInDir(input_path);
    assertWithInfo(!list.empty(), string("no file in: ")+input_path);

    Mat frame = imread(input_path + list[0]);
    assertWithInfo(!frame.empty(), string("file can not open: ")+input_path + list[0]);

    string video_name = output_path + list[0] + "__" + list[list.size()-1] + ".avi";
    VideoWriter writer(video_name, CV_FOURCC('M', 'J', 'P', 'G'), 25.0, frame.size());

    writer << frame;
    const int n_frames = (int)list.size();
    for(int i=1; i<n_frames; ++i){
        frame = imread(input_path + list[i]);
        writer << frame;
    }
    writer.release();
    return 0;
}