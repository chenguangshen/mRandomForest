from sklearn.datasets import load_svmlight_file;
from sklearn.ensemble import RandomForestClassifier
import csv
import numpy as np
import random
import sys
from sklearn import cross_validation
from sklearn.metrics import auc_score, accuracy_score, precision_score, f1_score

data = []
label = []

####################################################
# load letter dataset
f = open('data/letter-recognition.data', 'r')

csv_reader = csv.reader(f)

for row in csv_reader:
	t = []
	for i in range(16):
		t.append(row[i + 1])
	if row[0] > 'M':
		ltemp = -1
	else:
		ltemp = 1
	t.append(ltemp)
	data.append(t)

data = np.array(data)
print data[0]

random.shuffle(data)

ndata = []
label = []

for row in data:
	ndata.append(row[0:16])
	label.append(int(row[16]))

data = np.array(ndata)
label = np.array(label)

test_data = data[4000:5000]
test_label = label[4000:5000]

data = data[0:4000]
label = label[0:4000]

print len(data), len(label), len(test_data), len(test_label)

fout = open('data/letter.csv', 'wb')
cwriter = csv.writer(fout, delimiter=',')
for t in data:
	cwriter.writerow(t)
fout.close()

fout = open('data/label.csv', 'wb')
for t in label:
	print >>fout, t
fout.close()

fout = open('data/letter.t.csv', 'wb')
cwriter = csv.writer(fout, delimiter=',')
for t in test_data:
	cwriter.writerow(t)
fout.close()

fout = open('data/label.t.csv', 'wb')
cwriter = csv.writer(fout, delimiter=',')
for t in test_label:
	print >>fout, t
fout.close()
