# -*- coding: utf-8 -*-
from __future__ import print_function

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (A_OO)"
__copyright__= "Copyright 2015, LCPT and AO_O"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com ana.ortega.ort@gmal.com"

# Testing fiber section model of Steel01 uniaxial material.

import xc_base
import geom
import xc
from model import predefined_spaces
from materials import typical_materials

width= 1
depth= 1
nDivIJ= 2
nDivJK= 2
nFibTeor= nDivIJ*nDivJK
areaTeor= width*depth
iyTeor= 1/12.0*width*depth**3
izTeor= 1/12.0*depth*width**3
y0= 0
z0= 0
F= 1000 # Force magnitude

# Problem type
feProblem= xc.FEProblem()
preprocessor=  feProblem.getPreprocessor
nodes= preprocessor.getNodeHandler
modelSpace= predefined_spaces.StructuralMechanics2D(nodes)

# Node definition

nodes.defaultTag= 1 # First node number.
nod1= nodes.newNodeXY(1,0)
nod2= nodes.newNodeXY(1,0)

# Materials definition: uniaxial bilinear steel
fy= 2600 # Yield stress.
E= 2.1e6 # Young modulus.
b=0.001    # strain-hardening ratio: ratio between post-yield tangent and initial elastic tangent
steel= typical_materials.defSteel01(preprocessor=preprocessor,name="steel",E=E,fy=fy,b=b)

# Sections
import os
pth= os.path.dirname(__file__)
# print("pth= ", pth)
if(not pth):
  pth= "."
exec(open(pth+"/../../../../aux/testQuadRegion.py").read())
quadFibers= preprocessor.getMaterialHandler.newMaterial("fiber_section_3d","quadFibers")
fiberSectionRepr= quadFibers.getFiberSectionRepr()
fiberSectionRepr.setGeomNamed(testQuadRegion.name)
quadFibers.setupFibers()
fibras= quadFibers.getFibers()

nfibras= fibras.getNumFibers
Iz= fibras.getIz
Iy= fibras.getIy
centerOfMassZ= fibras.getCenterOfMassZ()
centerOfMassY= fibras.getCenterOfMassY()
'''
             \for_each_fiber

print("fibra: ",tag, " mat. tag:", getMaterial.tag)
'''



# Elements definition
elements= preprocessor.getElementHandler
elements.defaultMaterial= quadFibers.name
elements.dimElem= 1 # Dimension of element space
elements.defaultTag= 1
zl= elements.newElement("ZeroLengthSection",xc.ID([1,2]))

# Constraints
constraints= preprocessor.getBoundaryCondHandler
#
spc= constraints.newSPConstraint(1,0,0.0) # Node 1
spc= constraints.newSPConstraint(1,1,0.0)
spc= constraints.newSPConstraint(1,2,0.0)
spc= constraints.newSPConstraint(2,1,0.0)
spc= constraints.newSPConstraint(2,2,0.0)

# Load definition.
lp0= modelSpace.newLoadPattern(name= '0')
lp0.newNodalLoad(2,xc.Vector([F,0.0,0.0]))

# We add the load case to domain.
modelSpace.addLoadCaseToDomain(lp0.name)

# Solution
result= modelSpace.analyze(calculateNodalReactions= True)

nod2= nodes.getNode(2)
deltax= nod2.getDisp[0] 
nod1= nodes.getNode(1)
R= nod1.getReaction[0] 

elements= preprocessor.getElementHandler

elem1= elements.getElement(1)
elem1.getResistingForce()
scc= elem1.getSection()

nfib= scc.getFibers().getNumFibers()
avgStrain= 0.0
fibras= scc.getFibers()
for f in fibras:
# print("fiber tag: ",tag, " strain: ", getMaterial.strain)
   avgStrain+= f.getMaterial().getStrain()
avgStrain/= nfib

ratio1= -R/F
ratio2= (E*deltax)/F
ratio3= (avgStrain-deltax)/deltax

''' 
print("R= ",R)
print("dx= ",deltax)
print("ratio1= ",(ratio1))
print("ratio2= ",(ratio2))
print("ratio3= ",(ratio3))
   '''
  
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if (abs(ratio1-1.0)<1e-5) & (abs(ratio2-1.0)<1e-5) & (abs(ratio3)<1e-5) :
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')
