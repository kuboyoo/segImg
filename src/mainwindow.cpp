#include "mainwindow.h"
#include "segmentimage.h"
#include "Tools.h"

MainWindow::MainWindow(Mat _input_img, Mat _contour_img, int **_id, int _num_of_superpixs){
    //画像・ラベル(領域ID)行列読み込み・初期化
    input_img = _input_img.clone();
    contour_img = _contour_img.clone();
    id = _id;
    num_of_superpixs = _num_of_superpixs;

    //Region Graph構築
    s = new SegmentImage(input_img, id, num_of_superpixs);

    D1 = input_img.clone();
    L1 = Mat::zeros(input_img.rows, input_img.cols, CV_8UC3);
    //L1 = input_img.clone();
    L1 = Mat::zeros(D1.size(), CV_8UC3);
    S = Mat::zeros(input_img.rows, input_img.cols, CV_32S); //教示されたラベル行列
    /*
    //ラベル画像を白で初期化する場合
    for(int j=0, h=input_img.rows; j<h; ++j){
        for(int i=0, w=input_img.cols; i<w; ++i){
            L1.at<Vec3b>(j,i) = Vec3b(255,255,255);
        }
    }
    */
    pixmap_input = mat2pixmap(_input_img);
    pixmap_contour = mat2pixmap(_contour_img);
    pixmap_painted = pixmap_contour;
    pixmap_label = mat2pixmap(L1);
    
    pen_mode = 1;

    //カーソル描画用変数初期化
    scribbling = false;
    my_radius = 30;
    my_pen_color = Qt::red;

    //画像領域のラベル作成
    label_img = new QLabel;

    //スライダー作成
    slider_tau = new QSlider(Qt::Horizontal);
    slider_radius = new QSlider(Qt::Horizontal);

    //ボタン作成
    button1 = new QPushButton("Segment Switch");
    button2 = new QPushButton("Paint Switch");
    button3 = new QPushButton("Reset");
    button4 = new QPushButton("Region 1");
    button5 = new QPushButton("Region 2");
    button6 = new QPushButton("Region 3");
    button7 = new QPushButton("Region 4");
    button8 = new QPushButton("Region 5");
    button9 = new QPushButton("Region 6");
    button10 = new QPushButton("Eraser");
    button11 = new QToolButton;
    button12 = new QPushButton("Region 7");
    button13 = new QPushButton("Region 8");
    button14 = new QPushButton("Region 9");
    button15 = new QPushButton("Region 10");
    button16 = new QPushButton("Region 11");
    button17 = new QPushButton("Region 12");
    button18 = new QPushButton("Region 13");
    button19 = new QPushButton("Region 14");
    button20 = new QPushButton("Region 15");
    
    button4->setStyleSheet("background-color:#FF0000;");
    button5->setStyleSheet("background-color:#00FF00;");
    button6->setStyleSheet("background-color:#FFFF00;");
    button7->setStyleSheet("background-color:#0000FF;");
    button8->setStyleSheet("background-color:#FF00FF;");
    button9->setStyleSheet("background-color:#00FFFF;");
    button10->setStyleSheet("background-color:gray;");
    button12->setStyleSheet("background-color:#FFFFFF;");
    button13->setStyleSheet("background-color:#800000;");
    button14->setStyleSheet("background-color:#008000;");
    button15->setStyleSheet("background-color:#808000;");
    button16->setStyleSheet("background-color:#000080;");
    button17->setStyleSheet("background-color:#800080;");
    button18->setStyleSheet("background-color:#008080;");
    button19->setStyleSheet("background-color:#FF8000;");
    button20->setStyleSheet("background-color:#80FF00;");

    QIcon icon = QIcon(style()->standardPixmap(QStyle::SP_DialogSaveButton));
    button11->setIcon(icon);
    button11->setText(QObject::tr("Save"));
    button11->setIconSize(QSize(140, 20));
    button11->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);

    //ON/OFFボタン
    off_seg = new QState();
    off_seg->assignProperty(button1, "text", "Segment OFF");
    on_seg = new QState();
    on_seg->assignProperty(button1, "text", "Segment On");
    off_seg->addTransition(button1, SIGNAL(clicked()), on_seg);
    on_seg->addTransition(button1, SIGNAL(clicked()), off_seg);
    machine_seg.addState(off_seg);
    machine_seg.addState(on_seg);

    off_paint = new QState();
    off_paint->assignProperty(button2, "text", "Paint OFF");
    off_paint->setObjectName("PaintOFF");
    on_paint = new QState();
    on_paint->setObjectName("PaintON");
    on_paint->assignProperty(button2, "text", "Paint On");
    off_paint->addTransition(button2, SIGNAL(clicked()), on_paint);
    on_paint->addTransition(button2, SIGNAL(clicked()), off_paint);
    machine_paint.addState(off_paint);
    machine_paint.addState(on_paint);

    machine_seg.setInitialState(off_seg);
    machine_seg.start();
    machine_paint.setInitialState(off_paint);
    machine_paint.start();

    //レイアウト作成
    window_layout = new QHBoxLayout;
    img_main_layout = new QVBoxLayout;
    img_sub_layout = new QGridLayout;
    button_main_layout = new QVBoxLayout;
    button_sub_layout = new QGridLayout;

    slider_radius->setSliderPosition(my_radius);

    img_sub_layout->addWidget(new QLabel(QObject::tr("Auto Seg Assist")), 0,0);
    img_sub_layout->addWidget(slider_tau, 0,1);
    img_sub_layout->addWidget(new QLabel(QObject::tr("Cursor Radius")), 1,0);
    img_sub_layout->addWidget(slider_tau, 0,1);
    img_sub_layout->addWidget(slider_radius, 1,1);
    img_main_layout->addWidget(label_img);
    img_main_layout->addLayout(img_sub_layout);
    
    button_main_layout->addWidget(button1);
    button_main_layout->addWidget(button2);
    button_main_layout->addWidget(button3);
    button_sub_layout->addWidget(button4, 0,0);
    button_sub_layout->addWidget(button5, 0,1);
    button_sub_layout->addWidget(button6, 1,0);
    button_sub_layout->addWidget(button7, 1,1);
    button_sub_layout->addWidget(button8, 2,0);
    button_sub_layout->addWidget(button9, 2,1);
    button_sub_layout->addWidget(button12, 3,0);
    button_sub_layout->addWidget(button13, 3,1);
    button_sub_layout->addWidget(button14, 4,0);
    button_sub_layout->addWidget(button15, 4,1);
    button_sub_layout->addWidget(button16, 5,0);
    button_sub_layout->addWidget(button17, 5,1);
    button_sub_layout->addWidget(button18, 6,0);
    button_sub_layout->addWidget(button19, 6,1);
    button_sub_layout->addWidget(button20, 7,0);
    button_sub_layout->addWidget(button10, 8,0,1,2,Qt::AlignTop);
    button_main_layout->addLayout(button_sub_layout);
    button_main_layout->addWidget(button11);
 
    window_layout->addLayout(img_main_layout);
    window_layout->addLayout(button_main_layout);

    this->setLayout(window_layout);

    //スライダー機能結合
    connect(slider_tau, &QSlider::valueChanged, this, &MainWindow::setTau);
    connect(slider_radius, &QSlider::valueChanged, this, &MainWindow::setPenWidth);

    //ボタン機能結合
    connect(on_seg, &QState::entered, this, &MainWindow::setContourImage);
    connect(off_seg, &QState::entered, this, &MainWindow::setLabelImage);
    connect(on_paint, &QState::entered, this, &MainWindow::setPaintedImage);
    connect(off_paint, &QState::entered, this, &MainWindow::setInputImage);

    connect(button3, &QPushButton::clicked, this, &MainWindow::clearImage);
    connect(button4, &QPushButton::clicked, this, &MainWindow::setPenColorRed);
    connect(button5, &QPushButton::clicked, this, &MainWindow::setPenColorGreen);
    connect(button6, &QPushButton::clicked, this, &MainWindow::setPenColorBlue);
    connect(button7, &QPushButton::clicked, this, &MainWindow::setPenColorCyan);
    connect(button8, &QPushButton::clicked, this, &MainWindow::setPenColorMagenta);
    connect(button9, &QPushButton::clicked, this, &MainWindow::setPenColorYellow);

   connect(button12, &QPushButton::clicked, this, &MainWindow::setPenColor12);
   connect(button13, &QPushButton::clicked, this, &MainWindow::setPenColor13);
   connect(button14, &QPushButton::clicked, this, &MainWindow::setPenColor14);
   connect(button15, &QPushButton::clicked, this, &MainWindow::setPenColor15);
   connect(button16, &QPushButton::clicked, this, &MainWindow::setPenColor16);
   connect(button17, &QPushButton::clicked, this, &MainWindow::setPenColor17);
   connect(button18, &QPushButton::clicked, this, &MainWindow::setPenColor18);
   connect(button19, &QPushButton::clicked, this, &MainWindow::setPenColor19);
   connect(button20, &QPushButton::clicked, this, &MainWindow::setPenColor20);


    connect(button10, &QPushButton::clicked, this, &MainWindow::setEraser);

    connect(button11, &QPushButton::clicked, this, &MainWindow::saveFile);
}

