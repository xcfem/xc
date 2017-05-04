# -*- coding: utf-8 -*-
# Verification test of the modelo de fibras.

import xc_base
import geom
import xc
from model import predefined_spaces
from solution import predefined_solutions
from materials import typical_materials
import math

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (A_OO)"
__copyright__= "Copyright 2015, LCPT and AO_O"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com ana.ortega.ort@gmal.com"

yF= 2.0 # Fibra en el cuarto cuadrante.
zF= -3.0
F= 2100
MomY= F*zF
MomZ= F*yF
MomYDqFibras= 0.0
MomZDqFibras= 0.0
Es= 2.1e6
Area= 1.0
lado= math.sqrt(Area)
eps= F/(Area*Es)

# Problem type
prueba= xc.ProblemaEF()
preprocessor=  prueba.getPreprocessor
nodes= preprocessor.getNodeLoader
modelSpace= predefined_spaces.StructuralMechanics3D(nodes)
nodes.defaultTag= 1 #First node number.
nod= nodes.newNodeXYZ(1,0,0)
nod= nodes.newNodeXYZ(1,0,0)

# Materials definition
A= 0.0
yG= 0.0
zG= 0.0

IEA= 0.0  ; IK12= 0.0 ; IK13= 0.0
IK21= 0.0 ; IEIy= 0.0 ; IK23= 0.0
IK31= 0.0 ; IK32= 0.0 ; IEIz= 0.0

TEA= 0.0  ; TK12= 0.0 ; TK13= 0.0
TK21= 0.0 ; TEIy= 0.0 ; TK23= 0.0
TK31= 0.0 ; TK32= 0.0 ; TEIz= 0.0

elast0= typical_materials.defElasticMaterial(preprocessor, "elast0",Es)
# Secciones
prb= preprocessor.getMaterialLoader.newMaterial("fiber_section_3d","prb")

prb.addFiber("elast0",Area/4.0,xc.Vector([yF+lado/4,zF+lado/4]))
prb.addFiber("elast0",Area/4.0,xc.Vector([yF-lado/4,zF+lado/4]))
prb.addFiber("elast0",Area/4.0,xc.Vector([yF-lado/4,zF-lado/4]))
prb.addFiber("elast0",Area/4.0,xc.Vector([yF+lado/4,zF-lado/4]))
A= prb.getArea
yG= prb.getCdgY()
zG= prb.getCdgZ()
IEA= prb.getInitialTangentStiffness().at(1,1)
IK12= prb.getInitialTangentStiffness().at(1,2)
IK13= prb.getInitialTangentStiffness().at(1,3)
IK21= prb.getInitialTangentStiffness().at(2,1)
IEIy= prb.getInitialTangentStiffness().at(2,2)
IK23= prb.getInitialTangentStiffness().at(2,3)
IK31= prb.getInitialTangentStiffness().at(3,1)
IK32= prb.getInitialTangentStiffness().at(3,2)
IEIz= prb.getInitialTangentStiffness().at(3,3)

TEA= prb.getTangentStiffness().at(1,1)
TK12= prb.getTangentStiffness().at(1,2)
TK13= prb.getTangentStiffness().at(1,3)
TK21= prb.getTangentStiffness().at(2,1)
TEIy= prb.getTangentStiffness().at(2,2)
TK23= prb.getTangentStiffness().at(2,3)
TK31= prb.getTangentStiffness().at(3,1)
TK32= prb.getTangentStiffness().at(3,2)
TEIz= prb.getTangentStiffness().at(3,3)


# Elements definition
elementos= preprocessor.getElementLoader
elementos.defaultMaterial= "prb"
elementos.defaultTag= 1
zl= elementos.newElement("zero_length_section",xc.ID([1,2]))

# Constraints
coacciones= preprocessor.getConstraintLoader
#
spc= coacciones.newSPConstraint(1,0,0.0) # Nodo 1
spc= coacciones.newSPConstraint(1,1,0.0)
spc= coacciones.newSPConstraint(1,2,0.0)
spc= coacciones.newSPConstraint(1,3,0.0)
spc= coacciones.newSPConstraint(1,4,0.0)
spc= coacciones.newSPConstraint(1,5,0.0)

