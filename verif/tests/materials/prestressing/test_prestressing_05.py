# -*- coding: utf-8 -*-
from __future__ import print_function

# Home made test: prestressing a concrete truss.

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AOO)"
__copyright__= "Copyright 2021, LCPT and AOO"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

import statistics
import xc_base
import geom
import xc
from model import predefined_spaces
from materials.ehe import EHE_materials
from materials.sections import section_properties
from solution import predefined_solutions
#from postprocess import output_handler
from misc_utils import log_messages as lmsg

l= 20 # Bar length
alpha= 1.0e-5 # Thermal expansion coefficient of concrete and steel 1/ºC
b= 0.1
h= 0.1

feProblem= xc.FEProblem()
preprocessor=  feProblem.getPreprocessor
nodes= preprocessor.getNodeHandler

# Model geometry

## Points
modelSpace= predefined_spaces.StructuralMechanics3D(nodes)
p1= modelSpace.newKPoint(x= 0.0, y= -b/2.0, z= 0.0)
p2= modelSpace.newKPoint(x= l, y= -b/2.0, z= 0.0)
p3= modelSpace.newKPoint(x= l, y= 0.0, z= 0.0)
p4= modelSpace.newKPoint(x= 0.0, y= 0.0, z= 0.0)
p5= modelSpace.newKPoint(x= l, y= b/2.0, z= 0.0)
p6= modelSpace.newKPoint(x= 0.0, y= b/2.0, z= 0.0)

## Surfaces
s1= modelSpace.newSurface([p1, p2, p3, p4])
s1.setElemSizeIJ(h/2.0,b/2.0)
s2= modelSpace.newSurface([p4, p3, p5, p6])
s2.setElemSizeIJ(h/2.0,b/2.0)

# Materials
concrete= EHE_materials.HP40
conc= concrete.defElasticMembranePlateSection(preprocessor, name= "conc",thickness= h)
strand= EHE_materials.Y1860S7Strand_15_3
initialStressLimit= strand.getInitialStressLimit()
strandInitialStress= initialStressLimit-41.4e6
prestressingSteel= strand.defDiagK(preprocessor,strandInitialStress)

# Mesh

## Concrete
seedElemHandler= preprocessor.getElementHandler.seedElemHandler
seedElemHandler.defaultMaterial= conc.name
elem= seedElemHandler.newElement("ShellMITC4",xc.ID([0,0,0,0]))
s1.genMesh(xc.meshDir.I)
s2.genMesh(xc.meshDir.I)

## Steel
ln= modelSpace.getLineWithEndPoints(p4, p3)
seedElemHandler.defaultMaterial= prestressingSteel.name
seedElemHandler.dimElem= 3 # Bars defined in a two dimensional space.
seedElemHandler.defaultMaterial= prestressingSteel.name
strandTruss= seedElemHandler.newElement("Truss",xc.ID([0,0]))
strandTruss.sectionArea= strand.area
ln.genMesh(xc.meshDir.I)

# Constraints
modelSpace.fixNode('FF0_FFF', p1.getNode().tag)
modelSpace.fixNode('000_FFF', p4.getNode().tag)
modelSpace.fixNode('FF0_FFF', p6.getNode().tag)
modelSpace.fixNode('FF0_FFF', p2.getNode().tag)
modelSpace.fixNode('FF0_FFF', p3.getNode().tag)
modelSpace.fixNode('FF0_FFF', p5.getNode().tag)

# Solution procedure
tol= 1e-6
solProc= predefined_solutions.PenaltyNewtonLineSearchUMF(feProblem, printFlag= 0,convergenceTestTol= 1e-5, maxNumIter= 50, convTestType= 'norm_unbalance_conv_test')

def getStrandStresses():
    initStresses= list()
    stresses= list()
    for e in ln.elements:
        initStresses.append(e.getMaterial().initialStress)
        stresses.append(e.getMaterial().getStress())
    avgInitStress= statistics.mean(initStresses)
    avgStress= statistics.mean(stresses)
    return avgInitStress, avgStress

def getConcreteStress():
    stresses= list()
    for e in s1.elements:
        e.getResistingForce()
        mats= e.getPhysicalProperties.getVectorMaterials # Materials at gauss points.
        for m in mats:
            stresses.append(m.getStressResultantComponent('n1'))
    for e in s2.elements:
        e.getResistingForce()
        mats= e.getPhysicalProperties.getVectorMaterials # Materials at gauss points.
        for m in mats:
            stresses.append(m.getStressResultantComponent('n1'))

    return statistics.mean(stresses)

## Solve for prestressing.
#print('Solve for prestressing.', flush= True)
analOk= solProc.solve(True, reactionCheckTolerance= tol)
if(analOk!=0):
    lmsg.error('Failed to solve for prestressing.')
    quit()
#print('Done.')

R01= p1.getNode().getReaction[0]+p4.getNode().getReaction[0]+p6.getNode().getReaction[0]
ratio01= abs(R01)
R02= abs(p2.getNode().getReaction[0])+abs(p3.getNode().getReaction[0])+abs(p5.getNode().getReaction[0])
ratio02= abs(R02)
strandInitStress0, strandStress0= getStrandStresses()
concreteStress0= getConcreteStress()
uX0= (p2.getNode().getDisp[0]+p3.getNode().getDisp[0]+p5.getNode().getDisp[0])/3.0

