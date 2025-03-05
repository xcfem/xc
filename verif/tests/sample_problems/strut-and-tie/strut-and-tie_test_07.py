# -*- coding: utf-8 -*-
''' Compare the results given by PileCap3Piles strut-and-tie model whith
    those obtained from the design performed with Statik-8.
'''

from __future__ import division
from __future__ import print_function

__author__= "Luis Claudio Pérez Tato (LCPT)"
__copyright__= "Copyright 2025, LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

import os
import sys
import math
import geom
import xc
from model import predefined_spaces
from materials.ec2 import EC2_materials
from materials.sections import section_properties as sp
from materials.sections.fiber_section import def_simple_RC_section
from materials.sections.fiber_section import def_column_RC_section
from misc_utils import log_messages as lmsg
from solution import predefined_solutions
from model.mesh import strut_and_tie_utils
from postprocess import def_vars_control as vc

# Problem type.
feProblem= xc.FEProblem()
preprocessor=  feProblem.getPreprocessor
nodes= preprocessor.getNodeHandler
modelSpace= predefined_spaces.StructuralMechanics3D(nodes)

pierHeight= 10.949
lintelHeight= 2.0
supportsDistY= 1.5
halfSupportsDistY= supportsDistY/2.0
supportsDistX= 2.5
halfSupportsDistX= supportsDistX/2.0

insertionDepth= 0.2 # penetration of the pier and the piles into the
                    # pile cap. That makes place to the struts.
pileCapHeight= 2.3-2*insertionDepth

## Define mesh.
### Top of the pier.
n0= modelSpace.newNode(0.0, 0.0, pierHeight) # lintel top.
#### Lower left support.
n0LL= modelSpace.newNode(-halfSupportsDistX, -halfSupportsDistY, pierHeight)
#### Lower right support.
n0LR= modelSpace.newNode(halfSupportsDistX, -halfSupportsDistY, pierHeight)
#### Upper right support.
n0UR= modelSpace.newNode(halfSupportsDistX, halfSupportsDistY, pierHeight)
#### Upper left support.
n0UL= modelSpace.newNode(-halfSupportsDistX, halfSupportsDistY, pierHeight)
n0b= modelSpace.newNode(0.0, 0.0, pierHeight-lintelHeight) # lintel bottom.
n1= modelSpace.newNode(0.0, 0.0, -insertionDepth) # pier bottom.

### Bottom of the pile cap.
pileRadius= 2.6 # Distance between the piles and the center of the pile cap.
pileLength= pierHeight/2.0+insertionDepth
angle= math.radians(60)
cosAngle= math.cos(angle)
sinAngle= math.sin(angle)
pileTopZ= -insertionDepth-pileCapHeight
pileATop= geom.Pos3d(pileRadius, 0.0, pileTopZ) # top of the pile A.
pileBTop= geom.Pos3d(-pileRadius*cosAngle, -pileRadius*sinAngle, pileTopZ) # top of the pile B.
pileCTop= geom.Pos3d(pileBTop.x, -pileBTop.y, pileTopZ) # top of the pile C.
pileTopPoints= [pileATop, pileBTop, pileCTop]
pileTopNodes= list()
pileBottomNodes= list()
for pt in pileTopPoints:
    pileTopNodes.append(modelSpace.newNode(pt.x, pt.y, pt.z))
    pileBottomNodes.append(modelSpace.newNode(pt.x, pt.y, pt.z-pileLength))
    
# Define materials. 
concrete= EC2_materials.C30
reinfSteel= EC2_materials.S500B

## Define pier material.
pierRCSection= sp.RectangularHollowSection(name= 'pierRCSection', b= 4.0, h= 2.0, t= 0.35)
xcPierSectionMaterial= pierRCSection.defElasticShearSection3d(preprocessor, material= concrete.getElasticMaterialData())

