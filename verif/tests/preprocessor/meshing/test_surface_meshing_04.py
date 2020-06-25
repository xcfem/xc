# -*- coding: utf-8 -*-
from __future__ import print_function

import xc_base
import geom
import xc
import math
import os
from model import predefined_spaces
from materials import typical_materials

__author__= "Luis C. Pérez Tato (LCPT)"
__copyright__= "Copyright 2014, LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

R= 2.0
cos45= math.cos(math.radians(45))
sin45= cos45
E= 30e6 # Young modulus (psi)
nu= 0.3 # Poisson's ratio
rho= 0.0 # Density

area= 0

feProblem= xc.FEProblem()
preprocessor=  feProblem.getPreprocessor
nodes= preprocessor.getNodeHandler
modelSpace= predefined_spaces.StructuralMechanics3D(nodes)

elast= typical_materials.defElasticMaterial(preprocessor, "elast",3000)
matPrb= typical_materials.defElasticMembranePlateSection(preprocessor, "matPrb",E,nu,rho,0.25)



seedElemHandler= preprocessor.getElementHandler.seedElemHandler
seedElemHandler.defaultMaterial= "matPrb"
elem= seedElemHandler.newElement("ShellMITC4",xc.ID([0,0,0,0]))


points= preprocessor.getMultiBlockTopology.getPoints
pt= points.newPntIDPos3d(1,geom.Pos3d(R,0.0,0.0))
points.newPntFromPos3d(geom.Pos3d((R*cos45),(R*sin45),0.0))
points.newPntFromPos3d(geom.Pos3d(0.0,R,0.0))
points.newPntFromPos3d(geom.Pos3d(R,0.0,1.0))
points.newPntFromPos3d(geom.Pos3d((R*cos45),(R*sin45),1.0))
points.newPntFromPos3d(geom.Pos3d(0.0,R,1.0))

surfaces= preprocessor.getMultiBlockTopology.getSurfaces
s1= surfaces.newQuadSurfaceGridPts([[1,2,3],[4,5,6]])
s1.nDivI= 100
s1.nDivJ= 1

for i in surfaces.getKeys():
  surfaces.get(i).genMesh(xc.meshDir.I)

setTotal= preprocessor.getSets.getSet("total")
elements= setTotal.getElements
area= 0.0
for e in elements:
  area+= e.getArea(True)


ratio1= (area-math.pi)/math.pi

''' 
print("area= ",area)
print("ratio1= ",ratio1)
'''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if(ratio1<=5e-5):
  print("test ",fname,": ok.")
else:
  lmsg.error(fname+' ERROR.')
