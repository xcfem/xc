# -*- coding: utf-8 -*-
# Home made test

E= 2.1e6 # Módulo de Young del material.
Nsteps= 10 # Número de pasos para el análisis.
lStrain=[] 
lStress=[]
vStrainTeor= [-0.01,-0.008,-0.006,-0.004,-0.002,0,0.002,0.004,0.006,0.008,0.01]
vStressTeor= [E*-0.01,E*-0.008,E*-0.006,E*-0.004,E*-0.002,E*0,E*0,E*0,E*0,E*0,E*0]

import math
import xc_base
import geom
import xc
from solution import predefined_solutions
from model import predefined_spaces
from model import fix_node_6dof
from materials import typical_materials

# Model definition
prueba= xc.ProblemaEF()
prueba.logFileName= "/tmp/borrar.log" # Para no imprimir mensajes de advertencia.
preprocessor=  prueba.getPreprocessor
# Definimos materiales
sIni= -0.01
sFin= 0.01
incr= ((sFin-sIni)/Nsteps)
ent= typical_materials.defElastNoTensMaterial(preprocessor, "ent",E)

j=sIni
while(j<=sFin):
  ent.setTrialStrain(j,0.0)
  ent.commitState()
  lStrain.append(ent.getStrain())
  lStress.append(ent.getStress())
  j= j+incr 


resta_strain= []
resta_stress= []
def substract(x,y): return x-y
resta_strain= map(substract,lStrain,vStrainTeor)
resta_stress= map(substract,lStress,vStressTeor)

ratio1= 0
for d in resta_strain:
  ratio1= ratio1+d**2
ratio3= math.sqrt(ratio1)
ratio2= 0
for d in resta_stress:
  ratio2= ratio2+d**2
ratio4= math.sqrt(ratio2)

''' 
print "lStrain= ",lStrain
print "lStress= ",lStress
print "vStrain= ",vStrain
print "vStress= ",vStress
print "ratio1= ",ratio1
print "ratio2= ",ratio2
 '''

import os
fname= os.path.basename(__file__)
if (ratio1 < 1e-15) & (ratio2 < 1e-15):
  print "test ",fname,": ok."
else:
  print "test ",fname,": ERROR."
