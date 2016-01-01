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
#include <algorithm>
#include <amp.h>

#define LABEL_SIZE 62
#define DATA_SIZE 784
#define TRAINING_STEP 0.003
#define MIN_TRAIN_TIME 6000
#define TARGET_CROSS_ENTROPY 0.015
#define INFINITY 100000
#define BATCH_SIZE 401

using namespace concurrency;
using namespace cv;
using namespace std;


// -t "D:\Downloads\Images\Reformatted"  "D:\Downloads\training2\allinone" for training

// -r "D:\Downloads\training2\allInOne" "D:\Workspace\UAV\cv16-classification\SoftmaxRegression\SoftmaxRegression\savednnBAK2.txt" for testing

//--------------------------------------------------------------------
// INPUT IMAGE CLASS
//--------------------------------------------------------------------

class InputImage
{
private:
	Mat image;
	vector<int> label;
	char charLabel;
	int labelIndex;
	bool valid;
	void cropImage();

public:
	InputImage(string path, bool testImage = false);
	~InputImage();
	vector<int> getVectorizedImage();
	Mat* getImage();
	bool isValid();
	vector<int>* getLabelVector();
	char getCharLabel();
	int getLabelIndex();
	float **imageArray;

	static int charToOneHotIndex(char c);
	static int oneHotIndexToChar(int i);
};

InputImage::~InputImage()
{
	delete[] imageArray;
}

InputImage::InputImage(string path, bool testImage)
{
	// Load image
	image = imread(path, CV_LOAD_IMAGE_GRAYSCALE);

	if (testImage)
	{
		cropImage();
	}

	// Check data validity
	if (image.empty())
	{
		valid = false;
		return;
	}

	if (image.rows != 28 || image.cols != 28)
	{
		resize(image, image, Size(28, 28));
	}

	// Make binary (1 channel with a 0 or 255 value)
	threshold(image, image, 128.0, 255.0, THRESH_BINARY);

	// Reshape image into 400 rows and 1 column
	image = image.reshape(0, DATA_SIZE);

	valid = true;

	// Get label character
	int charIndex = path.find_last_of('\\');
	char character = path.at(charIndex + 1);
	charLabel = character;

	// Get label
	int labelArray[LABEL_SIZE] = { 0 };
	labelIndex = charToOneHotIndex(character);
	labelArray[labelIndex] = 1;

	// Push label (one-hot encoded vector)
	label = vector<int>(labelArray, labelArray + sizeof(labelArray) / sizeof(int));

	// Concatenate a 1 to the end of the matrix
	Mat one = Mat::ones(Size(1, 1), CV_8UC1);
	vconcat(image, one, image);

	// Convert to floats
	image.convertTo(image, CV_32FC1);

	imageArray = new float*[image.rows];
	for (int i = 0; i < image.rows; i++)
	{
		image.at<float>(i, 0) = image.at<float>(i, 0) >= 1 ? 0.5 : -0.5;
		imageArray[i] = &(image.at<float>(i, 0));
	}

	//cout << image << endl;
}

void InputImage::cropImage()
{
	// Convert colours
	threshold(image, image, 128, 255, CV_THRESH_BINARY);
	Mat imageCpy;
	image.copyTo(imageCpy);

	// Get bounding boxes
	int largestX = 0, smallestX = 100, largestY = 0, smallestY = 100;
	for (int i = 0; i < image.rows; i++)
	{
		for (int j = 0; j < image.cols; j++)
		{
			if (image.at<uchar>(i, j) > 0)
			{
				if (i < smallestX)
					smallestX = i;
				if (j < smallestY)
					smallestY = j;
				if (i > largestX)
					largestX = i;
				if (j > largestY)
					largestY = j;
			}
		}
	}
	Rect cropRect(smallestX, smallestY, largestX - smallestX + 1, largestY - smallestY + 1);

	// Modify rect for smallest possible square dimensions
	int diff;
	if (cropRect.width > cropRect.height)
	{
		diff = cropRect.width - cropRect.height;
		cropRect.y -= round((double)diff / 2);
		if (cropRect.y <= 0)
			cropRect.y = 0;
		cropRect.height += diff;
		if (cropRect.y + cropRect.height >= image.rows)
			cropRect.height = image.rows - cropRect.y - 1;
	}
	else
	{
		diff = cropRect.height - cropRect.width;
		cropRect.x -= round((double)diff / 2);
		if (cropRect.x <= 0)
			cropRect.x = 0;
		cropRect.width += diff;
		if (cropRect.x + cropRect.width >= image.cols)
			cropRect.width = image.cols - cropRect.x - 1;
	}


	// Crop image
	try
	{
		image = image(cropRect);
	}
	catch (Exception &e)
	{
		cout << "hmmm something went wrong" << endl;
	}

	// Resize
	resize(image, image, Size(28, 28));
}

