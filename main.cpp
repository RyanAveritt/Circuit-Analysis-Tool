#include "Circuit.h"
void testMatrices();
void testInversion();
void testSparse();
void testInvertSparse();
void actual();

int main(){
    actual();
    return 0;
}

void actual(){
    Circuit c = Circuit();
    c.readNetListFile("netlist.txt");
    c.writeOutputFile();
}

void testMatrices(){
    Circuit c = Circuit();
    c.readNetListFile("netlist.txt");
    c.formCombinedMatrix();
    c.printCombinedMatrix();
}

void testInversion(){
    Circuit c = Circuit();
    c.readNetListFile("netlist.txt");
    c.formCombinedMatrix();
    c.formVoltageCoefficientsMatrix();
    std::vector<std::vector<float>>b = c.printVoltageCoefficientsMatrix();
    std::vector<std::vector<float>>a = c.printCombinedMatrix();
    std::vector<float> x(5);
    c.invertMatrix(a, x, b);
}

void testSparse(){
    Circuit c = Circuit();
    c.readNetListFile("netlist.txt");
    c.formCombinedMatrix();
    c.printCombinedMatrix();
    c.formCombinedSparseMatrix();
    c.printCombinedSparseMatrix();
}

void testInvertSparse(){
    Circuit c = Circuit();
    c.readNetListFile("netlist.txt");
    c.formCombinedSparseMatrix();
    c.formVoltageCoefficientsMatrix();
    std::vector<std::vector<float>>b = c.printVoltageCoefficientsMatrix();
    std::map<std::pair<int, int>, float> a = c.printCombinedSparseMatrix();
    std::vector<float> x(5);
    c.invertMatrix(a, x, b);
}
