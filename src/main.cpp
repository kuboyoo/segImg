#include "main.h"
#include "SeedsRevised.h"
#include "Tools.h"
#include "mainwindow.h"

int main(int argc, char *argv[])
{
    //Qt Application 起動
    QApplication app(argc, argv);

    
    QString selFilter = QObject::tr("画像(*.jpg *.png)");
    QString filename = QFileDialog::getOpenFileName(nullptr,
        QObject::tr("ファイルを開く"),
        "C:/Qt/Qt5.10.0",
        QObject::tr("すべて(*.*);;画像(*.jpg *.png)")
    );
    if (filename.isEmpty()) {
        printf("filename is empty !\n");
        exit(1);
    }
    
    std::string input_path = filename.toStdString();

    //std::string input_path = "../../datasets/ORIGINAL/figure_pair/00000.jpg"; //For debug

    Mat img = imread(input_path, 1);
    if(img.empty()){
        printf("file cannnot load error.\n");
        exit(1);
    }
    width = img.cols;
    height = img.rows;
    Mat input_img, lab, mask, contour_img;

    //小さすぎたり、大きすぎたりする場合、リサイズを行う
    /*
    double aspect_ratio = height / (double)(width); //縦わる横
    
    
    if(width < 800 || height < 800){
        cv::resize(img, input_img, cv::Size(), 1708./width ,(1708./(double)height*aspect_ratio));
        //cv::resize(img, input_img, cv::Size(), 1.0 , 1.0);
    }

    else if(width > 1200 ||  height > 1200){
        cv::resize(img, input_img, cv::Size(), 1200.0/width ,(1200.0/(double)height*aspect_ratio));
    }

    else*/ 
        input_img = img.clone();

    int _width  = input_img.cols;
    int _height = input_img.rows;
    


    //SEEDSでSuperpixel画像を作成
    cvtColor(input_img, lab, COLOR_BGR2Lab);
    superpixels = (int)((_width*_width + _height*_height) * 0.00085207);
    SEEDSRevisedMeanPixels seeds(lab, superpixels, bin, neighSize, minConf, spatialWeight);
    seeds.initialize();
    seeds.iterate(iterations);
    int num_of_superpixs = seeds.getNumberOfSuperpixels();
    int **seeds_label = seeds.getLabels();
    int contour_color[] = {255, 255, 255}; //White
    contour_img = Draw::contourImage(seeds_label, input_img, contour_color);
/*
    num_superpixels = (int)((_width*_width + _height*_height) * 0.0015207);
    Ptr<SuperpixelSEEDS> seeds = createSuperpixelSEEDS(width, height, input_img.channels(), num_superpixels,
        num_levels, prior, num_histogram_bins, double_step);
    Mat converted;
    cvtColor(input_img, converted, COLOR_BGR2Lab);
    seeds->iterate(converted, num_iterations);
    int num_of_superpixs = seeds->getNumberOfSuperpixels();
    Mat seeds_label_mat;
    seeds->getLabels(seeds_label_mat);

    int **seeds_label, label;
    
    seeds_label = (int**)malloc(sizeof(int*) * _height);
    for(int i=0; i<_height; ++i) seeds_label[i] = (int*)malloc(sizeof(int) * _width);
    for(int j=0; j<_height; ++j){
        for(int i=0; i<_width; ++i){
            seeds_label[j][i] = seeds_label_mat.at<int>(j,i);
        }
    }

    seeds->getLabelContourMask(mask, false);
    contour_img.setTo(Scalar(255, 255, 255), mask);
*/
    MainWindow *w = new MainWindow(input_img, contour_img, seeds_label, num_of_superpixs);
    //w->setFixedSize(QSize(_width + 180, _height + 80));
    w->show();

    return app.exec();
}