### Define pier.
#### Define pier elements.
linZ= modelSpace.newLinearCrdTransf("linZ", xzVector= xc.Vector([0,0,1]))
modelSpace.setDefaultCoordTransf(linZ)
modelSpace.setDefaultMaterial(xcPierSectionMaterial)
modelSpace.newElement('ElasticBeam3d', [n0.tag, n0LL.tag])
modelSpace.newElement('ElasticBeam3d', [n0.tag, n0LR.tag])
modelSpace.newElement('ElasticBeam3d', [n0.tag, n0UL.tag])
modelSpace.newElement('ElasticBeam3d', [n0.tag, n0UR.tag])
linY= modelSpace.newLinearCrdTransf("linY", xzVector= xc.Vector([0,-1,0]))
modelSpace.setDefaultCoordTransf(linY)
lintelElement= modelSpace.newElement('ElasticBeam3d', [n0.tag, n0b.tag])
pierElement= modelSpace.newElement('ElasticBeam3d', [n0b.tag, n1.tag])

### Define piles.
#### Define pile material.
pileDiameter= 1.42
pilesRCSection= def_column_RC_section.RCCircularSection(name= 'pilesRCSection', sectionDescr= 'piles section', concrType= concrete, reinfSteelType= reinfSteel, Rext= pileDiameter/2.0)
pilesMaterial= pilesRCSection.defElasticShearSection3d(preprocessor, reductionFactor= 1.0)
modelSpace.setDefaultMaterial(pilesMaterial)
pileElements= list()
for nTop, nBottom in zip(pileTopNodes, pileBottomNodes):
    pileElements.append(modelSpace.newElement('ElasticBeam3d', [nTop.tag, nBottom.tag]))

pierMinInertia= min(pierRCSection.Iy(),pierRCSection.Iz())
eqRadius= math.pow(4*pierMinInertia/math.pi,1/4.0)
pierEffectiveDiameter= 0.8*2*eqRadius

pileCap= strut_and_tie_utils.PileCap3Piles(pierBottomNode= n1, pileTopNodeA= pileTopNodes[0], pileTopNodeB= pileTopNodes[1], pileTopNodeC= pileTopNodes[2],  pierEffectiveDiameter= pierEffectiveDiameter)


# Define pile cap.
strutAndTieModel= True
if(strutAndTieModel):
    strutArea= math.pi*(pierEffectiveDiameter/2.0)**2/math.sqrt(2)
    diam32Area= math.pi*(32e-3/2.0)**2
    diam25Area= math.pi*(25e-3/2.0)**2
    diam16Area= math.pi*(16e-3/2.0)**2
    pileCap.createStrutAndTieModel(modelSpace, strutArea= strutArea, concrete= concrete, pileTiesArea= 24*diam25Area, radialTiesArea= 10*diam16Area, bottomChordsTiesArea= 10*diam32Area, topChordsTiesArea= 10*diam25Area, shearTiesArea= 10*diam16Area, reinfSteel= reinfSteel, xcPierSectionMaterial= xcPierSectionMaterial, linearElastic= False)
else:
    pileCap.createDummyElasticModel(modelSpace, concrete= concrete, releaseExtremities= True)

## Define constraints
for n in pileBottomNodes:
    modelSpace.fixNode('000_000', n.tag)
    
# Load definition.
gravityVector= xc.Vector([0,0,10.0])
## Self weight.
selfWeight= modelSpace.newLoadPattern(name= 'G')
modelSpace.setCurrentLoadPattern(selfWeight.name)
### Pier self weight.
pierElement.createInertiaLoad(gravityVector)
### Pile cap self weight.
selfWeight.newNodalLoad(n1.tag,xc.Vector([0, 0, -2118e3, 0, 0, 0]))
### Add the load case to domain.
# modelSpace.addLoadCaseToDomain(selfWeight.name)

