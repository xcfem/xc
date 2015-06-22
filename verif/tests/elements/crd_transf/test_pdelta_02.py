# -*- coding: utf-8 -*-
from __future__ import division
# Análisis del efecto P-Delta en un pórtico.
# Tomado del ejemplo «PDelta-3» del manual de verificación de Ansys.

# Este ejemplo es completamente análogo a "test_pdelta_01.xc" salvo
# que en lugar de emplear análisis no lineal usa dos análisis lineales
# consecutivos: es decir un análisis de segundo orden.


B= 100 # Ancho del pórtico expresado en pulgadas
H= 100 # Altura del pórtico expresado en pulgadas
# Propiedades de la sección (IPE-80)
Es= 29e6 # Young modulus (psi)
area= 1 # Área de la sección expresada en in2
Iz= 1/12 # Momento de inercia de la sección expresada en in4
P= 1000 # Carga en libras-fuerza.
nDivLineas= 8 # Carga en libras-fuerza.
offset= 2*B # Carga en libras-fuerza.

import xc_base
import geom
import xc
from solution import predefined_solutions
from model import predefined_spaces
from model import fix_node_3dof
from materials import typical_materials

prueba= xc.ProblemaEF()
mdlr= prueba.getModelador
nodos= mdlr.getNodeLoader
predefined_spaces.gdls_resist_materiales2D(nodos)
nodos.newSeedNode()

trfs= mdlr.getTransfCooLoader
pd= trfs.newPDeltaCrdTransf2d("pd")

# Materials definition
scc= typical_materials.defElasticSection2d(mdlr,"scc",area,Es,Iz)

seedElemLoader= mdlr.getElementLoader.seedElemLoader
seedElemLoader.defaultMaterial= "scc"
seedElemLoader.defaultTransformation= "pd"
seedElemLoader.defaultTag= 1 #Tag for next element.
beam2d= seedElemLoader.newElement("elastic_beam_2d",xc.ID([1,2]))

puntos= mdlr.getCad.getPoints
pt1= puntos.newPntIDPos3d(1, geom.Pos3d(0.0,0.0,0))
pt2= puntos.newPntIDPos3d(2, geom.Pos3d(0.0,H,0))
pt3= puntos.newPntIDPos3d(3, geom.Pos3d(B,0.0,0))
pt4= puntos.newPntIDPos3d(4, geom.Pos3d(B,H,0))
pt5= puntos.newPntIDPos3d(5, geom.Pos3d(offset,0.0,0))
pt6= puntos.newPntIDPos3d(6, geom.Pos3d(offset,H,0))
pt7= puntos.newPntIDPos3d(7, geom.Pos3d(offset+B,0.0,0))
pt8= puntos.newPntIDPos3d(8, geom.Pos3d(offset+B,H,0))

lineas= mdlr.getCad.getLines
l= lineas.newLine(1,2)
l.nDiv= nDivLineas
l= lineas.newLine(2,4)
l.nDiv= nDivLineas*2
l= lineas.newLine(4,3)
l.nDiv= nDivLineas
l= lineas.newLine(5,6)
l.nDiv= nDivLineas
l= lineas.newLine(6,8)
l.nDiv= nDivLineas*2
l= lineas.newLine(8,7)
l.nDiv= nDivLineas

setTotal= mdlr.getSets.getSet("total")
setTotal.genMesh(xc.meshDir.I)

pt1.getNode().fix(xc.ID([0,1,2]),xc.Vector([0.0,0.0,0.0]))
pt3.getNode().fix(xc.ID([0,1,2]),xc.Vector([0.0,0.0,0.0]))
pt5.getNode().fix(xc.ID([0,1,2]),xc.Vector([0.0,0.0,0.0]))
pt7.getNode().fix(xc.ID([0,1,2]),xc.Vector([0.0,0.0,0.0]))

tagNodo2= 0.0
tagNodo6= 0.0
tagNodo8= 0.0
tagNodoCargaCentral= 0.0
tagNodoCargaLateral= 0.0
tagElem1= 0.0
tagElem2= 0.0
tagElem4= 0.0
tagElem5D= 0.0
tagElem5F= 0.0
tagElem6= 0.0


