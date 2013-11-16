#include <stdio.h>
#include <string.h>
#include <math.h>
#include <stdlib.h>
#include <assert.h>

#define NUM_TREE 32
#define NUM_NODE 600
#define NUM_FEATURE 16

typedef unsigned int uint32;
typedef unsigned short uint16;
typedef unsigned char uchar;
typedef unsigned char byte;
typedef enum {EMPTY, BUILD_ME, TERMINAL, SPLIT} NodeStatusType;

typedef struct tree_node_ {
  NodeStatusType status;
  uchar label;
  uint16 attr;
  //uint16 start;
  //uint16 size;
  uint16 left;
  uint16 right;
  //float entropy;
  float split_point;
  //uchar depth;
} tree_node;

typedef struct tree_ {
	short num_nodes;
	tree_node nodes[NUM_NODE];
} tree;

static short num_tree = 32;
static short num_feature = 6;
static tree forest[NUM_TREE];
static char spacer[10];
static short status_int;
static short cur_node;
static short label_int;
int i;

tree read_tree(FILE *fin) {
	tree cur_tree;
	tree_node cur;
  fscanf(fin, "%s %d\n", spacer, &cur_tree.num_nodes);
  //printf("from read_tree: num_node=%d\n", cur_tree.num_nodes);
  //memset(cur_tree.nodes, 0, cur_tree.num_nodes * sizeof(tree_node));
  for (i = 0; i < cur_tree.num_nodes; ++i) {
    fscanf(fin, "%d", &cur_node);
    //printf("from tree.cc: cur_node=%d , ", cur_node);
    fscanf(fin, "%d", &status_int);

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
  FILE *fin = fopen(filename, "r");

  fscanf(fin, "%d %d\n", &num_tree, &num_feature);
  //printf("read random forest: num_tree=%d, num_feature=%d\n", num_tree, num_feature);

  for (i = 0; i < num_tree; ++i) {
    forest[i] = read_tree(fin);
    //printf("%f\n", forest[i].nodes[1].split_point);
  }
  fclose(fin);
}

static float attributes[NUM_FEATURE];
static char result = 0;
static short label = 0, predict_label = 0, true_label = 0;
static short total_sample;
static tree_node n;
int k, j;
static short neg_vote = 0, pos_vote = 0;
static short count = 0;

void predict(char *featurefile, char *labelfile) {
  FILE *ffec = fopen(featurefile, "r");
  FILE *flabel = fopen(labelfile, "r");
  fscanf(ffec, "%d\n", &total_sample);

  for (k = 0; k < total_sample; k++) {
    for (j = 0; j < NUM_FEATURE; j++) {
      fscanf(ffec, "%f", &attributes[j]);
    }
    fscanf(flabel, "%d", &true_label);

    for (i = 0; i < num_tree; i++) {
      result = 0;
      cur_node = 0;
      label = 0;

      while (result != 0) {
        n = forest[i].nodes[cur_node];
        assert(n.status == TERMINAL || n.status == SPLIT);
        if (n.status == TERMINAL) {
          result = 1;
          label = n.label;
        } else {
          if (attributes[n.attr] < n.split_point) {
            cur_node = n.left;
          } else {
            cur_node = n.right;
          }
        }
      }
      if (label == 1) {
        pos_vote++;
      }
      else {
        neg_vote++;
      }
    }
    if (pos_vote > neg_vote) {
      predict_label = 1;
    }
    else {
      predict_label = -1;
    }
    if (true_label == predict_label) {
      count++;
    }
  }
  fclose(ffec);
  fclose(flabel);
  printf("%d %d\nAccuracy=%f\n", count, total_sample, ((float)count / (float)total_sample));

}

int main() {
	printf("Load model...\n");
	read_rf("model/letter.model");
  printf("Classification...\n");
  predict("data/test_attribute", "data/test_label");
	return 0;
}