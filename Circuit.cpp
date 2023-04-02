#include "Circuit.h"
Branch::Branch(std::string _branchLabel, int _sourceNodeLabel, int _destinationNodeLabel, float _value): branchLabel(_branchLabel), sourceNodeLabel(_sourceNodeLabel), destinationNodeLabel(_destinationNodeLabel), value(_value){};
Circuit::Circuit(){}

void Circuit::readNetListFile(std::string name){
    std::ifstream inFile(name);
    if(inFile.is_open()) {
        std::string line;
        while(std::getline(inFile, line)) {
            std::istringstream stream(line);
            std::string token1, token2, token3, token4 = "";
            std::getline(stream, token1, ' ');
            std::getline(stream, token2, ' ');
            std::getline(stream, token3, ' ');
            std::getline(stream, token4, ' ');
            netlist.push_back(Branch(token1, std::stoi(token2), std::stoi(token3), std::stod(token4)));
        }
    }else std::cout << "Not open." << std::endl;
}

void Circuit::writeOutputFile(std::string name, int round){
    std::vector<float> x = formSolution();
    std::ofstream outFile(name);
    for (int i = 0; i < x.size(); i++){
        outFile << std::fixed << std::setprecision(round) << x.at(i) << ((i == x.size()) ? "" : " ");
    }
    outFile.close();
}

void Branch::printBranch(){
    std::cout << "branchLabel: " << branchLabel << 
        ", sourceNodeLabel: " << sourceNodeLabel << 
        ", destinationNodeLabel: " << destinationNodeLabel << 
        ", value: " << value << std::endl;
}

void Circuit::printNetlist(){
    std::cout << "netlist" << std::endl;
    for (unsigned int i = 0; i < netlist.size(); i++){
        netlist.at(i).printBranch();
    }
}

//Used to use varying types of matrices (double, int, float) 
template<typename T>
void Circuit::printMatrix(std::vector<std::vector<T>> &matrix, std::string name){
    std::cout << name << std::endl;
    for (unsigned int i = 0; i < matrix.size(); i++){
        for (unsigned int j = 0; j < matrix.at(i).size(); j++){
            std::cout << matrix.at(i).at(j) << " ";
        }
        std::cout << std::endl;
    }
}

std::vector<std::vector<float>> Circuit::printIncidenceMatrix(){
    printMatrix(incidenceMatrix, "incidenceMatrix");
    return incidenceMatrix;
}

std::vector<std::vector<float>> Circuit::printTransposedIncidenceMatrix(){
    printMatrix(transposedIncidenceMatrix, "transposedIncidenceMatrix");
    return transposedIncidenceMatrix;
}

std::vector<std::vector<float>> Circuit::printDiagonalMatrix(){
    printMatrix(diagonalMatrix, "diagonalMatrix");
    return diagonalMatrix;
}

std::vector<std::vector<float>> Circuit::printVoltageCoefficientsMatrix(){
    printMatrix(voltageMatrix, "voltageMatrix");
    return voltageMatrix;
}

std::vector<std::vector<float>> Circuit::printCurrentCoefficientsMatrix(){
    printMatrix(currentMatrix, "currentMatrix");
    return currentMatrix;
}

std::vector<std::vector<float>> Circuit::printCombinedMatrix(){
    printMatrix(combinedMatrix, "combinedMatrix");
    return combinedMatrix;
}

std::map<std::pair<int, int>, float> Circuit::printCombinedSparseMatrix(){
    std::cout << "Row | Col | Value\n";
    for (auto& element : combinedSparseMatrix) {
        std::cout 
        << element.first.first << " | "
        << element.first.second << " | "
        << element.second << std::endl;
    }
    return combinedSparseMatrix;
}

