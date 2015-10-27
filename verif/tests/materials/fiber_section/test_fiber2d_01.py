# -*- coding: utf-8 -*-
# Test de funcionamiento del comando addFiber
import xc_base
import geom
import xc
from solution import predefined_solutions
from model import predefined_spaces
from materials import typical_materials


yF= 2.0
F= 2100
MomZ= F*yF
MomZDqFibras= 0.0
Es= 2.1e6
Area= 1.0
eps= F/(Area*Es)

# Problem type
prueba= xc.ProblemaEF()
prueba.logFileName= "/tmp/borrar.log" # Para no imprimir mensajes de advertencia.
preprocessor=  prueba.getPreprocessor
nodos= preprocessor.getNodeLoader
predefined_spaces.gdls_resist_materiales2D(nodos)

# Definimos nodos
nodos.defaultTag= 1 #First node number.
nod= nodos.newNodeXY(1,0)
nod= nodos.newNodeXY(1,0)


# Definimos materiales
A= 0.0
yG= 0.0

IEA= 0.0  ; IK12= 0.0
IK21= 0.0 ; IEIy= 0.0

TEA= 0.0  ; TK12= 0.0
TK21= 0.0 ; TEIy= 0.0

elast0= typical_materials.defElasticMaterial(preprocessor, "elast0",Es)
# Secciones
materiales= preprocessor.getMaterialLoader
prueba= materiales.newMaterial("fiber_section_2d","prueba")
fiber= prueba.addFiber("elast0",Area,xc.Vector([yF]))

A= prueba.getArea()
yG= prueba.getCdgY()
initialTangent= prueba.getInitialTangentStiffness()
IEA= initialTangent.at(1,1)
IK12= initialTangent.at(1,2)
IK21= initialTangent.at(2,1)
IEIy= initialTangent.at(2,2)

prueba.setTrialSectionDeformation(xc.Vector([eps,0.0]))
tangent= prueba.getTangentStiffness()
TEA= tangent.at(1,1)
TK12= tangent.at(1,2)
TK21= tangent.at(2,1)
TEIy= tangent.at(2,2)

R= prueba.getStressResultant()
fibers= prueba.getFibers()
MomZDqFibras= fibers.getMz(0.0)

ratio1= R[0]-F
ratio2= R[1]-MomZ
ratio3= R[1]-MomZDqFibras
ratio4= yG-yF


''' 
print "F= ",F
print "MomZ= ",MomZ
print "MomZDqFibras= ",MomZDqFibras
print "A= ",A
print "eps= ",eps
print "yF= ",yF
print "yG= ",yG
print "IEA= ",IEA  \print{" IK12= ",IK12
print "IK21= ",IK21} \print{" IEIy= ",IEIy,"\n\n"


print "TEA= ",TEA}   \print{" TK12= ",TK12
print "TK21= ",TK21} \print{" TEIy= ",TEIy,"\n\n"

print "R= ",R

print "ratio1= ",(ratio1)
print "ratio2= ",(ratio2)
print "ratio3= ",(ratio3)
print "ratio4= ",(ratio4)
'''

import os
fname= os.path.basename(__file__)
if (abs(ratio1)<1e-15) & (abs(ratio2)<1e-15) & (abs(ratio3)<1e-15) & (abs(ratio4)<1e-15) :
  print "test ",fname,": ok."
else:
  print "test ",fname,": ERROR."
