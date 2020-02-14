#ifndef MAIN_WINDOW
#define MAIN_WINDOW

#include <QWidget>
#include <QFileDialog>
#include <QMessageBox>
#include <QPushButton>
#include <QToolButton>
#include <QStateMachine>
#include <QLabel>
#include <QSlider>
#include <QPixmap>
#include <QPainter>
#include <QMouseEvent>
#include <QString>
#include <QVector>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QStyle>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/ximgproc.hpp> //For SEEDS
#include <iostream>

#include "segmentimage.h"

using namespace cv;
using namespace std;

class MainWindow : public QWidget
{
    Q_OBJECT
    
public:
    MainWindow(Mat input_img, Mat contour_img, int** _id, int _num_of_superpixs);

    void setTau(int new_tau);
    void setPenWidth(int new_width);

public slots:
    inline void setPenColorRed();
    inline void setPenColorGreen();
    inline void setPenColorBlue();
    inline void setPenColorCyan();
    inline void setPenColorMagenta();
    inline void setPenColorYellow();
    inline void setPenColor12();
    inline void setPenColor13();
    inline void setPenColor14();
    inline void setPenColor15();
    inline void setPenColor16();
    inline void setPenColor17();
    inline void setPenColor18();
    inline void setPenColor19();
    inline void setPenColor20();
    inline void setEraser();
    void setInputImage();
    void setContourImage();
    void setPaintedImage();
    void setLabelImage();
    
    void clearImage();

protected:
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    //void paintEvent(QPaintEvent *event) override;

private:
    void drawCircle(const QPoint &point);
    bool isRegistered(int r, QVector<int> *v);
    void drawLabel(const QPoint &point);
    QPixmap mat2pixmap(Mat &M);

    void writeMat(Mat &M);
    void saveFile();

    //ウインドウ全体
    QHBoxLayout *window_layout;

    //画像領域
    QVBoxLayout *img_main_layout;
    QGridLayout *img_sub_layout;
    QLabel *label_img;
    QSlider *slider_tau;
    QSlider *slider_radius;

    //画面表示
    QPixmap pixmap_input;
    QPixmap pixmap_contour;
    QPixmap pixmap_painted;
    QPixmap pixmap_label;
    QPixmap pixmap_dist;

    //ボタン領域
    QVBoxLayout *button_main_layout;
    QGridLayout *button_sub_layout;
    QPushButton *button1;
    QPushButton *button2;
    QPushButton *button3;
    QPushButton *button4;
    QPushButton *button5;
    QPushButton *button6;
    QPushButton *button7;
    QPushButton *button8;
    QPushButton *button9;
    QPushButton *button10;
    QToolButton *button11;
    QPushButton *button12;
    QPushButton *button13;
    QPushButton *button14;
    QPushButton *button15;
    QPushButton *button16;
    QPushButton *button17;
    QPushButton *button18;
    QPushButton *button19;
    QPushButton *button20;


    QStateMachine machine_seg;
    QState *on_seg;
    QState *off_seg;
    QStateMachine machine_paint;
    QState *off_paint;
    QState *on_paint;

    bool is_paint_on;
    bool is_seg_on;

    //円描画関連
    bool scribbling;
    int my_radius;
    QColor my_pen_color;
    QPoint release_point;

    //OpenCV関連
    Mat input_img, contour_img;
    Mat D1, D2; //描画用行列 
    Mat L1, L2; //ラベル行列
    Mat S;
    int **id;
    int num_of_superpixs;
    int pen_mode;
/*    
    Vec3b colors[7] = {
        Vec3b(0,0,255),
        Vec3b(0,255,0),
        Vec3b(255,0,0),
        Vec3b(255,255,0),
        Vec3b(255,0,255),
        Vec3b(0,255,255),
        Vec3b(255,255,255) //消しゴム用の色(白)
    };
*/

    Vec3b colors[23] = {
        Vec3b(0,0,0), Vec3b(0,0,255), Vec3b(0,255,0), Vec3b(0,255,255),
        Vec3b(255,0,0), Vec3b(255,0,255), Vec3b(255,255,0), Vec3b(255, 255, 255),
        Vec3b(0,0,128), Vec3b(0,128,0), Vec3b(0,128,128),  Vec3b(128,0,0),
        Vec3b(128,0,128), Vec3b(128,128,0), Vec3b(0,128,255), Vec3b(0,255,128),
        Vec3b(255,128,0), Vec3b(255,191,0), Vec3b(0,191,0), Vec3b(0,191,255),
        Vec3b(191,128,0), Vec3b(255,128,255), Vec3b(128,128,128) //消しゴム用の色(白)
    };
  
   /*
    Vec3b colors[7] = {
        Vec3b(0,0,0),
        Vec3b(0,0,0), 
        Vec3b(0,0,128), 
        Vec3b(0,128,0), 
        Vec3b(0,128,128),
        Vec3b(128,0,0),
        Vec3b(255,255,255) //消しゴム用の色(白)
    };
    */
    QVector<int> pointed_ids;

    //グラフ分割関連
    SegmentImage *s;

};
#endif