void MainWindow::setInputImage(){
    label_img->setPixmap(pixmap_input);
}

void MainWindow::setContourImage(){
    label_img->setPixmap(pixmap_contour);
}

void MainWindow::setPaintedImage(){
    label_img->setPixmap(pixmap_painted);
}

void MainWindow::setLabelImage(){
    label_img->setPixmap(pixmap_label);
}

void MainWindow::clearImage(){
    label_img->setPixmap(pixmap_contour);
    pixmap_painted = pixmap_contour;
    D1 = D2 = input_img.clone();
    L1 = input_img.clone();
    /*
    //ラベル画像を白で初期化する場合
    for(int j=0, h=input_img.rows; j<h; ++j){
        for(int i=0, w=input_img.cols; i<w; ++i){
            L1.at<Vec3b>(j,i) = Vec3b(255,255,255);
        }
    }
    */
    //L2 = L1.clone();
    pixmap_label = mat2pixmap(L1);
}

//ペンカラー変更SLOT関数
inline void MainWindow::setPenColorRed(){ 
    my_pen_color = Qt::red; 
    pen_mode = 1;    
}
inline void MainWindow::setPenColorGreen(){ 
    my_pen_color = Qt::green; 
    pen_mode = 2;    
}
inline void MainWindow::setPenColorBlue(){ 
    my_pen_color = Qt::blue;
    pen_mode = 3;
}
inline void MainWindow::setPenColorCyan(){ 
    my_pen_color = Qt::cyan;
    pen_mode = 4;
}
inline void MainWindow::setPenColorMagenta(){ 
    my_pen_color = Qt::magenta;
    pen_mode = 5;
}
inline void MainWindow::setPenColorYellow(){ 
    my_pen_color = Qt::yellow; 
    pen_mode = 6;
}
inline void MainWindow::setPenColor12(){
    my_pen_color = Qt::darkGray;
    pen_mode = 7;
}
inline void MainWindow::setPenColor13(){
    my_pen_color = Qt::darkGray;
    pen_mode = 8;
}
inline void MainWindow::setPenColor14(){
    my_pen_color = Qt::darkGray;
    pen_mode = 9;
}
inline void MainWindow::setPenColor15(){
    my_pen_color = Qt::darkGray;
    pen_mode = 10;
}
inline void MainWindow::setPenColor16(){
    my_pen_color = Qt::darkGray;
    pen_mode = 11;
}
inline void MainWindow::setPenColor17(){
    my_pen_color = Qt::darkGray;
    pen_mode = 12;
}
inline void MainWindow::setPenColor18(){
    my_pen_color = Qt::darkGray;
    pen_mode = 13;
}
inline void MainWindow::setPenColor19(){
    my_pen_color = Qt::darkGray;
    pen_mode = 14;
}
inline void MainWindow::setPenColor20(){
    my_pen_color = Qt::darkGray;
    pen_mode = 15;
}

