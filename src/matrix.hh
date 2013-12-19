/*
 * Providing basic and advanced operations on matrix. 
 *
 * Attention: This class works only for a 0-1 matrix.
 */
#include <cstdio>
#include <cstdlib>
#include <cmath>
#include <cstring>

class matrix{
        int _rowCnt;
        int _columnCnt;
        int* _mat;
        int switchCol(int* mat,int rowCnt,int columnCnt,int col1,int col2);
        int switchRow(int* mat,int rowCnt,int columnCnt,int row1,int row2);
        /* add row2 to row1 */
        int addRow(int* mat,int rowCnt,int columnCnt,int row1,int row2);
        /* add col2 to col1 */
        int addCol(int* mat,int rowCnt,int columnCnt,int col1,int col2);
    public:
        matrix(int row,int column,int* mat){_rowCnt=row;_columnCnt=column;_mat=mat;};
        int rowRank();
        int showMatrix();
};

