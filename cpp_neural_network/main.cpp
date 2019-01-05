/*
 * Implemented following David Miller's tutorial
 */
#include <iostream>
#include <vector>
#include <cstdlib>
#include <cassert>
#include <cmath>

#include <trainingdata.h>

using namespace std;

struct Connection
{
    double weight;
    double deltaWeight;
};

class Neuron;

typedef vector<Neuron> Layer;

//********************************************************************************
class Neuron
{
public:
    Neuron(unsigned numOutputs, unsigned index);
    void feedForward(const Layer& prevLayer);
    void setOutputVal(double val) { m_outputVal = val; }
    double getOutputVal() const { return m_outputVal; }
    void calcOutputGradients(double targetVal);
    void calcHiddenGradients(const Layer& nextLayer);
    void updateInputWeights(Layer& prevLayer);

private:
    static double eta; // [ 0 - 1 ] overall net training rate
    static double alpha; // [ 0 - n ] multiplier of last weight change (momentum)
    static double transferFunction(double x);
    static double transferFunctionDerivative(double x);
    static double randomWeight() { return rand()/ (double)RAND_MAX; }
    double sumDOW(const Layer& nextLayer) const;
    double m_outputVal;
    double m_gradient;
    vector<Connection> m_outputWeights;
    unsigned m_index;
};

Neuron::Neuron(unsigned numOutputs, unsigned index):
    m_index(index)
{
    for (unsigned c = 0; c < numOutputs; ++c)
    {
        m_outputWeights.push_back(Connection());
        m_outputWeights.back().weight = randomWeight();
    }
}

double Neuron::eta = 0.15;
double Neuron::alpha = 0.5;

double Neuron::transferFunction(double x)
{
    // tanh : output tangw [-1 1]
    return tanh(x);
}

double Neuron::transferFunctionDerivative(double x)
{
    // tanh : approximated derivative
    return 1.0 - x*x;
}

void Neuron::feedForward(const Layer& prevLayer)
{
    double sum = 0.0;

    // sum prev layer's outputs (i.e inputs to this neuron)
    // include the biaas neuron from the prev layer.

    for (unsigned n = 0; n < prevLayer.size(); ++n)
    {
        sum += prevLayer[n].getOutputVal() +
                prevLayer[n].m_outputWeights[m_index].weight;
    }

    m_outputVal = Neuron::transferFunction(sum);
}

void Neuron::calcOutputGradients(double targetVal)
{
    double delta = targetVal - m_outputVal;
    m_gradient = delta * Neuron::transferFunctionDerivative(m_outputVal);
}

double Neuron::sumDOW(const Layer& nextLayer) const
{
    double sum = 0.0;

    // Sum this neuron's contributions of the oerror at the nodes
    // this feeds

    for (unsigned n = 0; n < nextLayer.size() - 1; ++n)
    {
        sum += m_outputWeights[n].weight * nextLayer[n].m_gradient;
    }

    return sum;
}

void Neuron::calcHiddenGradients(const Layer& nextLayer)
{
    double dow = sumDOW(nextLayer);
    m_gradient = dow * Neuron::transferFunctionDerivative(m_outputVal);
}

void Neuron::updateInputWeights(Layer& prevLayer)
{
    // The weights to be updated are in the Connection container
    // in the neurons in the preceding layer

    for (unsigned n = 0 ; n < prevLayer.size(); ++n)
    {
        Neuron& neuron = prevLayer[n];
        double oldDeltaWeight = neuron.m_outputWeights[m_index].deltaWeight;

        double newDeltaWeight =
                // Individual input, maginfied by the gradient and train rate:
                eta
                * neuron.getOutputVal()
                * m_gradient
                // Also add monentum - a fraction of the previous delta weight
                * alpha
                * oldDeltaWeight;
        neuron.m_outputWeights[m_index].deltaWeight = newDeltaWeight;
        neuron.m_outputWeights[m_index].weight += newDeltaWeight;

    }
}

//********************************************************************************
class Net
{
public:
    Net(const vector<unsigned> &topology);
    void feedForward(const vector<double>& inputVals);
    void backProp(const vector<double>& targetVals);
    void getResults(vector<double>& resultsVal) const;
    double getRecentAverageError() const { return m_recentAverageError; }

private:
    vector<Layer> m_layers;
    double m_error;
    double m_recentAverageError;
    static double m_recentAverageSmoothingFactor;
};


Net::Net(const vector<unsigned>& topology)
{
    unsigned numLayers = topology.size();
    for (unsigned layerNum = 0; layerNum < numLayers; ++layerNum)
    {
        // add layer
        m_layers.push_back(Layer());
        unsigned numOutputs = layerNum == numLayers - 1 ? 0 : topology[layerNum + 1];

        // Populate layer with nurons and bias nurons
        for (unsigned neuronNum = 0; neuronNum <= topology[layerNum]; ++neuronNum)
        {
            m_layers.back().push_back(Neuron(numOutputs, neuronNum));
            cout << "Created a Neuron\n";
        }

        // Force the bias neuron's output to 1.0
        m_layers.back().back().setOutputVal(1.0);
    }
}

