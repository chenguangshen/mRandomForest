#include <stdio.h>
#include <string.h>
#include <math.h>
#include <stdlib.h>
#include <assert.h>

#define NUM_TREE 32
#define NUM_NODE 550
#define NUM_FEATURE 16
#define DEBUG 0

typedef unsigned int uint32;
typedef unsigned short uint16;
typedef unsigned char uchar;
typedef enum {EMPTY, BUILD_ME, TERMINAL, SPLIT} NodeStatusType;

// basic element: tree node
typedef struct tree_node_ {
  NodeStatusType status;
  uchar label;
  uint16 attr;
  // store the index (in array) of childen nodes
  uint16 left;
  uint16 right;
  float split_point;
} tree_node;

// tree in the forest
// contains an array of treenode
typedef struct tree_ {
	uint16 num_nodes;
	tree_node nodes[NUM_NODE];
} tree;

static uint16 num_tree = 32;
static uint16 num_feature = 6;
static tree forest[NUM_TREE];
static char spacer[10];
static uint16 status_int;
static uint16 cur_node;
static uint16 label_int;
uint32 i, j, k;

// read random forest model from file
void read_rf(char *filename) {
  FILE *fin = fopen(filename, "r");

  fscanf(fin, "%hd %hd\n", &num_tree, &num_feature);
  printf("read random forest: num_tree=%d, num_feature=%d\n", num_tree, num_feature);

  for (i = 0; i < num_tree; ++i) {
    fscanf(fin, "%s %hd\n", spacer, &forest[i].num_nodes);
    if (DEBUG) {
      printf("tree#%d, num_nodes=%d\n", i, forest[i].num_nodes);
    }
    for (j = 0; j < forest[i].num_nodes; ++j) {
      fscanf(fin, "%hd", &cur_node);
      fscanf(fin, "%hd", &status_int);
      if (DEBUG) {
        printf("cur_node=%d, status_int=%d\n", cur_node, status_int);
      }
      forest[i].nodes[j].status = (NodeStatusType)status_int;
      assert(forest[i].nodes[j].status != EMPTY);
      if (forest[i].nodes[j].status == 3) {
        fscanf(fin, "%hd %hd %hd %f\n", &forest[i].nodes[j].left, &forest[i].nodes[j].right, &forest[i].nodes[j].attr, &forest[i].nodes[j].split_point);
      }
      else if (forest[i].nodes[j].status == 2) {
          fscanf(fin, "%hd\n", &label_int);
          forest[i].nodes[j].label = (uchar)label_int;
      }
    }    
  }
  fclose(fin);
}

static float attributes[NUM_FEATURE];
static uchar result = 0;
static uint16 label = 0, predict_label = 0, true_label = 0;
static uint16 total_sample;
static tree_node n;
static uint16 neg_vote = 0, pos_vote = 0;
static uint16 count = 0;

// predict the samples from a file
void predict(char *featurefile, char *labelfile) {
  FILE *ffec = fopen(featurefile, "r");
  FILE *flabel = fopen(labelfile, "r");
  fscanf(ffec, "%hd\n", &total_sample);

  for (k = 0; k < total_sample; k++) {
    for (j = 0; j < NUM_FEATURE; j++) {
      fscanf(ffec, "%f", &attributes[j]);
      if (DEBUG) {
        printf("%f ", attributes[j]);
      }
    }
    fscanf(flabel, "%hd", &true_label);
    if (DEBUG) {
      printf("\nlabel=%hd\n", true_label);
    }
    

    for (i = 0; i < num_tree; i++) {
      result = 0;
      cur_node = 0;
      label = 0;

      while (result == 0) {
        n = forest[i].nodes[cur_node];
        if (DEBUG) {
          printf("status=%d\n", n.status);
        }
        assert(n.status == TERMINAL || n.status == SPLIT);

        // if this is a leaf node
        if (n.status == TERMINAL) {
          if (DEBUG) {
            printf("terminal label=%d\n", n.label);
          }
          result = 1;
          label = n.label;
        } 
        // if this is a non-leaf node, should split over a feature
        else {
          if (DEBUG) {
            printf("split: cur_node=%d, n.attr=%d, attr=%f, split=%f\n", cur_node, n.attr, attributes[n.attr], n.split_point);
          }
          if (attributes[n.attr] < n.split_point) {          
            cur_node = n.left;
          } else {
            cur_node = n.right;
          }
        }
      }
      // add a vote for this label
      if (label == 1) {
        pos_vote++;
      }
      else {
        neg_vote++;
      }
    }
    // generate the predicted label using the majority vote
    if (pos_vote > neg_vote) {
      predict_label = 1;
    }
    else {
      predict_label = -1;
    }
    pos_vote = 0;
    neg_vote = 0;
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