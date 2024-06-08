# -*- coding: utf-8 -*-
''' Check values of the fictitious eccentricity using the results of a linear
 buckling analysis. Same test as test_buckling_check_09.py but with pure axial
load.
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

__author__= "Luis C. Pérez Tato (LCPT)"
__copyright__= "Copyright 2024, LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

# Reinforced concrete section.
concr= EHE_materials.HA30
steel= EHE_materials.B500S
## Section geometry.
sectionWidth= 1.4
sectionDepth= 0.45
rcSection= def_simple_RC_section.RCRectangularSection(name= 'flatColumnsRCSection', sectionDescr= 'columns section', concrType= concr, reinfSteelType= steel, width= sectionWidth, depth= sectionDepth, swapReinforcementAxes= True)
## Reinforcement.
firstTopLayer, secondTopLayer, firstBottomLayer, secondBottomLayer= rcSection.defineMainReinforcement(nominalCover= 0.04, fiStirr= 12e-3, topLayersDiameters= [32e-3, None], bottomLayersDiameters= [32e-3, None], lateralLayersDiameters= [25e-3, None], nRebarsHoriz= 6, nRebarsVert= 11)
rcSection.defineShearReinforcementYZ(nShReinfBranchesY= 4, fiStirrY= 12e-3, spacingY= 0.15, nShReinfBranchesZ= 8, fiStirrZ= 12e-3, spacingZ= 0.15)

# Finite element model.
## Problem type
feProblem= xc.FEProblem()
feProblem.title= 'Building columns effective lengths.'
preprocessor= feProblem.getPreprocessor
nodes= preprocessor.getNodeHandler
modelSpace= predefined_spaces.StructuralMechanics3D(nodes)

## Problem geometry
H= 13.0 # Buckling length.
p2= modelSpace.newKPoint(0,0,H)
p1= modelSpace.newKPoint(0,0,0)
l1= modelSpace.newLine(p1, p2)
l1.nDiv= 6
p4= modelSpace.newKPoint(5,0,H)
p3= modelSpace.newKPoint(5,0,0)
l2= modelSpace.newLine(p3, p4)
l2.nDiv= 6

## Geometric transformations
corot1= modelSpace.newCorotCrdTransf("corot1", xzVector= xc.Vector([1,0,0]))
corot2= modelSpace.newCorotCrdTransf("corot2", xzVector= xc.Vector([0,-1,0]))

## Finite element material
xcSection= rcSection.defElasticShearSection3d(preprocessor)
modelSpace.setDefaultMaterial(xcSection)
## Finite element type.
modelSpace.setDefaultCoordTransf(corot1)
modelSpace.newSeedElement("ElasticBeam3d")

## Mesh generation.
l1.genMesh(xc.meshDir.I)
modelSpace.setDefaultCoordTransf(corot2)
modelSpace.newSeedElement("ElasticBeam3d")
l2.genMesh(xc.meshDir.I)

## Constraints.
bottomNode1= p1.getNode()
topNode1= p2.getNode()
modelSpace.fixNode('000_FF0',bottomNode1.tag) # pinned
modelSpace.fixNode('00F_FFF',topNode1.tag) # pinned
bottomNode2= p3.getNode()
topNode2= p4.getNode()
modelSpace.fixNode('000_FF0',bottomNode2.tag) # pinned
modelSpace.fixNode('00F_FFF',topNode2.tag) # pinned

## Loads
lp0= modelSpace.newLoadPattern(name= 'lp0')
Nd= -5000e3
lp0.newNodalLoad(topNode1.tag, xc.Vector([0, 0, Nd, 0, 0, 0]))
lp0.newNodalLoad(topNode2.tag, xc.Vector([0, 0, Nd, 0, 0, 0]))
modelSpace.addLoadCaseToDomain(lp0.name)

# Perform linear buckling analysis.
linearBucklingAnalysis= predefined_solutions.SpectraLinearBucklingAnalysis(prb= feProblem, numModes= 6)
linearBucklingAnalysis.setup()
analOk= linearBucklingAnalysis.solve()

# Get results.
eulerBucklingLoadFactor1= linearBucklingAnalysis.analysis.getEigenvalue(1)
eulerBucklingLoadFactor2= linearBucklingAnalysis.analysis.getEigenvalue(2)
eulerBucklingLoadFactor3= linearBucklingAnalysis.analysis.getEigenvalue(3)
eulerBucklingLoadFactor4= linearBucklingAnalysis.analysis.getEigenvalue(4)
bucklingLoadFactors= [eulerBucklingLoadFactor1, eulerBucklingLoadFactor2, eulerBucklingLoadFactor3, eulerBucklingLoadFactor4]

# Normalize eigenvectors if needed.
norms= preprocessor.getDomain.getMesh.normalizeEigenvectors()

xcTotalSet= modelSpace.getTotalSet()

# Check direction of eigenvectors for buckling mode 1.
dotProduct= 0.0
for e in xcTotalSet.elements:
    elementNodes= e.nodes
    eigenvector0= e.nodes[0].getEigenvector(1)
    eigenvector1= e.nodes[1].getEigenvector(1)
    dispEigenvector= 0.5*(eigenvector0+eigenvector1)
    dispEigenvector= xc.Vector(list(dispEigenvector)[0:3])
    elementWeakAxis= e.getVDirWeakAxisGlobalCoord(True) # initialGeometry= True
    dotProduct+= elementWeakAxis.dot(dispEigenvector)**2
dotProduct= math.sqrt(dotProduct)
normal= (dotProduct<1e-8) # buckling mode normal to weak axis.

# Compute the fictitious eccentricity according to clause 43.5.1 of EHE.
Cz= 0.24 # table 43.1.2 of EHE-08.
Cy= 0.24
beta= 1.0
radiusOfGyration= math.sqrt(rcSection.Iz()/rcSection.A())


results= dict()
ef_mode_1_ref= xc.Vector([0.16134533341878296, 0])
ef_mode_2_ref= ef_mode_1_ref
ef_mode_3a_ref= xc.Vector([0.037466046039909, 0])
ef_mode_3b_ref= xc.Vector([0.0224999999999, 0])
ef_mode_4a_ref= ef_mode_3a_ref
ef_mode_4b_ref= ef_mode_3b_ref
err_ef_mode_1= 0.0
err_ef_mode_2= 0.0
err_ef_mode_3= 0.0
err_ef_mode_4= 0.0
for e in xcTotalSet.elements:
    # Critical axial load.
    reinforcementFactorZ= 1 # Rectangular section table 43.5.1 of EHE-08.
    reinforcementFactorY= 1 # Rectangular section table 43.5.1 of EHE-08.
    sectionDepthZ= sectionDepth
    sectionDepthY= sectionWidth
    Leffi, mechLambdai, Efi, strongAxisBucklingPercent= EHE_limit_state_checking.get_buckling_parameters(element= e, rcSection= rcSection, bucklingLoadFactors= bucklingLoadFactors, sectionDepthZ= sectionDepthZ, Cz= Cz, reinforcementFactorZ= reinforcementFactorZ, sectionDepthY= sectionDepthY, Cy= Cy, reinforcementFactorY= reinforcementFactorY)
    ef_mode_1= xc.Vector([Efi[0][0], Efi[0][1]])
    tmp= min((ef_mode_1-ef_mode_1_ref).Norm(), (ef_mode_1+ef_mode_1_ref).Norm())
    err_ef_mode_1+= tmp**2
    
    ef_mode_2= xc.Vector([Efi[1][0], Efi[1][1]])
    tmp= min((ef_mode_2-ef_mode_2_ref).Norm(), (ef_mode_2+ef_mode_2_ref).Norm())
    err_ef_mode_2+= tmp**2
    
    ef_mode_3= xc.Vector([Efi[2][0], Efi[2][1]])
    tmp_a= min((ef_mode_3-ef_mode_3a_ref).Norm(), (ef_mode_3+ef_mode_3a_ref).Norm())
    tmp_b= min((ef_mode_3-ef_mode_3b_ref).Norm(), (ef_mode_3+ef_mode_3b_ref).Norm())
    tmp= min(tmp_a, tmp_b)
    err_ef_mode_3+= tmp**2
    
    ef_mode_4= xc.Vector([Efi[3][0], Efi[3][1]])
    tmp_a= min((ef_mode_4-ef_mode_4a_ref).Norm(), (ef_mode_4+ef_mode_4a_ref).Norm())
    tmp_b= min((ef_mode_4-ef_mode_4b_ref).Norm(), (ef_mode_4+ef_mode_4b_ref).Norm())
    tmp= min(tmp_a, tmp_b)
    err_ef_mode_4+= tmp**2
    
err_ef_mode_1= math.sqrt(err_ef_mode_1)
err_ef_mode_2= math.sqrt(err_ef_mode_2)
err_ef_mode_3= math.sqrt(err_ef_mode_3)
err_ef_mode_4= math.sqrt(err_ef_mode_4)
err= max(err_ef_mode_1, err_ef_mode_2, err_ef_mode_3, err_ef_mode_4)

'''
print('bucklingLoadFactors= ', bucklingLoadFactors)
print('err_ef_mode_1=', err_ef_mode_1)       
print('err_ef_mode_2=', err_ef_mode_2)       
print('err_ef_mode_3=', err_ef_mode_3)       
print('err_ef_mode_4=', err_ef_mode_4)
print('err=', err)
'''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if(normal and (err<1e-6)):
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')

# # Graphic stuff.
# from postprocess import output_handler
# oh= output_handler.OutputHandler(modelSpace)
# # oh.displayFEMesh()
# # oh.displayLoads()
# # oh.displayLocalAxes()
# oh.displayStrongWeakAxis()
# for mode in [1, 2, 3, 4]:
#     oh.displayEigenvectors(mode= mode, showDispComponents= True, showRotComponents= False)
#     #oh.displayEigenResult(mode, defFScale= 1.0)
# # oh.displayIntForcDiag('Mz')
# # oh.displayIntForcDiag('My')
# # oh.displayEigenResult(1, defFScale= 10.0)
# # oh.displayReactions()
