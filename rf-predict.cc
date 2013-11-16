#include "librf.h"
//#include "hexagon_sim_timer.h"

using namespace std;
using namespace librf;

//int main(int argc, char*argv[]) {
int main() {
    // Check arguments
    // SwitchArg headerFlag("","header","CSV file has a var name header",false);
    // ValueArg<string> delimArg("","delim","CSV delimiter", false,",","delimiter");
    // ValueArg<string> labelArg("l", "label",
    //                           "Label file", false, "", "labels");

    // CmdLine cmd("rf-predict", ' ', "0.1");
    // ValueArg<string>  dataArg("d", "data",
    //                                "Training Data", true, "", "testdata");
    // ValueArg<string> modelArg("m", "model",
    //                           "Model file output", true, "", "rfmodel");

    // ValueArg<int> numfeaturesArg("f", "features", "# features", false,
    //                              -1, "int");
    // ValueArg<string> outputArg("o", "output", "predictions", true, "", "output");
    // cmd.add(delimArg);
    // cmd.add(headerFlag);
    // cmd.add(labelArg);
    // cmd.add(outputArg);
    // cmd.add(numfeaturesArg);
    // cmd.add(dataArg);
    // cmd.add(modelArg);
    // cmd.parse(argc, argv);

    bool header = false;
    string delim = ",";
    string labelfile = "data/label.t.csv";
    string datafile = "data/letter.t.csv";
    string modelfile = "model/letter.model";
    //string outfile = "result.txt";

    // int num_features = numfeaturesArg.getValue();
    InstanceSet* set = NULL;
    set = InstanceSet::load_csv_and_labels(datafile, labelfile, header, delim);

    RandomForest rf;
    rf.read(modelfile.c_str());

    // hexagon_sim_init_timer();
    // hexagon_sim_start_timer();
    //long start = hexagon_sim_read_cycles();
    cout << "\n\n******\nTest accuracy: " << rf.testing_accuracy(*set) << endl;
    // hexagon_sim_end_timer();
    // hexagon_sim_show_timer(stdout);
    //long end = hexagon_sim_read_cycles();

   // printf("cycle count=%ld\n", end - start);

    /********
    1584640 cycles for 462 classifications, 6.86 microsecond for each classification
    use optimization for conditional statement: 1287866 cycles, ~18.7% improvement
    0.006885s for 462 classifications on NDK, 14.90 micros for each classification

    0.006404s for 462 classifications on NDK, with optimization
    ********/

    /**
    For the letter dataset:
    7085632 cycles for 1000 classifications, 0.014ms for each classification
    **/

    // ofstream out(outfile.c_str());
    // for (int i = 0; i < set->size(); ++i) {
    //   out << rf.predict_prob(*set, i, 0) << endl;
    // }
    // cout << "Confusion matrix" << endl;
    // rf.test_confusion(*set);
    // vector<pair<float, float> > rd;
    // vector<int> hist;
    // cout << "Reliability" << endl;
    // rf.reliability_diagram(*set, 10, &rd, &hist, 0);
    // cout << "bin fraction 1 0 total" << endl;
    // for (int i = 0; i < rd.size(); ++i) {
    //   int positive = int(round(hist[i]*rd[i].second));
    //   cout << rd[i].first << " " << rd[i].second << " ";
    //   cout << positive << " " << (hist[i] - positive) << " " << hist[i] <<endl;
    // }

    delete set;
    return 0;
}
