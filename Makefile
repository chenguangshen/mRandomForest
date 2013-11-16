CXX = g++
HCXX = hexagon-g++
HSIM = hexagon-sim
VERSION = 5
OPT = 2
CXXFLAGS = -mv$(VERSION) -O$(OPT)
CXXLIBS = -lm -lhexagon
SIMFLAGS = -mv$(VERSION) --timing

all:
	$(CXX) -O2 -Os -lm types.h weights.h stringutils.h utils.h discrete_dist.h discrete_dist.cc instance_set.h instance_set.cc tree_node.h tree_node.cc tree.h tree.cc random_forest.h random_forest.cc librf.h rf-predict.cc

hexagon:
	$(HCXX) $(CXXFLAGS) types.h weights.h stringutils.h utils.h discrete_dist.h discrete_dist.cc instance_set.h instance_set.cc tree_node.h tree_node.cc tree.h tree.cc random_forest.h random_forest.cc librf.h rf-predict.cc -o test_rf $(CXXLIBS)

sim:
	$(HSIM) $(SIMFLAGS) test_rf

clean:
	rm -rf *.o test_rf stats.txt pmu_statsfile.txt *.gch

mrf:
	gcc mRF.c -lm -o mRF