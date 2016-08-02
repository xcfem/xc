# -*- coding: utf-8 -*-
# Tomado del example 2-005 del manual de verificación de SAP 2000
# nverborrea= 0
NumDivI= 8
NumDivJ= 8
CooMaxX= 10
CooMaxY= 2
E= 17472000 # Elastic modulus en lb/in2
nu= 0.3 # Coeficiente de Poison
G= 6720000
thickness= 0.0001 # Cross section heightexpresado en pulgadas.
unifLoad= 0.0001 # Carga uniforme en lb/in2.
ptLoad= 100 # Carga puntual en lb.

import xc_base
import geom
import xc
from solution import predefined_solutions
from model import predefined_spaces
from model import fix_node_6dof
from materials import typical_materials

# Problem type
prueba= xc.ProblemaEF()
preprocessor=  prueba.getPreprocessor
nodos= preprocessor.getNodeLoader

predefined_spaces.gdls_resist_materiales3D(nodos)
# Definimos materiales
elast= typical_materials.defElasticMaterial(preprocessor, "elast",E)

nodos.newSeedNode()

# Definimos materiales
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
nNodos= f1.getNumNodes
 
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
export_internal_forces.exportaEsfuerzosShellSet(preprocessor, "prueba",setTotal,f)
f.close()


mean=[0,0,0,0,0,0,0,0,0]
nCols= len(mean)
nRows= 0
import csv
cr = csv.reader(open(fName,"rb"))
for row in cr:
  nRows+= 1
  for i in range(1,nCols):
    mean[i]+= eval(row[i])

for i in range(0,nCols):
  mean[i]/= nRows

meanRef= [0, 32, 0.0, 0.0, 0.0, -0.755991356310675, -2.4972837939920614, -1.4220260169048315e-13, -3.033006243120112e-08]

ratio1= 0.0
for i in range(0,nCols):
  ratio1+= (meanRef[i]-mean[i])**2


#print mean
#print ratio1

import os
fname= os.path.basename(__file__)
if (abs(ratio1)<1e-6):
  print "test ",fname,": ok."
else:
  print "test ",fname,": ERROR."