int InputImage::getLabelIndex()
{
	return this->labelIndex;
}

char InputImage::getCharLabel()
{
	return this->charLabel;
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

int InputImage::charToOneHotIndex(char c)
{
	if (c >= '0' && c <= '9')
	{
		return c - '0';
	}
	else if (c >= 'a' && c <= 'z')
	{
		return c - 'a' + 10;
	}
	else if (c >= 'A' && c <= 'Z')
	{
		return c - 'A' + 36;
	}
}

int InputImage::oneHotIndexToChar(int i)
{
	// Check cases
	if (i >= 0 && i <= 9)
	{
		// is a number
		return '0' + i;
	}
	else if (i >= 10 && i <= 35)
	{
		// is a lowercase letter
		return 'a' + i - 10;
	}
	else if (i >= 36 && i <= 61)
	{
		// is a uppercase letter
		return 'A' + i - 36;
	}
}


//--------------------------------------------------------------------
// TRAINING SET CLASS
//--------------------------------------------------------------------

class TrainingSet
{
private:
	vector<InputImage *> data;

public:
	TrainingSet(string path, bool testSet = false);
	~TrainingSet();

	vector<InputImage *>* getData();
	int getDataSize();
};

TrainingSet::TrainingSet(string path, bool testSet)
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

		InputImage *inputImage = new InputImage(*file, testSet);

		// Validity check
		if (!inputImage->isValid())
		{
			cout << path << " is not valid data" << endl;
			continue;
		}

		// Add to data
		data.push_back(inputImage);
	}

	// Shuffle vector
	srand(time(0));
	random_shuffle(data.begin(), data.end());

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
	float **parameterPointers;
	void predictHelper(Mat &V, vector<float> &predictions);

public:
	NeuralNetwork();
	NeuralNetwork(string serializedPath);
	~NeuralNetwork();
	void train(TrainingSet &trainingSet);
	void test(TrainingSet &trainingSet);

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

	// Allocate pointers
	parameterPointers = new float*[(LABEL_SIZE)*(DATA_SIZE + 1)];

	// Assign pointers for fast access
	for (int i = 0; i < parameters.rows; i++)
	{
		for (int j = 0; j < parameters.cols; j++)
		{
			parameterPointers[i*(DATA_SIZE + 1) + j] = &(parameters.at<float>(i, j));
		}
	}
}

NeuralNetwork::NeuralNetwork(string serializedPath)
{
	parameters = Mat(LABEL_SIZE, DATA_SIZE + 1, DataType<float>::type);

	ifstream serializedNN;
	serializedNN.open(serializedPath);

	// Loads NN parameters from file
	for (int i = 0; i < parameters.rows; ++i)
	{
		for (int j = 0; j < parameters.cols; ++j)
		{
			float val;
			serializedNN >> val;
			parameters.at<float>(i, j) = val;
		}
	}

	serializedNN.close();

	// Allocate pointers
	parameterPointers = new float*[(LABEL_SIZE)*(DATA_SIZE + 1)];

	// Assign pointers for fast access
	for (int i = 0; i < parameters.rows; i++)
	{
		for (int j = 0; j < parameters.cols; j++)
		{
			parameterPointers[i*(DATA_SIZE + 1) + j] = &(parameters.at<float>(i, j));
		}
	}
}

NeuralNetwork::~NeuralNetwork()
{
	delete[] parameterPointers;
}

void NeuralNetwork::predictHelper(Mat &V, vector<float> &predictions)
{
	vector<float> exponentials(LABEL_SIZE);
	float exponentialSum = 0;

	// Compute softmax values
	//#pragma omp parallel for reduction(+:exponentialSum)
	for (int i = 0; i < LABEL_SIZE; i++)
	{
		exponentials[i] = expf(V.at<float>(i, 0));
		if (isinf(exponentials[i]))
			exponentials[i] = INFINITY;
		exponentialSum += exponentials[i];
	}

	if (exponentialSum == 0)
	{
		exponentialSum = 0.0000001;
	}

	// Compute predictions
	//#pragma omp parallel for
	for (int i = 0; i < LABEL_SIZE; i++)
	{
		//predictions.push_back(exponentials[i] / exponentialSum);
		predictions[i] = exponentials[i] / exponentialSum;
		if (isinf(predictions[i]) || predictions[i] > 1)
			predictions[i] = 1;
		else if (predictions[i] < 0)
			predictions[i] = 0;
	}
}