# Shrinkage.
concreteElementTags= list()
for e in s1.elements:
    concreteElementTags.append(e.tag)
for e in s2.elements:
    concreteElementTags.append(e.tag)
    
lp0= modelSpace.newLoadPattern(name= '0')
eleLoad= lp0.newElementalLoad("shell_strain_load")
eleLoad.elementTags= xc.ID(concreteElementTags)
initialShrinkage=  -6.733333333333333e-05
eleLoad.setStrainComp(0,0,initialShrinkage) #(id of Gauss point, id of component, value)
eleLoad.setStrainComp(1,0,initialShrinkage)
eleLoad.setStrainComp(2,0,initialShrinkage)
eleLoad.setStrainComp(3,0,initialShrinkage)
modelSpace.addLoadCaseToDomain(lp0.name)

## Solve for shrinking.
#print('Solve for shrinking.', flush= True)
analOk= solProc.solve(True, reactionCheckTolerance= tol)
if(analOk!=0):
    lmsg.error('Failed to solve for shrinking.')
    quit()
#print('Done.')

R11= p1.getNode().getReaction[0]+p4.getNode().getReaction[0]+p6.getNode().getReaction[0]
ratio11= abs(R11)
R12= abs(p2.getNode().getReaction[0])+abs(p3.getNode().getReaction[0])+abs(p5.getNode().getReaction[0])
ratio12= abs(R12)
strandInitStress1, strandStress1= getStrandStresses()
concreteStress1= getConcreteStress()
uX1= (p2.getNode().getDisp[0]+p3.getNode().getDisp[0]+p5.getNode().getDisp[0])/3.0

# Thermal expansion.
AT= 200.0
lp1= modelSpace.newLoadPattern(name= '1')
beamEleLoad= lp1.newElementalLoad("shell_strain_load")
beamEleLoad.elementTags= xc.ID(concreteElementTags)
beamEleLoad.setStrainComp(0,0,alpha*AT) #(id of Gauss point, id of component, value)
beamEleLoad.setStrainComp(1,0,alpha*AT)
beamEleLoad.setStrainComp(2,0,alpha*AT)
beamEleLoad.setStrainComp(3,0,alpha*AT)
beamEleLoad.setStrainComp(0,1,alpha*AT)
beamEleLoad.setStrainComp(1,1,alpha*AT)
beamEleLoad.setStrainComp(2,1,alpha*AT)
beamEleLoad.setStrainComp(3,1,alpha*AT)
strandElementTags= list()
for e in ln.elements:
    strandElementTags.append(e.tag)
strandEleLoad= lp1.newElementalLoad("truss_strain_load")
strandEleLoad.elementTags= xc.ID(strandElementTags)
strandEleLoad.eps1= alpha*AT
strandEleLoad.eps2= alpha*AT
modelSpace.addLoadCaseToDomain(lp1.name)

## Solve for thermal expansion.
#print('Solve for thermal expansion.', flush= True)
analOk= solProc.solve(True, reactionCheckTolerance= tol)
if(analOk!=0):
    lmsg.error('Failed to solve for thermal expansion.')
    quit()
#print('Done.')

R21= p1.getNode().getReaction[0]+p4.getNode().getReaction[0]+p6.getNode().getReaction[0]
ratio21= abs(R21)
R22= abs(p2.getNode().getReaction[0])+abs(p3.getNode().getReaction[0])+abs(p5.getNode().getReaction[0])
ratio22= abs(R22)
strandInitStress2, strandStress2= getStrandStresses()
concreteStress2= getConcreteStress()
uX2= (p2.getNode().getDisp[0]+p3.getNode().getDisp[0]+p5.getNode().getDisp[0])/3.0
uX2Ref= alpha*AT*l+uX1
ratio23= abs(uX2-uX2Ref)/abs(uX2Ref)

# Thermal contraction.
modelSpace.removeLoadCaseFromDomain(lp1.name)

## Solve for thermal contraction.
#print('Solve for thermal contraction.', flush= True)
analOk= solProc.solve(True, reactionCheckTolerance= tol)
if(analOk!=0):
    lmsg.error('Failed to solve for thermal contraction.')
    quit()
#print('done.')

R31= p1.getNode().getReaction[0]+p4.getNode().getReaction[0]+p6.getNode().getReaction[0]
ratio31= abs(R31)
R32= abs(p2.getNode().getReaction[0])+abs(p3.getNode().getReaction[0])+abs(p5.getNode().getReaction[0])
ratio32= abs(R32)
strandInitStress3, strandStress3= getStrandStresses()
concreteStress3= getConcreteStress()
uX3= (p2.getNode().getDisp[0]+p3.getNode().getDisp[0]+p5.getNode().getDisp[0])/3.0
uX3Ref= -alpha*AT*l+uX2
ratio33= abs(uX3-uX3Ref)/abs(uX2Ref)

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
if ratio01<tol and ratio02<tol and ratio11<tol and ratio12<tol and ratio21<tol and ratio22<tol and ratio23<1e-3 and ratio31<tol and ratio32<tol and ratio33<1e-6:
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')

# # Graphic stuff.
# oh= output_handler.OutputHandler(modelSpace)
# #oh.displayFEMesh()
# #oh.displayLocalAxes()
# #oh.displayReactions()
# #oh.displayDispRot(itemToDisp='uX', defFScale= 100.0)