## H1ELU
h1ELU= modelSpace.newLoadPattern(name= 'H1ELU')
modelSpace.setCurrentLoadPattern(h1ELU.name)
p1= h1ELU.newNodalLoad(n0UL.tag, xc.Vector([0, 826e3, 0, 0, 0, 0]))
p2= h1ELU.newNodalLoad(n0UR.tag, xc.Vector([0, 826e3, 0, 0, 0, 0]))
p3= h1ELU.newNodalLoad(n0UR.tag, xc.Vector([0, 0, -5902e3, 0, 0, 0]))
p4= h1ELU.newNodalLoad(n0UL.tag, xc.Vector([0, 0, -3062e3, 0, 0, 0]))
p5= h1ELU.newNodalLoad(n0UL.tag, xc.Vector([354e3, 0, 0, 0, 0, 0]))
p6= h1ELU.newNodalLoad(n0LR.tag, xc.Vector([0, 0, -5902e3, 0, 0, 0]))
p7= h1ELU.newNodalLoad(n0LL.tag, xc.Vector([0, 0, -3062e3, 0, 0, 0]))
p8= h1ELU.newNodalLoad(n0LL.tag, xc.Vector([354e3, 0, 0, 0, 0, 0]))
### Add the load case to domain.
# modelSpace.addLoadCaseToDomain(h1ELU.name)

## H2ELU
h2ELU= modelSpace.newLoadPattern(name= 'H2ELU')
modelSpace.setCurrentLoadPattern(h2ELU.name)
p1= h2ELU.newNodalLoad(n0LL.tag, xc.Vector([0, 0, -3062e3, 0, 0, 0]))
p2= h2ELU.newNodalLoad(n0UL.tag, xc.Vector([0, 0, -3296e3, 0, 0, 0]))
p3= h2ELU.newNodalLoad(n0UR.tag, xc.Vector([0, 0, -5642e3, 0, 0, 0]))
p4= h2ELU.newNodalLoad(n0LR.tag, xc.Vector([0, 0, -5902e3, 0, 0, 0]))
p5= h2ELU.newNodalLoad(n0LL.tag, xc.Vector([354e3, 0, 0, 0, 0, 0]))
p6= h2ELU.newNodalLoad(n0UL.tag, xc.Vector([209e3, 0, 0, 0, 0, 0]))
p7= h2ELU.newNodalLoad(n0UL.tag, xc.Vector([0, 1007e3, 0, 0, 0, 0]))
p8= h2ELU.newNodalLoad(n0UR.tag, xc.Vector([0, 1007e3, 0, 0, 0, 0]))
### Add the load case to domain.
# modelSpace.addLoadCaseToDomain(h2ELU.name)

## H3ELU
h3ELU= modelSpace.newLoadPattern(name= 'H3ELU')
modelSpace.setCurrentLoadPattern(h3ELU.name)
p1= h3ELU.newNodalLoad(n0UL.tag, xc.Vector([0, 72e3, 0, 0, 0, 0]))
p2= h3ELU.newNodalLoad(n0UR.tag, xc.Vector([0, 72e3, 0, 0, 0, 0]))
p3= h3ELU.newNodalLoad(n0UR.tag, xc.Vector([0, 0, -5952e3, 0, 0, 0]))
p4= h3ELU.newNodalLoad(n0UL.tag, xc.Vector([0, 0, -1417e3, 0, 0, 0]))
p5= h3ELU.newNodalLoad(n0UL.tag, xc.Vector([744e3, 0, 0, 0, 0, 0]))
p6= h3ELU.newNodalLoad(n0LR.tag, xc.Vector([0, 0, -5952e3, 0, 0, 0]))
p7= h3ELU.newNodalLoad(n0LL.tag, xc.Vector([0, 0, -1417e3, 0, 0, 0]))
p8= h3ELU.newNodalLoad(n0LL.tag, xc.Vector([744e3, 0, 0, 0, 0, 0]))
### Add the load case to domain.
# modelSpace.addLoadCaseToDomain(h3ELU.name)

# Define load combinations.
combs= modelSpace.getLoadCombinationsHandler()
comb1= combs.newLoadCombination("ELU001","1.00*G")
comb2= combs.newLoadCombination("ELU002","1.35*G")
comb3= combs.newLoadCombination("ELU003","1.00*G + 1.00*H1ELU")
comb4= combs.newLoadCombination("ELU004","1.00*G + 1.00*H2ELU")
comb5= combs.newLoadCombination("ELU005","1.00*G + 1.00*H3ELU")
comb6= combs.newLoadCombination("ELU006","1.35*G + 1.00*H1ELU")
comb7= combs.newLoadCombination("ELU007","1.35*G + 1.00*H2ELU")
comb8= combs.newLoadCombination("ELU008","1.35*G + 1.00*H3ELU")

