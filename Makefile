CXX = g++
HCXX = hexagon-g++
HSIM = hexagon-sim
VERSION = 5
OPT = 2
CXXFLAGS = -mv$(VERSION) -O$(OPT)
CXXLIBS = -lm -lhexagon
SIMFLAGS = -mv$(VERSION) --timing

all:
	$(CXX) -O2 -Os types.h weights.h stringutils.h utils.h discrete_dist.h discrete_dist.cc instance_set.h instance_set.cc tree_node.h tree_node.cc tree.h tree.cc random_forest.h random_forest.cc librf.h rf-predict.cc -lm

hexagon:
	$(HCXX) $(CXXFLAGS) types.h weights.h stringutils.h utils.h discrete_dist.h discrete_dist.cc instance_set.h instance_set.cc tree_node.h tree_node.cc tree.h tree.cc random_forest.h random_forest.cc librf.h rf-predict.cc -o test_rf $(CXXLIBS)

sim:
	$(HSIM) $(SIMFLAGS) test_rf

mRF:
	gcc -O2 -Os mRF.c -o mRF

mhexagon:
	hexagon-gcc -O2 -Os -mv5 mRF.c -o mRF

msim:
	$(HSIM) $(SIMFLAGS) mRF

clean:
	rm -rf *.o test_rf stats.txt pmu_statsfile.txt *.gch mRF a.out