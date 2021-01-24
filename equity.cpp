#include "omp/EquityCalculator.h"
#include <iostream>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <string>
#include <algorithm>

namespace py = pybind11;

using namespace omp;
using namespace std;

float getEquity(vector<string> stringRanges, string stringBoard, string stringDead)
{
    EquityCalculator eq;
    vector<CardRange> ranges;
    for (int i = 0; i < stringRanges.size(); i++) {
	ranges.push_back(CardRange(stringRanges[i]));
    }
    uint64_t board = CardRange::getCardMask(stringBoard);
    uint64_t dead = CardRange::getCardMask(stringDead);
    double stdErrMargin = 2e-5; // stop when standard error below 0.002%
    auto callback = [&eq](const EquityCalculator::Results& results) {
        cout << results.equity[0] << " " << 100 * results.progress
                << " " << 1e-6 * results.intervalSpeed << endl;
        if (results.time > 5) // Stop after 5s
            eq.stop();
    };
    double updateInterval = 0.25; // Callback called every 0.25s.
    unsigned threads = 0; // max hardware parallelism (default)
    eq.start(ranges, board, dead, false, stdErrMargin, callback, updateInterval, threads);
    eq.wait();
    auto r = eq.getResults();
    cout << endl << r.equity[0] << " " << r.equity[1] << " " << r.equity[2] << endl;
    cout << r.wins[0] << " " << r.wins[1] << " " << r.wins[2] << endl;
    cout << r.hands << " " << r.time << " " << 1e-6 * r.speed << " " << r.stdev << endl;
    return r.equity[0];
}

PYBIND11_MODULE(OMPEval, m) {
    m.doc() = "sick equity calculator"; // optional module docstring

    m.def("getEquity", &getEquity, "A function which calcs equity");
}
