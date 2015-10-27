# -*- coding: utf-8 -*-
import xc_base
import geom
import xc
import math
import os
from model import predefined_spaces
from materials import typical_materials

R= 2.0
cos45= math.cos(math.radians(45))
sin45= cos45
E= 30e6 # Young modulus (psi)
nu= 0.3 # Coeficiente de Poison
rho= 0.0 # Densidad

nNodos= 0

prueba= xc.ProblemaEF()
preprocessor=  prueba.getPreprocessor
elast= typical_materials.defElasticMaterial(preprocessor, "elast",3000)
prueba= typical_materials.defElasticMembranePlateSection(preprocessor, "prueba",E,nu,rho,0.25)

nodos= preprocessor.getNodeLoader
nodos.newSeedNode()

seedElemLoader= preprocessor.getElementLoader.seedElemLoader
seedElemLoader.defaultMaterial= "prueba"
seedElemLoader.defaultTag= 1
elem= seedElemLoader.newElement("shell_mitc4",xc.ID([0,0,0,0]))

puntos= preprocessor.getCad.getPoints
pt= puntos.newPntIDPos3d(1,geom.Pos3d(R,0.0,0.0))
puntos.newPntFromPos3d(geom.Pos3d((R*cos45),(R*sin45),0.0))
puntos.newPntFromPos3d(geom.Pos3d(0.0,R,0.0))
puntos.newPntFromPos3d(geom.Pos3d(R,0.0,0.5))
puntos.newPntFromPos3d(geom.Pos3d((R*cos45),(R*sin45),0.5))
puntos.newPntFromPos3d(geom.Pos3d(0.0,R,0.5))
puntos.newPntFromPos3d(geom.Pos3d(R,0.0,1.0))
puntos.newPntFromPos3d(geom.Pos3d((R*cos45),(R*sin45),1.0))
puntos.newPntFromPos3d(geom.Pos3d(0.0,R,1.0))

surfaces= preprocessor.getCad.getSurfaces
surfaces.defaultTag= 1
s1= surfaces.newQuadSurfaceGridPts([[1,2,3],[4,5,6]])
s1.nDivI= 5
s1.nDivJ= 1

s2= surfaces.newQuadSurfaceGridPts([[4,5,6],[7,8,9]])
s2.nDivI= 5
s2.nDivJ= 1


Z1= preprocessor.getSets.defSet("Z1")
Z1.getSurfaces.append(s1)
Z1.fillDownwards()
Z2= preprocessor.getSets.defSet("Z2")
Z2.getSurfaces.append(s2)
Z2.fillDownwards()

Z1.genMesh(xc.meshDir.I)
Z2.genMesh(xc.meshDir.I)


nNodos= preprocessor.getSets.getSet("total").getNodes.size

ratio1= (nNodos-18)/18

''' 
print "nNod= ",nNodos
print "ratio1= ",ratio1
   '''

import os
fname= os.path.basename(__file__)
if(ratio1<=5e-5):
  print "test ",fname,": ok."
else:
  print "test ",fname,": ERROR."