inline void MainWindow::setEraser(){
    my_pen_color = Qt::gray;
    pen_mode = 22;
}

void MainWindow::setTau(int new_tau){
    s->segmentGraph(new_tau * 0.01);
    id = s->getLabel(input_img.rows, input_img.cols);
    int contour_color[] = {255, 255, 255};
    D2 = Draw::contourImage(id, D1, contour_color);
    pixmap_dist = mat2pixmap(D2);
    label_img->setPixmap(pixmap_dist);
}

void MainWindow::setPenWidth(int new_width){
    my_radius = new_width;
    drawCircle(release_point);
    label_img->setPixmap(pixmap_painted);
}

void MainWindow::drawCircle(const QPoint &point)
{
    pixmap_painted = pixmap_dist;
    QPainter painter(&pixmap_painted);
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.setPen(QPen(my_pen_color, 2, Qt::DashDotLine, Qt::RoundCap));
    painter.setBrush(QBrush(Qt::green, Qt::NoBrush));
    painter.drawEllipse(point, my_radius, my_radius);
}

bool MainWindow::isRegistered(int r, QVector<int> *v){
    for(int i=v->size()-1; i>=0; --i){
        if(v->at(i) == r) return true;
    }
    return false;
}

void MainWindow::drawLabel(const QPoint &point){
    //マウスポイントされた座標を中心とする円の内部の全領域IDを取得
    int pointed_id;
    int h = D1.rows;
    int w = D1.cols;
    int px = point.x();
    int py = point.y();
    int sx = max(0, px-my_radius);
    int sy = max(0, py-my_radius);
    int ex = min(px+my_radius, w);
    int ey = min(py+my_radius, h);
    for(int j=sy; j<ey; ++j){
        for(int i=sx; i<ex; ++i){
            if((px-i)*(px-i)+(py-j)*(py-j) < my_radius*my_radius){
                if(!isRegistered(id[j][i], &pointed_ids))
                    pointed_ids.append(id[j][i]);
                if(pen_mode == 22){
                    D1.at<Vec3b>(j,i) = input_img.at<Vec3b>(j,i);
                    L1.at<Vec3b>(j,i) = colors[0];
                }
            }
        }
    }
    double alpha = 0.5;
    for(int j=0; j<h; ++j){
        for(int i=0; i<w; ++i){
            for(auto e: pointed_ids){
                if(id[j][i] == e){
                    if(pen_mode != 22){
                        Vec3b pixel = input_img.at<Vec3b>(j,i);
                        D1.at<Vec3b>(j,i) = pixel * alpha + colors[pen_mode] * (1.-alpha);
                        L1.at<Vec3b>(j,i) = colors[pen_mode];
                        S.at<int>(j,i) = pen_mode;
                    }
                }
            }
        }
    }
    pointed_ids.clear();
    int contour_color[] = {255, 255, 255};
    D2 = Draw::contourImage(id, D1, contour_color);
    pixmap_dist = mat2pixmap(D2);
    L2 = L1.clone();
    pixmap_label = mat2pixmap(L2);
    
    //if(pen_mode == 7) label_img->setPixmap(pixmap_label);
    //else label_img->setPixmap(pixmap_dist);
}

