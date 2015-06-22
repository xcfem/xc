# -*- coding: utf-8 -*-
# home made test
# Criterio de signos elementos ZeroLengthSection.

#     El axil y los cortantes tienen la misma direccion y sentido que los ejes locales.
#     El torsor Mx y el flector My tienen las direcciones y sentido de los ejes X e Y locales.
#     El flector Mz tiene la misma dirección y sentido CONTRARIO al del eje Z local.
#     Section's y axis is element z axis.
#Section scheme:

#             y
#             ^
#             |
#             |
#       o 3   |     o 2
#             |
# z<----------+
#             
#       o 4         o 1
# 

from __future__ import division
import xc_base
import geom
import xc
from solution import predefined_solutions
from model import predefined_spaces
from model import fix_node_6dof
from materials import typical_materials
from actions import utilsAcciones
from postprocess import prop_statistics



# Propiedades del material
E= 2.1e6 # Módulo elástico en Pa
epsilon= 3.5e-3
sigma= E*epsilon

# Propiedades de la sección
width= 4
depth= 2
fiberArea= 1e-4 # Área de cada fibra expresada en m2
A= 4*fiberArea # Área de cada fibra expresada en m2
Iy= 4*(fiberArea*(width/2.0)**2) # Momento de inercia de la sección expresada en m4
Iz= 4*(fiberArea*(depth/2.0)**2) # Momento de inercia de la sección expresada en m4

F= sigma*fiberArea
N= 4*F
My= 0.0
Mz= 0.0

# Problem type
prueba= xc.ProblemaEF()
mdlr= prueba.getModelador   

# Materials definition
elast= typical_materials.defElasticMaterial(mdlr,"elast",E)

# Fibers
y1= -depth/2.0
z1= -width/2.0
fourFibersSection= mdlr.getMaterialLoader.newMaterial("fiber_section_3d","fourFibersSection")

f1= fourFibersSection.addFiber("elast",fiberArea,xc.Vector([y1,z1]))
f2= fourFibersSection.addFiber("elast",fiberArea,xc.Vector([-y1,z1]))
f3= fourFibersSection.addFiber("elast",fiberArea,xc.Vector([-y1,-z1]))
f4= fourFibersSection.addFiber("elast",fiberArea,xc.Vector([y1,-z1]))


f1.getMaterial().setTrialStrain(epsilon,0.0)
f2.getMaterial().setTrialStrain(epsilon,0.0)
f3.getMaterial().setTrialStrain(epsilon,0.0)
f4.getMaterial().setTrialStrain(epsilon,0.0)

N0= fourFibersSection.getFibers().getResultant()
Mz0= fourFibersSection.getFibers().getMz(0.0)
My0= fourFibersSection.getFibers().getMy(0.0)

fourFibersSection.setupFibers()
RR= fourFibersSection.getStressResultant()
R0= xc.Vector([RR[0],RR[2],RR[1]])

fourFibersSection.revertToStart()
nodos= mdlr.getNodeLoader
predefined_spaces.gdls_resist_materiales3D(nodos)
nodos.defaultTag= 1 #First node number.
nod= nodos.newNodeXYZ(0.0,0.0,0.0)
nod= nodos.newNodeXYZ(0.0,0.0,0.0)

# Elements definition
elementos= mdlr.getElementLoader
elementos.defaultMaterial= "fourFibersSection"
elementos.dimElem= 1
zl= elementos.newElement("zero_length_section",xc.ID([1,2]))

# Constraints
coacciones= mdlr.getConstraintLoader
fix_node_6dof.fixNode6DOF(coacciones,1)
spc= coacciones.newSPConstraint(2,1,0.0)
spc= coacciones.newSPConstraint(2,2,0.0)
spc= coacciones.newSPConstraint(2,3,0.0)


# Loads definition
cargas= mdlr.getLoadLoader
casos= cargas.getLoadPatterns
#Load modulation.
ts= casos.newTimeSeries("constant_ts","ts")
casos.currentTimeSeries= "ts"
#Load case definition
lp0= casos.newLoadPattern("default","0")
lp0.newNodalLoad(2,xc.Vector([N,0,0,0,My,Mz])) #Section's y axis is element z axis.

#Agregamos el caso de carga al dominio.
casos.addToDomain("0")

# Solution
analisis= predefined_solutions.simple_static_linear(prueba)
result= analisis.analyze(1)

zl.getResistingForce()
section= zl.getSection()

N1= section.getFibers().getResultant()
Mz1= section.getFibers().getMz(0.0)
My1= section.getFibers().getMy(0.0)

section.setupFibers()
RR= section.getStressResultant()
R1= xc.Vector([RR[0],RR[2],RR[1]])

vTeor= xc.Vector([N,My,Mz])
v0= xc.Vector([N0,My0,Mz0])
v1= xc.Vector([N1,My1,Mz1])

import math
error= math.sqrt((vTeor-v0).Norm2()+(vTeor-v1).Norm2()+(vTeor-R0).Norm2()+(vTeor-R1).Norm2())

'''
print "I= ", zl.getIVector
print "K= ", zl.getKVector
print "EA= ",E*A
print "EIz= ",E*Iz
print "EIy= ",E*Iy
print fourFibersSection.getInitialTangentStiffness()

print "v0= ",v0
print "v1= ",v1
print "R0= ",R0
print "R1= ",R1
print "vTeor= ", vTeor
print "error= ", error
'''

import os
fname= os.path.basename(__file__)
if (error < 1e-3):
  print "test ",fname,": ok."
else:
  print "test ",fname,": ERROR."
