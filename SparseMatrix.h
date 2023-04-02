#include <map>
#include <vector>

class SparseMatrix{
private:
    std::map<int, float> matrix;
    int numRows, numCols;
public:
    SparseMatrix();
    SparseMatrix(int _numRows, int _numCols);
    SparseMatrix(std::vector<std::vector<float>>);
    float getSparseValue(int row, int column);
    void setSparseValue(int row, int column, float value);
};

SparseMatrix::SparseMatrix(){
    this->numRows = 0;
    this->numCols = 0;
}

SparseMatrix::SparseMatrix(int _numRows, int _numCols){
    this->numRows = _numRows;
    this->numCols = _numCols;
}

//i * numCols + j
SparseMatrix::SparseMatrix(std::vector<std::vector<float>> mat){
    this->numCols = mat.at(0).size();
    this->numRows = mat.size();
    for (int i = 0; i < this->numRows; i++) {
        for (int j = 0; j < this->numCols; j++) {
            if (mat.at(i).at(j) != 0.0) {      
                matrix.at(i*this->numRows+j) = mat.at(i).at(j);
            }
        }
    }
}

float SparseMatrix::getSparseValue(int row, int column){
    return matrix.at(row*this->numRows+column);
}

void SparseMatrix::setSparseValue(int row, int column, float value){
    if(value == 0.0) return;
    matrix.at(row*this->numRows+column) = value;
}