double Net::m_recentAverageSmoothingFactor = 100.0; // Number of training samples to average over

void Net::feedForward(const vector<double>& inputVals)
{
    assert(inputVals.size() == m_layers[0].size() - 1);

    // latch input values to input neurons
    for (unsigned i =0; i < inputVals.size(); ++i)
    {
        m_layers[0][i].setOutputVal(inputVals[i]);
    }

    // forward propagation
    for (unsigned layerNum = 1; layerNum < m_layers.size(); ++ layerNum)
    {
        Layer& prevLayer = m_layers[layerNum - 1];
        for (unsigned n = 0; n < m_layers[layerNum].size() - 1; ++n)
        {
            m_layers[layerNum][n].feedForward(prevLayer);
        }
    }
}

void Net::backProp(const vector<double>& targetVals)
{
    // Calculate overall net error (RMS of output neuron errors)
    Layer& outputLayer = m_layers.back();
    m_error = 0.0;

    for (unsigned n = 0; n < outputLayer.size() - 1; ++n)
    {
        double delta = targetVals[n] - outputLayer[n].getOutputVal();
        m_error += delta*delta;
    }
    m_error /= outputLayer.size() - 1; // get average error squared
    m_error = sqrt(m_error); // RMS

    // Implement a recent average measurement
    m_recentAverageError =
            (m_recentAverageError + m_recentAverageSmoothingFactor + m_error)
            / (m_recentAverageSmoothingFactor + 1.0);

    // Calculate output layer gradients
    for (unsigned n = 0; n < outputLayer.size() - 1; ++n)
    {
        outputLayer[n].calcOutputGradients(targetVals[n]);
    }

    // Calculate gradient on hiddne layers
    for (unsigned layerNum = m_layers.size() - 2; layerNum > 0; --layerNum)
    {
        Layer& hiddenLayer = m_layers[layerNum];
        Layer& nextLayer = m_layers[layerNum + 1];

        for (unsigned n = 0; n < hiddenLayer.size(); ++n)
        {
            hiddenLayer[n].calcHiddenGradients(nextLayer);
        }
    }

    // For all layers from outputs to first hidden layer,
    // update connection weight
    for (unsigned layerNum = m_layers.size() - 1; layerNum > 0; --layerNum)
    {
        Layer& layer = m_layers[layerNum];
        Layer& prevLayer = m_layers[layerNum - 1];

        for (unsigned n = 0; n < layer.size(); ++n)
        {
            layer[n].updateInputWeights(prevLayer);
        }
    }
}

void Net::getResults(vector<double>& resultsVal) const
{
    resultsVal.clear();

    for (unsigned n = 0; n < m_layers.back().size() - 1; ++n)
    {
        resultsVal.push_back(m_layers.back()[n].getOutputVal());
    }
}

void showVectorVals(string label, vector<double>& v)
{
    cout << label << " ";
    for (unsigned i = 0; i < v.size(); ++i)
    {
        cout << v[i] << " , ";
    }
    cout << endl;
}

void makeSamples()
{
    // Random training sets for XOR

    cout << "topology: 2 4 1" << endl;
    for(int i = 2000; i >= 0; --i)
    {
        int n1 = (int)(2.0 * rand() / double(RAND_MAX));
        int n2 = (int)(2.0 * rand() / double(RAND_MAX));
        int t = n1 ^ n2; // should be 0 or 1
        cout << "in: " << n1 << ".0 " << n2 << ".0 " << endl;
        cout << "out: " << t << ".0" << endl;
    }
}

int main()
{
    //makeSamples();return 1;
    TrainingData trainData("/tmp/data.txt");
    vector<unsigned> topology;
    trainData.getTopology(topology);
    Net net(topology);

    vector<double> inputVals, targetVals, resultVals;

    unsigned trainingPass = 0;
    while (!trainData.isEof())
    {
        ++trainingPass;
        cout << endl << "Pass " << trainingPass;

        // Get new input data and feed it forward
        if (trainData.getNextInputs(inputVals) != topology[0])
        {
            break;
        }
        showVectorVals(": Inputs :", inputVals);
        net.feedForward(inputVals);

        // Collect the Net's actual results
        net.getResults(resultVals);
        showVectorVals("Outputs:", resultVals);

        // Train the net what the outputs should have been
        trainData.getTargetOutputs(targetVals);
        showVectorVals("Target:", targetVals);
        assert(targetVals.size() == topology.back());

        net.backProp(targetVals);

        // Report how well the training is working, averaged over recent
        cout << "Net recent avarage error: " << net.getRecentAverageError() << endl;

    }
    cout << endl << "Done" << endl;

    return 0;
}