tagNodo2= pt2.getTagNode
tagNodo6= pt6.getTagNode
tagNodo8= pt8.getTagNode

mesh= prueba.getDomain.getMesh
tagNodoCargaCentral= mesh.getNearestNode(geom.Pos3d(B/2,H,0.0)).tag
tagNodoCargaLateral= mesh.getNearestNode(geom.Pos3d(offset+B/4,H,0.0)).tag
tagElem1= mesh.getNearestElement(geom.Pos3d(0,H/(10*nDivLineas),0.0)).tag
tagElem2= mesh.getNearestElement(geom.Pos3d(B/(10*nDivLineas),H,0.0)).tag
tagElem4= mesh.getNearestElement(geom.Pos3d(offset,H/(10*nDivLineas),0.0)).tag
tagElem5D= mesh.getNearestElement(geom.Pos3d(offset+B/(10*nDivLineas),H,0.0)).tag
tagElem5F= mesh.getNearestElement(geom.Pos3d(offset+B-B/(10*nDivLineas),H,0.0)).tag
tagElem6= mesh.getNearestElement(geom.Pos3d(offset+B,H/(10*nDivLineas),0.0)).tag


# Loads definition
cargas= mdlr.getLoadLoader
casos= cargas.getLoadPatterns
#Load modulation.
ts= casos.newTimeSeries("constant_ts","ts")
casos.currentTimeSeries= "ts"
#Load case definition
lp0= casos.newLoadPattern("default","0")
#casos.currentLoadPattern= "0"
lp0.newNodalLoad(tagNodoCargaCentral,xc.Vector([0,-P,0]))
lp0.newNodalLoad(tagNodoCargaLateral,xc.Vector([0,-P,0]))
#We add the load case to domain.
casos.addToDomain("0")

# Procedimiento de solución

analisis= predefined_solutions.simple_static_linear(prueba)
result= analisis.analyze(1)
result= analisis.analyze(1)

theta2= 0.0 # Giro del nodo 2
theta2Teor= -0.09192 # Valor teórico del giro del nodo 2
ratioTheta2= 0.0
M12= 0.0 # Flector en arranque de elemento 1
M12Teor= -4606.6 # Valor teórico del flector en arranque de elemento 1
ratioM12= 0.0
M21= 0.0 # Momento dorsal en elemento 2
M21Teor= 8254.0 # Valor teórico del momento dorsal en elemento 2
ratioM21= 0.0
Q= 0.0 # Cortante en elemento 1
QTeor= -128.6 # Valor teórico del cortante en elemento 1
ratioQ= 0.0
delta6= 0.0 # Desplazamiento horizontal del nodo 6
delta6Teor= 1.893 # Valor teórico del desplazamiento horizontal del nodo 6
ratioDelta6= 0.0
theta6= 0.0 # Giro del nodo 6
theta6Teor= -0.1013 # Valor teórico del giro del nodo 6
ratioTheta6= 0.0
theta8= 0.0 # Giro del nodo 6
theta8Teor= 0.0367 # Valor teórico del giro del nodo 6
ratioTheta8= 0.0
M61= 0.0 # Flector en arranque de elemento 4
M61Teor= -2544.9 # Valor teórico del flector en arranque de elemento 4
ratioM61= 0.0
Q4= 0.0 # Cortante en elemento 1
Q4Teor= -101.4 # Valor teórico del cortante en elemento 1
ratioQ4= 0.0
M51= 0.0 # Momento dorsal en elemento 5
M51Teor= 6088.6 # Valor teórico del momento dorsal en elemento 5
ratioM51= 0.0
M71= 0.0 # Flector en arranque de elemento 6
M71Teor= 4456.9 # Valor teórico del flector en arranque de elemento 6
ratioM71= 0.0
M52= 0.0 # Momento dorsal en elemento 5
M52Teor= -6153.0 # Valor teórico del momento frontal en elemento 5
ratioM52= 0.0


