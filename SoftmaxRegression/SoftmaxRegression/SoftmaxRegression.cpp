#include "stdafx.h"

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <map>
#include <unordered_map>
#include <fstream>
#include <cstdio>
#include <math.h>
#include <time.h>
#include <stdlib.h>

using namespace cv;
using namespace std;

#define LABEL_SIZE 26
#define DATA_SIZE 400
#define TRAINING_STEP 0.006
#define TARGET_CROSS_ENTROPY 0.0001


//--------------------------------------------------------------------
// INPUT IMAGE CLASS
//--------------------------------------------------------------------

class InputImage
{
private:
	Mat image;
	vector<int> label;
	bool valid;

public:
	InputImage(string path);
	vector<int> getVectorizedImage();
	Mat* getImage();
	bool isValid();
	vector<int>* getLabelVector();
};

InputImage::InputImage(string path)
{
	// Load image
	image = imread(path, CV_LOAD_IMAGE_GRAYSCALE);

	// Check data validity
	if (image.empty() || image.rows != 20 || image.cols != 20)
	{
		valid = false;
		return;
	}

	// Make binary (1 channel with a 0 or 255 value)
	threshold(image, image, 128.0, 255.0, THRESH_BINARY);

	// Reshape image into 400 rows and 1 column
	image = image.reshape(0, DATA_SIZE);

	valid = true;

	// Get label character
	int charIndex = path.find_last_of('\\');
	char character = path.at(charIndex + 1);

	// Get label
	int labelArray[LABEL_SIZE] = { 0 };
	labelArray[character - 'A'] = 1;

	// Push label (one-hot encoded vector)
	label = vector<int>(labelArray, labelArray + sizeof(labelArray) / sizeof(int));

	// Concatenate a 1 to the end of the matrix
	Mat one = Mat::ones(Size(1, 1), CV_8UC1);
	vconcat(image, one, image);

	// Convert to floats
	image.convertTo(image, CV_32FC1);

	for (int i = 0; i < image.rows; i++)
	{
		image.at<float>(i, 0) = image.at<float>(i, 0) >= 1 ? 1 : 0;
	}

	//cout << image << endl;
}

bool InputImage::isValid()
{
	return this->valid;
}

// Returns a pointer to the matrix
Mat* InputImage::getImage()
{
	return &(this->image);
}

vector<int>* InputImage::getLabelVector()
{
	return &(this->label);
}

// Returns a vector version of the image
vector<int> InputImage::getVectorizedImage()
{
	vector<int> vectorized;

	for (int i = 0; i < image.rows; i++)
	{
		vectorized.push_back((int) image.at<uchar>(i, 0));
	}

	return vectorized;
}


//--------------------------------------------------------------------
// TRAINING SET CLASS
//--------------------------------------------------------------------

class TrainingSet
{
private:
	vector<InputImage *> data;

public:
	TrainingSet(string path);
	~TrainingSet();

	vector<InputImage *>* getData();
	int getDataSize();
};

TrainingSet::TrainingSet(string path)
{
	// Get file names
	vector<string> files;

	FILE* pipe = NULL;
	string pCmd = "dir /B /S " + string(path);
	char buf[256];

	// Attempt opening directory
	if (NULL == (pipe = _popen(pCmd.c_str(), "rt")))
	{
		cout << "Shit" << endl;
		return;
	}

	// Get file names
	while (!feof(pipe))
	{
		if (fgets(buf, 256, pipe) != NULL)
		{
			files.push_back(string(buf));
		}
	}

	// Close
	_pclose(pipe);

	// Import data
	for (vector<string>::iterator file = files.begin(); file != files.end(); ++file)
	{
		// Remove \n character
		file->pop_back();

		InputImage *inputImage = new InputImage(*file);

		// Validity check
		if (!inputImage->isValid())
		{
			cout << path << " is not valid data" << endl;
			continue;
		}

		// Add to data
		data.push_back(inputImage);
	}

	// Testing
	//vector<int> vectorizedImage = data.at(5)->getVectorizedImage();
	//vector<int> *label = data.at(5)->getLabelVector();
	/*
	Mat *mat = data.at(20)->getImage();
	*mat = mat->reshape(0, 20);
	imwrite("test.jpg", *mat);
	*/
}

TrainingSet::~TrainingSet()
{
	// Deallocate data set
	for (vector<InputImage *>::iterator it = data.begin(); it != data.end(); ++it)
	{
		delete (*it);
	}
}

int TrainingSet::getDataSize()
{
	return this->data.size();
}

vector<InputImage *>* TrainingSet::getData()
{
	return &(this->data);
}


//--------------------------------------------------------------------
// NN CLASS
//--------------------------------------------------------------------

class NeuralNetwork
{
private:
	Mat parameters;

public:
	NeuralNetwork();
	void train(TrainingSet &trainingSet);

};

