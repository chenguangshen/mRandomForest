/* tree_node.cc
 * 
 * The only implementation here is in reading/writing
 *
 */
#include "tree_node.h"
#include "assert.h"
namespace librf {
void tree_node::write(ostream& o) const {
  // we shouldn't be saving any other kind of node
  assert(status == TERMINAL || status == SPLIT);
  o << int(status);
  switch(status) {
    case TERMINAL:
      o << " " << int(label) << endl;
    break;
    case SPLIT:
      o << " " << left << " " << right << " " <<  attr << " " << split_point << endl;
    break;
  }
}

void tree_node::read(FILE *fin) {
  int status_int;

  // i >> status_int;
  fscanf(fin, "%d", &status_int);
  
  status = NodeStatusType(status_int);
  assert(status != EMPTY);

  //printf("from tree_node.cc: status=%d\n", status_int);

  if (status_int == 3) {
      //printf("before read in SPLIT\n");
      fscanf(fin, "%hd %hd %hd %f\n", &left, &right, &attr, &split_point);
          // //printf("before read in SPLIT\n");
          // fscanf(fin, "%hd", &right);
          // //printf("before read in SPLIT\n");
          // fscanf(fin, "%hd", &attr);
          // //printf("before read in SPLIT\n");
          // fscanf(fin, "%f", &split_point);
      //printf("after read in SPLIT\n");
      //printf("%d %d %d %f\n", left, right, attr, split_point);
  }
  else if (status_int == 2) {
      int label_int;
      // i >> label_int;
      //printf("before read in TERMINAL\n");
      fscanf(fin, "%hd\n", &label_int);
      //printf("after read in TERMINAL\n");
      label = uchar(label_int);
  }
}

} // namespace
