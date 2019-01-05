#ifndef TRAININGDATA_H
#define TRAININGDATA_H

#include <vector>
#include <fstream>
#include <sstream>

using namespace std;

class TrainingData
{
public:
    TrainingData(const char* filename);
    bool isEof() { return m_trainingDataFile.eof(); }
    void getTopology(vector<unsigned>& topology);

    unsigned getNextInputs(vector<double> &inputVals);
    unsigned getTargetOutputs(vector<double>& targetOutputVals);

private:
    ifstream m_trainingDataFile;

};

#endif // TRAININGDATA_H
