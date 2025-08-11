# -*- coding: utf-8 -*-
''' Test based on Benchmark 4 (Ultimate load of a single column -page 46-)
of the document:

Comparison of global analysis methods and design rules for steel frames 
according to Eurocode 3
Klemann, B. W. (Author). 31 Jan 2010

http://https://research.tue.nl/en/studentTheses/comparison-of-global-analysis-methods-and-design-rules-for-steel-
'''

from __future__ import division
from __future__ import print_function

__author__= "Ana Ortega (AO_O) and Luis C. Pérez Tato (LCPT)"
__copyright__= "Copyright 2020, AO_O and LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= " ana.Ortega.Ort@gmail.com, l.pereztato@gmail.com"

import math
import os
import geom
import xc
from materials.ec3 import EC3_materials
from materials.ec3 import EC3_limit_state_checking
from model import predefined_spaces
from actions import load_cases
from actions import combinations as combs
from postprocess import limit_state_data as lsd
from postprocess.config import default_config
from solution import predefined_solutions

#Materials
## Steel material
steel= EC3_materials.S355JR
steel.gammaM= 1.00
## Profile geometry
shape= EC3_materials.MicropileTubeShape(steel= steel, name= 'MP_177.8x10.5')
shape.sectionClass= 1

## Problem geometry
height= 1.0 # column height.

# Compute critical load.
Ncr= shape.getNcr(LeqY= height, LeqZ= height)
refNcr= math.pi**2*shape.EIz()/height**2
ratio1= abs(Ncr-refNcr)/refNcr

# Non-dimensional slenderness.
lambdaZ= shape.getAdimensionalSlendernessZ(Leq= height)
# Bow imperfection
McRdz= shape.getMcRdz()
NcRd= shape.getNcRd()
e0= 0.34*(lambdaZ-0.2)*McRdz/NcRd

# Buckling reduction factor (equal for both axis).
chiN= shape.getBucklingReductionFactorZ(Leq= height)

# Ultimate load:
Fult= -chiN*NcRd

# Finite element model.
## Problem type
steelColumn= xc.FEProblem()
steelColumn.title= 'Benchmark 4'
preprocessor= steelColumn.getPreprocessor
nodes= preprocessor.getNodeHandler
modelSpace= predefined_spaces.StructuralMechanics3D(nodes)

## Model geometry

### Points.
p0= modelSpace.newKPoint(0.0,0.0,0.0)
p1= modelSpace.newKPoint(0.0,0.0,height)

### Lines
l1= modelSpace.newLine(p0,p1)
l1.nDiv= 6

### Material.
xcSection= shape.defElasticShearSection3d(preprocessor)
### Mesh generation

### Geometric transformations
cooTrf= modelSpace.newCorotCrdTransf("corot", xzVector= xc.Vector([0,1,0])) # Corotational transformation.

seedElemHandler= preprocessor.getElementHandler.seedElemHandler
seedElemHandler.defaultMaterial= xcSection.name
seedElemHandler.defaultTransformation= cooTrf.name
elem= seedElemHandler.newElement("ElasticBeam3d")
xcTotalSet= preprocessor.getSets.getSet('total')
mesh= xcTotalSet.genMesh(xc.meshDir.I)

### Constraints
modelSpace.fixNode('000_FF0', p0.getNode().tag)
modelSpace.fixNode('00F_FFF', p1.getNode().tag)

### Imperfection
for n in xcTotalSet.nodes:
    pos= n.getInitialPos3d
    newX= e0*math.sin(pos.z*math.pi/height)
    pos.x+= newX
    n.setPos(pos)
# the nodes.
for e in xcTotalSet.elements:
    e.resetNodalCoordinates()

# Actions
loadCaseManager= load_cases.LoadCaseManager(preprocessor)
loadCaseNames= ['LC0']
loadCaseManager.defineSimpleLoadCases(loadCaseNames)

## Point load.
cLC= loadCaseManager.setCurrentLoadCase('LC0')
pLoad= xc.Vector([0.0, 0.0, Fult, 0.0, 0.0, 0.0])
p1.getNode().newLoad(pLoad)

## Load combinations
combContainer= combs.CombContainer()

combContainer.ULS.perm.add('c0', '1.0*LC0')

### Dump combination definition into XC.
combContainer.dumpCombinations(preprocessor)

# Compute internal forces.

## Solution procedure.
class SolProc(predefined_solutions.PenaltyNewtonRaphson):
    ''' Custom solution procedure.'''
    def __init__(self, prb, name= None, maxNumIter= 30, convergenceTestTol= 1e-4, printFlag= 0, numSteps= 1, numberingMethod= 'rcm', convTestType= 'norm_unbalance_conv_test'):
        super().__init__(prb= prb, name= name, maxNumIter= maxNumIter, convergenceTestTol= convergenceTestTol, printFlag= printFlag, numSteps= numSteps, numberingMethod= numberingMethod, convTestType= convTestType)
        
## Setup working directory.
cfg= default_config.get_temporary_env_config()
lsd.LimitStateData.envConfig= cfg

## Set combinations to compute.
loadCombinations= preprocessor.getLoadHandler.getLoadCombinations

## Limit states to calculate internal forces for.
limitStates= [EC3_limit_state_checking.normalStressesResistance, # Normal stresses resistance.
EC3_limit_state_checking.shearResistance, # shear resistance
]

# Assign element properties.
ec3CalcSet= xcTotalSet
for e in ec3CalcSet.elements:
    e.setProp('crossSection', shape)
    e.setProp('chiLT', 1.0)
    e.setProp('chiN', chiN)
## Compute internal forces for each combination
for ls in limitStates:
    ls.analyzeLoadCombinations(combContainer= combContainer, setCalc= ec3CalcSet, solutionProcedureType= SolProc)
    
# Check limit state.
## Limit state to check.
limitState= limitStates[0]
## Build controller.
controller= limitState.getController(biaxialBending= True)

## Perform checking.
average= limitState.check(setCalc=ec3CalcSet, appendToResFile='N', listFile='N', calcMeanCF='Y', controller= controller)

# Check results.
err= (average[0]-1)**2
err+= (average[1]-1)**2
err= math.sqrt(err)

'''
print('Iy= ', shape.Iy(), 'm4')
print('Iz= ', shape.Iz(), 'm4')
print('Elastic critical load: ', Ncr/1e3, 'kN')
print('Elastic critical load (reference value): ', refNcr/1e3, 'kN')
print('Non-dimensional slenderness: ', lambdaZ)
print('Bending resistance: ', McRdz/1e3, 'kN.m')
print('Axial compression resistance: ', NcRd/1e3, 'kN')
print('Bow imperfection: ', e0*1e3, 'mm')
print('Buckling reduction factor: ', chiN)
print('Buckling ultimate load: ', Fult/1e3, 'kN')
print(average)
print(err)
'''


cfg.cleandirs() # Clean after yourself.
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if(err<1e-4):
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')

# # #########################################################
# # # Graphic stuff.
# from postprocess import output_handler
# oh= output_handler.OutputHandler(modelSpace)
# oh.displayFEMesh()

# # oh.displayElementValueDiagram('chiN', setToDisplay= aiscCalcSet)

# oh.displayBeamResult(attributeName= limitState.label, itemToDisp='CF', beamSetDispRes=ec3CalcSet, setToDisplay=xcTotalSet)
