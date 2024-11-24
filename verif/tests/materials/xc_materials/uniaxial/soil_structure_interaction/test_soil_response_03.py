# -*- coding: utf-8 -*-
''' Pressure-displacement diagram verification test.'''

from __future__ import print_function

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AO_O)"
__copyright__= "Copyright 2022, LCPT and AO_O"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@ciccp.es ana.ortega@ciccp.es"

import math
import xc
from geotechnics import earth_pressure
from materials import typical_materials
from model import predefined_spaces
from solution import predefined_solutions

phi= math.pi/6.0
rankineSoil= earth_pressure.RankineSoil(phi, rho= 2000, Kh= 30e6)

depth= 3.0 # 3 m
tributaryArea= 1.5 # 1.5 m2

sg_v= rankineSoil.getVerticalStressAtDepth(z= depth)
samplePoints, initStrain= earth_pressure.get_horizontal_soil_reaction_diagram(sg_v= sg_v, tributaryArea= tributaryArea, Ka= rankineSoil.Ka(), K0= rankineSoil.K0Jaky(), Kp= rankineSoil.Kp(), Kh= rankineSoil.Kh)

# Model definition
feProblem= xc.FEProblem()
preprocessor=  feProblem.getPreprocessor
## Material definition
soilResponse= typical_materials.defMultiLinearMaterial(preprocessor, "soilResponse", samplePoints)

## Problem type
nodes= preprocessor.getNodeHandler
modelSpace= predefined_spaces.StructuralMechanics2D(nodes)

## Define mesh.
n1= nodes.newNodeXY(0,0)
n2= nodes.newNodeXY(0,0)

## Define element.
elements= preprocessor.getElementHandler
elements.defaultMaterial= soilResponse.name
elements.dimElem= 2 # Dimension of element space
zl= elements.newElement("ZeroLength",xc.ID([n1.tag,n2.tag]))

## Constraints
modelSpace.fixNode000(n1.tag)
modelSpace.fixNodeF00(n2.tag)

# Define load.
## Load definition.
ts= modelSpace.newTimeSeries(name= "ts", tsType= "linear_ts")
lp0= modelSpace.newLoadPattern(name= '0')
lp0.newNodalLoad(n2.tag,xc.Vector([1,0,0]))
## We add the load case to domain.
modelSpace.addLoadCaseToDomain(lp0.name)

## Solution recipe
dispIncrement= .001
maxU= 0.0
numSteps= 5
solProc= predefined_solutions.SimpleNewtonRaphsonDisplacementControl(prb= feProblem, node= n2, dof= 0, increment= dispIncrement, numSteps= numSteps, maxNumIter= 100)
solProc.setup()
integrator= solProc.integrator
material= zl.getMaterials()[0]

# Get sample values.
unzipped= [list(t) for t in zip(*samplePoints)]
sampleStrains= unzipped[0]
sampleStresses= unzipped[1]

stresses= list()
prevStrain= 0.0
for strain in sampleStrains:
    strainIncrement= strain-prevStrain
    integrator.increment= strainIncrement/numSteps
    analOk= solProc.solve()
    if(analOk!=0):
        print("Can't solve.")
        exit(1)
    matStrain= material.getStrain()
    deltax= n2.getDisp[0]
    stress= material.getStress()
    stresses.append(stress)
    prevStrain= deltax

# Compute error.
err= 0
for s, rs in zip(stresses, sampleStresses):
    err+= (s-rs)**2
err= math.sqrt(err)

#print(err)
    
import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if(abs(err)<1e-8):
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')
    
# # FIGURES & REPORTS
# import matplotlib.pyplot as plt
# plt.title(fname)
# plt.xlabel('index')
# plt.ylabel('strain')
# xi= range(0, len(sampleStrains))
# plt.plot(xi, sampleStrains, color= 'blue')
# plt.show()

# plt.title(fname)
# plt.xlabel('strain')
# plt.ylabel('stress')
# plt.plot(sampleStrains, stresses, color= 'blue')
# plt.show()