if(strutAndTieModel):
    analysis= predefined_solutions.penalty_newton_raphson(feProblem, maxNumIter= 20, printFlag= 0)
else:
    modelSpace.setupAnalysis()
    analysis= modelSpace.analysis

vc.def_reactions_envelope_vars(nodes= pileBottomNodes)
vc.def_vars_envelope_internal_forces_beam_elems(elems= [lintelElement])
for combName in combs.getKeys():
    comb= combs[combName]
    modelSpace.removeAllLoadsAndCombinationsFromDomain()
    modelSpace.revertToStart()
    modelSpace.addLoadCaseToDomain(combName)
    result= analysis.analyze(1)
    if(result!=0):
        lmsg.error("Can't solve.")
        info= None
        solver= analysis.linearSOE.solver
        if(solver.hasProp("info")):
            info= solver.getProp("info")
        unconstrainedNode= modelSpace.locateEquationNumber(eqNumber= info-1)
        lmsg.error('unconstrained node id: '+str(unconstrainedNode.tag))
        lmsg.error('unconstrained node position: '+str(unconstrainedNode.getInitialPos3d)+'\n')
        # exit(1)
    modelSpace.calculateNodalReactions()
    varNames= vc.update_reactions_envelope(nodes= pileBottomNodes)
    vc.update_envelope_internal_forces_beam_elem_3d(lintelElement)

pileSet= modelSpace.defSet('pileSet', elements= pileElements)
pileSet.fillDownwards()

lintelMinVy= lintelElement.getProp('Vy-')
lintelMinVy= sum(lintelMinVy)/len(lintelMinVy)
ratio1= abs(lintelMinVy+1488e3)/1488e3
lintelMaxVz= lintelElement.getProp('Vz+')
lintelMaxVz= sum(lintelMaxVz)/len(lintelMaxVz)
ratio2= abs(lintelMaxVz-2014e3)/2014e3

RzMax= 0.0
RzMin= 0.0
for n in pileBottomNodes:
    RzMax= max(RzMax, n.getProp('Rz+'))
    RzMin= min(RzMin, n.getProp('Rz-'))
if(strutAndTieModel):
    RzMaxRef= 14133.369744409514e3
    RzMinRef= -648.2710344892942e3
else:
    RzMaxRef= 14176.22576663143e3
    RzMinRef= -933.1167556855067e3
ratio3= abs(RzMax-RzMaxRef)/RzMaxRef

ratio4= abs(RzMin-RzMinRef)/abs(RzMinRef)

testOK= (abs(ratio1)<1e-10) and (abs(ratio2)<1e-10) and (abs(ratio3)<1e-10) and (abs(ratio4)<1e-10)

'''
print('lintelVyMin= ', lintelMinVy/1e3, ratio1)
print('lintelVzMax= ', lintelMaxVz/1e3, ratio2)
print('RzMax= ', RzMax/1e3, ratio3)
print('RzMin= ', RzMin/1e3, ratio4)
'''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if testOK:
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')

# # Graphic stuff.
# from postprocess import output_handler
# oh= output_handler.OutputHandler(modelSpace)
# # oh.displayBlocks()
# oh.displayFEMesh()
# oh.displayLoads()
# oh.displayReactions()
# oh.displayIntForcDiag('N')
# oh.displayIntForcDiag('My', setToDisplay= pileSet)
# oh.displayIntForcDiag('Mz', setToDisplay= pileSet)
# oh.displayIntForcDiag('Vy', setToDisplay= pileSet)
# oh.displayIntForcDiag('Vz', setToDisplay= pileSet)
# oh.displayDispRot(itemToDisp='uX', defFScale= 10)
# oh.displayDispRot(itemToDisp='uY', defFScale= 10)
# oh.displayDispRot(itemToDisp='uZ', defFScale= 10)

# # DXF output
# from import_export import mesh_entities
# xcTotalSet= modelSpace.getTotalSet()
# me= mesh_entities.MeshData()
# me.readFromXCSet(xcSet= xcTotalSet)
# outputFileName= fname.replace('.py', '.dxf')
# me.writeDxfFile(outputFileName)
