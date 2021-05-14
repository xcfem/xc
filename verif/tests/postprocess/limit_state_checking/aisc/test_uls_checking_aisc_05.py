# -*- coding: utf-8 -*-
'''Verification test based on the paper: "Exercise 13-Point Bending 
Using the GUI and the Bottom-up-Method" Dr.-Ing. Ulrich Simon. Ulm University.
'''

from __future__ import division
from __future__ import print_function

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AOO)"
__copyright__= "Copyright 2020, LCPT and AOO"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

import os
import math
import xc_base
import geom
import xc
from solution import predefined_solutions
from model import predefined_spaces
from materials import typical_materials
from actions import load_cases
from actions import combinations as combs
from postprocess import limit_state_data as lsd
from postprocess.config import default_config
from materials.astm_aisc import ASTM_materials
from materials.astm_aisc import AISC_limit_state_checking as aisc
#from postprocess import output_handler

# Data
F= 5e3 # Applied (full!) force (N)
L= 2 # Length of the(full!)beam (m)
h= 60e-3 # Height of the beam cross section (m)
t= 20e-3 # Thickness of the beam cross section (m)

# Problem type
feProblem= xc.FEProblem()
preprocessor=  feProblem.getPreprocessor
nodes= preprocessor.getNodeHandler 
modelSpace= predefined_spaces.StructuralMechanics3D(nodes)


# Material definition
## Steel material
steel= ASTM_materials.A992
steel.gammaM= 1.00
sg_yield= steel.fy # Allowable stress: yield stress of steel (Pa)

## XC material model
steel3d= typical_materials.defElasticIsotropic3d(preprocessor, "steel3d",steel.E,steel.nu,rho= steel.rho)

plateFiber= typical_materials.defMembranePlateFiberSection(preprocessor, name= "plateFiber", h= t, nDMaterial= steel3d);

# Problem geometry
# The “bottom-up method” generates the geometry of the system
# going  from  key  points (0D) to lines (1D) and areas (2D)
# up to volumes (3D).

p1= modelSpace.newKPoint(0.0,h/2.0)
p2= modelSpace.newKPoint(L/2.0,h/2.0)
p3= modelSpace.newKPoint(L/2.0,-h/2.0)
p4= modelSpace.newKPoint(0.0,-h/2.0)

s= modelSpace.newSurface([p1,p2,p3,p4])
s.setElemSizeIJ(1.5*10e-3,10e-3)
#s.nDivI= 50 # Number of divisions on p1->p2 and p3->p4
#s.nDivJ= 10 # Number of divisions on p2->p3 and p4->p1

# Meshing

## Define template element
## Before we can create a mesh for our model, we need to choose which
## type of finite element we will want to use for our mesh.
seedElemHandler= preprocessor.getElementHandler.seedElemHandler
seedElemHandler.defaultMaterial= plateFiber.name
quad= seedElemHandler.newElement("ShellMITC4",xc.ID([0,0,0,0]))

## Generate mesh
s.genMesh(xc.meshDir.I)

xcTotalSet= modelSpace.getTotalSet()
numElementsTeor= s.nDivI*s.nDivJ
numElements= xcTotalSet.getNumElements
numNodesTeor= (s.nDivI+1)*(s.nDivJ+1)
numNodes= xcTotalSet.getNumNodes

## Boundary conditions

### Get the line between p4 and p1
l14= modelSpace.getLineWithEndPoints(p1,p4)
### Fix nodes in 1->4 line.
for n in l14.nodes:
    modelSpace.fixNode000_F00(n.tag)

## Material yield strength.
for e in xcTotalSet.elements:
    e.setProp('yieldStress', steel.fy)

## Actions
loadCaseManager= load_cases.LoadCaseManager(preprocessor)
loadCaseNames= ['deadLoad','liveLoad']
loadCaseManager.defineSimpleLoadCases(loadCaseNames)

### Dead load.
deadLoad= xc.Vector([0.0,steel.rho*t,0.0])
cLC= loadCaseManager.setCurrentLoadCase('deadLoad')
for e in xcTotalSet.elements:
    e.vector3dUniformLoadGlobal(deadLoad)

### Live load.
cLC= loadCaseManager.setCurrentLoadCase('liveLoad')
#### Put the load at point 3.
load= F/2.0 # Half! force
n3= p3.getNode()
cLC.newNodalLoad(n3.tag,xc.Vector([0.0,load,0.0,0.0,0.0,0.0]))

## Load combinations
combContainer= combs.CombContainer()
### Ultimate limit state.
combContainer.ULS.perm.add('combULS01','1.2*deadLoad+1.6*liveLoad')
### Dump combination definition into XC.
combContainer.dumpCombinations(preprocessor)

## Compute internal forces.

### Setup working directory.
fname= os.path.basename(__file__)
cfg= default_config.EnvConfig(language='en', resultsPath= 'tmp_results/', intForcPath= 'internalForces/',verifPath= 'verifications/',reportPath='./',reportResultsPath= 'annex/',grWidth='120mm')
cfg.projectDirTree.workingDirectory= '/tmp/'+os.path.splitext(fname)[0]
lsd.LimitStateData.envConfig= cfg

### Set combinations to compute.
loadCombinations= preprocessor.getLoadHandler.getLoadCombinations

### Limit states to calculate internal forces for.
limitStates= [lsd.vonMisesStressResistance]

### Compute internal forces for each combination
for ls in limitStates:
    ls.saveAll(combContainer,xcTotalSet)

### Check material resistance.
outCfg= lsd.VerifOutVars(setCalc=xcTotalSet, appendToResFile='Y', listFile='N', calcMeanCF='Y')
limitState= lsd.vonMisesStressResistance
limitState.controller= aisc.VonMisesStressController(limitState.label)
average= limitState.runChecking(outCfg)
ratio= ((average[0]-0.27083778356986787)/0.27083778356986787)**2

'''
print(average)
print('ratio= ', ratio)
'''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if(ratio<1e-6):
  print('test '+fname+': ok.')
else:
  lmsg.error(fname+' ERROR.')
  
# Graphic stuff.
#oh= output_handler.OutputHandler(modelSpace)
#oh.displayFEMesh()
#oh.displayField(limitStateLabel= limitState.label, section= None, argument= 'CF', component= None, setToDisplay= None, fileName= None)
#oh.displayLoads()
#oh.displayDispRot('uY')
#oh.displayVonMisesStresses(vMisesCode= 'max_von_mises_stress')
