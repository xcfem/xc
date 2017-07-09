# -*- coding: utf-8 -*-

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
nu= 0.3 # Coeficiente de Poison
rho= 0.0 # Densidad

area= 0

prueba= xc.ProblemaEF()
preprocessor=  prueba.getPreprocessor

elast= typical_materials.defElasticMaterial(preprocessor, "elast",3000)
matPrb= typical_materials.defElasticMembranePlateSection(preprocessor, "matPrb",E,nu,rho,0.25)

nodes= preprocessor.getNodeLoader
nodes.newSeedNode()

seedElemLoader= preprocessor.getElementLoader.seedElemLoader
seedElemLoader.defaultMaterial= "matPrb"
seedElemLoader.defaultTag= 1
elem= seedElemLoader.newElement("shell_mitc4",xc.ID([0,0,0,0]))


puntos= preprocessor.getCad.getPoints
pt= puntos.newPntIDPos3d(1,geom.Pos3d(R,0.0,0.0))
puntos.newPntFromPos3d(geom.Pos3d((R*cos45),(R*sin45),0.0))
puntos.newPntFromPos3d(geom.Pos3d(0.0,R,0.0))
puntos.newPntFromPos3d(geom.Pos3d(R,0.0,1.0))
puntos.newPntFromPos3d(geom.Pos3d((R*cos45),(R*sin45),1.0))
puntos.newPntFromPos3d(geom.Pos3d(0.0,R,1.0))

surfaces= preprocessor.getCad.getSurfaces
surfaces.defaultTag= 1
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
print "area= ",area
print "ratio1= ",ratio1
'''

import os
from miscUtils import LogMessages as lmsg
fname= os.path.basename(__file__)
if(ratio1<=5e-5):
  print "test ",fname,": ok."
else:
  lmsg.error(fname+' ERROR.')
