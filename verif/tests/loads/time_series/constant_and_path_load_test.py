# -*- coding: utf-8 -*-
''' Trivial test cheking the use of different time series. '''

from __future__ import print_function

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AOO)"
__copyright__= "Copyright 2015, LCPT and AOO"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

import json
import os
import xc
import math
from model import predefined_spaces
from materials import typical_materials
from solution import predefined_solutions

feProblem= xc.FEProblem()
preprocessor=  feProblem.getPreprocessor
nodes= preprocessor.getNodeHandler
modelSpace= predefined_spaces.StructuralMechanics2D(nodes)

# Get the path to the data file
pth= os.path.dirname(__file__)
if(not pth):
  pth= "."
path_time_values_file_path= pth+'/data/test_path_time_values.json'
path_time_values= json.load( open(path_time_values_file_path, 'r'))


# Get load modulation from JSON file.
timeValues= path_time_values['timeValues']
loadValues= path_time_values['loadValues']

loadHandler= preprocessor.getLoadHandler
lPatterns= loadHandler.getLoadPatterns

# Define path time series.
pathTS= lPatterns.newTimeSeries("path_time_ts","pathTS")
pathTS.path= xc.Vector(loadValues)
pathTS.time= xc.Vector(timeValues)

# Define constant series.
constantTS= lPatterns.newTimeSeries("constant_ts","constantTS")

# Define constant load pattern.
constantLP= lPatterns.newLoadPattern("default","0")
constantLP.timeSeries= constantTS # We set a constant time series.

# Define variable load pattern.
pathLP= lPatterns.newLoadPattern("default","1")
pathLP.timeSeries= pathTS # We set a path time series.


# Define mesh                 
## Define nodes
n1= nodes.newNodeXY(0.0,0.0)
n2= nodes.newNodeXY(1.0,0.0)

## Fix all degrees of freedom except axial.
modelSpace.fixNode000(n1.tag)
modelSpace.fixNodeF00(n2.tag)

## Define materials
mat= typical_materials.defElasticMaterial(preprocessor, name= 'mat', E= 2e9)

## Define element
elements= preprocessor.getElementHandler
elements.defaultMaterial= mat.name
elements.dimElem= 2 # Dimension of element space
truss= elements.newElement("Truss",xc.ID([n1.tag,n2.tag]))
truss.sectionArea= .01


## Create recorders
ti= list()
dxi= list()
ni= list()
recDFree= modelSpace.newRecorder("node_prop_recorder",None)
recDFree.setNodes(xc.ID([n2.tag]))
recDFree.callbackRecord= "ti.append(self.getDomain.getTimeTracker.getCurrentTime); dxi.append(self.getDisp[0])"
myRecorder= feProblem.getDomain.newRecorder("element_prop_recorder",None)
myRecorder.setElements(xc.ID([truss.tag]))
myRecorder.callbackRecord= "ni.append(self.getN())"
#myRecorder.callbackSetup= "self.getDomain.calculateNodalReactions(True,1e-4)

## Define constant axial load
modelSpace.setCurrentLoadPattern(constantLP.name)
n2.newLoad(xc.Vector([10e3,0,0]))
modelSpace.addLoadCaseToDomain(constantLP.name)

## Define variable axial load.
modelSpace.setCurrentLoadPattern(pathLP.name)
n2.newLoad(xc.Vector([5e3,0,0]))
modelSpace.addLoadCaseToDomain(pathLP.name)

# Define analysis parameters
solProc= predefined_solutions.PlainNewtonRaphson(prb= feProblem, printFlag= 0)
solProc.setup()
analysis= solProc.analysis

# Do one analysis for constant axial load
result= analysis.analyze(10)


niRef= [10000.0, 10000.0, 524476.6351501325, 2473071.612585906, 1550000.0000000016, 9999.99999999973, 10000.0, 10000.0, 10000.0, 10000.0]
error= 0.0
for n, nRef in zip(ni, niRef):
    error+= (n-nRef)**2
error= math.sqrt(error)

'''
print(dxi)
print(ni)
print(error)
'''
import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if (abs(error)<1e-8):
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')

# # Plot axial load diagram.
# import matplotlib.pyplot as plt
# plt.plot(ti, ni, "-b")
# # Add title and axis names
# plt.title('N-t diagram')
# plt.xlabel('time')
# plt.ticklabel_format(axis='x', style='sci', scilimits=(0,0))
# plt.xticks(rotation = 90)
# plt.ylabel('axial load')
# plt.show()

# # Plot time series diagram.
# from actions import time_series
# time_series.plot_time_series(timeSeries= pathTS)
