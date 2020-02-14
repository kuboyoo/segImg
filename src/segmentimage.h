#ifndef SEGMENT_IMAGE
#define SEGMENT_IMAGE

#include <QVector>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

using namespace std;
using namespace cv;

typedef struct {
    int a, b;
    double w;
} edge;
inline bool compWeight(const edge &a, const edge &b);

class universe;
class SegmentImage {

public:
    SegmentImage(Mat image, int **_sp_label, int _num_of_superpixs);

    double threshold(double size, double c);
    void segmentGraph(double tau);
    int** getLabel(int h, int w);
    
private:
    inline void addHist(double ***hist, int i, Vec3b p);
    void makeHist(double ***hist, int **l, Mat lab_image);
    void normalizeHist(double ***hist, int num);
    inline bool isIn(int x, int y, int w, int h);
    inline bool isEdge(int a, int b, QVector<edge> &e);
    double chiSqrDist(int a, int b, double ***hist);
    double getCurvature(int a, int b, int **l, QVector<QVector<Point> > contours, int h, int w);
    void makeContours(QVector<QVector<Point> > &c, int **l, int n, int h, int w);
    void makeGraph(double ***hist, int **l, Mat I);

    QVector<edge> edges; //Region Imageの辺

    //定数
    const int BIN = 20; //Region Histgramのbinの数

    //変数
    universe *u;
    int **sp_label;
    int **label;
    int num_of_superpixs;
    double *thre;
    QVector<QVector<Point> > contours;
    

};
#endif