nodos= mdlr.getNodeLoader
nodT2= nodos.getNode(tagNodo2)
theta2= nodT2.getDisp[2]
nodT6= nodos.getNode(tagNodo6)
delta6= nodT6.getDisp[0]
theta6= nodT6.getDisp[2]
nodT8= nodos.getNode(tagNodo8)
theta8= nodT8.getDisp[2]

elementos= mdlr.getElementLoader
eletagElem1= elementos.getElement(tagElem1)
eletagElem1.getResistingForce()
Q= eletagElem1.getV1
M12= eletagElem1.getM1

eletagElem2= elementos.getElement(tagElem2)
eletagElem2.getResistingForce()
M21= eletagElem2.getM1

eletagElem4= elementos.getElement(tagElem4)
eletagElem4.getResistingForce()
Q4= eletagElem4.getV1
M61= eletagElem4.getM1

eletagElem5D= elementos.getElement(tagElem5D)
eletagElem5D.getResistingForce()
M51= eletagElem5D.getM1

eletagElem5F= elementos.getElement(tagElem5F)
eletagElem5F.getResistingForce()
M52= eletagElem5F.getM2

eletagElem6= elementos.getElement(tagElem6)
eletagElem6.getResistingForce()
M71= eletagElem6.getM2

ratioTheta2= abs((theta2-theta2Teor)/theta2Teor)
ratioDelta6= abs((delta6-delta6Teor)/delta6Teor)
ratioM12= abs((M12-M12Teor)/M12Teor)
ratioM21= abs((M21-M21Teor)/M21Teor)
ratioQ= abs((Q-QTeor)/QTeor)
ratioTheta6= abs((theta6-theta6Teor)/theta6Teor)
ratioTheta8= abs((theta8-theta8Teor)/theta8Teor)
ratioM61= abs((M61-M61Teor)/M61Teor)
ratioQ4= abs((Q4-Q4Teor)/Q4Teor)
ratioM51= abs((M51-M51Teor)/M51Teor)
ratioM71= abs((M71-M71Teor)/M71Teor)
ratioM52= abs((M52-M52Teor)/M52Teor)

''' 
print "theta2= ",theta2
print "theta2Teor= ",theta2Teor
print "ratioTheta2= ",ratioTheta2
print "M12= ",M12
print "M12Teor= ",M12Teor
print "ratioM12= ",ratioM12
print "M21= ",M21
print "M21Teor= ",M21Teor
print "ratioM21= ",ratioM21
print "Q= ",Q
print "QTeor= ",QTeor
print "ratioQ= ",ratioQ
print "delta6= ",delta6
print "delta6Teor= ",delta6Teor
print "ratioDelta6= ",ratioDelta6
print "theta6= ",theta6
print "theta6Teor= ",theta6Teor
print "ratioTheta6= ",ratioTheta6
print "theta8= ",theta8
print "theta8Teor= ",theta8Teor
print "ratioTheta8= ",ratioTheta8
print "M61= ",M61
print "M61Teor= ",M61Teor
print "ratioM61= ",ratioM61
print "Q4= ",Q4
print "Q4Teor= ",Q4Teor
print "ratioQ4= ",ratioQ4
print "M51= ",M51
print "M51Teor= ",M51Teor
print "ratioM51= ",ratioM51
print "M71= ",M71
print "M71Teor= ",M71Teor
print "ratioM71= ",ratioM71
print "M52= ",M52
print "M52Teor= ",M52Teor
print "ratioM52= ",ratioM52
 '''

import os
fname= os.path.basename(__file__)
if (ratioTheta2<1e-2) & (ratioDelta6<1e-2) & (ratioM12<0.05) & (ratioM21<2e-2) & (ratioQ<0.06) & (ratioTheta6<1e-2) & (ratioTheta8<1e-2) & (ratioM61<0.05) & (ratioQ4<0.06) & (ratioM51<0.05) & (ratioM71<0.05) & (ratioM52<0.05):
  print "test ",fname,": ok."
else:
  print "test ",fname,": ERROR."
