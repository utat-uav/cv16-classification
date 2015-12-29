// SoftmaxRegression.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <map>
#include <unordered_map>
#include <fstream>
#include <cstdio>

using namespace cv;
using namespace std;


//--------------------------------------------------------------------
// FILE MANAGER
//--------------------------------------------------------------------

class FileManager
{
private:
	vector<string> files;
	void organizeFolders();

public:
	FileManager(string folder);
	
	unordered_map<string, vector<string>> organizedFolders;
};

FileManager::FileManager(string folder)
{
	FILE* pipe = NULL;
	string pCmd = "dir /B /S " + string(folder);
	char buf[256];

	if (NULL == (pipe = _popen(pCmd.c_str(), "rt")))
	{
		cout << "Shit" << endl;
		return;
	}

	while (!feof(pipe))
	{
		if (fgets(buf, 256, pipe) != NULL)
		{
			files.push_back(string(buf));
		}
	}

	_pclose(pipe);

	organizeFolders();
}

void FileManager::organizeFolders()
{
	// Get folders
	for (vector<string>::iterator it = files.begin(); it != files.end(); ++it)
	{
		it->pop_back(); // Removes \n character

		if (it->find('.') == string::npos)
		{

			// Initialize
			organizedFolders[*it] = vector<string>();
		}
		else
		{
			// Check which folder it belongs to if it isn't a folder
			for (unordered_map<string, vector<string>>::iterator folder = organizedFolders.begin(); folder != organizedFolders.end(); ++folder)
			{
				// Search *it for the folder name
				if (it->find(folder->first) != string::npos)
				{
					organizedFolders[folder->first].push_back(*it);
				}
			}
		}
	}
}

void resaveImages(string outputFolder, FileManager& files)
{
	for (unordered_map<string, vector<string>>::iterator folder = files.organizedFolders.begin(); folder != files.organizedFolders.end(); ++folder)
	{
		int count = 0;
		for (vector<string>::iterator file = folder->second.begin(); file != folder->second.end(); ++file)
		{
			if (file->at(file->length() - 1) == 'g' && file->at(file->length() - 2) == 'n' && file->at(file->length() - 3) == 'p')
			{
				Mat image;
				image = imread(*file, IMREAD_COLOR); // Read the file

				// Resave folder
				string finalFileName = outputFolder + folder->first.at(folder->first.length() - 1) + "_" + to_string(count) + ".png";
				cout << "Writing: " << finalFileName << endl;
				imwrite(finalFileName, image);
				count++;
			}
		}
	}

	cout << "Done reformatting images" << endl;
}

//--------------------------------------------------------------------
// MAIN
//--------------------------------------------------------------------

int main(int argc, char** argv)
{
	FileManager files("D:\\Downloads\\training2\\upper");

	//resaveImages("D:\\Downloads\\training2\\allInOne\\", files);

	return 0;
}

