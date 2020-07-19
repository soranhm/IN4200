infile = open('Ellingham_data.txt','r')

import numpy as np
import matplotlib.pyplot as plt

#hopper over 3 forste linjene
for i in range(3):
    infile.readline()

# Lager tomme array
navn = []
entalpi = []
entropi = []

for line in infile:
    words = line.split()
    navn.append(words[0])
    entalpi.append(float(words[1]))
    entropi.append(float(words[2]))

infile.close()
print('navn     entalpi     entropi')
for i in len(navn):
    print ('%s    %.0f    %.0f' % (navn[i],entalpi[i],entropi[i]))


T = np.linspace(298,1273,N)
