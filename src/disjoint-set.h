/*
Copyright (C) 2006 Pedro Felzenszwalb

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
*/

#ifndef DISJOINT_SET
#define DISJOINT_SET

// disjoint-set forests using union-by-rank and path compression (sort of).

typedef struct {
  int rank;
  int p;
  int size;
  int init_size;
} uni_elt;

class universe {
public:
  universe(int elements);
  ~universe();
  int find(int x);  
  void join(int x, int y);
  int size(int x) const { return elts[x].size; }
  int init_size(int x) const { return elts[x].init_size; }
  int num_sets() const { return num; }

  //xのヒストグラムにyのヒストグラムを上乗せ
  //void joinHist(int x, int y);

  //xの領域サイズをセット
  void size_sets(int x, int s){ 
    elts[x].size = s; 
    elts[x].init_size = s;  
  }

  //初期状態に戻す
  void clear(int elements);

private:
  uni_elt *elts;
  int num;
};

universe::universe(int elements) {
  elts = new uni_elt[elements];
  num = elements;
  for (int i = 0; i < elements; i++) {
    elts[i].rank = 0;
    elts[i].size = 1;
    elts[i].p = i;
  }
}
  
universe::~universe() {
  delete [] elts;
}

/*
//ヒストグラムの統合(aのヒストグラムにbのヒストグラムを上乗せ)
void universe::joinHist(int x, int y){
    for(int i=0; i<BIN; i++){
        hist[x][i][0] += hist[y][i][0];
        hist[x][i][1] += hist[y][i][1];
        hist[x][i][2] += hist[y][i][2];
    }
}
*/

//初期状態に戻す
void universe::clear(int elements){
  num = elements;
  for(int i=0; i<elements; i++){
    elts[i].rank = 0;
    elts[i].size = elts[i].init_size;
    elts[i].p = i;
  }
}

int universe::find(int x) {
  int y = x;
  while (y != elts[y].p)
    y = elts[y].p;
  elts[x].p = y;
  return y;
}

void universe::join(int x, int y) {
  if (elts[x].rank > elts[y].rank) {
    elts[y].p = x;
    elts[x].size += elts[y].size;
    //joinHist(x,y);
  } else {
    elts[x].p = y;
    elts[y].size += elts[x].size;
    //joinHist(y,x);
    if (elts[x].rank == elts[y].rank)
      elts[y].rank++;
  }
  num--;
}

#endif
