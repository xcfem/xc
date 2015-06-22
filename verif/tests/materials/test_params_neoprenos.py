# -*- coding: utf-8 -*-
# Home made test
# Obtención de las rigideces de un apoyo de neopreno rectangular.

from materials import apoyosNeopreno

G= 900e3 # Módulo de cortante del neopreno
a= 0.3 # Dimensión paralela al eje del dintel.
b= 0.3 # Dimensión normal al eje del dintel.
e= 0.002 # Espesor neto del neopreno (sin chapas).

E= apoyosNeopreno.getEneopreno(G,a,b,e)
Kv= apoyosNeopreno.getKvert(G,a,b,e)
Kh= apoyosNeopreno.getKhoriz(G,a,b,e)
KgiroX= apoyosNeopreno.getKgiroEjeDintel(G,a,b,e)
KgiroZ= apoyosNeopreno.getKgiroEjeDintel(G,a,b,e)
KgiroY= apoyosNeopreno.getKgiroEjeVertical(G,a,b,e)

ratio1= abs(8505000000.0-E)/8505000000.0
ratio2= abs(382725000000.0-Kv)/382725000000.0
ratio3= abs(40500000.0-Kh)/40500000.0
ratio4= abs(956971000.0-KgiroX)/956971000.0
ratio5= abs(956972000.0-KgiroZ)/956972000.0
ratio6= abs(510192.0-KgiroY)/510192.0

''' 
print "E= ",E
print "ratio1= ",ratio1
print "Kv= ",Kv
print "ratio2= ",ratio2
print "Kh= ",Kh
print "ratio3= ",ratio3
print "KgiroX= ",KgiroX
print "ratio4= ",ratio4
print "KgiroZ= ",KgiroZ
print "ratio5= ",ratio5
print "KgiroY= ",KgiroY
print "ratio6= ",ratio6
  '''
import os
fname= os.path.basename(__file__)
if (abs(ratio1)<1e-15) & (abs(ratio2)<1e-15) & (abs(ratio3)<1e-15) & (abs(ratio4)<1e-5) & (abs(ratio5)<1e-8) & (abs(ratio6)<1e-3):
  print "test ",fname,": ok."
else:
  print "test ",fname,": ERROR."
