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

l= 0.15 # Bar length
alpha= 1.0e-5 # Thermal expansion coefficient of concrete and steel 1/ºC
b= 0.1
h= 0.1
concreteSectionArea= b*h 

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
elemSize= h/20.0
s1= modelSpace.newSurface([p1, p2, p3, p4])
s1.setElemSizeIJ(elemSize,elemSize)
s2= modelSpace.newSurface([p4, p3, p5, p6])
s2.setElemSizeIJ(elemSize,elemSize)

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

## Fixed nodes set.
fixedNodesSet= modelSpace.defSet('fixedNodesSet')
ln14= modelSpace.getLineWithEndPoints(p1, p4)
for n in ln14.nodes:
    fixedNodesSet.nodes.append(n)
ln64= modelSpace.getLineWithEndPoints(p6, p4)
for n in ln64.nodes:
    fixedNodesSet.nodes.append(n)

def getFixedNodesReaction():
    ''' Return sum of the reactions in the fixed nodes.'''
    retval= 0.0
    for n in fixedNodesSet.nodes:
        retval+= n.getReaction[0]
    return retval

## Free nodes set.
freeNodesSet= modelSpace.defSet('freeNodesSet')
ln23= modelSpace.getLineWithEndPoints(p2, p3)
for n in ln23.nodes:
    freeNodesSet.nodes.append(n)
ln53= modelSpace.getLineWithEndPoints(p5, p3)
for n in ln53.nodes:
    freeNodesSet.nodes.append(n)

def getFreeNodesReaction():
    ''' Return sum of the reactions in the fixed nodes.'''
    retval= 0.0
    for n in freeNodesSet.nodes:
        retval+= abs(n.getReaction[0])
    return retval

def getFreeNodesDispX():
    ''' Return sum of the reactions in the fixed nodes.'''
    disp= list()
    for n in freeNodesSet.nodes:
        disp.append(n.getDisp[0])
    retval= statistics.mean(disp)
    return retval

# Constraints
for n in fixedNodesSet.nodes:
    modelSpace.fixNode('000_FFF', n.tag)
    
modelSpace.fixNode('FF0_FFF', p2.getNode().tag)
modelSpace.fixNode('FF0_FFF', p3.getNode().tag)
modelSpace.fixNode('FF0_FFF', p5.getNode().tag)

# Solution procedure
tol= 1e-6
solProc1= predefined_solutions.PenaltyNewtonLineSearchUMF(feProblem, printFlag= 0,convergenceTestTol= 1e-8, maxNumIter= 40, convTestType= 'norm_unbalance_conv_test')

strandElementsSet= modelSpace.defSet('strandElementsSet')
for e in ln.elements:
    strandElementsSet.elements.append(e)

def getStrandStresses():
    initStresses= list()
    stresses= list()
    for e in strandElementsSet.elements:
        initStresses.append(e.getMaterial().initialStress)
        stresses.append(e.getMaterial().getStress())
    avgInitStress= statistics.mean(initStresses)
    avgStress= statistics.mean(stresses)
    return avgInitStress, avgStress

concreteSet= modelSpace.defSet('concreteSet')
for e in s1.elements:
    if(e.getDimension==2):
        concreteSet.elements.append(e)
for e in s2.elements:
    if(e.getDimension==2):
        concreteSet.elements.append(e)


def getConcreteStress():
    stresses= list()
    for e in concreteSet.elements:
        e.getResistingForce()
        mats= e.getPhysicalProperties.getVectorMaterials # Materials at gauss points.
        for m in mats:
            stresses.append(m.getStressResultantComponent('n1'))
    return statistics.mean(stresses)

def getConcreteStrain():
    strains= list()
    initialStrains= list()
    for e in concreteSet.elements:
        mats= e.getPhysicalProperties.getVectorMaterials # Materials at gauss points.
        for m in mats:
            strains.append(m.getSectionDeformation()[0])
            initialStrains.append(m.initialSectionDeformation[0])
    return statistics.mean(strains), statistics.mean(initialStrains)

