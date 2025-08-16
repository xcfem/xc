# -*- coding: utf-8 -*-
''' Check element reactivation with nodes belonging to reactivated and deactivated sets.'''

from __future__ import division
from __future__ import print_function

import geom
import xc
from model import predefined_spaces
from materials import typical_materials

__author__= "Luis C. Pérez Tato (LCPT)"
__copyright__= "Copyright 2023, LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

feProblem= xc.FEProblem()
preprocessor=  feProblem.getPreprocessor
nodes= preprocessor.getNodeHandler
# Problem type
modelSpace= predefined_spaces.SolidMechanics2D(nodes)

# Define k-points.
A= 5; B= 3; C= 1
p1= modelSpace.newKPoint(-A,-A,0)
p2= modelSpace.newKPoint(A, -A, 0)
p3= modelSpace.newKPoint(A, A, 0)
p4= modelSpace.newKPoint(-A, A, 0)
p5= modelSpace.newKPoint(-B, -B, 0)
p6= modelSpace.newKPoint(B, -B, 0)
p7= modelSpace.newKPoint(B, B, 0)
p8= modelSpace.newKPoint(-B, B, 0)
p9= modelSpace.newKPoint(-C, -C, 0)
p10= modelSpace.newKPoint(C, -C, 0)
p11= modelSpace.newKPoint(C, C, 0)
p12= modelSpace.newKPoint(-C, C, 0)

# Define surfaces.
s1= modelSpace.newQuadSurface(p1, p2, p6, p5)
s2= modelSpace.newQuadSurface(p6, p2, p3, p7)
s3= modelSpace.newQuadSurface(p7, p3, p4, p8)
s4= modelSpace.newQuadSurface(p1, p5, p8, p4)

s5= modelSpace.newQuadSurface(p5, p6, p10, p9)
s6= modelSpace.newQuadSurface(p10, p6, p7, p11)
s7= modelSpace.newQuadSurface(p12, p11, p7, p8)
s8= modelSpace.newQuadSurface(p5, p9, p12, p8)

s9= modelSpace.newQuadSurface(p9, p10, p11, p12)

for s in [s1, s2, s3, s4, s5, s6, s7, s8, s9]:
    s.nDivI= 2
    s.nDivJ= 2

# Define material.
E= 30e6 # Young modulus (psi)
nu= 0.3 # Poisson's ratio
rho= 1e3 # Density
elast2d= typical_materials.defElasticIsotropicPlaneStress(preprocessor, "elast2d",E,nu,rho)

# Create mesh.
seedElemHandler= modelSpace.getSeedElementHandler()
seedElemHandler.defaultMaterial= elast2d.name
quad4n= seedElemHandler.newElement("FourNodeQuad")
xcTotalSet= modelSpace.getTotalSet()
xcTotalSet.genMesh(xc.meshDir.I)

# Create constraints.
alreadyVisited= list()
for e in s1.edges:
    centroid= e.getCentroid()
    if(abs(centroid.y+A)<1e-3):
        for n in e.nodes:
            if(not n.tag in alreadyVisited):
                modelSpace.fixNode('F0', n.tag)
                alreadyVisited.append(n.tag)
alreadyVisited= list()
for e in s2.edges:
    centroid= e.getCentroid()
    if(abs(centroid.x-A)<1e-3):
        for n in e.nodes:
            if(not n.tag in alreadyVisited):
                modelSpace.fixNode('0F', n.tag)
                alreadyVisited.append(n.tag)
for e in s4.edges:
    centroid= e.getCentroid()
    if(abs(centroid.x+A)<1e-3):
        for n in e.nodes:
            if(not n.tag in alreadyVisited):
                modelSpace.fixNode('0F', n.tag)
                alreadyVisited.append(n.tag)

# Create sets.
exteriorSet= modelSpace.defSet('exteriorSet')
for s in [s1, s2, s3, s4]:
    exteriorSet.surfaces.append(s)
exteriorSet.fillDownwards()
middleSet= modelSpace.defSet('middleSet')
for s in [s5, s6, s7, s8]:
    middleSet.surfaces.append(s)
middleSet.fillDownwards()
interiorSet= modelSpace.defSet('interiorSet')
for s in [s9]:
    interiorSet.surfaces.append(s)
interiorSet.fillDownwards()

modelSpace.deactivateElements(middleSet)
modelSpace.deactivateElements(interiorSet)

modelSpace.activateElements(middleSet)

frozenNodes= list()
for n in xcTotalSet.nodes:
    if n.isFrozen:
        frozenNodes.append(n)

# Check results.
ratio1= len(frozenNodes)-1
ratio2= geom.Pos2d(0,0).dist(frozenNodes[0].getInitialPos2d)

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if((ratio1==0) and (abs(ratio2)<1e-8)):
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')


# # Graphic stuff.
# from postprocess import output_handler
# oh= output_handler.OutputHandler(modelSpace)
# # oh.displayBlocks()
# oh.displayFEMesh()
# # oh.displayLocalAxes()
# # oh.displayReactions()
# # oh.displayLoads()
# # oh.displayDispRot(itemToDisp='uX', defFScale= 10.0)
# # oh.displayDispRot(itemToDisp='uY', defFScale= 10.0)
# # oh.displayStresses('sigma_11', setToDisplay= galleryElements)
# # oh.displayStresses('sigma_22', setToDisplay= gallerySet)
