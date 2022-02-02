# -*- coding: utf-8 -*-

from __future__ import division
from __future__ import print_function

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AO_O)"
__copyright__= "Copyright 2015, LCPT and AO_O"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@ciccp.es ana.ortega@ciccp.es"

import xc_base
import geom
import xc

from misc_utils import log_messages as lmsg
from import_export import freecad_reader
from import_export import neutral_mesh_description as nmd
from model import predefined_spaces
from materials.awc_nds import AWCNDS_materials
from materials.awc_nds import structural_panels
from materials.awc_nds import dimensional_lumber
import os
from actions import load_cases as lcm
from actions import combinations as combs
from solution import predefined_solutions

# Unit conversion.
gravity= 9.81 # m/s2

# Input data
def getRelativeCoo(pt):
    return [pt[0]/1000.0,pt[1]/1000.0,pt[2]/1000.0]

groupsToImport= ['.*Column*', 'Lintel.*', '.*Support.*']

pth= os.path.dirname(__file__)
if(not pth):
  pth= "."
filePath= pth+'/../../aux/freecad' 
baseName= 'test_pinned_member'
freeCADFileName= baseName+'.FCStd'

# Finite element problem.
FEcase= xc.FEProblem()
FEcase.title= 'Test pinned member.'
preprocessor= FEcase.getPreprocessor
nodes= preprocessor.getNodeHandler
modelSpace= predefined_spaces.StructuralMechanics3D(nodes)

## Problem geometry
## Import geometry from FreeCAD.
FEcase.outputFileName= '/tmp/out.txt' # Avoid message about topology version.
modelSpace.importFromFreeCAD(fileName= filePath+'/'+freeCADFileName, groupsToImport= groupsToImport, getRelativeCoo= getRelativeCoo, threshold= 0.001)

## Define sets
xcTotalSet= modelSpace.getTotalSet()
lintelSet= preprocessor.getSets.defSet('lintelSet')
columnSet= preprocessor.getSets.defSet('columnSet')
anchorSet= preprocessor.getSets.defSet('anchorSet')

meshSets= [lintelSet, columnSet]

### Classify the block topology objects (points, lines, surfaces, volumes).
setsFromLabels= [('Lintel.*', lintelSet), ('Column.*', columnSet), ('Support.*', anchorSet)]
modelSpace.classifyBlockTopologyObjects(setsFromLabels)

## Define element orientation (normally this must be read from the IFC file),
globalZVector= geom.Vector3d(0,0,1)
globalXVector= geom.Vector3d(1,0,0)
predefined_spaces.setWebOrientation(xcTotalSet.lines, vertBarsWebOrientationVector= globalXVector, barsWebOrientationVector= globalZVector)
elementLength= 0.5
## Set element size.
for l in xcTotalSet.lines:
    l.setElemSize(elementLength)

## Template coordinate transformation.
lin= preprocessor.getTransfCooHandler.newLinearCrdTransf3d('lin')
lin.xzVector= xc.Vector([1.0,0,0])

# Materials
wood= dimensional_lumber.SouthernPineWood(name='SouthernPine', grade= 'no_2', sub_grade= '')
#columnsSection= AWCNDS_materials.DimensionLumberSection(name= '10x2', woodMaterial= wood).defElasticShearSection3d(preprocessor)
lintelsSection= structural_panels.LVLHeaderSections['1.75x18'].defElasticShearSection3d(preprocessor)
columnsSection= lintelsSection

# Mesh generation.
seedElemHandler= preprocessor.getElementHandler.seedElemHandler
seedElemHandler.defaultTransformation= "lin"

def createMesh(xcSet, section):
    seedElemHandler.defaultMaterial= section.name

    for l in xcSet.getLines:
        vDir= l.getProp('webOrientation')
        lin.xzVector= xc.Vector([vDir.x, vDir.y, vDir.z])
        elem= seedElemHandler.newElement("ElasticBeam3d",xc.ID([0,0]))
        l.genMesh(xc.meshDir.I)
    xcSet.fillDownwards()
    
