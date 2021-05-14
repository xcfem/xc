# -*- coding: utf-8 -*-
# Home made test.

from __future__ import division
from __future__ import print_function

__author__= "Ana Ortega (AO_O) and Luis C. Pérez Tato (LCPT)"
__copyright__= "Copyright 2020, AO_O and LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= " ana.Ortega.Ort@gmail.com, l.pereztato@gmail.com"

import math
import os
import xc_base
import geom
import xc
from materials.astm_aisc import ASTM_materials
from materials.astm_aisc import AISC_limit_state_checking as aisc
from model import predefined_spaces
from actions import load_cases
from actions import combinations as combs
from solution import predefined_solutions
# from postprocess import output_handler
from postprocess import limit_state_data as lsd
from postprocess.config import default_config

inch2meter= 0.0254
MPa2ksi= 0.145038
kN2kips= 0.2248
kip2kN= 1.0/kN2kips
foot2meter= 0.3048
m2Toin2= 1.0/inch2meter**2

# Problem type
steelBeam= xc.FEProblem()
steelBeam.title= 'Example F.1-1A'
preprocessor= steelBeam.getPreprocessor
nodes= preprocessor.getNodeHandler

#Materials
## Steel material
steel= ASTM_materials.A992
steel.gammaM= 1.00
## Profile geometry
shape= ASTM_materials.WShape(steel,'W18X50')
xcSection= shape.defElasticShearSection3d(preprocessor)

# Model geometry

## Points.
span= 35.0*foot2meter
pointHandler= preprocessor.getMultiBlockTopology.getPoints
p0= pointHandler.newPntFromPos3d(geom.Pos3d(0.0,0.0,0.0))
p1= pointHandler.newPntFromPos3d(geom.Pos3d(span,0.0,0.0))

## Lines
lineHandler= preprocessor.getMultiBlockTopology.getLines
l1= lineHandler.newLine(p0.tag,p1.tag)
l1.nDiv= 10

# Mesh
modelSpace= predefined_spaces.StructuralMechanics3D(nodes)
trfs= preprocessor.getTransfCooHandler
lin= trfs.newLinearCrdTransf3d('lin')
lin.xzVector= xc.Vector([0,1,0])
seedElemHandler= preprocessor.getElementHandler.seedElemHandler
seedElemHandler.defaultMaterial= xcSection.name
seedElemHandler.defaultTransformation= lin.name
elem= seedElemHandler.newElement("ElasticBeam3d",xc.ID([0,0]))

xcTotalSet= preprocessor.getSets.getSet('total')
mesh= xcTotalSet.genMesh(xc.meshDir.I)

# Constraints
modelSpace.fixNode('000_0FF',p0.getNode().tag)
modelSpace.fixNode('F00_FFF',p1.getNode().tag)

# Actions
loadCaseManager= load_cases.LoadCaseManager(preprocessor)
loadCaseNames= ['deadLoad','liveLoad']
loadCaseManager.defineSimpleLoadCases(loadCaseNames)

## Dead load.
deadLoad= xc.Vector([0.0,0.0,-0.45e3*kip2kN/foot2meter])
cLC= loadCaseManager.setCurrentLoadCase('deadLoad')
for e in xcTotalSet.elements:
    e.vector3dUniformLoadGlobal(deadLoad)
  
## Live load.
liveLoad= xc.Vector([0.0,0.0,-0.75e3*kip2kN/foot2meter])
cLC= loadCaseManager.setCurrentLoadCase('liveLoad')
for e in xcTotalSet.elements:
    e.vector3dUniformLoadGlobal(liveLoad)

## Load combinations
combContainer= combs.CombContainer()
### Serviceability limit states.
combContainer.SLS.qp.add('combSLS01', '1.0*liveLoad')
### Ultimate limit state.
combContainer.ULS.perm.add('combULS01','1.2*deadLoad+1.6*liveLoad')
### Dump combination definition into XC.
combContainer.dumpCombinations(preprocessor)

xcTotalSet= modelSpace.getTotalSet()

# Compute internal forces.

## Setup working directory.
fname= os.path.basename(__file__)
cfg= default_config.EnvConfig(language='en', resultsPath= 'tmp_results/', intForcPath= 'internalForces/',verifPath= 'verifications/',reportPath='./',reportResultsPath= 'annex/',grWidth='120mm')
cfg.projectDirTree.workingDirectory= '/tmp/'+os.path.splitext(fname)[0]
lsd.LimitStateData.envConfig= cfg

## Set combinations to compute.
loadCombinations= preprocessor.getLoadHandler.getLoadCombinations

## Limit states to calculate internal forces for.
limitStates= [lsd.normalStressesResistance, # Normal stresses resistance.
lsd.shearResistance, # Shear stresses resistance (IS THE SAME AS NORMAL STRESSES, THIS IS WHY IT'S COMMENTED OUT).
] 

## Create AISC Member objects.
aiscMembers= list()
for l in xcTotalSet.getLines:
    #Lx= l.getLength()
    Lx= 0.5 # continuously braced.
    member= aisc.Member(name= l.name, section= shape,unbracedLengthX= Lx, lstLines= [l])
    #member.setControlPoints()
    member.installULSControlRecorder(recorderType="element_prop_recorder")
    aiscMembers.append(member)

## Compute internal forces for each combination
for ls in limitStates:
    ls.saveAll(combContainer,xcTotalSet,lstSteelBeams=aiscMembers)

outCfg= lsd.VerifOutVars(setCalc=xcTotalSet, appendToResFile='Y', listFile='N', calcMeanCF='Y')
limitState=lsd.normalStressesResistance
limitState.controller= aisc.BiaxialBendingNormalStressController(limitState.label)
average= limitState.runChecking(outCfg)

ratio= ((average[0]-0.46258149840917434)/0.46258149840917434)**2
ratio+= ((average[1]-0.46258149840917479)/0.46258149840917479)**2
ratio= math.sqrt(ratio)

'''
print('average= ', average)
print('ratio= ', ratio)
'''

from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if(ratio<1e-8):
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')

# #########################################################
# # Graphic stuff.
# oh= output_handler.OutputHandler(modelSpace)

# oh.displayElementValueDiagram('chiN', setToDisplay= xcTotalSet)

# oh.displayBeamResult(attributeName=lsd.normalStressesResistance.label, itemToDisp='CF', beamSetDispRes=xcTotalSet, setToDisplay=xcTotalSet)