def printShellStrainLoad(lp):
    ''' Print shell strain load values. '''
    print(' load pattern name: ', lp.name)
    print(' number of elemental loads: ', lp.getNumElementalLoads)
    eleLoadIter= lp.loads.getElementalLoadIter
    eleLoad= eleLoadIter.next()
    while(eleLoad):
        tags= eleLoad.elementTags
        for tag in tags:
            preprocessor= lp.getDomain.getPreprocessor
            elem= preprocessor.getElementHandler.getElement(tag)
            strains= eleLoad.getStrains()
            for s in strains:
                print(lp.name, type(eleLoad) , tag, s)
        eleLoad= eleLoadIter.next()
    print(' load pattern name: ', lp.name, ' printed.')

## Recorder for strand elements.
x= []
y= []
steelRecorder= feProblem.getDomain.newRecorder("element_prop_recorder",None)
steelElementTag= xc.ID(strandElementsSet.getElementTags())[1]
steelRecorder.setElements(xc.ID([steelElementTag]))
steelRecorder.callbackRecord= "x.append(self.getMaterial().getStrain()*1e3); y.append(self.getMaterial().getStress()/1e6)"
steelRecorder.callbackRestart= "print(\"Restart method called.\")"

## Solve for prestressing.
if(strandInitialStress>0.0):
    #print('** Solve for presstressing.', flush= True)
    analOk= solProc1.solve(True)
    if(analOk!=0):
        lmsg.error('Failed to solve for prestressing.')
        quit()
    #print('** Solved.', flush= True)

    
R01= getFixedNodesReaction()
ratio01= abs(R01)
R02= getFreeNodesReaction()
ratio02= R02
strandInitStress0, strandStress0= getStrandStresses()
concreteStress0= getConcreteStress()
concreteStrain0, concreteInitialStrain0= getConcreteStrain()

# Shrinkage.
concreteElementTags= list()
for e in s1.elements:
    concreteElementTags.append(e.tag)
for e in s2.elements:
    concreteElementTags.append(e.tag)
    
lpSHR= modelSpace.newLoadPattern(name= 'Shrinkage')
eleLoad= lpSHR.newElementalLoad("shell_strain_load")
eleLoad.elementTags= xc.ID(concreteElementTags)
initialShrinkage=  -6.733333333333333e-05
eleLoad.setStrainComp(0,0,initialShrinkage) #(id of Gauss point, id of component, value)
eleLoad.setStrainComp(1,0,initialShrinkage)
eleLoad.setStrainComp(2,0,initialShrinkage)
eleLoad.setStrainComp(3,0,initialShrinkage)
eleLoad.setStrainComp(0,1,initialShrinkage)
eleLoad.setStrainComp(1,1,initialShrinkage)
eleLoad.setStrainComp(2,1,initialShrinkage)
eleLoad.setStrainComp(3,1,initialShrinkage)
modelSpace.addLoadCaseToDomain(lpSHR.name)

## Solve for shrinking.
#print('** Solve for shrinking.', flush= True)
analOk= solProc1.solve(True)
if(analOk!=0):
    lmsg.error('Failed to solve for shrinking.')
    quit()
#print('** Solved.', flush= True)
#printShellStrainLoad(lpSHR)

R11= getFixedNodesReaction()
ratio11= abs(R11)
R12= getFreeNodesReaction()
ratio12= R12
strandInitStress1, strandStress1= getStrandStresses()
concreteStress1= getConcreteStress()
concreteStrain1, concreteInitialStrain1= getConcreteStrain()

# Thermal expansion.

## Solution procedure
AT= 51.0#600 #400.0 #49.7425626 #42.1125 #50.0
### Some temperatore loads (i. e. 50.0) make the solution algorithm
### to crash. A small perturbation (50->51) solves the problem.
lpTE= modelSpace.newLoadPattern(name= 'ThermalExpansion')
beamEleLoad= lpTE.newElementalLoad("shell_strain_load")
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
strandEleLoad= lpTE.newElementalLoad("truss_strain_load")
strandEleLoad.elementTags= xc.ID(strandElementTags)
strandEleLoad.eps1= alpha*AT
strandEleLoad.eps2= alpha*AT
modelSpace.addLoadCaseToDomain(lpTE.name)