//finds the highest value of the labels and return +1 of that
int Circuit::numberOfNodes(){
    int highestComp = -1;
    for (unsigned int i = 0; i < netlist.size(); i++){
        int sourceNodeLabel = netlist.at(i).sourceNodeLabel;
        int destinationNodeLabel = netlist.at(i).destinationNodeLabel;
        if (sourceNodeLabel > highestComp) highestComp = sourceNodeLabel;
        if (destinationNodeLabel > highestComp) highestComp = destinationNodeLabel;
    }
    return highestComp + 1;
}

//Netlist is a vector of Branches
int Circuit::numberOfBranches(){
    return netlist.size();
}

//<first> - c * <second>
void Circuit::subtractRows(std::vector<float> &first, std::vector<float> &second, float c){
    if(first.size() == 0 || second.size() == 0 ||first.size() != second.size()) throw "size error";
    for (int i = 0; i < first.size(); i++){
        first.at(i) -= (c * second.at(i));
    }
}

//<second> <-> <first>
void Circuit::swapRows(std::vector<float> &first, std::vector<float> &second){
    std::vector<float> temp = first;
    first = second;
    second = temp;
}

void Circuit::swapSpareRows(std::map<std::pair<int, int>, float> &a, int row1, int row2){
    std::map<std::pair<int, int>, float> tempA = a;
    for (auto& element : tempA) {
        if(element.first.first == row1){
            std::cout << element.first.first << " " << element.first.second << "\n";
            a[{row2, element.first.second}] = element.second;
            auto it = a.find(element.first);
            a.erase(it);
        }else if(element.first.first == row2){
            a[{row1, element.first.second}] = element.second;
            auto it = a.find(element.first);
            a.erase(it);
        }
    }
}

void Circuit::formIncidenceMatrix(bool reducedIncidenceMatrix){
    incidenceMatrix.clear();
    //resize matrix
    incidenceMatrix.resize(numberOfNodes(), std::vector<float>(numberOfBranches()));
    //go through netlist and set labeled nodes 
    for (unsigned int i = 0; i < netlist.size(); i++){
        //Assume “1” is source and “-1” is destination
        incidenceMatrix.at(netlist.at(i).sourceNodeLabel).at(i) = 1.0;
        incidenceMatrix.at(netlist.at(i).destinationNodeLabel).at(i) = -1.0;
    }

    //erases/deletes first row
    if(reducedIncidenceMatrix){
        auto it = incidenceMatrix.begin();
        incidenceMatrix.erase(it);
    }
}

void Circuit::formTransposedIncidenceMatrix(bool negative){
    transposedIncidenceMatrix.clear();
    transposedIncidenceMatrix.resize(incidenceMatrix.at(0).size(), std::vector<float>(incidenceMatrix.size()));
    for (unsigned int i = 0; i < transposedIncidenceMatrix.size(); i++){
        for (unsigned int j = 0; j < transposedIncidenceMatrix.at(i).size(); j++){
            //forms a negative transposed array depesning on function param
            if (negative && incidenceMatrix.at(j).at(i) != 0.0) transposedIncidenceMatrix.at(i).at(j) = -incidenceMatrix.at(j).at(i);
            else transposedIncidenceMatrix.at(i).at(j) = incidenceMatrix.at(j).at(i);
        }
    }
}

void Circuit::formDiagonalMatrix(){
    diagonalMatrix.clear();
    diagonalMatrix.resize(numberOfBranches(), std::vector<float>(numberOfBranches()));
    for (unsigned int i = 0; i < diagonalMatrix.size(); i++){
        diagonalMatrix.at(i).at(i) = 1;
    }
}

void Circuit::formVoltageCoefficientsMatrix(){
    voltageMatrix.clear();
    voltageMatrix.resize(numberOfBranches(), std::vector<float>(1));
    for (unsigned int i = 0; i < netlist.size(); i++){
        if (netlist.at(i).branchLabel.at(0) == 'V' || netlist.at(i).branchLabel.at(0) == 'v') voltageMatrix.at(i).at(0) = netlist.at(i).value;
    }
}

