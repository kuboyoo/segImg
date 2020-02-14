#include "segmentimage.h"
#include "disjoint-set.h"

SegmentImage::SegmentImage(Mat image, int **_sp_label, int _num_of_superpixs)
{
    num_of_superpixs = _num_of_superpixs;
    sp_label = _sp_label;
    label = (int**)malloc(sizeof(int*) * image.rows);
    for(int i=0, h=image.rows; i<h; ++i) label[i] = (int*)malloc(sizeof(int) * image.cols);

    //メモリ確保
    u = new universe(num_of_superpixs);
    thre = (double*)malloc(sizeof(double) * num_of_superpixs);
    double ***hist = (double***)calloc(num_of_superpixs, sizeof(double**));
    for(int j=0; j<num_of_superpixs; j++){
        hist[j] = (double**)calloc(BIN, sizeof(double*));
        for(int i=0; i<BIN; i++) hist[j][i] = (double*)calloc(3, sizeof(double));
    }

    Mat lab_image;
    cvtColor(image, lab_image, CV_BGR2Lab);

    makeHist(hist, sp_label, lab_image);
    normalizeHist(hist, num_of_superpixs);
    makeContours(contours, sp_label, num_of_superpixs, image.rows, image.cols);
    makeGraph(hist, sp_label, lab_image);

    //ヒストグラムのメモリは開放
    for(int i=0; i<num_of_superpixs; ++i){
        for(int j=0; j<BIN; ++j){
            free(hist[i][j]);
        }
        free(hist[i]);
    }
    free(hist);
}

inline void SegmentImage::addHist(double ***hist, int i, Vec3b p){
    uchar bin;
    for(int j=0; j<3; ++j){
        bin = p[j] / (256 / (float)BIN);
        hist[i][bin][j] += 1.;
    }
}

void SegmentImage::makeHist(double ***hist, int **l, Mat I){
    for(int j=0, h=I.rows; j<h; ++j)
        for(int i=0, w=I.cols; i<w; ++i)
            addHist(hist, l[j][i], I.at<Vec3b>(j,i));
}

void SegmentImage::normalizeHist(double ***hist, int num){
    double sum;

    for(int i=0; i<num; ++i){
        sum = 0.;
        for(int k=0; k<BIN; k++) sum += hist[i][k][0];
        u->size_sets(i, (int)sum);
        for(int j=0; j<3; ++j){
            for(int k=0; k<BIN; k++) hist[i][k][j] /= (double)(sum * 3.);
        }
    }
}

inline bool SegmentImage::isIn(int x, int y, int w, int h){
    return (x>=0 && x<w && y>=0 && y<h);
}

inline bool SegmentImage::isEdge(int a, int b, QVector<edge> &e){
    for(int i=e.size()-1; i>=0; --i){
        if((a == e.at(i).a && b == e.at(i).b)
        || (a == e.at(i).b && b == e.at(i).a)) return true;
    }
    return false;
}

double SegmentImage::chiSqrDist(int a, int b, double ***hist){
    double dist = 0.;
    double d; //差
    double s; //和

    //L*, a*, b*を結合してカイ２乗距離を計算
    for(int i=0; i<3; i++){
        for(int j=0; j<BIN; j++){
            d = hist[a][j][i] - hist[b][j][i];
            s = hist[a][j][i] + hist[b][j][i];

            if(s != 0.00) dist += d*d/(double)s;
        }
    }
    return dist * 0.5;
}

//領域ごとの境界データを取得 c:境界座標の点列, **l:Superpixelラベル行列, n:Superpixelの数, h:height, w:width
void SegmentImage::makeContours(QVector<QVector<Point> > &c, int **l, int n, int h, int w){
    Mat I, L, S, C; //Image, Label, Stats, Centroids
    int num, *p, region_w, region_h;
    vector<vector<Point> > contour;

    for(int k=0; k<n; ++k){
        I = Mat::zeros(h, w, CV_8UC1); //初期化
        //2値化処理
        for(int j=0; j<h; ++j)
            for(int i=0; i<w; ++i)
                if(l[j][i] == k)
                    I.at<uchar>(j,i) = 255;

        //バウンディングボックスの縦幅, 横幅を計算
        num = connectedComponentsWithStats(I, L, S, C);
        for(int i=0; i<num; ++i){
            p = S.ptr<int>(i);
            region_w = p[ConnectedComponentsTypes::CC_STAT_WIDTH];
            region_h = p[ConnectedComponentsTypes::CC_STAT_HEIGHT];
        }

        //ひも状の領域(横幅=1 or 縦幅=1)はfindContour()しない
        if(region_w > 1 && region_h > 1){
            findContours(I, contour, RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);
            c.append(QVector<Point>::fromStdVector(contour.at(0)));
        } else {
            c.append({{0,0}}); //ひも状のSuperpixelは境界の点を1つとし，例外処理で弾く
        }

    }
}

//領域rが領域リストvに登録されているか確認
inline bool isRegion(int r, QVector<int> *v){
    for(int i=v->size()-1; i>=0; --i){
        if(v->at(i) == r) return true;
    }
    //その色が登録済みでなければ
    return false;
}

