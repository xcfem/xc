# -*- coding: utf-8 -*-
''' Trivial test for the reinforceMidPoints method.'''

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (A_OO)"
__copyright__= "Copyright 2026, LCPT and AO_O"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com ana.ortega.ort@gmal.com"

import math
import geom
import xc
from model import predefined_spaces
from materials import typical_materials

width= 1
depth= 2
y0= 0
z0= 0
nRebarsA= 6
nRebarsB= 6
nRebars= nRebarsA+nRebarsB
totalArea= 0.5
As= totalArea/nRebars
F= 500.0 # Force magnitude

# Problem type
feProblem= xc.FEProblem()
preprocessor=  feProblem.getPreprocessor
nodes= preprocessor.getNodeHandler
modelSpace= predefined_spaces.StructuralMechanics2D(nodes)

n1= nodes.newNodeXY(1,0)
n2= nodes.newNodeXY(1,0)

# Materials definition: uniaxial bilinear steel
fy= 2600   # yield strength
E= 2.1e6   # initial elastic tangent
b=0.001    # strain-hardening ratio: ratio between post-yield tangent and initial elastic tangent
steel= typical_materials.defSteel01(preprocessor=preprocessor,name="steel",E=E,fy=fy,b=b)


quadFibersGeom= preprocessor.getMaterialHandler.newSectionGeometry("quadFibersGeom")
y1= width/2.0
z1= depth/2.0

reinforcement= quadFibersGeom.getReinfLayers
segmentA= geom.Segment2d(geom.Pos2d(y0-depth/2.0,z0-width/2.0), geom.Pos2d(y0+depth/2.0,z0-width/2.0)) # positions of the first and last rebars.
spacing= segmentA.getLength()/(nRebarsA-1)
diameter= 2*math.sqrt(As/math.pi)
reinforcementA= reinforcement.reinforceSegment(steel.name, spacing, diameter, segmentA)

reinforcementAA= reinforcement.secondaryReinfLayer(reinforcementA, spacing/2.0, 2*spacing, diameter/2.0)
reinforcementAAPositions= reinforcementAA.getReinfBarsCenterPositions()

segmentB= geom.Segment2d(geom.Pos2d(y0-depth/2.0,z0+width/2.0), geom.Pos2d(y0+depth/2.0,z0+width/2.0)) # positions of the first and last rebars.
spacing= segmentA.getLength()/(nRebarsB-1)
diameter= 2*math.sqrt(As/math.pi)
reinforcementB= reinforcement.reinforceSegment(steel.name, spacing, diameter, segmentB)
reinforcementBB= reinforcement.secondaryReinfLayer(reinforcementB, spacing/2.0, 2*spacing, diameter/2.0)
reinforcementBBPositions= reinforcementBB.getReinfBarsCenterPositions()


materialHandler= preprocessor.getMaterialHandler
# Sections
quadFibers= materialHandler.newMaterial("fiber_section_3d","quadFibers")
fiberSectionRepr= quadFibers.getFiberSectionRepr()
fiberSectionRepr.setGeomNamed(quadFibersGeom.name)
quadFibers.setupFibers()
fibers= quadFibers.getFibers()

# nfibers= fibers.getNumFibers()
# sumAreas= fibers.getArea(1.0)
# Iz= fibers.getIz(1.0,y0)
# Iy= fibers.getIy(1.0,z0)
# centerOfMassZ= fibers.getCenterOfMassZ()
# centerOfMassY= fibers.getCenterOfMassY()

# Elements definition
elements= preprocessor.getElementHandler
elements.defaultMaterial= quadFibers.name
elements.dimElem= 1 # Dimension of element space
elem= elements.newElement("ZeroLengthSection",xc.ID([n1.tag, n2.tag]))

# Constraints
constraints= preprocessor.getBoundaryCondHandler
spc= constraints.newSPConstraint(n1.tag, 0,0.0) # Node 1
spc= constraints.newSPConstraint(n1.tag, 1,0.0)
spc= constraints.newSPConstraint(n1.tag, 2,0.0)
spc= constraints.newSPConstraint(n2.tag, 1,0.0)
spc= constraints.newSPConstraint(n2.tag, 2,0.0)

# Load definition.
lp0= modelSpace.newLoadPattern(name= '0')
lp0.newNodalLoad(n2.tag, xc.Vector([F,0.0,0.0]))

# We add the load case to domain.
modelSpace.addLoadCaseToDomain(lp0.name)

# Solution
result= modelSpace.analyze(calculateNodalReactions= True)

deltax= n2.getDisp[0] 
R= n1.getReaction[0] 


elem.getResistingForce()
scc= elem.getSection()
nfib= scc.getFibers().getNumFibers()
avgStrain= 0.0
fibers= scc.getFibers()
sumAreas= fibers.getArea(1.0)
centerOfMassZ= fibers.getCenterOfMassZ()
centerOfMassY= fibers.getCenterOfMassY()

avgStrain= 0.0
for f in fibers:
# print("fiber: ",tag, " strain: ", getMaterial.strain)
   avgStrain+= f.getMaterial().getStrain()
avgStrain/= nfib

nRebarsTot= nRebars+6 # original + mid points.
totalArea+= 6*(As/4) # area of the mid points reinforcement.
ratio1= (nfib-nRebarsTot)/nRebarsTot
ratio2= (sumAreas-totalArea)/totalArea
ratio3= centerOfMassY-y0
ratio4= centerOfMassZ-z0
ratio5= 0.0
ratio6= 0.0
for i, x in enumerate([-0.8, 0, 0.8]):
   ratio5+= (reinforcementAAPositions[i].x-x)**2
   ratio6+= (reinforcementBBPositions[i].x-x)**2
ratio5= math.sqrt(ratio5)
ratio6= math.sqrt(ratio6)
ratio7= (-R-F)/F
ratio8= ((E*deltax*totalArea)-F)/F
ratio9= (avgStrain-deltax)/deltax

''' 
print("R= ",R)
print("dx= ",deltax)
print("ratio1= ",ratio1)
print("ratio2= ",ratio2)
print("ratio3= ",ratio3)
print("ratio4= ",ratio4)
print('Reinforcement AA positions: ', reinforcementAAPositions)
print("ratio5= ",ratio5)
print('Reinforcement BB positions: ', reinforcementBBPositions)
print("ratio6= ",ratio6)
print("ratio7= ",ratio7)
print("ratio8= ",ratio8)
print("ratio9= ",ratio9)
   '''
import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if (abs(ratio1)<1e-5) & (abs(ratio2)<1e-5) & (abs(ratio3)<1e-5) & (abs(ratio4)<1e-5) & (abs(ratio5)<1e-5) & (abs(ratio6)<1e-5) & (abs(ratio7)<1e-5) & (abs(ratio8)<1e-5) & (abs(ratio9)<1e-5) :
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')
    
# # Section output
# import matplotlib.pyplot as plt
# from materials.sections.fiber_section import plot_fiber_section
# fig = plt.figure()
# ax= fig.add_subplot(111)
# plot_fiber_section.mplot_section_geometry(ax, sectionGeometry= quadFibersGeom)
# plt.show()
