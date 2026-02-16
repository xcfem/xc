# -*- coding: utf-8 -*-
'''Railway bridge loads. Compute the deck twist "t" from the displacements of
the deck. Bridge deck twist refers to the rotational deformation of a bridge 
deck about its longitudinal axis.
'''

__author__= "Luis C. Pérez Tato (LCPT)"
__copyright__= "Copyright 2025, LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

import math
import geom
import xc
from actions.railway_traffic import track_axis as ta
from actions.railway_traffic import train_load_model as tlm
from actions.railway_traffic import EC1_rail_load_models
from model import predefined_spaces
from materials import typical_materials

def to_3d(p2d, z):
    return geom.Pos3d(p2d.x, p2d.y, z)

# Problem geometry.
width= 10.0
p0= geom.Pos2d(0.0, 0.0)
p1= geom.Pos2d(20.0, -3.0)
p2= geom.Pos2d(40.0, 1.0)
p3= geom.Pos2d(70.0, 7.0)
p4= geom.Pos2d(100.0, 0.0)

firstCurve= geom.CircularSector2d(p0, p1, p2)
secondCurve= geom.CircularSector2d(p2, p3, p4)

ptsA= firstCurve.getArcPoints(11)
ptsB= secondCurve.getArcPoints(11)

axisPoints2d= [p0]
for p in ptsA[1:-1]:
    axisPoints2d.append(p)
axisPoints2d.append(p2)
for p in ptsB[1:-1]:
    axisPoints2d.append(p)
axisPoints2d.append(p4)

axisPolyline2d= geom.Polyline2d(axisPoints2d)
pointsLeftBorder2d= axisPolyline2d.offset(-width/2.0).getVertexList()
pointsRightBorder2d= axisPolyline2d.offset(width/2.0).getVertexList()

# Define FE problem
feProblem= xc.FEProblem()
preprocessor=  feProblem.getPreprocessor
nodes= preprocessor.getNodeHandler
modelSpace= predefined_spaces.StructuralMechanics3D(nodes)
points= preprocessor.getMultiBlockTopology.getPoints

## Define problem geometry.
quadPoints= list()
zDeck= 5.0
elemSize= 1.0
pI0= pointsLeftBorder2d[0]
ptI0= points.newPoint(geom.Pos3d(pI0.x, pI0.y, zDeck))
pD0= pointsRightBorder2d[0]
ptD0= points.newPoint(geom.Pos3d(pD0.x, pD0.y, zDeck))
leftKPoints= [ptI0]
rightKPoints= [ptD0]
slabSet= modelSpace.defSet('slabSet')
for pI, pD in zip(pointsLeftBorder2d[1:], pointsRightBorder2d[1:]):
    ptI= points.newPoint(geom.Pos3d(pI.x, pI.y, zDeck))
    ptD= points.newPoint(geom.Pos3d(pD.x, pD.y, zDeck))
    s= modelSpace.newSurface([ptD0, ptD, ptI, ptI0])
    s.setElemSizeIJ(elemSize, elemSize)
    slabSet.surfaces.append(s)
    leftKPoints.append(ptI)
    rightKPoints.append(ptD)
    ptI0= ptI
    ptD0= ptD

slabSet.fillDownwards()

### Conciliate the number of divisions of the lines.
modelSpace.conciliaNDivs()

## Define materials
E= 30e9 # Elastic modulus
nu= 0.2 # Poisson's ratio
deckThickness= 0.35 # Deck thickness.
mat= typical_materials.defElasticMembranePlateSection(preprocessor, "mat",E= E, nu= nu, rho= 0.0, h= deckThickness)

## Define seed element.
seedElemHandler= preprocessor.getElementHandler.seedElemHandler
seedElemHandler.defaultMaterial= mat.name
elem= seedElemHandler.newElement("ShellMITC4")

## Mesh generation. 
slabSet.genMesh(xc.meshDir.I)
slabSet.fillDownwards()

## Define constraints.
constrainedLines= list()
### Abutments.
l= slabSet.lines.getNearest(to_3d(axisPoints2d[0], zDeck))
constrainedLines.append(l)
l= slabSet.lines.getNearest(to_3d(axisPoints2d[-1], zDeck))
constrainedLines.append(l)

length= 0.0
span= 10.0
p0= axisPoints2d[0]
for p in axisPoints2d[1:-1]:
    length+= p.dist(p0)
    if(length>=span):
        p3d= to_3d(p, zDeck)
        l= slabSet.lines.getNearest(p3d)
        constrainedLines.append(l)
        length= 0.0 # start again.
    p0= p

for l in constrainedLines:
    for n in l.nodes:
        modelSpace.fixNode000_FFF(n.tag)
    