## Columns.
createMesh(columnSet, columnsSection)
## Lintels.
createMesh(lintelSet, lintelsSection)

## Constraints
### Supports.
for p in anchorSet.getPoints:
    if(not p.hasNode):
        lmsg.warning('point: '+str(p.name)+' not meshed.')
    else:
        n= p.getNode()
        modelSpace.fixNode('000_000',n.tag)

### Pinned joints.
for l in xcTotalSet.lines:
    predefinedType= l.getProp('attributes')['PredefinedType']
    if(predefinedType=='PIN_JOINED_MEMBER'):
        modelSpace.releaseLineExtremities(l)

# Loads

## Define "spacing" property
for l in lintelSet.lines:
    l.setProp('spacing',0.5)

loadCaseManager= lcm.LoadCaseManager(preprocessor)
loadCaseNames= ['deadLoad']
loadCaseManager.defineSimpleLoadCases(loadCaseNames)

def loadOnLines(xcSet, loadVector):
    for l in xcSet.getLines:
        spacing= l.getProp('spacing')
        for e in l.getElements:
            e.vector3dUniformLoadGlobal(spacing*loadVector)

## Dead load.
cLC= loadCaseManager.setCurrentLoadCase('deadLoad')

# ### Self weight on elements.
gravityVector= xc.Vector([0,0,gravity])
for e in xcTotalSet.elements:
    dim= e.getDimension
    if(dim==1):
        e.createInertiaLoad(gravityVector)

### Dead load on elements.
deadL= 10e3
uniformLoad= xc.Vector([0.0,0.0,-deadL])
loadOnLines(lintelSet,uniformLoad)

# Load combination definition
combContainer= combs.CombContainer()

## Serviceability limit states.

combContainer.SLS.qp.add('EQ1608', '1.0*deadLoad')


combContainer.dumpCombinations(preprocessor)

preprocessor.getLoadHandler.addToDomain('EQ1608')

# Solution
# Linear static analysis.
result= modelSpace.analyze(calculateNodalReactions= True)

if(result!=0):
    quit()

nA= anchorSet.points[0].getNode()
RA= nA.getReaction
nB= anchorSet.points[1].getNode()
RB= nB.getReaction
orderOfMag= (RA.Norm()+RB.Norm())/2.0
ratio1= abs(RA[0]+RB[0])/orderOfMag # Rx
ratio2= abs(RA[1]+RB[1])/orderOfMag # Ry
ratio3= abs(RA[2]+RB[2]-12912.9*2.0)/orderOfMag # Rz
ratio4= abs(RA[3]+RB[3])/orderOfMag # Mx
ratio5= abs(RA[4]-RB[4])/orderOfMag # My
ratio6= abs(RA[5]+RB[5])/orderOfMag # Mz

'''
print(RA)
print(RB)
print('ratio1= ', ratio1)
print('ratio2= ', ratio2)
print('ratio3= ', ratio3)
print('ratio4= ', ratio4)
print('ratio5= ', ratio5)
print('ratio6= ', ratio6)
'''

# Graphic stuff.
# from postprocess import output_handler
# oh= output_handler.OutputHandler(modelSpace)

# oh.displayBlocks()#setToDisplay= columnSet)
# #oh.displayFEMesh()
# #oh.displayDispRot(itemToDisp='uZ')
# oh.displayLoads()#setToDisplay= lvlBlindFasciaSet)
# #oh.displayStrongWeakAxis()
# oh.displayIntForcDiag(itemToDisp= 'Mz', setToDisplay= lintelSet)
import os
fname= os.path.basename(__file__)
if((abs(ratio1)<1e-10) & (abs(ratio2)<1e-15) & (abs(ratio3)<1e-5) & (abs(ratio4)<1e-15) & (abs(ratio5)<1e-2) & (abs(ratio6)<1e-15)):
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')
