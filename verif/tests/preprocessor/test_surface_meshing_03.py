# -*- coding: utf-8 -*-
import xc_base
import geom
import xc
import math
import os
import sys
from model import predefined_spaces
from materials import typical_materials

__author__= "Luis C. Pérez Tato (LCPT)"
__copyright__= "Copyright 2014, LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

E= 30e6 # Young modulus (psi)
nu= 0.3 # Coeficiente de Poison
rho= 0.0 # Densidad
# nverborrea= 0

numNodes= 0
numElem= 0

# Problem type
prueba= xc.ProblemaEF()
preprocessor=  prueba.getPreprocessor
nodes= preprocessor.getNodeLoader

modelSpace= predefined_spaces.StructuralMechanics3D(nodes)
# Materials definition
elast= typical_materials.defElasticMaterial(preprocessor, "elast",3000)

nodes.newSeedNode()
# Materials definition
matPrb= typical_materials.defElasticMembranePlateSection(preprocessor, "matPrb",E,nu,rho,0.25)

seedElemLoader= preprocessor.getElementLoader.seedElemLoader
seedElemLoader.defaultMaterial= "matPrb"
seedElemLoader.defaultTag= 1
elem= seedElemLoader.newElement("shell_mitc4",xc.ID([0,0,0,0]))



puntos= preprocessor.getCad.getPoints
pt= puntos.newPntIDPos3d(1,geom.Pos3d(1,0,0))
pt= puntos.newPntIDPos3d(2,geom.Pos3d(1,1,0))
pt= puntos.newPntIDPos3d(3,geom.Pos3d(1,1,1))
pt= puntos.newPntIDPos3d(4,geom.Pos3d(1,0,1))
pt= puntos.newPntIDPos3d(5,geom.Pos3d(0,1,0))
pt= puntos.newPntIDPos3d(6,geom.Pos3d(0,1,1))
pt= puntos.newPntIDPos3d(7,geom.Pos3d(0,0,1))
pt= puntos.newPntIDPos3d(8,geom.Pos3d(0,0,0))

surfaces= preprocessor.getCad.getSurfaces
surfaces.defaultTag= 1
s= surfaces.newQuadSurfacePts(1,2,3,4)
s.nDivI= 1
s.nDivJ= 1

s= surfaces.newQuadSurfacePts(3,6,7,4)
s.nDivI= 1
s.nDivJ= 4

s= surfaces.newQuadSurfacePts(5,6,7,8)
s.nDivI= 2
s.nDivJ= 3

s= surfaces.newQuadSurfacePts(5,8,1,2)
s.nDivI= 4
s.nDivJ= 4

divsOk= surfaces.conciliaNDivs()


setTotal= preprocessor.getSets.getSet("total")
setTotal.genMesh(xc.meshDir.I)


numNodes= setTotal.getNodes.size
numElem= setTotal.getElements.size



areaTotal= 0.0
mesh= prueba.getDomain.getMesh
eIter= mesh.getElementIter
elem= eIter.next()
while not(elem is None):
  areaTotal+= elem.getArea(True)
  elem= eIter.next()

''' 
\path{"/usr/local/lib/macros_lcmd"
execfile("vtk/vtk_crea_array_set_data.xcm")
execfile("vtk/displayVtk.lcmm")
execfile("vtk/malla_cad/vtk_define_malla_cad.xcm")
execfile("vtk/malla_ef/vtk_define_malla_elementos.xcm")
execfile("vtk/vtk_cargas.xcm")
execfile("vtk/utilsVtk.lcmm")
\VtkLoadElemMesh("uGridElem","total")

\vtk

    \define["vtkRenderer","renderer"]

        \set_background{.95, .95, .95}

    \VtkDefineActorNodo("uGridElem","renderer",0.02)
    \VtkDefineElementsActor("uGridElem","renderer","surface")


    \renderer
      { \reset_camera{} }

\VtkDibujaIdsNodos("uGridElem","total","renderer")
execfile("vtk/vtk_vista_yneg.lcmm")
execfile("vtk/vtk_muestra_ventana.lcmm")
\VtkMuestraVentana("renderer",800,600)

\preprocessor.

    \for_each_edge
      {print "linea: ",nombre," p1: ",getTagP1," p2: ",getTagP2, " ndiv: ",ndiv}
    \for_each_face
      {print "cara: ",nombre," ndivI: ",ndivI," ndivJ: ",ndivJ}
  }}

print "Area total: ",areaTotal
print "Número de nodos: ",numNodes
print "Número de elementos: ",numElem
'''


ratio1= abs(numNodes-40)
ratio2= abs(numElem-32)
ratio3= abs(areaTotal-4)

import os
from miscUtils import LogMessages as lmsg
fname= os.path.basename(__file__)
if((ratio1<=1e-10) & (ratio2<=1e-10) & (ratio3<=1e-15)):
  print "test ",fname,": ok."
else:
  lmsg.error(fname+' ERROR.')