void Circuit::formCurrentCoefficientsMatrix(){
    currentMatrix.clear();
    currentMatrix.resize(numberOfBranches(), std::vector<float>(numberOfBranches()));
    for (unsigned int i = 0; i < netlist.size(); i++){
        if (netlist.at(i).branchLabel.at(0) == 'R' || netlist.at(i).branchLabel.at(0) == 'r') currentMatrix.at(i).at(i) = -netlist.at(i).value;
    }
}

int Circuit::formCombinedMatrix(){
    //setup
    formIncidenceMatrix(true);
    formTransposedIncidenceMatrix(true);
    formCurrentCoefficientsMatrix();
    formDiagonalMatrix();
    combinedMatrix.clear();
    combinedMatrix.resize(incidenceMatrix.size()+transposedIncidenceMatrix.size()+currentMatrix.size(), std::vector<float>(incidenceMatrix.at(0).size()+transposedIncidenceMatrix.at(0).size()+currentMatrix.at(0).size()));
    int colIndex = 0, rowIndex = 0;
    //A
    for (int i = 0; i < incidenceMatrix.size(); i++){
        colIndex = transposedIncidenceMatrix.at(0).size() + diagonalMatrix.at(0).size();
        for (int j = 0; j < incidenceMatrix.at(i).size(); j++){
            combinedMatrix.at(rowIndex).at(colIndex++) = incidenceMatrix.at(i).at(j);
        }
        rowIndex++;
    }

    //-AT
    for (int i = 0; i < transposedIncidenceMatrix.size(); i++){
        colIndex = 0;
        for (int j = 0; j < transposedIncidenceMatrix.at(i).size(); j++){
            combinedMatrix.at(rowIndex).at(colIndex++) = transposedIncidenceMatrix.at(i).at(j);
        }
        rowIndex++;
    }

    rowIndex-=transposedIncidenceMatrix.size();
    //1
    for (int i = 0; i < diagonalMatrix.size(); i++){
        colIndex = transposedIncidenceMatrix.at(0).size();
        for (int j = 0; j < diagonalMatrix.at(i).size(); j++){
            combinedMatrix.at(rowIndex).at(colIndex++) = diagonalMatrix.at(i).at(j);
        }
        rowIndex++;
    }

    //M
    for (int i = 0; i < diagonalMatrix.size(); i++){
        colIndex = transposedIncidenceMatrix.at(0).size();
        for (int j = 0; j < diagonalMatrix.at(i).size(); j++){
            combinedMatrix.at(rowIndex).at(colIndex++) = diagonalMatrix.at(i).at(j);
        }
        rowIndex++;
    }

    rowIndex-=diagonalMatrix.size();
    //N
    for (int i = 0; i < currentMatrix.size(); i++){
        colIndex = transposedIncidenceMatrix.at(0).size() + diagonalMatrix.at(0).size();
        for (int j = 0; j < currentMatrix.at(i).size(); j++){
            combinedMatrix.at(rowIndex).at(colIndex++) = currentMatrix.at(i).at(j);
        }
        rowIndex++;
    }
    //return size
    return rowIndex;
}

