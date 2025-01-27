# -*- coding: utf-8 -*-
''' Symmetry constraint trivial test.

See section 4.2 of the book: Finite Element Procedures Klaus-Jurgen Bathe, 2006
 ISBN 097900490X, 9780979004902.  Page 190 et seq.
'''

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AOO)"
__copyright__= "Copyright 2025, LCPT and AOO"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

import math
import geom
import xc
from materials import typical_materials
from model import predefined_spaces
from solution import predefined_solutions
from misc_utils import log_messages as lmsg

def get_half_slab():
    ''' Return the geometry of the half slab.'''
    origin= geom.Pos3d(0,0,0)
    p0= origin
    p1= p0+slabWidth*dirVectorWidth
    p2= p1+slabLength*dirVectorLength
    p3= p2-slabWidth*dirVectorWidth
    return [p0, p1, p2, p3]

def get_whole_slab():
    ''' Return the geometry of the half slab.'''
    origin= geom.Pos3d(0,0,10)
    p0= origin
    p1= p0+slabWidth*dirVectorWidth
    p2= p1+2*slabLength*dirVectorLength
    p3= p2-slabWidth*dirVectorWidth
    return [p0, p1, p2, p3]     

# Problem geometry.
slabWidth= 2.0
slabLength= 5.0
dirVectorLength= geom.Vector3d(1,1,0).normalized()
dirVectorWidth= geom.Vector3d(-1,1,0).normalized()
# Half slab.
halfSlabQuadPositions= get_half_slab()
wholeSlabQuadPositions= get_whole_slab()

# Define FE problem.
feProblem= xc.FEProblem()
preprocessor= feProblem.getPreprocessor
nodes= preprocessor.getNodeHandler

# Problem type
modelSpace= predefined_spaces.StructuralMechanics3D(nodes)

## Define nodes.
### Half simply-suppoted slab (symmetry at points p2 and p3)
halfSlabQuadPoints= list()
for pos in halfSlabQuadPositions:
    halfSlabQuadPoints.append(modelSpace.newKPoint(pos.x, pos.y, pos.z))
### Vertical plane passing through points p2 and p3 (symmetry at points p2 and p3).
pA= halfSlabQuadPositions[-2]
pB= halfSlabQuadPositions[-1]
vertPlane= geom.Plane3d(pA, pB, pB+geom.Vector3d(0,0,100)) # Vertical plane at beam end.

wholeSlabQuadPoints= list()
for pos in wholeSlabQuadPositions:
    wholeSlabQuadPoints.append(modelSpace.newKPoint(pos.x, pos.y, pos.z))

halfSlabFace= modelSpace.newSurface(halfSlabQuadPoints)
wholeSlabFace= modelSpace.newSurface(wholeSlabQuadPoints)

## Define materials.
E= 2.1e9 # Young modulus of the steel (MPa)
nu= 0.3 # Poisson's ratio.
h= 0.2 # Thickness.
dens= 1.33 # Density kg/m2.
shellMat= typical_materials.defElasticMembranePlateSection(preprocessor, "shellMat",E,nu,dens,h)

## Define mesh.
modelSpace.setDefaultMaterial(shellMat)
modelSpace.newSeedElement("ShellMITC4")

elemSize= 0.25
for face in [halfSlabFace, wholeSlabFace]:
    face.setElemSizeIJ(elemSize, elemSize)
    face.genMesh(xc.meshDir.I)

## Define constraints
halfBeamSymmetryNodes= list()
for l in wholeSlabFace.edges:
    length= l.getLength()
    if(abs(length-slabWidth)<1e-3): # is a short side
        for n in l.nodes:
            modelSpace.fixNode('000_FFF', n.tag)
for l in halfSlabFace.edges:
    length= l.getLength()
    if(abs(length-slabWidth)<1e-3): # is a short side
        for n in l.nodes:
            pos= n.getInitialPos3d
            dist= vertPlane.dist(pos)
            if(dist>1e-3):
                modelSpace.fixNode('000_FFF', n.tag)
            else:
                symmetry= modelSpace.newSymmetryConstraint(n.tag, vertPlane)
                halfBeamSymmetryNodes.append(n)

## Define loads.
lp0= modelSpace.newLoadPattern(name= '0')
modelSpace.setCurrentLoadPattern(lp0.name)
for face in [halfSlabFace, wholeSlabFace]:
    for e in face.elements:
        e.vector3dUniformLoadGlobal(xc.Vector([0,0,-1e3]))
### We add the load case to domain.
modelSpace.addLoadCaseToDomain(lp0.name)

# Solve.
analysis= predefined_solutions.simple_static_linear(feProblem)
result= analysis.analyze(1)
if(result!=0):
    errMsg= 'Solution algorithm failed.'
    lmsg.error(errMsg)
    exit(1)
    
# Check results.
## Check displacement results (verify symmetry).
wholeBeamSymmetryNodes= list()
for n in wholeSlabFace.nodes:
    pos= n.getInitialPos3d
    dist= vertPlane.dist(pos)
    if(dist<1e-3):
        wholeBeamSymmetryNodes.append(n)
## Compute average displacements at mid-span.
### Of the whole beam.
wholeBeamAvgDeflection= geom.Vector3d(0,0,0)
for n in wholeBeamSymmetryNodes:
    disp= n.getDisp
    wholeBeamAvgDeflection+= geom.Vector3d(disp.getList())
wholeBeamAvgDeflection*=(1.0/len(wholeBeamAvgDeflection))
### Of the half beam.
halfBeamAvgDeflection= geom.Vector3d(0,0,0)
for n in halfBeamSymmetryNodes:
    disp= n.getDisp
    halfBeamAvgDeflection+= geom.Vector3d(disp.getList())

halfBeamAvgDeflection*=(1.0/len(halfBeamAvgDeflection))

errorDisp= (halfBeamAvgDeflection-wholeBeamAvgDeflection).getModulus()

'''
print(wholeBeamAvgDeflection)
print(halfBeamAvgDeflection)
print(errorDisp)
'''

import os
fname= os.path.basename(__file__)
if (abs(errorDisp)<1e-2):
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')

# # Graphic stuff.
# from postprocess import output_handler
# oh= output_handler.OutputHandler(modelSpace)
# # oh.displayBlocks()
# oh.displayFEMesh()
# # # oh.displayLocalAxes()
# oh.displayLoads()
# oh.displayReactions()
# oh.displayDispRot(itemToDisp='uX')
# oh.displayDispRot(itemToDisp='uY')
# oh.displayDispRot(itemToDisp='uZ')