//境界形状の複雑さを計算する
double SegmentImage::getCurvature(int a, int b, int **l, QVector<QVector<Point> > contours, int h, int w){
    QVector<Point> a_contour = contours[a];
    QVector<Point> b_contour = contours[b];

    QVector<Point> p; //a,bが共有する境界の座標群

    int m, n, shift = 0, shifted_i; //端点の添字保存用
    //8近傍探索窓
    int dt[8][2] = { { 1, 0 }, { 1, 1 }, { 0, 1 }, {-1, 1 }, {-1, 0 }, {-1,-1 }, { 0,-1 }, { 1,-1 } };
    bool flag;
    QVector<int> region_list; //領域IDのリスト

    /* 端点の位置の種類
     * [1] 2領域a,b間の境界上にあるとき
     * [2] 2領域a,b間でない境界上にあるとき
     * aの輪郭を基準に探索を行う
     */

    //初期端点が[1]か[2]かをチェックする
    region_list.clear();
    for(int j=0; j<8; j++){
        m = a_contour[0].x + dt[j][0];
        n = a_contour[0].y + dt[j][1];
        if(isIn(m,n,w,h)){
            //もし登録済みでない領域であれば，リストに登録
            if(!isRegion(l[n][m], &region_list))
                region_list.append(l[n][m]);
        }
    }

    //もし，領域IDのリストにbが登録されていなければ，[1](ずらし幅headを計算)
    if(isRegion(b, &region_list)){
        //2領域間の境界の端点を探索(線形探索)
        for(int i=1; i<a_contour.size(); i++){
            region_list.clear();
            for(int j=0; j<8; j++){
                m = a_contour[i].x + dt[j][0];
                n = a_contour[i].y + dt[j][1];
                if(isIn(m,n,w,h)){
                    //もし登録済みでない領域であれば，リストに登録
                    if(!isRegion(l[n][m], &region_list))
                        region_list.append(l[n][m]);
                }
            }
            //窓内部の領域の種類が3以上であれば，端点の添字を保存
            if(region_list.size() > 2){
                shift = i;
                break;
            }
        }
    }

    for(int i=0; i<a_contour.size(); i++){
        flag = false;
        for(int j=0; j<8; j++){
            shifted_i = (i + shift) % a_contour.size();
            m = a_contour[shifted_i].x + dt[j][0];
            n = a_contour[shifted_i].y + dt[j][1];
            if(isIn(m,n,w,h)){
                if(l[n][m] == b){ 
                    flag = true;
                    break;
                }
            }
        }
        if(flag) p.append(a_contour.at(i));
    }
    if(p.size() <= 3) return 0;
    double total_curvature = 0.;

    //x,yのtに関する微分から全曲率を求める方法
    double dx, dy, ddx, ddy;
    for(int i=1; i<p.size()-1; i++){
        dx  = (p[i-1].x - p[i+1].x) / 2.;
        dy  = (p[i-1].y - p[i+1].y) / 2.;
        ddx = p[i+1].x + p[i-1].x - 2.*p[i].x;
        ddy = p[i+1].y + p[i-1].y - 2.*p[i].y;
        if((dx*dx + dy*dy) != 0.000)
            total_curvature += fabs(ddy*dx - ddx*dy) / (dx*dx + dy*dy);
    }

    return total_curvature;
}

void SegmentImage::makeGraph(double ***hist, int **l, Mat I){
    int a, b, ni, nj, neigh[4][2] = { {1,0}, {0,1}, {1,1}, {-1,1} };
    double curvature, c, d, w_denom, weight, s = 100., g = 0.5;

    edges.clear();
    for(int j=0, h=I.rows; j<h; ++j){
        for(int i=0, w=I.cols; i<w; ++i){
            a = l[j][i];
            for(int k=0; k<4; ++k){
                ni = i + neigh[k][0];
                nj = j + neigh[k][1];
                if(isIn(ni, nj, w, h)){
                    b = l[nj][ni];
                    if(a != b && !isEdge(a, b, edges)){
                        c = chiSqrDist(a, b, hist);
                        curvature = getCurvature(a, b, l, contours, h, w);
                        d = exp(-1. * curvature / s);
                        //printf("d: %f\n", d);
                        w_denom = g*c + (1.-g)*d;
                        if(w_denom == 0){
                            weight = 0;
                        } else weight = c*d / w_denom;
                        edges.append({ a, b, weight });
                        
                        //edges.append({a,b,c});
                    }

                }
            }

        }
    }

}

inline bool compWeight(const edge &a, const edge &b){ return (a.w > b.w); }
inline double SegmentImage::threshold(double size, double c){ return (c / size); }

void SegmentImage::segmentGraph(double tau){
    u->clear(num_of_superpixs);
    for(int i=0; i<num_of_superpixs; ++i) thre[i] = threshold(1,tau);
    sort(edges.begin(), edges.end(), compWeight);

    for(auto e: edges){
        edge *pedge = &e;
        int a = u->find(pedge->a);
        int b = u->find(pedge->b);

        if(a != b){
            if ((pedge->w <= thre[a]) && (pedge->w <= thre[b])) {
                u->join(a, b);
                a = u->find(a);
                thre[a] = pedge->w + threshold(u->size(a), tau);
            }
        }
    }

}

int** SegmentImage::getLabel(int h, int w){
    for(int j=0; j<h; ++j){
        for(int i=0; i<w; ++i){
            int now_label = sp_label[j][i];
            label[j][i] = u->find(now_label);
        }
    }
    return label;
}