//ラベル行列(整数)を書き出し
void MainWindow::writeMat(Mat &M){
    FILE *fp;
    if((fp = fopen("./00000.mat", "w")) == NULL) exit(1);

    Mat N = Mat::zeros(480, 854, CV_32S);

    for(int j=0, h=480; j<h; ++j){
        for(int i=0, w=854; i<w; ++i){

            N.at<int>(j, i) = M.at<int>(j*2,i*2);

        }
    }

    
    //最初の2行は行列の列数，行数で書き出し
    fwrite(&(N.cols), sizeof(int), 1, fp);
    fwrite(&(N.rows), sizeof(int), 1, fp);
    fwrite(N.data, sizeof(int), N.rows * N.cols, fp);
    
    fclose(fp);
}

//cv::Mat -> QPixmap の変換
QPixmap MainWindow::mat2pixmap(Mat &M){
    return QPixmap::fromImage(
                QImage((unsigned char*) M.data, M.cols, M.rows, static_cast<int>(M.step), QImage::Format_RGB888).rgbSwapped()
           );
}

//保存ボタン
void MainWindow::saveFile(){
    writeMat(S);
    QString fileName = QFileDialog::getSaveFileName(this,
        tr("Save Images"), "",
        tr("Images (*.png);;All Files (*)"));

    if (fileName.isEmpty())
        return;
    else {
        QFile file(fileName);
        if (!file.open(QIODevice::WriteOnly)) {
            QMessageBox::information(this, tr("Unable to open file"),
                file.errorString());
            return;
        }
        pixmap_label.save(&file, "PNG");
    }
}

//マウスイベント関連
void MainWindow::mousePressEvent(QMouseEvent *event){
    QPoint pos = event->pos() - QPoint(25, 25);
    if (event->button() == Qt::LeftButton) {
        scribbling = true;
        drawLabel(pos);
        drawCircle(pos);
        if(pen_mode != 22) label_img->setPixmap(pixmap_painted);
        else label_img->setPixmap(pixmap_label);
    }
}

void MainWindow::mouseMoveEvent(QMouseEvent *event){
    QPoint pos = event->pos() - QPoint(25, 25);
    if ((event->buttons() & Qt::LeftButton) && scribbling){
        drawLabel(pos);
        drawCircle(pos);
        if(pen_mode != 22) label_img->setPixmap(pixmap_painted);
        else label_img->setPixmap(pixmap_label);
    }
}

void MainWindow::mouseReleaseEvent(QMouseEvent *event){
    QPoint pos = event->pos();
    if (event->button() == Qt::LeftButton && scribbling) {
        scribbling = false;
        release_point = pos;
    }
}
