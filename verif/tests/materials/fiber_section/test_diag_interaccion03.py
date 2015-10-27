# -*- coding: utf-8 -*-
''' Test de funcionamiento del cálculo del diagrama de interacción.
   Elaboración propia. Carga el diagrama de interacción obtenido en test_diag_interaccion01. '''
from __future__ import division
# Macros
import xc_base
import geom
import xc
from materials.ehe import auxEHE

from materials.ehe import hormigonesEHE
from materials.ehe import EHE_reinforcing_steel

prueba= xc.ProblemaEF()
preprocessor=  prueba.getPreprocessor
materiales= preprocessor.getMaterialLoader
diagInteg= materiales.newInteractionDiagram("diagInteg")
diagInteg.readFrom("/tmp/prueba_diag_interaccion01.dat")

ratio1= diagInteg.getCapacityFactor(geom.Pos3d(352877,0,0))-1
ratio2= diagInteg.getCapacityFactor(geom.Pos3d(352877/2.0,0,0))-0.5
ratio3= diagInteg.getCapacityFactor(geom.Pos3d(-574457,41505.4,2.00089e-11))-1.0
ratio4= diagInteg.getCapacityFactor(geom.Pos3d(-978599,-10679.4,62804.3))-1.0


import os
os.system("rm -f /tmp/prueba_diag_interaccion01.dat")
''' 
print "ratio1= ",(ratio1)
print "ratio2= ",(ratio2)
print "ratio3= ",(ratio3)
print "ratio4= ",(ratio4)
 '''

''' Como el método con el que se graba el diagrama de interacción no es exacto
   (las coordenadas de los vértices son «calculadas») se pierde precisión por lo
   que ha habido que aumentar la toleracia en el ratio3 respecto al test 01
   (test_diag_interaccion01) '''
fname= os.path.basename(__file__)
if((abs(ratio1)<1e-5) & (abs(ratio2)<1e-5) & (abs(ratio3)<2e-2) & (abs(ratio4)<1e-5)):
  print "test ",fname,": ok."
else:
  print "test ",fname,": ERROR."
