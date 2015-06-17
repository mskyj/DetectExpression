#include"Header.h"
#include"MyOpencvLibMD.h"
#include"Labeling.h"
#include"svm.h"
#include"Histogram.h"
#include"Calc.h"
#include<time.h>

int labels[13];
struct svm_model *model;
vector<Symbol> symbolList;
vector<Express> expressList;

void DetectNumber(cv::Mat image);
void DetectExpression(cv::Mat image);

int main(int argc, char** argv)
{
	char* modelPath = "mnist.model";
	cv::Mat frame,image;

	// モデルデータのロード
	if( (model=svm_load_model(modelPath))==0 )
	{
		cerr << "can't open model file " << modelPath << endl;
		return 1;
	}

	// モデルチェック
	if(svm_check_probability_model(model)==0)
	{
		cerr << "Model does not support probabiliy estimates\n" << endl;
		return 1;
	}

	svm_get_labels(model,labels);

	
	cv::namedWindow("test", CV_WINDOW_AUTOSIZE);

	cv::VideoCapture cap(0);
	// カメラ設定...
	cap.set(CV_CAP_PROP_FRAME_WIDTH, 640);
	cap.set(CV_CAP_PROP_FRAME_HEIGHT, 480);
	// カメラがオープンできたかの確認
	if(!cap.isOpened()) return -1;

	/*
	string outPath = "test.avi";
	double outputFPS = 10.0;
	cv::Size outputSize = cv::Size(640,480);
	cv::VideoWriter writer(outPath, CV_FOURCC_DEFAULT, outputFPS, outputSize);
	*/

	char imagePath[256];
	int index = 0;

	// メイン処理
	while(1)
	{
		clock_t start,end,detect_start,detect_end;

		start = clock();

		cap >> frame;
		if(frame.empty()) break;

		cv::cvtColor(frame, image, CV_RGB2GRAY);
		
		detect_start = clock();

		DetectNumber(image);

		detect_end = clock();

		cout << "Detect time : " << detect_end - detect_start << " ms" << endl;

		cv::cvtColor(image, image, CV_GRAY2RGB);
		DetectExpression(image);

		end = clock();

		//cout << "Process time : " << end - start << " ms" << endl;

		char fpsStr[128];
		sprintf_s(fpsStr, "%2.1ffps", 1000.0/(end-start));
		cv::putText(image, fpsStr, cv::Point(500,50), cv::FONT_ITALIC, 1.2, cv::Scalar(0,255,0), 3, CV_AA);

		cv::imshow("test", image);
		//writer << image;

		sprintf_s(imagePath,"C:\\Users\\Yuji\\Desktop\\image\\%d.png",index);
		cv::imwrite(imagePath,image);
		++index;

		if( clock()-start < 95 )
			cv::waitKey( 100-clock()+start );
		else
			cv::waitKey(5);

		// ×ボタンで終了
		void* life = cvGetWindowHandle( "test" );
		if( life == NULL )
			break;
	}

//	writer.release();

	// 後処理
	cv::destroyWindow("test");
	
	svm_free_and_destroy_model(&model);

	return 0;
}

