# -*- coding: utf-8 -*-
''' Check buckling computations using the results of a linear buckling 
analysis. 2D structural analysis model.
'''

from __future__ import division
from __future__ import print_function

import math
import os
import xc
from model import predefined_spaces
from solution import predefined_solutions
from materials.ehe import EHE_materials
from materials.ehe import EHE_limit_state_checking
from materials.sections.fiber_section import def_simple_RC_section
from materials.sections.fiber_section import def_column_RC_section
from misc_utils import log_messages as lmsg

__author__= "Luis Claudio Pérez Tato (LCPT)"
__copyright__= "Copyright 2024, LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

# Reinforced concrete section.
concr= EHE_materials.HA30
steel= EHE_materials.B500S
## Section geometry.
diameter= 1.25 # Section diameter expressed in meters.
cover= 0.05 # Concrete cover expressed in meters.
rcSection= def_column_RC_section.RCCircularSection(name='test',Rext= diameter/2.0, concrType=concr, reinfSteelType= steel)
## Reinforcement.
rebarDiam= 25e-3
rebarArea= math.pi*(rebarDiam/2.0)**2
numOfRebars= 24
shearRebarsDiam= 16e-3
mechCover= cover-shearRebarsDiam-rebarDiam/2.0
mainReinf= def_simple_RC_section.LongReinfLayers([def_simple_RC_section.ReinfRow(rebarsDiam= rebarDiam, nRebars= numOfRebars, width= math.pi*(diameter-2*mechCover), nominalCover= cover+shearRebarsDiam)])
## Put the reinforcement in the section.
rcSection.mainReinf= mainReinf

# Finite element model.
## Problem type
feProblem= xc.FEProblem()
feProblem.title= 'Building columns effective lengths.'
preprocessor= feProblem.getPreprocessor
nodes= preprocessor.getNodeHandler
modelSpace= predefined_spaces.StructuralMechanics2D(nodes)

## Problem geometry
H= 20.36 # Buckling length.
p2= modelSpace.newKPoint(0,H,0)
p1= modelSpace.newKPoint(0,0,0)
l1= modelSpace.newLine(p1, p2)
l1.nDiv= 6

## Geometric transformations
corot= modelSpace.newCorotCrdTransf("corot")
modelSpace.setDefaultCoordTransf(corot)

## Finite element material
xcSection= rcSection.defElasticShearSection2d(preprocessor)
modelSpace.setDefaultMaterial(xcSection)

## Finite element type.
modelSpace.newSeedElement("ElasticBeam2d")

## Mesh generation.
l1.genMesh(xc.meshDir.I)

## Constraints.
bottomNode= p1.getNode()
topNode= p2.getNode()
modelSpace.fixNode('00F',bottomNode.tag)
modelSpace.fixNode('0FF',topNode.tag)

## Loads
lp0= modelSpace.newLoadPattern(name= 'lp0')
Nd= -2990e3
MdTop= 2000e3
MdBottom= 1970e3
lp0.newNodalLoad(topNode.tag, xc.Vector([0, Nd,MdTop]))
lp0.newNodalLoad(bottomNode.tag, xc.Vector([0,0,MdBottom]))
modelSpace.addLoadCaseToDomain(lp0.name)

# Perform linear buckling analysis.
linearBucklingAnalysis= predefined_solutions.SpectraLinearBucklingAnalysis(prb= feProblem, numModes= 3)
linearBucklingAnalysis.setup()
analOk= linearBucklingAnalysis.solve()

# Get results.
eulerBucklingLoadFactor1= linearBucklingAnalysis.analysis.getEigenvalue(1)
eulerBucklingLoadFactor2= linearBucklingAnalysis.analysis.getEigenvalue(2)

xcTotalSet= modelSpace.getTotalSet()

avgLeff= 0.0 # Average effective length.
avgMechLambda= 0.0 # Average mechanical slenderness.
avgEf= 0.0 # Average fictitious eccentricity.
avgStrongAxisBucklingPercent= 0.0 # dot product (projection) of the buckling eigenvector over the strong axis.
for e in xcTotalSet.elements:
    # Critical axial load.
    bucklingLoadFactors= [eulerBucklingLoadFactor1, eulerBucklingLoadFactor2]
    reinforcementFactor= 2 # Circular section table 43.5.1
    Leffi, mechLambdai, Efi, strongAxisBucklingPercent= EHE_limit_state_checking.get_buckling_parameters(element= e, rcSection= rcSection, bucklingLoadFactors= bucklingLoadFactors, sectionDepthZ= diameter, Cz= 0.2, reinforcementFactorZ= reinforcementFactor)
    avgLeff+= Leffi[0] # Effective length for the first mode.
    avgMechLambda+= mechLambdai[0] # Mechanical slenderness for the first mode.
    avgEf+= Efi[0] # Fictitious eccentricity for the first mode.
    avgStrongAxisBucklingPercent+= strongAxisBucklingPercent[0] # dot product (projection) of the buckling eigenvector over the strong axis.

sz= len(xcTotalSet.elements)
avgLeff/=sz
ratio1= abs(avgLeff-22.111953551507593)/22.111953551507593
avgMechLambda/=sz
ratio2= abs(avgMechLambda-70.75825136482429)/70.75825136482429
avgEf/=sz
ratio3= abs(avgEf-0.2318153467323909)/0.2318153467323909
avgStrongAxisBucklingPercent/= sz
ratio4= abs(avgStrongAxisBucklingPercent)

'''
print('first euler buckling load factor: ', eulerBucklingLoadFactor1)
print('second euler buckling load factor: ', eulerBucklingLoadFactor2)
print('average effective length (first buckling mode): ', avgLeff, 'm', ratio1)
print('average mechanical slenderness (first buckling mode): ', avgMechLambda, ratio2)

print('average fictitious eccentricity (first buckling mode): ', avgEf, 'm', ratio3)
print('average strong axis buckling percentage (first buckling mode): ', avgStrongAxisBucklingPercent, ' ratio4= ', ratio4)
'''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if((ratio1<1e-3) and (ratio2<1e-3) and (ratio3<1e-3) and (ratio3<1e-4)):
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')

# # Graphic stuff.
# from postprocess import output_handler
# oh= output_handler.OutputHandler(modelSpace)
# oh.displayFEMesh()
# oh.displayLoads()
# oh.displayEigenvectors(1)
# oh.displayIntForcDiag('M')
# # oh.displayEigenResult(1, defFScale= 10.0)
# # oh.displayLocalAxes()
# # oh.displayReactions()
