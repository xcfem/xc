# -*- coding: utf-8 -*-
''' Trivial test simulating the evolution of the shrinkage over time.
'''

from __future__ import print_function

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AOO)"
__copyright__= "Copyright 2023, LCPT and AOO"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

import math
import json
import os
import xc
from solution import predefined_solutions
from model import predefined_spaces
from materials import typical_materials
from scipy import interpolate

L= 50.0 # Bar length (m)
E= 2.1e6*9.81/1e-4 # Elastic modulus
nu= 0.3 # Poisson's ratio
G= E/(2*(1+nu)) # Shear modulus
alpha= 1.2e-5 # Thermal expansion coefficient of the steel
A= 4e-4 # bar area expressed in square meters
Iz= 80.1e-8 # Cross section moment of inertia (m4)
Iy= 8.49e-8 # Cross section moment of inertia (m4)
J= 0.721e-8 # Cross section torsion constant (m4)

feProblem= xc.FEProblem()
preprocessor=  feProblem.getPreprocessor
nodes= preprocessor.getNodeHandler

#  n1    L
#   |----------+ n2
#

# Problem type
modelSpace= predefined_spaces.StructuralMechanics2D(nodes)

# Define nodes.
n1= nodes.newNodeXY(0,0)
n2= nodes.newNodeXY(L,0)

# Define materials.
scc= typical_materials.defElasticSection2d(preprocessor, "scc",A= A, E=E, I= Iz)
modelSpace.setDefaultMaterial(scc)

# Define element coordinate transformation.
lin= modelSpace.newLinearCrdTransf("lin")
modelSpace.setDefaultCoordTransf(lin)

# Elements definition
beam= modelSpace.newElement("ElasticBeam2d",[n1.tag, n2.tag])

# Constraints
modelSpace.fixNode000(n1.tag)
modelSpace.fixNodeF0F(n2.tag)
    
# Load case definition.
## Get the path to the data file
pth= os.path.dirname(__file__)
if(not pth):
  pth= "."
path_time_values_file_path= pth+'/../../../aux/shrinkage.json'
path_time_values= json.load( open(path_time_values_file_path, 'r'))
## Get load modulation from JSON file.
timeValues= path_time_values['ti']
loadValues= path_time_values['epsilon']
## Define time series.
pathTS= modelSpace.newTimeSeries(name= 'pathTS', tsType= 'path_time_ts')
pathTS.path= xc.Vector(loadValues)
pathTS.time= xc.Vector(timeValues)
## Define load pattern.
shrinkage= modelSpace.newLoadPattern(name= 'shrinkage')
shrinkage.timeSeries= pathTS # We set a path time series.
eleLoad= shrinkage.newElementalLoad("beam_strain_load")
eleLoad.elementTags= xc.ID([beam.tag])
thermalDeformation= xc.DeformationPlane(1.0)
eleLoad.backEndDeformationPlane= thermalDeformation
eleLoad.frontEndDeformationPlane= thermalDeformation

# We add the load case to domain.
modelSpace.addLoadCaseToDomain(shrinkage.name)

## Create recorders
ti= list()
dxi= list()
recDFree= modelSpace.newRecorder("node_prop_recorder",None)
recDFree.setNodes(xc.ID([n2.tag]))
recDFree.callbackRecord= "ti.append(self.getDomain.getTimeTracker.getCurrentTime); dxi.append(self.getDisp[0])"

# Define analysis parameters
solProc= predefined_solutions.PlainNewtonRaphson(prb= feProblem, printFlag= 0)
solProc.setup()
analysis= solProc.analysis

dt = 10 # days
solProc.integrator.dLambda1= dt # set new increment for the integrator.
solProc.integrator.setNumIncr(10) # IMPORTANT! otherwise it got stuck.

lastT= timeValues[-1] # Last time in timeValues.
t = 0
while t < lastT:
    ok = solProc.analysis.analyze(1)
    t+= dt

# Get displacement.
uX= n2.getDisp[0]
refUx= loadValues[-1]*L

# Check result
ratio0= abs(uX-refUx)/abs(refUx)

## Create interpolation function.
shr=  interpolate.interp1d(timeValues, loadValues)
err= 0
for t, uX in zip(ti, dxi):
    shrinkage= float(shr(t))
    uXRef= shrinkage*L
    err+= (uX-uXRef)**2
err= math.sqrt(err)

'''
print(uX, refUx, ratio0)
print(err)
#print(dxi)
'''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if(ratio0<1e-8 and err<1e-8):
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')

# # Graphic stuff
# from postprocess import output_handler
# oh= output_handler.OutputHandler(modelSpace)
# oh.displayFEMesh()
# #oh.displayReactions()
# oh.displayIntForcDiag('N', defFScale= 50.0)
# oh.displayIntForcDiag('M', defFScale= 50.0)
