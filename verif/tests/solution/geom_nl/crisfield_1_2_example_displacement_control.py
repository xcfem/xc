# -*- coding: utf-8 -*-
''' Test based on the example 1.2 of the Book: Non-linear Finite Element 
Analysis of Solids and Structures. Volume 1: Essentials. 
M. A. Crisfield. April 2000.
ISBN: 0-471-92956-5

https://www.google.es/books/edition/Non_Linear_Finite_Element_Analysis_of_So/hiTyAQAACAAJ?hl=en
'''

from __future__ import print_function
from __future__ import division

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AOO)"
__copyright__= "Copyright 2021, LCPT and AOO"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

import math
import numpy as np
import geom
import xc
from solution import predefined_solutions
from model import predefined_spaces
from materials import typical_materials
from postprocess import output_handler

feProblem= xc.FEProblem()
preprocessor=  feProblem.getPreprocessor
nodes= preprocessor.getNodeHandler

# Problem geometry.
L0= 2500e-3 # Initial lenght (see expression (1.12) in the book
z= 25e-3 # # Initial deflection.

x2= math.sqrt(L0**2-z**2)

# Problem type
modelSpace= predefined_spaces.SolidMechanics2D(nodes)

# Mesh
nod1= nodes.newNodeXY(0,0)
nod2= nodes.newNodeXY(x2,z)

## Constraints
### Zero movement for node 1.
spc1= modelSpace.fixNode('00',nod1.tag)
### Zero x disp. for node 2.
spc2= modelSpace.fixNode('0F',nod2.tag)

### Material
E= 5e7
A= 1.0 # Cross-section area of the truss.
elast= typical_materials.defElasticMaterial(preprocessor, "elast",E)

### Element.
elements= preprocessor.getElementHandler
elements.dimElem= 2 #Bars defined in a two dimensional space.
elements.defaultMaterial= elast.name
truss= elements.newElement("CorotTruss",xc.ID([nod1.tag,nod2.tag]))
truss.sectionArea= A

# Loads.
P= -100
## Linear time series
lts= modelSpace.newTimeSeries(name= 'lts', tsType= 'linear_ts')
## Load pattern.
lp= modelSpace.newLoadPattern(name= 'lp')
lp.newNodalLoad(nod2.tag,xc.Vector([0.0,P]))
## We add the load case to domain.
modelSpace.addLoadCaseToDomain(lp.name)

### Define RECORDERS
wi= []
recDFree= preprocessor.getDomain.newRecorder("node_prop_recorder",None)
recDFree.setNodes(xc.ID([nod2.tag]))
recDFree.callbackRecord= "wi.append(self.getDisp[1])"
Wi= []
recWi= preprocessor.getDomain.newRecorder("node_prop_recorder",None)
recWi.setNodes(xc.ID([nod1.tag]))
recWi.callbackRecord= "Wi.append(-self.getReaction[1])"
recWi.callbackSetup= "self.getDomain.calculateNodalReactions(True,1e-4)"


# Solution
maxU= -2.3*z
dispIncrement= .01*maxU
numSteps= int(maxU/dispIncrement)
solProc= predefined_solutions.SimpleNewtonRaphsonDisplacementControl(prb= feProblem, node= nod2, dof= 1, increment= dispIncrement, numSteps= numSteps)

solProc.solve(calculateNodalReactions= False)


# Compute theoretical value according to expression (1.11) of the book.
# No spring used so Ks== 0.0.
EA_l3= E*truss.sectionArea/L0**3
WiRef= list()
for w in wi:
    WiRef.append(EA_l3*(z**2*w+3/2.0*z*w**2+1/2.0*w**3))

# Compute root mean square error
def rae(actual, predicted):
    numerator = np.sum(np.abs(predicted - actual))
    denominator = np.sum(np.abs(np.mean(actual) - actual))
    return numerator / denominator

error= rae(np.array(WiRef),np.array(Wi))
 

# Check results.
xi= list()
for w in wi:
    xi.append(-w/z)
yi= list()
yyi= list()
factor= -1.0/E/truss.sectionArea*(L0/z)**3
for ra,rb in zip(Wi, WiRef):
    yi.append(ra*factor)
    yyi.append(rb*factor)

# print("Relative absolute error:", error)

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if(abs(error)<1e-3):
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')

# # Display result.
# import matplotlib.pyplot as plt
# plt.title("Load/deflection relationship.")
# plt.xlabel("-w/z")
# plt.ylabel(r'$\frac{-W}{EA}(\frac{l}{z})^3$')
# plt.plot(xi, yi)
# plt.plot(xi, yyi)
# plt.grid()
# plt.show()
