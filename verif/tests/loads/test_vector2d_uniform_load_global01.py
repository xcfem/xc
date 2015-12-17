# -*- coding: utf-8 -*-
# home made test
# Verificación del funcionamiento del comando vector2d_uniform_load_global.

import xc_base
import geom
import xc
from solution import predefined_solutions
from model import predefined_spaces
from model import fix_node_3dof
from materials import typical_materials
from model import movs_nodo_3gdl
import math

# Geometry
width= .05
depth= .1
nDivIJ= 5
nDivJK= 10
y0= 0
z0= 0
L= 2.0 # Bar length (m)
Iy= width*depth**3/12 # Momento de inercia de la sección expresada en m4
Iz= depth*width**3/12 # Momento de inercia de la sección expresada en m4
E= 210e9 # Módulo de Young del acero.
nu= 0.3 # Coeficiente de Poisson
G= E/(2*(1+nu)) # Módulo de elasticidad a cortante
J= .2e-1 # Momento de inercia a torsión expresado en m4

# Load
f= 1.5e3 # Carga uniforme longitudinal.
p= 1000 # Carga uniforme transversal.

prueba= xc.ProblemaEF()
preprocessor=  prueba.getPreprocessor
nodos= preprocessor.getNodeLoader

# Problem type
predefined_spaces.gdls_resist_materiales2D(nodos)

nodos.defaultTag= 1 #First node number.
nod= nodos.newNodeXY(0,0.0)
nod= nodos.newNodeXY(L*math.sqrt(2)/2,L*math.sqrt(2)/2)

# Definimos transformaciones geométricas
trfs= preprocessor.getTransfCooLoader
lin= trfs.newLinearCrdTransf2d("lin")


# Materials definition
fy= 275e6 # Tensión de cedencia del acero.
acero= typical_materials.defSteel01(preprocessor, "acero",E,fy,0.001)
respT= typical_materials.defElasticMaterial(preprocessor, "respT",G*J) # Respuesta de la sección a torsión.
respVy= typical_materials.defElasticMaterial(preprocessor, "respVy",1e9) # Respuesta de la sección a cortante según y.
respVz= typical_materials.defElasticMaterial(preprocessor, "respVz",1e9) # Respuesta de la sección a cortante según z.
# Secciones
import os
pth= os.path.dirname(__file__)
#print "pth= ", pth
if(not pth):
  pth= "."
execfile(pth+"/geomCuadFibrasTN.py")

materiales= preprocessor.getMaterialLoader
cuadFibrasTN= materiales.newMaterial("fiber_section_3d","cuadFibrasTN")
fiberSectionRepr= cuadFibrasTN.getFiberSectionRepr()
fiberSectionRepr.setGeomNamed("geomCuadFibrasTN")
cuadFibrasTN.setupFibers()
A= cuadFibrasTN.getFibers().getSumaAreas(1.0)

agg= materiales.newMaterial("section_aggregator","cuadFibras")
agg.setSection("cuadFibrasTN")
agg.setAdditions(["T","Vy","Vz"],["respT","respVy","respVz"])
 # Respuestas a torsión y cortantes.



# Elements definition
elementos= preprocessor.getElementLoader
elementos.defaultTransformation= "lin" # Transformación de coordenadas para los nuevos elementos
elementos.defaultMaterial= "cuadFibras"
elementos.numSections= 3 # Número de secciones a lo largo del elemento.
elementos.defaultTag= 1
el= elementos.newElement("force_beam_column_2d",xc.ID([1,2]))



# Constraints
coacciones= preprocessor.getConstraintLoader
fix_node_3dof.fixNode000(coacciones,1)

# Loads definition
cargas= preprocessor.getLoadLoader
casos= cargas.getLoadPatterns
#Load modulation.
ts= casos.newTimeSeries("constant_ts","ts")
casos.currentTimeSeries= "ts"
#Load case definition
lp0= casos.newLoadPattern("default","0")
casos.currentLoadPattern= "0"
crdTransf= el.getCoordTransf
vIElem= crdTransf.getIVector
vJElem= crdTransf.getJVector
vCarga= f*vIElem-p*vJElem
el.vector2dUniformLoadGlobal(vCarga)


cargas= preprocessor.getLoadLoader
cargas.addToDomain("0") # Añadimos la hipótesis al dominio

# Procedimiento de solución
analisis= predefined_solutions.simple_newton_raphson(prueba)
result= analisis.analyze(1)

nodos.calculateNodalReactions(True) 
nod2= nodos.getNode(2)
vDisp= xc.Vector([nod2.getDisp[0],nod2.getDisp[1]])
nod1= nodos.getNode(1)
vReac1= xc.Vector([nod1.getReaction[0],nod1.getReaction[1]])
nod2= nodos.getNode(2)
vReac2= xc.Vector([nod2.getReaction[0],nod2.getReaction[1]])

elementos= preprocessor.getElementLoader

elem1= elementos.getElement(1)
elem1.getResistingForce()
scc= elem1.getSections()[0]
N0= scc.getStressResultantComponent("N")

F= (f*L)
delta0= vDisp.dot(vIElem)
delta0teor= (f*L**2/(2*E*A))
ratio0= (abs((delta0-delta0teor)/delta0teor))
Q= p*L
delta1= vDisp.dot(vJElem)
delta1Teor= (-Q*L**3/8/E/Iz)
ratio1= ((delta1-delta1Teor)/delta1Teor)
ratio2= (abs((N0-F)/F))
RN= vReac1.dot(vIElem)
ratio3= (abs((RN+F)/F))
RN2= vReac2.dot(vIElem)

''' 
print "analOk: ",analOk
print "delta0: ",delta0
print "delta0Teor: ",delta0teor
print "ratio0= ",ratio0
print "delta1= ",delta1
print "delta1Teor= ",delta1Teor
print "ratio1= ",ratio1
print "N0= ",N0
print "ratio2= ",ratio2
print "vReac1= ",vReac1
print "RN= ",RN
print "ratio3= ",ratio3
print "vReac2= ",vReac2
print "RN2= ",RN2
   '''
import os
fname= os.path.basename(__file__)
if (abs(ratio0)<1e-6) & (abs(ratio1)<0.05) & (abs(ratio2)<1e-10) & (abs(ratio3)<1e-10):
  print "test ",fname,": ok."
else:
  print "test ",fname,": ERROR."
