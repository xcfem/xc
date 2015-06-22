# -*- coding: utf-8 -*-

import numpy

# Transforms an XC::Matrix in a NumPy matrix.
def matrixToNumpyArray(m):
  rn= m.noRows
  cn= m.noCols
  retval= numpy.empty([rn,cn])
  for i in range(0,rn):
    for j in range(0,cn):
      retval[i][j]= m(i,j)
  return retval

def vectorToNumpyArray(v):
  rn= v.size()
  retval= numpy.empty([rn,1])
  for i in range(0,rn):
    retval[i][0]= v[i]
  return retval