spc= coacciones.newSPConstraint(2,1,0.0)
spc= coacciones.newSPConstraint(2,2,0.0)
spc= coacciones.newSPConstraint(2,3,0.0)

# Loads definition
cargas= preprocessor.getLoadLoader
casos= cargas.getLoadPatterns
#Load modulation.
ts= casos.newTimeSeries("constant_ts","ts")
casos.currentTimeSeries= "ts"
#Load case definition
lp0= casos.newLoadPattern("default","0")
lp0.newNodalLoad(2,xc.Vector([F,0,0,0,MomY,MomZ]))

#We add the load case to domain.
casos.addToDomain("0")

# Solution
analisis= predefined_solutions.simple_static_linear(prueba)
result= analisis.analyze(1)


nodes.calculateNodalReactions(True)
nod2= nodes.getNode(2)
deltax= nod2.getDisp[0] 
nod1= nodes.getNode(1)
Reac= nod1.getReaction 

elementos= preprocessor.getElementLoader

elem1= elementos.getElement(1)
elem1.getResistingForce()
scc= elem1.getSection()
nfib= scc.getFibers().getNumFibers()
avgStrain= 0.0
fibras= scc.getFibers()
for f in fibras:
# print "fibra: ",tag, " deformación: ", getMaterial.strain
   avgStrain+= f.getMaterial().getStrain()
avgStrain/= nfib
MomYDqFibras= fibras.getMy(0.0)
MomZDqFibras= fibras.getMz(0.0)

Resul= scc.getStressResultant()

ratio1= (F+Reac[0])/F
ratio2= (Es*deltax)/F
ratio3= (avgStrain-deltax)/deltax
ratio4= (avgStrain-eps)/eps
ratio5= (Resul[0]+Reac[0])/Reac[0]
ratio6= (Resul[1]+Reac[5])/Reac[5]
ratio7= (Resul[2]+Reac[4])/Reac[4]
ratio8= (Resul[1]-MomZ)/MomZ
ratio9= (Resul[2]-MomY)/MomY
ratio10= (Resul[1]-MomZDqFibras)/MomZDqFibras
ratio11= (Resul[2]-MomYDqFibras)/MomYDqFibras

'''
print "F= ",F
print "A= ",A
print "eps= ",eps
print "yG= ",yG
print "zG= ",zG,"\n\n"
print "IEA= ",IEA}   \print{" IK12= ",IK12} \print{" IK13= ",IK13
print "IK21= ",IK21} \print{" IEIy= ",IEIy} \print{" IK23= ",IK23
print "IK31= ",IK31} \print{" IK32= ",IK32} \print{" IEIz= ",IEIz,"\n\n"

print "TEA= ",TEA}   \print{" TK12= ",TK12} \print{" TK13= ",TK13
print "TK21= ",TK21} \print{" TEIy= ",TEIy} \print{" TK23= ",TK23
print "TK31= ",TK31} \print{" TK32= ",TK32} \print{" TEIz= ",TEIz,"\n\n"

print "Reac= ",Reac
print "dx= ",deltax

print "Resul= ",Resul

print "ratio1= ",(ratio1)
print "ratio2= ",(ratio2)
print "ratio3= ",(ratio3)
print "ratio4= ",(ratio4)
print "ratio5= ",(ratio5)
print "ratio6= ",(ratio6)
print "ratio7= ",(ratio7)
print "ratio8= ",(ratio8)
print "ratio9= ",(ratio9)
print "ratio10= ",(ratio10)
print "ratio11= ",(ratio11)
'''

import os
fname= os.path.basename(__file__)
if (abs(ratio1)<1e-8) & (abs(ratio2-1.0)<1e-8) & (abs(ratio3)<1e-8) & (abs(ratio4)<1e-8) & (abs(ratio5)<1e-8) & (abs(ratio6)<1e-8) & (abs(ratio7)<1e-8) & (abs(ratio8)<1e-8) & (abs(ratio9)<1e-8) & (abs(ratio10)<1e-8) & (abs(ratio11)<1e-8) :
  print "test ",fname,": ok."
else:
  print "test ",fname,": ERROR."
