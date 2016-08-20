# -*- coding: utf-8 -*-
# Home made test
fy= 2600 # Tensión de cedencia del material.
E= 2.1e6 # Módulo de Young del material.
Nsteps= 10 # Número de pasos para el análisis.

import math
import xc_base
import geom
import xc
from solution import predefined_solutions
from model import predefined_spaces
from model import fix_node_6dof
from model import fija_nodos_lineas
from model import cargas_nodo
from materials import typical_materials

# Puntos de la función tensión - deformación
x_modelo= [0,0.000242405785552,0.000480663940905,0.000710697802964,0.000928571428571,0.00113055693959,0.00131319830792,0.00147337048911,0.00160833289274,0.00171577627466,0.00179386224882,0.00184125474255,0.00185714285714,0.00184125474255,0.00179386224882,0.00171577627466,0.00160833289274,0.00147337048911,0.00131319830792,0.00113055693959,0.000928571428571,0.000710697802964,0.000480663940905,0.000242405785552,2.27434405556e-19,-0.000242405785552,-0.000480663940905,-0.000710697802964,-0.000928571428571,-0.00113055693959,-0.00131319830792,-0.00147337048911,-0.00160833289274,-0.00171577627466,-0.00179386224882,-0.00184125474255,-0.00185714285714,-0.00184125474255,-0.00179386224882,-0.00171577627466,-0.00160833289274,-0.00147337048911,-0.00131319830792,-0.00113055693959,-0.000928571428571,-0.000710697802964,-0.000480663940905,-0.000242405785552,-4.54868811112e-19,0.000242405785552,0.000480663940905,0.000710697802964,0.000928571428571,0.00113055693959,0.00131319830792]
y_modelo= [0,509.052149658,1009.3942759,1492.46538622,1950,2374.16957313,2600,2600,2600,2600,2600,2600,2600,2566.63495936,2467.11072253,2303.13017679,2077.49907476,1794.07802714,1457.71644663,1074.16957313,650,192.465386224,-290.6057241,-790.947850342,-1300,-1809.05214966,-2309.3942759,-2600,-2600,-2600,-2600,-2600,-2600,-2600,-2600,-2600,-2600,-2566.63495936,-2467.11072253,-2303.13017679,-2077.49907476,-1794.07802714,-1457.71644663,-1074.16957313,-650,-192.465386224,290.6057241,790.947850342,1300,1809.05214966,2309.3942759,2600,2600,2600,2600]



# Model definition
prueba= xc.ProblemaEF()
prueba.logFileName= "/tmp/borrar.log" # Para no imprimir mensajes de advertencia.
preprocessor=  prueba.getPreprocessor
# Definimos materiales
epp= typical_materials.defElasticPPMaterial(preprocessor, "epp",E,fy,-fy)
A= 1.5*epp.eyp
fin= 54
incr= math.pi/24.0
x= []
y= []
j= 0
while (j<=fin):
  epp.setTrialStrain(A*math.sin(j*incr),0.0)
  epp.commitState()
  x.append(epp.getStrain())
  y.append(epp.getStress())
  j= j+1

resta_x= []
resta_y= []
def substract(x,y): return x-y
resta_x= map(substract,x,x_modelo)
resta_y= map(substract,y,y_modelo)

ratio1= 0
for d in resta_x:
    ratio1= ratio1+d**2
ratio1= math.sqrt(ratio1)
ratio2= 0
for d in resta_y:
    ratio2= ratio2+d**2
ratio2= math.sqrt(ratio2)

'''
print "ratio1= ",ratio1
print "ratio2= ",ratio2
'''

import os
fname= os.path.basename(__file__)
if((ratio1<1e-13) & (ratio2<1e-7)):
  print "test ",fname,": ok."
else:
  print "test ",fname,": ERROR."    
