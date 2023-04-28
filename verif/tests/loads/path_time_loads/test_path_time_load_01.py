# -*- coding: utf-8 -*-
''' Test load patterns modulated by path time series. '''
from __future__ import print_function

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AOO)"
__copyright__= "Copyright 2023, LCPT and AOO"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

import math
from scipy import interpolate
import xc
from solution import predefined_solutions
from model import predefined_spaces
from materials import typical_materials

# Problem type
feProblem= xc.FEProblem()
preprocessor=  feProblem.getPreprocessor
nodes= preprocessor.getNodeHandler
modelSpace= predefined_spaces.SolidMechanics2D(nodes)

# Materials.
elast= typical_materials.defElasticMaterial(preprocessor, "elast",1e9)
# Mesh

## Node definition
nod1= nodes.newNodeXY(0,0)
nod2= nodes.newNodeXY(0,.1)

# Elements.
elements= preprocessor.getElementHandler
elements.dimElem= 2 #Bars defined in a two dimensional space.
elements.defaultMaterial= elast.name
truss= elements.newElement("Truss",xc.ID([nod1.tag,nod2.tag]))
truss.sectionArea= .1
# Constraints
modelSpace.fixNode00(nod1.tag)
modelSpace.fixNode0F(nod2.tag)

## Load pattern definition.
loadHandler= preprocessor.getLoadHandler
lPatterns= loadHandler.getLoadPatterns
#### Create path time series TS1.
timeValues= [0, 10, 100]
loadValues1= [0, 1, 1]
ts1= lPatterns.newTimeSeries("path_time_ts","ts")
ts1.path= xc.Vector(loadValues1)
ts1.time= xc.Vector(timeValues)
#### Create load pattern LP1.
lp1= lPatterns.newLoadPattern('default', '1')
lp1.newNodalLoad(nod2.tag, xc.Vector([0,-1]))
modelSpace.addLoadCaseToDomain(lp1.name)
#### Create path time series TS2.
loadValues2= [0, 1, 5]
ts2= lPatterns.newTimeSeries("path_time_ts","ts2")
ts2.path= xc.Vector(loadValues2)
ts2.time= xc.Vector(timeValues)
#### Create load pattern LP2.
lp2= lPatterns.newLoadPattern('default', '2')
lp2.newNodalLoad(nod2.tag, xc.Vector([0,-1]))
modelSpace.addLoadCaseToDomain(lp2.name)

### Define recorders
rBase= [] # list of reactions.
recRBase= preprocessor.getDomain.newRecorder("node_prop_recorder",None)
recRBase.setNodes(xc.ID([nod1.tag]))
recRBase.callbackRecord= "rBase.append([self.getDomain.getTimeTracker.getCurrentTime,self.getReaction])"
recRBase.callbackSetup= "self.getDomain.calculateNodalReactions(True,1e-4)"

# Static analysis.
solProc= predefined_solutions.TransformationNewtonRaphsonBandGen(feProblem, convergenceTestTol= 1e-5, convTestType= 'norm_disp_incr_conv_test', maxNumIter= 100, printFlag= 0)
solProc.setup()
solProc.integrator.dLambda1= 1

solProc.analysis.analyze(100)

# Extract results.
ti= list()
Rzi= list()
for r in rBase:
    ti.append(r[0])
    Rzi.append(r[1][1])
# Compute values of input load at "ti"
totalLoadValues= list()
for la, lb in zip(loadValues1, loadValues2):
    totalLoadValues.append(la+lb)
inputLoadAtTi= interpolate.interp1d(timeValues, totalLoadValues, bounds_error= False, fill_value= 0.0)
interpolatedLoadValues= list()
for t in ti:
    interpolatedLoadValues.append(float(inputLoadAtTi(t)))
error= 0.0
for expected, computed in zip(interpolatedLoadValues, Rzi):
    error+= (expected-computed)**2
error= math.sqrt(error)

''' 
print("error= ",error)
'''
import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if abs(error)<1e-3:
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')
    
# # Display results
# import matplotlib.pyplot as plt
# plt.plot(ti, interpolatedLoadValues,'o', label='Reference values')
# plt.plot(ti, Rzi, label='Computed values')
# plt.legend() # display labels.
# plt.title("Nodal reaction.")
# plt.xlabel('T')
# plt.ylabel('Reaction')
# plt.show()
