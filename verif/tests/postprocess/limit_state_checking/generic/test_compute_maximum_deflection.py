# -*- coding: utf-8 -*-
''' Test computation of deflections along a beam.'''

__author__= "Luis C. Pérez Tato (LCPT)"
__copyright__= "Copyright 2025, LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

import os
import math
import geom
import xc
from model import predefined_spaces
from materials.ec3 import EC3_materials
from actions import load_cases as lcm
from actions import combinations as combs
from postprocess import recorders
from misc_utils import log_messages as lmsg


# Problem type
mainBeam= xc.FEProblem()
mainBeam.title= 'Falsework support beams'
preprocessor= mainBeam.getPreprocessor   
nodes= preprocessor.getNodeHandler
modelSpace= predefined_spaces.StructuralMechanics2D(nodes)

#Materials
steel= EC3_materials.S235JR
steel.gammaM= 1.00
crossSectionShape= EC3_materials.IPEShape(steel,'IPE_400') # Precast deck
crossSectionShape.sectionClass= 1
beamSection= crossSectionShape.defElasticShearSection2d(preprocessor, steel)

pt= dict()
slope= -0.1745
#a= 2.0; b= 15.25; c= 3.8 #Interior beam.
a= 6.65; b= 15.20; c= 4.6 #Exterior beam.
z0= 33.5+0.02*1.24
x= 0.0; z= z0+slope*x
pt[0]= modelSpace.newKPoint(x,z,0.0)
x+= a; z= z0+slope*x
pt[1]= modelSpace.newKPoint(x,z,0.0)
x+= b; z= z0+slope*x
pt[2]= modelSpace.newKPoint(x,z,0.0)
x+= c; z= z0+slope*x
pt[3]= modelSpace.newKPoint(x,z,0.0)

lineDict= dict()
span= 0.0
for i in range(0, 3):
    newLine= modelSpace.newLine(pt[i], pt[i+1])
    span= max(span, newLine.getLength())
    lineDict[i]= newLine

#Meshing
lin= modelSpace.newLinearCrdTransf('lin')
modelSpace.setDefaultCoordTransf(lin)
#lin.xzVector= xc.Vector([0,1,0])
modelSpace.setDefaultMaterial(beamSection)
modelSpace.newSeedElement('ElasticBeam2d')
seedElem= preprocessor.getElementHandler.seedElemHandler

for key in lineDict:
    l= lineDict[key]
    l.setElemSize(0.25)
    l.genMesh(xc.meshDir.I)

# Constraints
supportPoints= [pt[1],pt[2],pt[3]]
supportNodes= list()
for p in supportPoints:
    supportNodes.append(p.getNode())

for n in supportNodes:
    modelSpace.fixNode('00F', n.tag)

#ACTIONS
loadCaseManager= lcm.LoadCaseManager(preprocessor)
loadCaseNames= ['selfWeight','deadLoad','liveLoad']
loadCaseManager.defineSimpleLoadCases(loadCaseNames)

#Self weight.
cLC= loadCaseManager.setCurrentLoadCase('selfWeight')
grav=9.81 #Aceleración de la gravedad (m/s2)
massShape= crossSectionShape.getRho()

selfWeightLoad= grav*massShape
selfWeightLoadVector= xc.Vector([0.0, -selfWeightLoad])
for key in lineDict:
    l= lineDict[key]
    for e in l.elements:
        e.vector2dUniformLoadGlobal(selfWeightLoadVector)

concreteWidth= 2.47 
platformWidth= 4.8 # average platform width

#Dead load.
cLC= loadCaseManager.setCurrentLoadCase('deadLoad')
loadedBeams= 4.0 #All four beams support dead loads.
deadLoad= 0.5e3*platformWidth/loadedBeams#+0.67e3 #Load over one beam.
deadLoadVector= xc.Vector([0.0, -deadLoad])
for key in lineDict:
    l= lineDict[key]
    for e in l.elements:
      e.vector2dUniformLoadGlobal(deadLoadVector)

#Live load.
cLC= loadCaseManager.setCurrentLoadCase('liveLoad')
loadedBeams= 3.0 #Only three beams support live loads.
#concreteThickness= 0.35 #Cast in place deck.
concreteThickness= 0.0 #Precast deck.
concreteWeight= 25e3*concreteThickness
circulationLoad= 2e3
liveLoad= (concreteWeight+circulationLoad)*concreteWidth/loadedBeams #Load over one beam.
liveLoadVector= xc.Vector([0.0, -liveLoad])
for key in lineDict:
    l= lineDict[key]
    for e in l.elements:
        e.vector2dUniformLoadGlobal(liveLoadVector)

#Load combinations
combContainer= combs.CombContainer()

#Quasi-permanent situations.
combContainer.SLS.qp.add('ELS00', '1.0*selfWeight+1.0*deadLoad')
combContainer.SLS.qp.add('ELS01', '1.0*selfWeight+1.0*deadLoad+1.0*liveLoad')

#Permanent and transitory situations.
combContainer.ULS.perm.add('ELU00', '1.35*selfWeight+1.35*deadLoad+1.5*liveLoad')

#Sets
xcTotalSet= modelSpace.getTotalSet()
mainBeamSet= modelSpace.defSet("main_beam")

for key in lineDict:
    l= lineDict[key]
    mainBeamSet.lines.append(l)
mainBeamSet.fillDownwards()

# Assigns span value to allow deflection verification.
span= span
for n in xcTotalSet.getNodes:
    n.setProp('span',span)
# Set recorder.
recorder= recorders.install_node_displacement_recorder("node_prop_recorder",xcTotalSet.getNodes)

modelSpace.removeAllLoadsAndCombinationsFromDomain()
modelSpace.revertToStart()

for situation in [combContainer.SLS.qp, combContainer.SLS.freq]:
    for lc in situation:
        lcName= lc
        lcExpr= situation[lcName].expr
        modelSpace.addNewLoadCaseToDomain(loadCaseName= lcName, loadCaseExpression= lcExpr)
        result= modelSpace.analyze(calculateNodalReactions= True)
        if(result!=0):
            lmsg.error('Can\'t solve for load case:', lcName)
            exit(1)
        modelSpace.removeLoadCaseFromDomain(lcName)
        modelSpace.revertToStart()
            
# Compute maximum deflection.
dispMax= -1
combDispMax= None
fmax= 0
for n in xcTotalSet.getNodes:
    dm= n.getProp("dispMax")
    f= dm/n.getProp('span')
    n.setProp('f',f)
    if(dispMax<dm):
        x= n.getCoo[0]
        dispMax= dm
        fmax= dispMax/n.getProp('span')
        combDispMax= n.getProp("CombDispMax")

xRef= 0.0
ratio1= abs(x-xRef)
dispMaxRef= 16.050261292368003e-3
ratio2= abs(dispMax-dispMaxRef)/dispMaxRef
fMaxRef= 0.0010402195328601677
ratio3= abs(fmax-fMaxRef)/fMaxRef

'''
print('x= ', x, 'dispMax=', dispMax*1000, 'mm', 'combDispMax=', combDispMax, 'f= L/', 1.0/fmax)
print(ratio1)
print(ratio2)
print(ratio3)
'''

testOK= (ratio1<1e-6) and (ratio2<1e-6) and (combDispMax=='ELS01') and (ratio3<1e-6)

fname= os.path.basename(__file__)
if testOK:
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')

# # Graphic ouput
# from postprocess import output_handler
# oh= output_handler.OutputHandler(modelSpace)

# # Write graphic file.
# oh.displayNodeValueDiagram(itemToDisp= 'dispMax')
