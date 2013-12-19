#include "matrix.hh"

int matrix::rowRank(){
    int retVal=0;
    int* workSpace=(int*)calloc(_rowCnt*_columnCnt,sizeof(int));
    memcpy((char*)workSpace,(char*)_mat,_rowCnt*_columnCnt*sizeof(int));
    for(int i=0;i<_rowCnt;i++){
        /*
         * After the i-th round, the first i rows of the first i columns
         * of workSpace form an identical matrix
         */
        if(workSpace[i*_columnCnt+i]==1){
            for(int j=0;j<_rowCnt;j++){
                if(i!=j){
                    addRow(workSpace,_rowCnt,_columnCnt,j,i);
                }
            }
            retVal++;
        }else{
            int rowMark=-1;
            int colMark=-1;
            for(int j=i;j<_rowCnt;j++){
                for(int k=i;k<_columnCnt;k++){
                    if(workSpace[j*_columnCnt+k]==1){
                        rowMark=j;
                        colMark=k;
                        break;
                    }
                }
                if(rowMark!=-1){
                    break;
                }
            }
            if(rowMark==-1){
                /* all the elements in right bottom are 0s, return */
                return retVal;
            }else{
                switchCol(workSpace,_rowCnt,_columnCnt,i,rowMark);
                switchRow(workSpace,_rowCnt,_columnCnt,i,colMark);
                for(int j=0;j<_rowCnt;j++){
                    if(i!=j){
                        addRow(workSpace,_rowCnt,_columnCnt,j,i);
                    }
                }
                retVal++;
            }
        }
    }
    return retVal;
}

int matrix::addRow(int* mat,int rowCnt,int columnCnt,int row1,int row2){
    for(int i=0;i<columnCnt;i++){
        mat[row1*columnCnt+i]^=mat[row2*columnCnt+i];
    }
    return 0;
}

int matrix::switchRow(int* mat,int rowCnt,int columnCnt,int row1,int row2){
    if(row1==row2){
        return 0;
    }
    for(int i=0;i<columnCnt;i++){
        mat[row1*columnCnt+i]^=mat[row2*columnCnt+i];
        mat[row2*columnCnt+i]^=mat[row1*columnCnt+i];
        mat[row1*columnCnt+i]^=mat[row2*columnCnt+i];
    }
    return 0;
}

int matrix::addCol(int* mat,int rowCnt,int columnCnt,int col1,int col2){
    for(int i=0;i<rowCnt;i++){
        mat[i*columnCnt+col1]^=mat[i*columnCnt+col2];
    }
    return 0;
}

int matrix::switchCol(int* mat,int rowCnt,int columnCnt,int col1,int col2){
    if(col1==col2){
        return 0;
    }
    for(int i=0;i<rowCnt;i++){
        mat[i*columnCnt+col1]^=mat[i*columnCnt+col2];
        mat[i*columnCnt+col2]^=mat[i*columnCnt+col1];
        mat[i*columnCnt+col1]^=mat[i*columnCnt+col2];
    }
    return 0;
}

int matrix::showMatrix(){
    for(int i=0;i<_rowCnt;i++){
        for(int j=0;j<_columnCnt;j++){
            printf("%2d",_mat[i*_columnCnt+j]);
        }
        printf("\n");
    }
    printf("\n");
}