# Define load.
## Define spreading parameters.
sleeperThickness= 22e-2
sleeperLength= 2.6
standardGauge= 1.435
lateralWidth= sleeperLength-standardGauge
sleeperSpreadToDepthRatio= lateralWidth/sleeperThickness

ballastThickness= 0.6 # Thickness of the ballast under the sleeper.
ballastSpreadToDepthRatio= 1/4.0 # See Eurocode 1 part 2 figure 6.5.

spreadingLayers= [(sleeperThickness, sleeperSpreadToDepthRatio), (ballastThickness, ballastSpreadToDepthRatio)]

## Define track axis.
axisPoints3d= list()
for p in axisPoints2d:
    axisPoints3d.append(to_3d(p, zDeck+ballastThickness+sleeperThickness))
axisPolyline3d= geom.Polyline3d(axisPoints3d)

trackAxis= ta.TrackAxis(trackAxis= axisPolyline3d)

## Define train model.
locomotiveLM1= EC1_rail_load_models.get_locomotive_LM1()
trainLoadModel= tlm.TrainLoadModel(locomotive= locomotiveLM1, uniformLoad= 80e3, dynamicFactor= 1.0, classificationFactor= 1.21)

## Define load patterns
relativePositions= [0.15, 0.249, 0.25, 0.251,0.5 , 0.75, 0.9]
loadNames= list()
modelSpace.removeAllLoadPatternsFromDomain()
maxTwist= 0.0
maxTwistPos= 0.0
for index, relativePosition in enumerate(relativePositions):
    name= 'Q1a'+str(index+1)

    modelSpace.revertToStart()
    cLC= modelSpace.newLoadPattern(name= name)
    modelSpace.setCurrentLoadPattern(cLC.name)
    nodalLoads= trackAxis.defDeckLoadsThroughLayers(trainModel= trainLoadModel, relativePosition= relativePosition, spreadingLayers= spreadingLayers, deckThickness= deckThickness, deckSpreadingRatio= 1/1, originSet= slabSet)
    modelSpace.addLoadCaseToDomain(cLC.name)

    modelSpace.analyze(calculateNodalReactions= True)
    # trainModel: load model of the train (see TrainLoadModel class).
    # relativePosition: parameter (0.0->start of the axis, 1.0->end of
    #                   the axis).
    # xcSet: set to search the nodes on.
    # length: length to measure the twist over.
    # removeGeometricalTwist: remove the twist due to the mesh geometry.
    twists= trackAxis.getTwist(trainModel= trainLoadModel, relativePosition= relativePosition, xcSet= slabSet, length= 3.0, removeGeometricalTwist= False)
    if(twists):
        localMaxTwist= max(twists)
        if(localMaxTwist>maxTwist):
            maxTwist= localMaxTwist
            maxTwistPos= relativePosition

    modelSpace.removeLoadCaseFromDomain(cLC.name)
    modelSpace.revertToStart()
    loadNames.append(cLC.name)

ratio= abs(maxTwist-0.0015422930703060144)/0.0015422930703060144
testOK= (maxTwistPos==0.25) and abs(ratio)<1e-4
    
# print(maxTwist*1e3, maxTwistPos)

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if testOK:
    print("test "+fname+": ok.")
else:
    lmsg.error("test "+fname+": ERROR.")

# # Graphic stuff.
# from postprocess import output_handler
# oh= output_handler.OutputHandler(modelSpace)
# # oh.displayBlocks()
# oh.displayFEMesh()
# # oh.displayLocalAxes()
# for loadName in loadNames:
#     modelSpace.addLoadCaseToDomain(loadName)
#     oh.displayLoads()
#     modelSpace.removeLoadCaseFromDomain(loadName)
#     modelSpace.revertToStart()
# # oh.displayReactions()
# # oh.displayDispRot(itemToDisp='uX', defFScale= 10.0)
# # oh.displayDispRot(itemToDisp='uY', defFScale= 10.0)
# # oh.displayDispRot(itemToDisp='uZ', defFScale= 10.0)

# # Export the FE model into a DXF file.
# from import_export import mesh_entities
# xcTotalSet= modelSpace.getTotalSet()
# me= mesh_entities.MeshData()
# numEntitiesRead= me.readFromXCSet(xcTotalSet)
# ok1= (abs(numEntitiesRead-5)==0) # four nodes and one element.
# import ezdxf
# outputFileName= './'+fname.replace('.py', '.dxf')
# me.writeDxfFile(outputFileName)

# # Export the track axis to another DXF file
# from geom_utils import dxf_document
# doc = ezdxf.new("R2000") # Create DXF document.
# dxfDocument= dxf_document.DXFDocument(ezdxfDoc= doc)

# ## Get the axis geometry.
# track_axis= trackAxis
# dxfDocument.addPolyline3d(track_axis.trackAxis)

# outputFileName= './track_axis.dxf'
# doc.saveas(outputFileName)
