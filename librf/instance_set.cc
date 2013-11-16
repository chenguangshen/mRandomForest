#include "instance_set.h"
#include "weights.h"
#include "types.h"
#include "stringutils.h"
#include <stdio.h>
#include <string>
#include <algorithm>
#include <float.h>


using namespace std;

namespace librf {
InstanceSet::InstanceSet(){}
/***
 * Named constructor for loading from a csv file and a label file
 * Makes simpler to have a separate label file.
 * @param csv_data CSV filename
 * @param header whether there is a header with var. names
 * @param delim CSV delimiter - defaults to ','
 */
InstanceSet* InstanceSet::load_csv_and_labels(const string& csv_data,
                                      const string& label_file,
                                      bool header,
                                      const string& delim) {
  return new InstanceSet(csv_data, label_file, header, delim);
}


/**
 * Named constructor for creating a subset from an existing set
 * 
 * @param set existing set 
 * @param wl non-zeroes weights
 */
InstanceSet* InstanceSet::create_subset(const InstanceSet& set,
                                     const weight_list& wl) {
  return new InstanceSet(set, wl);
}
/***
 * Unnamed private constructor for loading csv file/label file
 */
InstanceSet::InstanceSet(const string& csv_data,
            const string& label_file,
            bool header, const string& delim) {
  
  // ifstream data(csv_data.c_str());
  load_csv(csv_data.c_str());
  load_labels(label_file.c_str());

  create_sorted_indices();
  assert(attributes_.size() > 0);
  assert(attributes_[0].size() == labels_.size());
}

/**
 * Named constructor for feature selection 
 * @param set existing InstanceSet
 * @param attrs attributes/features wanted 
 */
InstanceSet* InstanceSet::feature_select(const InstanceSet& set,
                                      const vector<int>& attrs) {
  return new InstanceSet(set, attrs);
}

/**
 * Private constructor for feature selection
 */
InstanceSet::InstanceSet(const InstanceSet& set,
                         const vector<int>& attrs) {
  // Copy labels
  labels_ = set.labels_;
  // Only copy given attrs
  attributes_.resize(attrs.size());
  sorted_indices_.resize(attrs.size());
  var_names_.resize(attrs.size());
  for (int i = 0; i < attrs.size(); ++i) {
    attributes_[i] = set.attributes_[attrs[i]];
    sorted_indices_[i] = set.sorted_indices_[attrs[i]];
    var_names_[i] = set.var_names_[attrs[i]];
  }
}
/***
 * Load labels from an istream
 *
 * LIMITATION: assumes binary labels... this needs to change
 */
void InstanceSet::load_labels(const char *filename) {
  float label;
  int true_label;

  printf("label filename=%s\n", filename);

  FILE *fin = fopen(filename, "r");

  int line_count = 0;
  int ch;
  while ((ch=fgetc(fin)) != EOF) {
    if (ch == '\n'){
      line_count++;  
    }
  }
  //printf("line_count=%d\n", line_count);
  rewind(fin);

  for (int i = 0; i < line_count; i++) {
    fscanf(fin, "%f\n", &label);
    if (label == -1.0) {
      true_label =0;
    } else if (label ==0.0) {
      true_label =0;
    } else if (label ==1.0) {
      true_label =1;
    } else {
      cerr << "Incorrect label (only +1, 0, -1 supported)" << endl;
      assert(false);
    }
    labels_.push_back(true_label);
    distribution_.add(true_label);
  }
  fclose(fin);

  printf("label size=%d\n", labels_.size());
}



/***
 * Load csv from an istream
 */
void InstanceSet::load_csv(const char *filename) {
  bool header = false;
  const string& delim = ",";
  printf("data filename=%s\n", filename);

  FILE *fin = fopen(filename, "r");

  int line_count = 0;
  int ch;
  while ((ch=fgetc(fin)) != EOF) {
    if (ch == '\n'){
      line_count++;  
    }
  }
  //printf("line_count=%d\n", line_count);
  rewind(fin);

  //read variable names
  int num_features = -1;
  char buffer[1024];
  
  fscanf(fin, "%s\n", buffer);
  //printf("%s\n", buffer);

  vector<string> ary;
  StringUtils::split(buffer, &ary, delim);
  num_features = ary.size();
  printf("feature_count=%d\n", num_features);
  rewind(fin);

  // create dummy var names
  //create_dummy_var_names(num_features);
  
  attributes_.resize(num_features);
  // get all data
  
  int ii = 0;
  for (ii = 0; ii < line_count; ii++) {
    memset(buffer, 0, sizeof(buffer));
    fscanf(fin, "%s\n", buffer);
    vector<string> ary;
    StringUtils::split(buffer, &ary, delim);
    assert(ary.size() == num_features);
    for (int i = 0; i < ary.size(); ++i) {
      //stringstream ss(ary[i]);
      //convert to float
      float val;
      sscanf(ary[i].c_str(), "%f", &val);
      attributes_[i].push_back(val);
    }
  }

  printf("attribute size=%d\n", attributes_[0].size());
  fclose(fin);
}

// void InstanceSet::create_dummy_var_names(int n) {
//   for (int i = 0; i <  n; ++i) {
//     stringstream ss;
//     ss << i;
//     var_names_.push_back(ss.str());
//   }
// }

void InstanceSet::create_sorted_indices() {
    // allocate sorted_indices_
    sorted_indices_.resize(attributes_.size());
    // sort 
    for (int i = 0; i < attributes_.size(); ++i) {
        sort_attribute(attributes_[i], &sorted_indices_[i]);
    }
}

void InstanceSet::sort_attribute(const vector<float>& attribute,
                                 vector<int>*indices) {
    vector<pair<float, int> > pairs;
    for (int i = 0; i < attribute.size(); ++i) {
        pairs.push_back(make_pair(attribute[i],i));
    }
    sort(pairs.begin(), pairs.end());
    for (int i = 0; i < pairs.size(); ++i) {
        indices->push_back(pairs[i].second);
    }
}

// Grab a subset of the instance (for getting OOB data
InstanceSet::InstanceSet(const InstanceSet& set,
                         const weight_list& weights) : attributes_(set.num_attributes()){
  // Calculate the number of OOB cases
  //cout << "creating OOB subset for weight list of size "
  //     << weights.size() << endl;
  for (int i = 0; i < weights.size(); ++i) {
    if (weights[i] == 0) {
      // append instance
      for (int j = 0; j < set.num_attributes(); ++j) {
          attributes_[j].push_back(set.get_attribute(i, j));
      }
      labels_.push_back(set.label(i));
    }
  }
}


/**
 * Permute method
 * Used for variable importance
 */
void InstanceSet::permute(int var, unsigned int *seed) {
  vector<float>& attr = attributes_[var];
  for (int i = 0; i < attr.size(); ++i) {
    int idx = rand_r(seed) % labels_.size(); // randomly select an index
    float tmp = attr[i];  // swap last value with random index value
    attr[i] = attr[idx];
    attr[idx] =  tmp;
  }
}

void InstanceSet::load_var(int var, const vector<float>& source) {
  // use the STL built-in copy/assignment
  attributes_[var] = source;
}

void InstanceSet::save_var(int var, vector<float>* target) {
  const vector<float>& attr = attributes_[var];
  // use the STL built-in copy/assignment
  *target = attr;
}

} // namespace
