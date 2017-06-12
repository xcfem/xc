# -*- coding: utf-8 -*-
from __future__ import division
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

CooMaxX= 3
CooMaxY= 1
E= 30e6 # Young modulus (psi)
nu= 0.3 # Coeficiente de Poison
rho= 0.0 # Densidad
#prueba.logFileName= "/tmp/borrar.log" # Ignore warning messages

numNodes= 0
numElem= 0

# Problem type
prueba= xc.ProblemaEF()
preprocessor=  prueba.getPreprocessor
nodes= preprocessor.getNodeLoader

modelSpace= predefined_spaces.SolidMechanics2D(nodes)
nodes.newSeedNode()

elast= typical_materials.defElasticMaterial(preprocessor, "elast",3000)
elast2d= typical_materials.defElasticIsotropicPlaneStress(preprocessor, "elast2d",E,nu,rho)


seedElemLoader= preprocessor.getElementLoader.seedElemLoader
seedElemLoader.defaultMaterial= "elast2d"
elem= seedElemLoader.newElement("quad4n",xc.ID([0,0,0,0]))


puntos= preprocessor.getCad.getPoints
pt= puntos.newPntIDPos3d(1,geom.Pos3d(0.0,0.0,0.0))
pt= puntos.newPntIDPos3d(2,geom.Pos3d(CooMaxX/3.0,0.0,0.0))
pt= puntos.newPntIDPos3d(3,geom.Pos3d(CooMaxX*2/3.0,0.0,0.0))
pt= puntos.newPntIDPos3d(4,geom.Pos3d(CooMaxX,0.0,0.0))
pt= puntos.newPntIDPos3d(5,geom.Pos3d(0.0,CooMaxY,0.0))
pt= puntos.newPntIDPos3d(6,geom.Pos3d(CooMaxX/3.0,CooMaxY,0.0))
pt= puntos.newPntIDPos3d(7,geom.Pos3d(CooMaxX*2/3.0,CooMaxY,0.0))
pt= puntos.newPntIDPos3d(8,geom.Pos3d(CooMaxX,CooMaxY,0.0))

surfaces= preprocessor.getCad.getSurfaces
surfaces.defaultTag= 1
s1= surfaces.newQuadSurfacePts(1,2,6,5)
s1.nDivI= 1
s1.nDivJ= 1

# print "s1 nDivI= ", s1.nDivI
# print "s1 nDivJ= ", s1.nDivJ

s2= surfaces.newQuadSurfacePts(2,3,7,6)
s2.nDivI= 2
s2.nDivJ= 1

divsOk= surfaces.conciliaNDivs()

# print "\ns1 nDivI= ", s1.nDivI
# print "s1 nDivJ= ", s1.nDivJ
# print "s2 nDivI= ", s2.nDivI
# print "s2 nDivJ= ", s2.nDivJ

s3= surfaces.newQuadSurfacePts(3,4,8,7)
s3.nDivI= 5
s3.nDivJ= 5

divsOk= divsOk & surfaces.conciliaNDivs()


# print "\ns1 nDivI= ", s1.nDivI
# print "s1 nDivJ= ", s1.nDivJ
# print "s2 nDivI= ", s2.nDivI
# print "s2 nDivJ= ", s2.nDivJ
# print "s3 nDivI= ", s3.nDivI
# print "s3 nDivJ= ", s3.nDivJ

s2.nverborrea= 5
total= preprocessor.getSets.getSet("total")
total.genMesh(xc.meshDir.I)

numNodes= total.getNodes.size
numElem= total.getElements.size



'''
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

\preprocessor.{\for_each_edge
    print "linea: ",nombre," p1: ",getTagP1," p2: ",getTagP2, " ndiv: ",ndiv
  }}}
print "Número de nodos: ",numNodes
print "Número de elementos: ",numElem
'''

ratio1= abs(numNodes-54)
ratio2= abs(numElem-40)

import os
from miscUtils import LogMessages as lmsg
fname= os.path.basename(__file__)
if (ratio1<=1e-10) & (ratio2<=1e-10) &divsOk:
  print "test ",fname,": ok."
else:
  lmsg.error(fname+' ERROR.')