NeuralNetwork::NeuralNetwork()
{
	// ROWS X COLUMNS initialization
	// DATA_SIZE + 1 for bias parameters
	parameters = Mat(LABEL_SIZE, DATA_SIZE + 1, DataType<float>::type);

	// Create rand number generator seed based on time
	srand(time(NULL));

	// (EXPERIMENTAL) Initalize all parameters to somewhat random number
	for (int i = 0; i < parameters.rows; i++)
	{
		for (int j = 0; j < parameters.cols; j++)
		{
			int randNumber = rand()%100;
			parameters.at<float>(i, j) = (float) randNumber / (float) 1000000;
		}
	}
}

void NeuralNetwork::train(TrainingSet &trainingSet)
{
	vector<InputImage *>* data = trainingSet.getData();

	// Repeat until convergence
	bool hasConverged = false;
	while (!hasConverged)
	{
		for (vector<InputImage *>::iterator trainingImage = data->begin(); trainingImage != data->end(); ++trainingImage)
		{
			Mat *trainingImageMat = (*trainingImage)->getImage();
			vector<int> *actualLabel = (*trainingImage)->getLabelVector();

			// Get V
			Mat V = parameters * (*trainingImageMat);

			// Compute softmax values
			vector<float> exponentials;
			float exponentialSum = 0;
			for (int i = 0; i < LABEL_SIZE; i++)
			{
				exponentials.push_back(expf(V.at<float>(i, 0)));
				exponentialSum += exponentials[i];
			}

			// Compute predictions
			vector<float> predictions;
			for (int i = 0; i < LABEL_SIZE; i++)
			{
				predictions.push_back(exponentials[i] / exponentialSum);
			}

			// Compute cross entropy
			float crossEntropy = 0;
			for (int i = 0; i < LABEL_SIZE; i++)
			{
				crossEntropy -= actualLabel->at(i) * logf(predictions[i]);
			}
			cout << crossEntropy << endl;
			if (crossEntropy <= TARGET_CROSS_ENTROPY)
			{
				hasConverged = true;
				break;
			}

			// Update Parameters
			for (int i = 0; i < parameters.rows; i++)
			{
				for (int j = 0; j < parameters.cols; j++)
				{
					float delta = TRAINING_STEP * (trainingImageMat->at<float>(j, 0)) * (actualLabel->at(i) - predictions[i]);
					parameters.at<float>(i, j) += delta;
				}
			}
		}
	}

	//cout << parameters << endl;
}


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


//--------------------------------------------------------------------
// MAIN
//--------------------------------------------------------------------

// Prototypes
void credits();
void resaveImages(string outputFolder, FileManager& files);
void train(string path);
void run(string path);

// PROGRAM START POINT
int main(int argc, char** argv)
{
	credits();

	if (argc < 2)
	{
		cout << "Insufficient arguments. Exiting program..." << endl;
		return -1;
	}

	string mode = argv[1];

	// Command inputs
	if (mode == "-t")
	{
		cout << "// COMMENCE TRAINING" << endl;
		string path = argv[2];

		train(path);

		cout << "Press enter to continue..." << endl;
		cin.ignore();
	}
	else if (mode == "-r")
	{
		cout << "// COMMENCE RUNNING" << endl;
		string path = argv[2];

		run(path);
	}
	else if (mode == "-help")
	{
		cout << "Press enter to continue..." << endl;
		cin.ignore();
	}

	// Reformats training set
	//FileManager files("D:\\Downloads\\training2\\upper");
	//resaveImages("D:\\Downloads\\training2\\allInOne\\", files);

	return 0;
}

/*
* Program initialization
*/
void credits()
{
	cout << "------------------------------------------------" << endl;
	cout << "// UTAT-UAV COMPUTER VISION CLASSIFICATION V1 \\\\" << endl;
	cout << "------------------------------------------------" << endl;
	cout << endl;
	cout << "Authors: Davis Wu" << endl;
	cout << "Creation Date: Dec 30, 2015" << endl;
	cout << "Info: attempts to classify characters" << endl;
	cout << endl;
	cout << "Command line info: [-t <folderpath>] to train program" << endl;
	cout << "                   [-r <folderpath>] to run pre-trained program" << endl;
	cout << "                   [-help me] for help" << endl;
	cout << endl;
}

void resaveImages(string outputFolder, FileManager& files)
{
	cout << "Resaving images..." << endl;

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
				imwrite(finalFileName, image);
				count++;
			}
		}
	}

	cout << "Done reformatting images" << endl;
}

void train(string path)
{
	// Initialize
	cout << "Importing and formatting training set..." << endl;
	TrainingSet trainingSet(path);
	NeuralNetwork neuralNetwork;
	
	// Train
	cout << "Training commencing..." << endl;
	neuralNetwork.train(trainingSet);
}

void run(string path)
{

}