void DetectNumber(cv::Mat image)
{
	unsigned char predict_label;
	double prob_estimates[13];
	double sorted_result[13];
	struct svm_node node[16*HIST_SIZE];

	int width = image.cols;
	int height = image.rows;

	// 2値化用に補正
	cv::Mat tempImg,tempImg2,thresholdImg;
	cv::threshold(image, thresholdImg,  130, 255, cv::THRESH_BINARY);
	cv::erode(thresholdImg, tempImg2, cv::Mat(), cv::Point(-1,-1), 2);
	cv::dilate(tempImg2, tempImg, cv::Mat(), cv::Point(-1,-1), 1);
	cv::namedWindow("binary", CV_WINDOW_AUTOSIZE);
	cv::imshow("binary", tempImg);

	// 2値化
	cv::Mat binaryImg;
	cv::threshold(tempImg, binaryImg, 100, 1, CV_THRESH_BINARY_INV);
	unsigned char* src = binaryImg.data;

	// ラベリング
	LabelingBS labeling;
	short *result = new short[width*height];
	labeling.Exec(src,result,width,height,false,30);
	int labelNum = labeling.GetNumOfResultRegions();

	if( labelNum>50 )
	{
		cout << "too much" << endl;
		return;
	}

	// ラベルに関する処理いろいろ
	for(int i=0; i<labelNum; ++i)
	{
		// 領域情報取得
		RegionInfoBS *ri;
		ri = labeling.GetResultRegionInfo(i);
		int ltopX,ltopY,rbottomX,rbottomY;
		ri->GetMin(ltopX, ltopY);
		ri->GetMax(rbottomX, rbottomY);
		int number = ri->GetResult();
		if( ri->GetNumOfPixels()<300 || ri->GetNumOfPixels()>10000 )
			continue;

		int regionW = rbottomX - ltopX + 1;
		int regionH = rbottomY - ltopY + 1;
		int size = (int)((double)max(regionW, regionH)*1.33);
		size += (7-size%7);		

		// 数字領域を抽出+補正
		cv::Mat numImg(size, size, CV_8U, 255);
		cv::Mat fixed_numImg(size, size, CV_8U, 255);
		cv::Mat fixed_numImg2(28, 28, CV_8U, 255);
		int gapX = (size-regionW)/2;
		int gapY = (size-regionH)/2;
		for(int y=0; y<regionH; ++y)
			for(int x=0; x<regionW; ++x)
				if(result[x+ltopX+(y+ltopY)*width]==number)
					numImg.data[x+gapX+(y+gapY)*size] = image.data[x+ltopX+(y+ltopY)*width];
		//cv::erode(numImg, fixed_numImg, cv::Mat(), cv::Point(-1,-1),1);
		cv::resize(numImg,fixed_numImg2,fixed_numImg2.size());

		// ヒストグラム作成
		for(int j=0; j<28*28; ++j)
			fixed_numImg2.data[j] = 255 - fixed_numImg2.data[j];
		Histogram hist(fixed_numImg2.data);
		hist.Make();

		int index = 0;
		for(int j=0; j<4; ++j)
			for(int i=0; i<4; ++i)
				for(int k=0; k<HIST_SIZE; ++k)
					if(hist.result[i][j][k]>0.0001)
					{
						node[index].index = (j*4+i)*HIST_SIZE+k+1;
						node[index].value = hist.result[i][j][k];
						++index;
					}
		if(index<16*HIST_SIZE)
			node[index].index = -1;

		predict_label = (int)svm_predict_probability(model,node,prob_estimates);

		for(int i=0; i<13; ++i)
			sorted_result[labels[i]] = prob_estimates[i];

		double percent = sorted_result[predict_label];

		/*
		// 領域描画
		cv::rectangle(image, cv::Point(ltopX-1,ltopY-1), cv::Point(rbottomX+1, rbottomY+1), cv::Scalar(0,0,0), 1, CV_AA);

		// 判定描画
		char str[128];
		sprintf_s(str, "%d(%3.1f%%)", (int)predict_label,percent*100.0);
		cv::putText(image, str, cv::Point((ltopX+rbottomX)/2-10,ltopY), cv::FONT_ITALIC, 1.2, cv::Scalar(0,0,0), 2, CV_AA);
		*/

		symbolList.push_back(Symbol(predict_label,ltopX,rbottomX,ltopY,rbottomY));
	}
}

void DetectExpression(cv::Mat image){

	// 座標が左から順番になるようにソート
	sort(symbolList.begin(),symbolList.end());
	
	for(int i=0; i<symbolList.size(); ++i){
		int j;
		// 左隣に式があるかどうか
		for(j=0; j<expressList.size(); ++j)
			if( expressList[j].isSymbol(symbolList[i]) ){
				expressList[j].addSymbol(symbolList[i]);
				break;
			}
		if( j!=expressList.size() )
			continue;

		// 分数式かどうか
		for(j=0; j<expressList.size(); ++j)
			if( expressList[j].isFraction(symbolList[i]) ){
				expressList[j].addFraction(symbolList[i],expressList.size());
				expressList.push_back(Express(symbolList[i]));
				break;
			}
		if( j!=expressList.size() )
			continue;

		// 指数かどうか
		for(j=0; j<expressList.size(); ++j)
			if( expressList[j].isEx(symbolList[i]) ){
				expressList[j].addEx(symbolList[i],expressList.size());
				expressList.push_back(Express(symbolList[i]));
				break;
			}
		if( j!=expressList.size() )
			continue;

		expressList.push_back(Express(symbolList[i]));
	}
	int index;
	for(int i=0; i<expressList.size(); ++i)
		while( (index = expressList[i].str.find("exp"))!=string::npos ){
			int expIndex = stoi(expressList[i].str.substr(index+3,3));
			expressList[i].str.replace(index, 6, expressList[expIndex].str.c_str());
			expressList[expIndex].partFlag = true;
			expressList[i].updateSize(expressList[expIndex]);
		}

	for(int i=0; i<expressList.size(); ++i)
		if( !expressList[i].partFlag ){
			Express ex = expressList[i];
			cv::rectangle(image, cv::Point(ex.left-1,ex.top-1), cv::Point(ex.right+1, ex.bottom+1), cv::Scalar(0,0,0), 1, CV_AA);
			char str[128];
			sprintf_s(str, "%s=%f", ex.str.c_str(),Calc(ex.str));
			cv::putText(image, str, cv::Point(ex.left,ex.bottom+35), cv::FONT_ITALIC, 1.2, cv::Scalar(0,0,255), 2, CV_AA);
		}

	symbolList.clear();
	expressList.clear();
}