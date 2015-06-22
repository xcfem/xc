# -*- coding: utf-8 -*-
from __future__ import division
import xc_base
import geom
import xc
import math
import os
from model import predefined_spaces
from materials import typical_materials

# home made test

NumDivI= 3
NumDivJ= 2
CooMax= 10
E= 30e6 # Young modulus (psi)
nu= 0.3 # Coeficiente de Poison
rho= 0.0 # Densidad

# Problem type
prueba= xc.ProblemaEF()
prueba.logFileName= "/tmp/borrar.log" # Para no imprimir mensajes de advertencia.
mdlr= prueba.getModelador
nodos= mdlr.getNodeLoader

predefined_spaces.gdls_elasticidad2D(nodos)# Materials definition

elast= typical_materials.defElasticMaterial(mdlr,"elast",3000)
elast2d= typical_materials.defElasticIsotropicPlaneStress(mdlr,"elast2d",E,nu,rho)

nodos.newSeedNode()

seedElemLoader= mdlr.getElementLoader.seedElemLoader
seedElemLoader.defaultMaterial= "elast2d"
elem= seedElemLoader.newElement("quad4n",xc.ID([0,0,0,0]))


puntos= mdlr.getCad.getPoints
pt= puntos.newPntIDPos3d(1,geom.Pos3d(0.0,0.0,0.0))
pt= puntos.newPntIDPos3d(2,geom.Pos3d(CooMax,0.0,0.0))
pt= puntos.newPntIDPos3d(3,geom.Pos3d(CooMax,CooMax,0.0))
pt= puntos.newPntIDPos3d(4,geom.Pos3d(0.0,CooMax,0.0))

#             l2
#  4 +--------------------+ 3
#    |                    |
# l4 |                    | l1
#    |        l3          |
#  1 +--------------------+ 2
#

lineas= mdlr.getCad.getLines
lineas.defaultTag= 3
l3= lineas.newLine(1,2)
lineas.defaultTag= 2
l2= lineas.newLine(4,3)
lineas.defaultTag= 1
l1= lineas.newLine(2,3)
lineas.defaultTag= 4
l1= lineas.newLine(4,1)

surfaces= mdlr.getCad.getSurfaces
surfaces.defaultTag= 1
s= surfaces.newQuadSurfaceLines(1,2,3,4)
s.nDivI= NumDivI
s.nDivJ= NumDivJ


''' 
            for_each_lado

                 print "linea: ", edge.nombre," directo: ",directo
                 edge{print "    p1: ",p1.nombre," p2: ",p2.nombre }

'''



f1= mdlr.getSets.getSet("f1")
f1.genMesh(xc.meshDir.I)




nnodCuadr= f1.getNumNodes
# print "nnod= ",getNumNodes
nelemCuadr= f1.getNumElements
# print "nelem= ",getNumElements



mesh= prueba.getDomain.getMesh
nnodDom= mesh.getNumNodes()

''' 
print "núm. nodos: ",nnod
nodos= mdlr.getNodeLoader

for_each
  print "  nodo: ",tag," x= ",coord[0],", y= ",coord[1]

'''
nelemDom= mesh.getNumElements()
#print "núm. elementos: ",nelem
perim= 0.0

eIter= mesh.getElementIter
elem= eIter.next()
while not(elem is None):
#  print elem.tag," nod. I: ",elem.getNodes[0].tag," nod. J: ",elem.getNodes[1].tag," nod. K: ",elem.getNodes[2].tag," nod. L: ",elem.getNodes[3].tag," L= ",elem.getPerimetro()
  perim+= elem.getPerimeter(True)
  elem= eIter.next()

pMed= perim/nelemCuadr
pMedTeor= 2*CooMax/NumDivI+2*CooMax/NumDivJ




ratio1= (abs(nnodCuadr-(NumDivI+1)*(NumDivJ+1)))
ratio2= (abs(nelemCuadr-(NumDivI*NumDivJ)))
ratio3= (abs(nnodDom-(NumDivI+1)*(NumDivJ+1)))
ratio4= (abs(nelemDom-(NumDivI*NumDivJ)))
ratio5= (abs(pMed-pMedTeor))

''' 
print "perim medio: ",pMed
print "perim medio teórico: ",pMedTeor
print "ratio1= ",ratio1
print "ratio2= ",ratio2
print "ratio3= ",ratio3
print "ratio4= ",ratio4
print "ratio5= ",ratio5
 '''
import os
fname= os.path.basename(__file__)
if (ratio1<=1e-10) & (ratio2<=1e-10) & (ratio3<=1e-10) & (ratio4<=1e-10) & (ratio5<=1e-10):
  print "test ",fname,": ok."
else:
  print "test ",fname,": ERROR."
