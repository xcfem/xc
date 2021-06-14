# -*- coding: utf-8 -*-
from __future__ import print_function

# Home made test: prestressing a concrete truss.

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AOO)"
__copyright__= "Copyright 2021, LCPT and AOO"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

import xc_base
import geom
import xc
from model import predefined_spaces
from materials.ehe import EHE_materials
from materials.sections import section_properties
from solution import predefined_solutions

concrete= EHE_materials.HP40
l= 20 # Bar length
alpha= 1.0e-5 # Thermal expansion coefficient of concrete and steel 1/ºC

feProblem= xc.FEProblem()
preprocessor=  feProblem.getPreprocessor
nodes= preprocessor.getNodeHandler

modelSpace= predefined_spaces.StructuralMechanics3D(nodes)
n1= nodes.newNodeXYZ(0.0,0.0,0.0)
n2= nodes.newNodeXYZ(l,0.0,0.0)

strand= EHE_materials.Y1860S7Strand_15_3
strandInitialStress= 1395e6-41.4e6
prestressingSteel= strand.defDiagK(preprocessor,strandInitialStress)

elements= preprocessor.getElementHandler
# Concrete
lin= modelSpace.newLinearCrdTransf("lin", xzVector= xc.Vector([0,1,0]))
sectionProperties= section_properties.RectangularSection(name="rectang",b=.10,h=.10) # Section geometry.
concreteSection= concrete.defElasticShearSection3d(preprocessor, sectionProperties)
elements.defaultMaterial= concreteSection.name
elements.defaultTransformation= lin.name
concreteBeam= elements.newElement("ElasticBeam3d",xc.ID([n1.tag,n2.tag]))

# Prestressing steel.
elements.dimElem= 3 # Bars defined in a two dimensional space.
elements.defaultMaterial= prestressingSteel.name
strandTruss= elements.newElement("Truss",xc.ID([n1.tag,n2.tag]))
strandTruss.sectionArea= strand.area

# Constraints
modelSpace.fixNode('000_0FF', n1.tag)
modelSpace.fixNode('F00_FFF', n2.tag)

# Solution procedure
tol= 1e-6
solProc= predefined_solutions.PenaltyModifiedNewtonUMF(feProblem, printFlag= 0,convergenceTestTol= tol, convTestType= 'norm_unbalance_conv_test')

## Solve for prestressing.
analOk= solProc.solve(True)

R01= n1.getReaction[0]
ratio01= abs(R01)
R02= n2.getReaction[0]
ratio02= abs(R02)
strandInitStress0= strandTruss.getMaterial().initialStress
strandStress0= strandTruss.getMaterial().getStress()
concreteStress0= (concreteBeam.getN1+concreteBeam.getN2)/2.0/sectionProperties.A()
uX0= n2.getDisp[0]

# Shrinkage.
lp0= modelSpace.newLoadPattern(name= '0')
eleLoad= lp0.newElementalLoad("beam_strain_load")
eleLoad.elementTags= xc.ID([concreteBeam.tag])
initialShrinkage=  -6.733333333333333e-05
beamDeformation= xc.DeformationPlane(initialShrinkage)
eleLoad.backEndDeformationPlane= beamDeformation
eleLoad.frontEndDeformationPlane= beamDeformation
modelSpace.addLoadCaseToDomain(lp0.name)

## Solve for shrinking.
analOk= solProc.solve(True)

R11= n1.getReaction[0]
ratio11= abs(R11)
R12= n2.getReaction[0]
ratio12= abs(R12)
strandInitStress1= strandTruss.getMaterial().initialStress
strandStress1= strandTruss.getMaterial().getStress()
concreteStress1= (concreteBeam.getN1+concreteBeam.getN2)/2.0/sectionProperties.A()
uX1= n2.getDisp[0]

# Thermal expansion.
AT= 50.0
lp1= modelSpace.newLoadPattern(name= '1')
beamEleLoad= lp1.newElementalLoad("beam_strain_load")
beamEleLoad.elementTags= xc.ID([concreteBeam.tag])
beamDeformation= xc.DeformationPlane(alpha*AT)
beamEleLoad.backEndDeformationPlane= beamDeformation
beamEleLoad.frontEndDeformationPlane= beamDeformation
strandEleLoad= lp1.newElementalLoad("truss_strain_load")
strandEleLoad.elementTags= xc.ID([strandTruss.tag])
strandEleLoad.eps1= alpha*AT
strandEleLoad.eps2= alpha*AT
modelSpace.addLoadCaseToDomain(lp1.name)

## Solve for thermal expansion.
analOk= solProc.solve(True)

R21= n1.getReaction[0]
ratio21= abs(R21)
R22= n2.getReaction[0]
ratio22= abs(R22)
strandInitStress2= strandTruss.getMaterial().initialStress
strandStress2= strandTruss.getMaterial().getStress()
concreteStress2= (concreteBeam.getN1+concreteBeam.getN2)/2.0/sectionProperties.A()
uX2= n2.getDisp[0]
uX2Ref= alpha*AT*l+uX1
ratio23= abs(uX2-uX2Ref)

# Thermal contraction.
modelSpace.removeLoadCaseFromDomain(lp1.name)

## Solve for thermal expansion.
analOk= solProc.solve(True)

R31= n1.getReaction[0]
ratio31= abs(R31)
R32= n2.getReaction[0]
ratio32= abs(R32)
strandInitStress3= strandTruss.getMaterial().initialStress
strandStress3= strandTruss.getMaterial().getStress()
concreteStress3= (concreteBeam.getN1+concreteBeam.getN2)/2.0/sectionProperties.A()
uX3= n2.getDisp[0]
uX3Ref= -alpha*AT*l+uX2
ratio33= abs(uX3-uX3Ref)

'''
print('Prestressing')
print("R01= ",R01)
print("ratio01= ",ratio01)
print("R02= ",R02)
print("ratio02= ",ratio02)
print("uX0= ",uX0)
print('concrete stress: ', concreteStress0/1e6)
print(strandInitStress0/1e6, strandStress0/1e6,'\n')
print('Shrinkage')
print("R11= ",R11)
print("ratio11= ",ratio11)
print("R12= ",R12)
print("ratio12= ",ratio12)
print("uX1= ",uX1)
print('concrete stress: ', concreteStress1/1e6)
print(strandInitStress1/1e6, strandStress1/1e6,'\n')
print('Thermal expansion')
print("R21= ",R21)
print("ratio21= ",ratio21)
print("R22= ",R22)
print("ratio22= ",ratio22)
print("uX2= ",uX2)
print("uX2Ref= ",uX2Ref)
print("ratio23= ",ratio23)
print('concrete stress: ', concreteStress2/1e6)
print(strandInitStress2/1e6, strandStress2/1e6,'\n')
print('Thermal contraction')
print("R31= ",R31)
print("ratio31= ",ratio31)
print("R32= ",R32)
print("ratio32= ",ratio32)
print("uX3= ",uX3)
print("uX3Ref= ",uX3Ref)
print("ratio33= ",ratio33)
print('concrete stress: ', concreteStress3/1e6)
print(strandInitStress3/1e6, strandStress3/1e6,'\n')
'''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if ratio01<1e-6 and ratio02<1e-6 and ratio11<1e-6 and ratio12<1e-6 and ratio21<1e-6and ratio22<1e-6 and ratio23<1e-6 and ratio31<1e-6and ratio32<1e-6 and ratio33<1e-6:
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')