#printShellStrainLoad(lpTE)
#printShellStrainLoad(lpSHR)
## Solve for thermal expansion.
#print('** Solve for thermal expansion.', flush= True)
reactionCheckToleranceTE= 5
analOk= solProc1.solve(True, reactionCheckTolerance= reactionCheckToleranceTE)
if(analOk!=0):
    lmsg.error('Failed to solve for thermal expansion.')
    quit()
#print('** Solved.', flush= True)

R21= getFixedNodesReaction()
ratio21= abs(R21)
R22= getFreeNodesReaction()
ratio22= R22
strandInitStress2, strandStress2= getStrandStresses()
concreteStress2= getConcreteStress()
concreteStrain2, concreteInitialStrain2= getConcreteStrain()

# Thermal contraction.
modelSpace.removeLoadCaseFromDomain(lpTE.name)

## Solve for thermal contraction.
analOk= solProc1.solve(True)
if(analOk!=0):
    lmsg.error('Failed to solve for thermal contraction.')
    quit()

R31= getFixedNodesReaction()
ratio31= abs(R31)
R32= getFreeNodesReaction()
ratio32= R32
strandInitStress3, strandStress3= getStrandStresses()
concreteStress3= getConcreteStress()
concreteStrain3, concreteInitialStrain3= getConcreteStrain()

'''
print('Prestressing')
print('fy= ', prestressingSteel.fy/1e6)
print("R01= ",R01)
print("ratio01= ",ratio01)
print("R02= ",R02)
print("ratio02= ",ratio02)
print('concrete stress: ', concreteStress0/1e6)
print(strandInitStress0/1e6, strandStress0/1e6)
print('concrete initial strain: ', concreteInitialStrain0)
print('concrete strain: ', concreteStrain0,'\n')

print('Shrinkage')
print("R11= ",R11)
print("ratio11= ",ratio11)
print("R12= ",R12)
print("ratio12= ",ratio12)
print('concrete stress: ', concreteStress1/1e6)
print(strandInitStress1/1e6, strandStress1/1e6)
print('concrete initial strain: ', concreteInitialStrain1)
print('concrete strain: ', concreteStrain1,'\n')

print('Thermal expansion')
print("R21= ",R21)
print("ratio21= ",ratio21)
print("R22= ",R22)
print("ratio22= ",ratio22)
print('concrete stress: ', concreteStress2/1e6)
print(strandInitStress2/1e6, strandStress2/1e6)
print('concrete initial strain: ', concreteInitialStrain2)
print('concrete strain: ', concreteStrain2,'\n')

print('Thermal contraction')
print("R31= ",R31)
print("ratio31= ",ratio31)
print("R32= ",R32)
print("ratio32= ",ratio32)
print('concrete stress: ', concreteStress3/1e6)
print(strandInitStress3/1e6, strandStress3/1e6)
print('concrete initial strain: ', concreteInitialStrain3)
print('concrete strain: ', concreteStrain3,'\n')
'''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if ratio01<1e-6 and ratio02<1e-6 and ratio11<1e-6 and ratio12<1e-6 and ratio21<1e-6 and ratio22<1e-4 and ratio31<1e-6 and ratio32<1e-6:
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')

# # Graphic stuff.
# oh= output_handler.OutputHandler(modelSpace)
# #oh.displayFEMesh()
# #oh.displayLocalAxes()
# oh.displayReactions()
# oh.displayDispRot(itemToDisp='uX', defFScale= 100.0)

# #   FIGURES & REPORTS
# print('x= ', x)
# print('y= ', y)
# import matplotlib.pyplot as plt
# plt.plot(x,y)
# #plt.axis('equal')
# plt.show()