int Circuit::formCombinedSparseMatrix(){
    formIncidenceMatrix(true);
    formTransposedIncidenceMatrix(true);
    formCurrentCoefficientsMatrix();
    formDiagonalMatrix();
    int colIndex = 0, rowIndex = 0;

    //A
    for (int i = 0; i < incidenceMatrix.size(); i++){
        colIndex = transposedIncidenceMatrix.at(0).size() + diagonalMatrix.at(0).size();
        for (int j = 0; j < incidenceMatrix.at(i).size(); j++){
            float value = incidenceMatrix.at(i).at(j);
            if(value != 0) combinedSparseMatrix.insert({{rowIndex+1, colIndex+1}, value});
            colIndex++;
        }
        rowIndex++;
    }

    //-AT
    for (int i = 0; i < transposedIncidenceMatrix.size(); i++){
        colIndex = 0;
        for (int j = 0; j < transposedIncidenceMatrix.at(i).size(); j++){
            float value = transposedIncidenceMatrix.at(i).at(j);
            if(value != 0) combinedSparseMatrix.insert({{rowIndex+1, colIndex+1}, value});
            colIndex++;
        }
        rowIndex++;
    }

    rowIndex-=transposedIncidenceMatrix.size();
    //1
    for (int i = 0; i < diagonalMatrix.size(); i++){
        colIndex = transposedIncidenceMatrix.at(0).size();
        for (int j = 0; j < diagonalMatrix.at(i).size(); j++){
            float value = diagonalMatrix.at(i).at(j);
            if(value != 0) combinedSparseMatrix.insert({{rowIndex+1, colIndex+1}, value});
            colIndex++;
        }
        rowIndex++;
    }

    //M
    for (int i = 0; i < diagonalMatrix.size(); i++){
        colIndex = transposedIncidenceMatrix.at(0).size();
        for (int j = 0; j < diagonalMatrix.at(i).size(); j++){
            float value = diagonalMatrix.at(i).at(j);
            if(value != 0) combinedSparseMatrix.insert({{rowIndex+1, colIndex+1}, value});
            colIndex++;
        }
        rowIndex++;
    }

    rowIndex-=diagonalMatrix.size();
    //N
    for (int i = 0; i < currentMatrix.size(); i++){
        colIndex = transposedIncidenceMatrix.at(0).size() + diagonalMatrix.at(0).size();
        for (int j = 0; j < currentMatrix.at(i).size(); j++){
            float value = currentMatrix.at(i).at(j);
            if(value != 0) combinedSparseMatrix.insert({{rowIndex+1, colIndex+1}, value});
            colIndex++;
        }
        rowIndex++;
    }
    //return size
    return rowIndex;
}

//sparse is defaulted to false
//in the case sparse matrices were successfully implemented and accounted for
std::vector<float> Circuit::formSolution(bool sparse){
    formVoltageCoefficientsMatrix();
    int n;
    if (sparse) n = formCombinedSparseMatrix();
    else n = formCombinedMatrix();
    std::vector<float>x(n);
    std::vector<std::vector<float>> b(n, std::vector<float>(1));
    for (int i = 0; i < voltageMatrix.size(); i++){
        b.at(n - voltageMatrix.size() + i).at(0) = voltageMatrix.at(i).at(0);
    }

    if (sparse) invertMatrix(combinedSparseMatrix, x, b);
    else invertMatrix(combinedMatrix, x, b);
    return x;
}

//Gauss Elimination
bool Circuit::invertMatrix(std::vector<std::vector<float>> &a, std::vector<float> &x, std::vector<std::vector<float>> &b){
    int n = b.size();

    //augmented matrix => Ab
    for (int i = 0; i < n; i++){
        a.at(i).push_back(b.at(i).at(0));
    }

    // pivot => swap rows 
    if(n != a.size()) return false;
    for (int i = 0; i < n; i++){
        for (int j = i+1; j < n; j++){
            if (std::abs(a.at(j).at(i)) > std::abs(a.at(i).at(i))) {
                swapRows(a.at(j), a.at(i));
            }
        }
    }

    //Forward Elimination
    for (int i = 0; i < n-1; i++){
        for (int j = i+1; j < n; j++){
            subtractRows(a.at(j), a.at(i), a.at(j).at(i)/a.at(i).at(i));
        }
    }

    //Backward Solve
    for (int i = n - 1; i >= 0; i--){
        x.at(i) = a.at(i).at(n);
        for (int j = i+1; j < n; j++){
            if(i == j) continue;
            x.at(i) -= a.at(i).at(j)*x.at(j);
        }
        x.at(i) /= a.at(i).at(i);
    }

    return true;
}
