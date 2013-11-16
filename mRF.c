#include <stdio.h>
#include <string.h>
#include <math.h>
#include <stdlib.h>
#include <assert.h>

#define NUM_FEATURE 6
#define NUM_TREE 32
#define NUM_NODE 600

typedef unsigned int uint32;
typedef unsigned short uint16;
typedef unsigned char uchar;
typedef unsigned char byte;
typedef enum {EMPTY, BUILD_ME, TERMINAL, SPLIT} NodeStatusType;

typedef struct tree_node_ {
  NodeStatusType status;
  uchar label;
  uint16 attr;
  uint16 start;
  uint16 size;
  uint16 left;
  uint16 right;
  float entropy;
  float split_point;
  uchar depth;
} tree_node;

typedef struct tree_ {
	int num_nodes;
	tree_node nodes[NUM_NODE];
} tree;

static int num_tree = 32;
static int num_feature = 6;
static tree forest[NUM_TREE];
static char spacer[10];
static int status_int;
static int cur_node;
static short label_int;

tree read_tree(FILE *fin) {
	tree cur_tree;
	tree_node cur;
  fscanf(fin, "%s %d\n", spacer, &cur_tree.num_nodes);
  printf("from read_tree: num_node=%d\n", cur_tree.num_nodes);
  //memset(cur_tree.nodes, 0, cur_tree.num_nodes * sizeof(tree_node));
  for (int i = 0; i < cur_tree.num_nodes; ++i) {
    fscanf(fin, "%d", &cur_node);
    //printf("from tree.cc: cur_node=%d , ", cur_node);
    //cur_tree.nodes[cur_node].read(fin);
    fscanf(fin, "%d", &status_int);

    //memset(cur, 0, sizeof(tree_node));
    cur.status = (NodeStatusType)status_int;
    assert(cur.status != EMPTY);

    if (status_int == 3) {
      //printf("before read in SPLIT\n");
      fscanf(fin, "%hd %hd %hd %f\n", &cur.left, &cur.right, &cur.attr, &cur.split_point);
      //printf("after read in SPLIT\n");
     // printf("%d %d %d %f\n", left, right, attr, split_point);
	  }
	  else if (status_int == 2) {
	      //printf("before read in TERMINAL\n");
	      fscanf(fin, "%hd\n", &label_int);
	      //printf("after read in TERMINAL\n");
	      cur.label = (uchar)label_int;
	  }
    
    cur_tree.nodes[cur_node] = cur;
  }
  return cur_tree;
};

void read_rf(char *filename) {
  //memset(forest, 0 sizeof(forest));
  FILE *fin = fopen(filename, "r");

  fscanf(fin, "%d %d\n", &num_tree, &num_feature);
  printf("read random forest: num_tree=%d, num_feature=%d\n", num_tree, num_feature);

  for (int i = 0; i < num_tree; ++i) {
    forest[i] = read_tree(fin);
    printf("%f\n", forest[i].nodes[1].split_point);
  }
  fclose(fin);
}

int main() {
	printf("hello world\n");
	read_rf("model/letter.model");
	return 0;
}