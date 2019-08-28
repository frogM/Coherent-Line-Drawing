#include "ETF.h"
#include "postProcessing.h"
#include <io.h>      
#include <string>    
#include <vector>    
#include <iostream>
#include <direct.h>


using namespace std;

void getAllFiles(string path, vector<string>& files, string format="")
{
	intptr_t  hFile = 0;//文件句柄  64位下long 改为 intptr_t
	struct _finddata_t fileinfo;//文件信息 
	string p;
	if ((hFile = _findfirst(p.assign(path).append("\\*" + format).c_str(), &fileinfo)) != -1) //文件存在
	{
		do
		{
			if ((fileinfo.attrib & _A_SUBDIR))//判断是否为文件夹
			{
				if (strcmp(fileinfo.name, ".") != 0 && strcmp(fileinfo.name, "..") != 0)//文件夹名中不含"."和".."
				{
					files.push_back(p.assign(path).append("\\").append(fileinfo.name)); //保存文件夹名
					getAllFiles(p.assign(path).append("\\").append(fileinfo.name), files, format); //递归遍历文件夹
				}
			}
			else
			{
				files.push_back(fileinfo.name);//如果不是文件夹，储存文件名
			}
		} while (_findnext(hFile, &fileinfo) == 0);
		_findclose(hFile);
	}
}

cv::Mat get_ETF(cv::Mat &src, const int &ETF_kernel, const int &iter_num)
{

    PP processing(cv::Size(256, 256));
    ETF etf;
    etf.initial_ETF(src, src.size());
    for (size_t i = 0; i < iter_num; i++)
    {
        etf.refine_ETF(ETF_kernel);
    }

    cv::Mat dis;
    processing.ETF(etf.flowField, dis);
    dis.convertTo(dis, CV_8UC1, 255);

    return dis;
}


int main(int argc, char const *argv[])
{
    if (argc!=5)
    {
        std::cout << "Please input as follows: " << std::endl;
        std::cout << argv[0] << "[kernel size] [iter_times] [input_file_path] [output_file_path]" << std::endl;
        //exit(0);
    }
	int ETF_kernel = atoi(argv[1]);// 1; //
	int iter_times = atoi(argv[2]);//2;// 
	string in_fold_path = argv[3];//"F:\\skyworks\\Coherent-Line-Drawing\\data";//
	string out_fold_path = argv[4];//"F:\\skyworks\\Coherent-Line-Drawing\\tmp_out";//
    cout << ETF_kernel<<endl;
    cout << iter_times << endl;
    cout << in_fold_path<< endl;
    cout << out_fold_path << endl;
	vector<string> files;
	getAllFiles(in_fold_path, files);

	if (_access(out_fold_path.c_str(), 0) == -1)
	{
		int flag = _mkdir(out_fold_path.c_str());
		if (flag == 0)
		{
			cout << "Build new folder " << out_fold_path << " successfully" << endl;
		}
		else {
			cout << "Build new folder " << out_fold_path << " errorly" << endl;
			exit(0);
		}
	}

	size_t size = files.size();
	for (int i = 0; i < size; i++)
	{
		cout << "Processing  " << files[i] << endl;

		cv::Mat src = cv::imread(in_fold_path + "\\" + files[i], cv::ImreadModes::IMREAD_COLOR);
		cv::Mat dis = get_ETF(src, ETF_kernel, iter_times);
		cv::imwrite(out_fold_path + "\\" + files[i], dis);
		cv::imshow("files[i]", dis);
		cv::waitKey(3);
	}

	cout << "Done" << endl;
	cin.get();
	return 0;
}
