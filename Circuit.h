#include <iostream>
#include <string>
#include <cstring>
#include <vector>
#include <map>
#include <fstream>
#include <sstream>
#include <iomanip>

//Netlist Struct
struct Branch{
    Branch(std::string _branchLabel, int _sourceNodeLabel, int _destinationNodeLabel, float _value);
    void printBranch(); 
    std::string branchLabel;
    int sourceNodeLabel, destinationNodeLabel;
    float value;
};

class Circuit{
    private:
        std::vector<Branch> netlist;
        std::vector<std::vector<float>> incidenceMatrix;
        std::vector<std::vector<float>> transposedIncidenceMatrix;
        std::vector<std::vector<float>> diagonalMatrix;
        std::vector<std::vector<float>> voltageMatrix;
        std::vector<std::vector<float>> currentMatrix;
        std::vector<std::vector<float>> combinedMatrix;
        //<row, col> val
        std::map<std::pair<int, int>, float> combinedSparseMatrix;
    public:
        Circuit();
        //I/O
        void readNetListFile(std::string name = "netlist.txt");
        void writeOutputFile(std::string name = "output.txt", int round = 3);

        //Netlist
        void printNetlist();
        int numberOfNodes();
        int numberOfBranches();

        //Matrices Formations
        void formIncidenceMatrix(bool reducedIncidenceMatrix = false);
        void formTransposedIncidenceMatrix(bool negative = false);
        void formDiagonalMatrix();
        void formVoltageCoefficientsMatrix();
        void formCurrentCoefficientsMatrix();
        int formCombinedMatrix();
        int formCombinedSparseMatrix();
        std::vector<float> formSolution(bool sparse = false);

        //Matrices Testing
        template<typename T>
        void printMatrix(std::vector<std::vector<T>> &matrix, std::string name = "");
        std::vector<std::vector<float>> printIncidenceMatrix();
        std::vector<std::vector<float>> printTransposedIncidenceMatrix();
        std::vector<std::vector<float>> printDiagonalMatrix();
        std::vector<std::vector<float>> printVoltageCoefficientsMatrix();
        std::vector<std::vector<float>> printCurrentCoefficientsMatrix();
        std::vector<std::vector<float>> printCombinedMatrix();
        std::map<std::pair<int, int>, float> printCombinedSparseMatrix();
        
        //matrix operations
        void subtractRows(std::vector<float> &first, std::vector<float> &second, float c);
        void swapRows(std::vector<float> &first, std::vector<float> &second);
        bool invertMatrix(std::vector<std::vector<float>> &a, std::vector<float> &x, std::vector<std::vector<float>> &b);
        //Sparse Matrix
        void swapSpareRows(std::map<std::pair<int, int>, float> &a, int row1, int row2);
        bool invertMatrix(std::map<std::pair<int, int>, float> &a, std::vector<float> &x, std::vector<std::vector<float>> &b);
};