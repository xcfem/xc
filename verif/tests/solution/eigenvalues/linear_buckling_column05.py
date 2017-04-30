# -*- coding: utf-8 -*-
''' Tomado de la figura 6.22 de "Finite Element Procedures"
de "Klaus Jurgen Bathe". '''
''' NO DA MUY BUENOS RESULTADOS

   Probablemente el problema esté relacionado con la incapacidad de los elementos
   de simular el efecto que su flexión produce en la orientación de los esfuerzos
   que trasmite a sus nodos (ver ejemplo test_pdelta_01.xc).
 '''
import xc_base
import geom
import xc

from model import fix_node_3dof
from model import predefined_spaces
from solution import predefined_solutions
from materials import typical_materials
import math

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AOO)"
__copyright__= "Copyright 2015, LCPT and AOO"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

L= 10 # Longitud de la columna en meters
b= 0.2 # Cross section width en meters
h= 0.2 # Cross section depth en meters
A= b*h # Cross section area en m2
Iz= 1/12.0*b*h**3 # Momento de inercia en m4
Iy= Iz # Momento de inercia en m4
E=1e4/Iz # Elastic modulus en N/m2
nu= 0.3 # Poisson's ratio
G= E/(2*(1+nu)) # Shear modulus
J= 0.001 # Cross section torsion constant (m4) (ES IRRELEVANTE)
P= -100 # Carga vertical sobre la columna.

NumDiv= 4

# Problem type
prueba= xc.ProblemaEF()
preprocessor=  prueba.getPreprocessor
nodes= preprocessor.getNodeLoader
predefined_spaces.gdls_resist_materiales3D(nodes)

# Materials definition
scc= typical_materials.defElasticShearSection3d(preprocessor, "scc",A,E,G,Iz,Iy,J,1)

nodes.newSeedNode()
# Geometric transformation(s)
trfs= preprocessor.getTransfCooLoader
lin= trfs.newPDeltaCrdTransf3d("lin")
lin.xzVector= xc.Vector([0,1,0])

# Definimos elemento semilla
seedElemLoader= preprocessor.getElementLoader.seedElemLoader
seedElemLoader.defaultMaterial= "scc"
seedElemLoader.defaultTransformation= "lin"
seedElemLoader.defaultTag= 1 #Tag for the next element.
beam3d= seedElemLoader.newElement("elastic_beam_3d",xc.ID([0,0]))
beam3d.rho= 0.0


puntos= preprocessor.getCad.getPoints
pt= puntos.newPntIDPos3d(1,geom.Pos3d(0.0,0.0,0.0))
pt= puntos.newPntIDPos3d(2,geom.Pos3d(0.0,0.0,L))
lines= preprocessor.getCad.getLines
lines.defaultTag= 1
l= lines.newLine(1,2)
l.nDiv= NumDiv


setTotal= preprocessor.getSets.getSet("total")
setTotal.genMesh(xc.meshDir.I)
# Constraints
coacciones= preprocessor.getConstraintLoader

#
spc= coacciones.newSPConstraint(1,0,0.0) # Nodo 2,gdl 0 # Nodo dorsal.
spc= coacciones.newSPConstraint(1,1,0.0) # Nodo 2,gdl 1
spc= coacciones.newSPConstraint(1,2,0.0) # Nodo 2,gdl 2
spc= coacciones.newSPConstraint(1,5,0.0) # Nodo 2,gdl 5
spc= coacciones.newSPConstraint(2,0,0.0) # Nodo 2,gdl 0 # Nodo frontal.
spc= coacciones.newSPConstraint(2,1,0.0) # Nodo 2,gdl 1

# Loads definition
cargas= preprocessor.getLoadLoader

casos= cargas.getLoadPatterns

#Load modulation.
ts= casos.newTimeSeries("constant_ts","ts")
casos.currentTimeSeries= "ts"
#Load case definition
lp0= casos.newLoadPattern("default","0")
lp0.newNodalLoad(2,xc.Vector([0,0,P,0,0,0]))

#We add the load case to domain.
casos.addToDomain("0")


# Procedimiento de solución
#analisis= predefined_solutions.simple_static_linear(prueba)
#result= analisis.analyze(1)
import os
pth= os.path.dirname(__file__)
if(not pth):
  pth= "."
#print "pth= ", pth
execfile(pth+"/../../aux/solu_linear_buckling.py")

eig1= analysis.getEigenvalue(1)

nod2= nodes.getNode(2)
deltay= nod2.getDisp[2] 
  

deltayTeor= P*L/(E*A)
ratio1= (deltay-deltayTeor)/deltayTeor
blCalc= eig1*P
blTeor= -1*math.pi**2*E*Iz/(L**2)
ratio2= (blCalc-blTeor)/blTeor

''' 
print "deltay= ",(deltay)
print "deltayTeor= ",(deltayTeor)
print "eig1= ",(eig1)
print "ratio1= ",(ratio1)
print "blCalc= ",(blCalc/1e3)," kN \n"
print "blTeor= ",(blTeor/1e3)," kN \n"
print "ratio2= ",(ratio2)
'''

import os
fname= os.path.basename(__file__)
if (abs(ratio1)<1e-5) & (abs(ratio2)<0.06):
  print "test ",fname,": ok."
else:
  print "test ",fname,": ERROR."
