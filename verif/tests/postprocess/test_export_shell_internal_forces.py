# -*- coding: utf-8 -*-
# Tomado del example 2-005 del manual de verificación de SAP 2000
# nverborrea= 0

__author__= "Luis C. Pérez Tato (LCPT)"
__copyright__= "Copyright 2014, LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

NumDivI= 8
NumDivJ= 8
CooMaxX= 10
CooMaxY= 2
E= 17472000 # Elastic modulus en lb/in2
nu= 0.3 # Coeficiente de Poison
G= 6720000
thickness= 0.0001 # Cross section depth expressed in inches.
unifLoad= 0.0001 # Carga uniforme en lb/in2.
ptLoad= 100 # Carga puntual en lb.

import xc_base
import geom
import xc
from solution import predefined_solutions
from model import predefined_spaces
from model import fix_node_6dof
from materials import typical_materials
from materials import ShellInternalForces as sif
from materials import CrossSectionInternalForces as csif

# Problem type
prueba= xc.ProblemaEF()
preprocessor=  prueba.getPreprocessor
nodes= preprocessor.getNodeLoader

predefined_spaces.gdls_resist_materiales3D(nodes)
# Define materials
elast= typical_materials.defElasticMaterial(preprocessor, "elast",E)

nodes.newSeedNode()

# Define materials
nmb1= typical_materials.defElasticMembranePlateSection(preprocessor, "memb1",E,nu,0.0,thickness)



seedElemLoader= preprocessor.getElementLoader.seedElemLoader
seedElemLoader.defaultMaterial= "memb1"
seedElemLoader.defaultTag= 1
elem= seedElemLoader.newElement("shell_mitc4",xc.ID([0,0,0,0]))



puntos= preprocessor.getCad.getPoints
pt= puntos.newPntIDPos3d(1,geom.Pos3d(0.0,0.0,0.0))
pt= puntos.newPntIDPos3d(2,geom.Pos3d(CooMaxX,0.0,0.0))
pt= puntos.newPntIDPos3d(3,geom.Pos3d(CooMaxX,CooMaxY,0.0))
pt= puntos.newPntIDPos3d(4,geom.Pos3d(0.0,CooMaxY,0.0))
surfaces= preprocessor.getCad.getSurfaces
surfaces.defaultTag= 1
s= surfaces.newQuadSurfacePts(1,2,3,4)
s.nDivI= NumDivI
s.nDivJ= NumDivJ



f1= preprocessor.getSets.getSet("f1")
f1.genMesh(xc.meshDir.I)
coacciones= preprocessor.getConstraintLoader
lados= s.getEdges
#Edge iterator
for l in lados:
  fix_node_6dof.Nodo6DOFGirosLibresLista(coacciones,l.getEdge.getNodeTags())




# Loads definition
cargas= preprocessor.getLoadLoader
casos= cargas.getLoadPatterns
#Load modulation.
ts= casos.newTimeSeries("constant_ts","ts")
casos.currentTimeSeries= "ts"
#Load case definition
lp0= casos.newLoadPattern("default","0")
#casos.currentLoadPattern= "0"


f1= preprocessor.getSets.getSet("f1")
nNodes= f1.getNumNodes
 
nodo= f1.getNodeIJK(1,NumDivI/2+1,NumDivJ/2+1)
# print "Nodo central: ",nodo.tag
# print "Coordenadas nodo central: ",nodo.getCoo
tagNod= nodo.tag
lp0.newNodalLoad(tagNod,xc.Vector([0,0,-ptLoad,0,0,0])) # Carga concentrada


nElems= f1.getNumElements
#We add the load case to domain.
casos.addToDomain("0")


# Procedimiento de solución
analisis= predefined_solutions.simple_static_linear(prueba)
analOk= analisis.analyze(1)

f1= preprocessor.getSets.getSet("f1")

from postprocess.reports import export_internal_forces
setTotal= preprocessor.getSets["total"]
fName= "/tmp/test_export_shell_internal_forces.txt"
f= open(fName,"w")
export_internal_forces.exportInternalForces("prueba",setTotal.getElements,f)
f.close()


mean= [csif.CrossSectionInternalForces(),csif.CrossSectionInternalForces()]
nCols= len(mean)
nRows= 0
import csv
cr = csv.reader(open(fName,"rb"))
cr.next()
for row in cr:
  nRows+= 1
  sectionIndex= eval(row[2])
  mean[sectionIndex]+= csif.CrossSectionInternalForces(eval(row[3]),eval(row[4]),eval(row[5]),eval(row[6]),eval(row[7]),eval(row[8]))

for m in mean:
  m*= 1.0/nRows

meanRef= [csif.CrossSectionInternalForces(0.0, -1.4141789118e-08, 0.0, 0.0, -0.377847769601, 0.0),csif.CrossSectionInternalForces(0.0, 3.746624204e-08, 0.0, 0.0, -1.6862614343, 0.0)]
#sif.ShellElementInternalForces(0.0,0.0,0.0,-0.755991356310675,-2.4972837939920614,-1.4220260169048315e-13, -3.033006243120112e-08).getWoodArmer()

ratio1= 0.0
for i in range(0,2):
  ratio1+= (meanRef[i]-mean[i]).getModulus()


# print "mean[0]= ", mean[0]
# print "meanRef[0]= ", meanRef[0]
# print "diff[0]= ", mean[0]-meanRef[0]
# print "mean[1]= ", mean[1]
# print "meanRef[1]= ", meanRef[1]
# print "diff[1]= ", mean[1]-meanRef[1]
# print "ratio1= ",ratio1

import os
fname= os.path.basename(__file__)
if (abs(ratio1)<1e-6):
  print "test ",fname,": ok."
else:
  print "test ",fname,": ERROR."