void NeuralNetwork::train(TrainingSet &trainingSet)
{
	vector<InputImage *>* data = trainingSet.getData();

	vector<float> G;

	// Repeat until convergence
	bool hasConverged = false;
	int count = 0;
	float avgCrossEntropy = 100;
	time_t timer;
	time(&timer);
	int k = 0;
	while (!hasConverged)
	{
		if (count > MIN_TRAIN_TIME)
		{
			hasConverged = true;
			break;
		}
		count++;

		if (count % 5 == 0)
		{
			cout << count << "th cycle with " << avgCrossEntropy << " avg cross entropy" << endl;
			cout << difftime(time(0), timer) << " seconds elapsed" << endl;
		}

		// Reset average crossentropy
		avgCrossEntropy = 0;

		// Get predictions
		vector<vector<float>> allPredictions;
		vector<InputImage *> inputImages;
		for (int m = k; m < k + BATCH_SIZE; ++m)
		{
			int ind = m % data->size();

			Mat *trainingImageMat = data->at(ind)->getImage();
			vector<int> *actualLabel = data->at(ind)->getLabelVector();

			// Get V
			Mat V = parameters * (*trainingImageMat);

			// Compute prediction
			vector<float> predictions(LABEL_SIZE);
			predictHelper(V, predictions);

			avgCrossEntropy -= (logf(predictions[data->at(ind)->getLabelIndex()]));

			allPredictions.push_back(predictions);
			inputImages.push_back(data->at(ind));
		}

		// Update parameters
		for (int i = 0; i < parameters.rows; ++i)
		{
			for (int j = 0; j < parameters.cols; ++j)
			{
				float grad = 0;
				#pragma omp parallel for reduction(+:grad)
				for (int p = 0; p < BATCH_SIZE; p++)
				{
					grad += inputImages.at(p)->getImage()->at<float>(j, 0) * (inputImages.at(p)->getLabelVector()->at(i) - allPredictions[p][i]);
				}

				parameters.at<float>(i, j) += TRAINING_STEP * grad;
			}
		}

		// Average the cross entropy
		avgCrossEntropy /= BATCH_SIZE;

		k += BATCH_SIZE;
	}

	// Save to file
	ofstream nnsave;
	nnsave.open("savednn.txt");
	for (int i = 0; i < parameters.rows; ++i)
	{
		for (int j = 0; j < parameters.cols; ++j)
		{
			nnsave << parameters.at<float>(i, j) << "\t";
		}
		nnsave << endl;
	}
	nnsave << endl;
	nnsave.close();

	//cout << parameters << endl;
}

void NeuralNetwork::test(TrainingSet &testSet)
{
	vector<InputImage *>* data = testSet.getData();

	int numCorrect = 0;
	for (vector<InputImage *>::iterator testImage = data->begin(); testImage != data->end(); ++testImage)
	{
		Mat *trainingImageMat = (*testImage)->getImage();
		vector<int> *actualLabel = (*testImage)->getLabelVector();

		// Get V
		Mat V = parameters * (*trainingImageMat);

		// Compute prediction
		vector<float> predictions(LABEL_SIZE);
		predictHelper(V, predictions);

		// Find max for prediction
		float max = 0;
		int maxInd = 0;
		int count = 0;
		for (vector<float>::iterator it = predictions.begin(); it != predictions.end(); ++it)
		{
			if (*it > max)
			{
				max = *it;
				maxInd = count;
			}
			count++;
		}

		char predictedChar = InputImage::oneHotIndexToChar(maxInd);
		cout << "Predicted: " << predictedChar << " | Actual: " << (*testImage)->getCharLabel() << endl;
		if (tolower(predictedChar) == tolower((*testImage)->getCharLabel()))
		{
			numCorrect++;
		}
	}

	float percentCorrect = ((float)numCorrect / (float)data->size()) * 100;
	cout << "Percent correct: " << (int)percentCorrect << "%" << endl;
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

	vector<string>* getFiles();
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

vector<string>* FileManager::getFiles()
{
	return &(this->files);
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
void fixOliversShit(string outputFolder, FileManager& files);
void train(string path, string testPath);
void run(string path, string serializedNN);

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
		cout << "// COMMENCE TRAINING" << endl << endl;
		string path = argv[2];
		string testPath = argv[3];

		train(path, testPath);
	}
	else if (mode == "-r")
	{
		cout << "// COMMENCE RUNNING" << endl << endl;
		string path = argv[2];
		string serializedPath = argv[3];

		run(path, serializedPath);
	}
	else if (mode == "-help")
	{
		cout << "Press enter to continue..." << endl;
		cin.ignore();
	}
	
	
	// Reformats training set
	//FileManager files("D:\\Downloads\\training2\\upper\\");
	//resaveImages("D:\\Downloads\\training2\\allinone\\", files);
	//fixOliversShit("D:\\Downloads\\Images\\Reformatted\\", files);

	//int test = InputImage::charToOneHotIndex('Z');
	//char test2 = InputImage::oneHotIndexToChar(test);

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
	cout << "Command line info: [-t <trainingset> <testset>] to train program" << endl;
	cout << "                   [-r <testset> <serializedNN>] to run pre-trained program" << endl;
	cout << "                   [-help me] for help" << endl;
	cout << endl;
}

void fixOliversShit(string outputFolder, FileManager& files)
{
	cout << "Fixing Oliver's shit..." << endl;

	vector<string>* filePaths = files.getFiles();

	int count = 0;
	for (vector<string>::iterator file = filePaths->begin(); file != filePaths->end(); ++file)
	{
		// Process string
		int slashLocation = file->find_last_of('\\');

		// Get file name
		string fileName = file->substr(slashLocation+1);

		// Get corresponding number
		string imgNumberStr = fileName.substr(3, 3);
		int imgNumber = stoi(imgNumberStr);

		char label;

		// Check cases
		if (imgNumber >= 1 && imgNumber <= 10)
		{
			// Image is a number
			label = '0' + imgNumber - 1;
		}
		else if (imgNumber >= 11 && imgNumber <= 36)
		{
			// Image is a capital letter
			label = 'A' + imgNumber - 11;
		}
		else if (imgNumber >= 37 && imgNumber <= 62)
		{
			// Image is a lowercase letter
			label = 'a' + imgNumber - 37;
		}
		else
		{
			label = ' ';
		}

		// Open file
		Mat image;
		image = imread(*file, IMREAD_COLOR); // Read the file

		// Convert colours
		cvtColor(image, image, CV_BGR2GRAY);
		threshold(image, image, 128, 255, CV_THRESH_BINARY);
		Mat imageCpy;
		image.copyTo(imageCpy);

		// Get bounding boxes
		int largestX = 0, smallestX = 100, largestY = 0, smallestY = 100;
		for (int i = 0; i < image.rows; i++)
		{
			for (int j = 0; j < image.cols; j++)
			{
				if (image.at<uchar>(i, j) > 0)
				{
					if (i < smallestX)
						smallestX = i;
					if (j < smallestY)
						smallestY = j;
					if (i > largestX)
						largestX = i;
					if (j > largestY)
						largestY = j;
				}
			}
		}
		Rect cropRect(smallestX, smallestY, largestX - smallestX + 2, largestY - smallestY + 2);

		// Modify rect for smallest possible square dimensions
		int diff;
		if (cropRect.width > cropRect.height)
		{
			diff = cropRect.width - cropRect.height;
			cropRect.y -= round((double) diff / 2);
			if (cropRect.y <= 0)
				cropRect.y = 0;
			cropRect.height += diff;
			if (cropRect.y + cropRect.height >= image.rows)
				cropRect.height = image.rows - cropRect.y - 1;
		}
		else
		{
			diff = cropRect.height - cropRect.width;
			cropRect.x -= round((double)diff / 2);
			if (cropRect.x <= 0)
				cropRect.x = 0;
			cropRect.width += diff;
			if (cropRect.x + cropRect.width >= image.cols)
				cropRect.width = image.cols - cropRect.x - 1;
		}
		
		
		// Crop image
		try
		{
			image = image(cropRect);
		}
		catch (Exception &e)
		{
			cout << "hmmm something went wrong" << endl;
		}

		// Resize
		resize(image, image, Size(28, 28));

		string finalFileName = outputFolder + label + "_" + to_string(count) + ".png";
		imwrite(finalFileName, image);
		count++;
	}
}

void resaveImages(string outputFolder, FileManager& files)
{
	cout << "Resaving images..." << endl;

	for (unordered_map<string, vector<string>>::iterator folder = files.organizedFolders.begin(); folder != files.organizedFolders.end(); ++folder)
	{
		int count = 500;
		for (vector<string>::iterator file = folder->second.begin(); file != folder->second.end(); ++file)
		{
			if ((file->at(file->length() - 1) == 'g' && file->at(file->length() - 2) == 'n' && file->at(file->length() - 3) == 'p') ||
				(file->at(file->length() - 1) == 'g' && file->at(file->length() - 2) == 'p' && file->at(file->length() - 3) == 'j'))
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

void train(string path, string testPath)
{
	// Initialize
	cout << "Importing and formatting training set..." << endl;
	TrainingSet trainingSet(path);
	TrainingSet testSet(testPath);
	NeuralNetwork neuralNetwork;
	
	// Train
	cout << "Training commencing..." << endl;
	neuralNetwork.train(trainingSet);
	cout << "Training complete" << endl;

	cout << "Testing..." << endl;
	neuralNetwork.test(testSet);
}

void run(string path, string serializedNN)
{
	cout << "Importing test images..." << endl;
	TrainingSet testSet(path, true);
	cout << "Initializing NN..." << endl;
	NeuralNetwork neuralNetwork(serializedNN);

	cout << "Testing..." << endl;
	neuralNetwork.test(testSet);
}
