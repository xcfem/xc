# -*- coding: utf-8 -*-

#Comprobación del cálculo de una ménsula corta según EHE.
#Home made test.

from __future__ import division
import sys

from materials.ehe import mensula_cortaEHE08

__author__= "Luis C. Pérez Tato (LCPT)"
__copyright__= "Copyright 2014, LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

Fvd=1000e3 #Carga vertical en la ménsula, positiva hacia abajo (N).
Fhd=100e3 #Carga horizontal en la ménsula, positiva hacia afuera (N).
tipoJunta="monolitica" #Tipo de junta de acuerdo con el apartado 64.1.2.1.
a=0.6 #Distancia (m) entre el eje de la carga aplicada y la sección de empotramiento de la ménsula (ver figura 64.1.2 de EHE-08).

fck=35E6 #Resistencia característica del hormigón (Pa).
fyk=500E6 #Resistencia característica del acerp (Pa).

#Resultados
dmin=mensula_cortaEHE08.getCantoUtilMinimo(tipoJunta,a)
dminTeor=0.988235294118
ratio1=abs(dmin-dminTeor)/dminTeor
T1d=mensula_cortaEHE08.getTraccionMainReinforcement(tipoJunta,Fvd,Fhd)
T1dTeor=814285.65
ratio2=abs(T1d-T1dTeor)/T1dTeor
As1=mensula_cortaEHE08.getAreaNecMainReinforcement(tipoJunta,Fvd,Fhd,fyk/1.15)
As1Teor=T1dTeor/400e6
ratio3=abs(As1-As1Teor)/As1Teor
Ase=mensula_cortaEHE08.getAreaNecCercos(Fvd,fyk/1.15)
AseTeor=0.2*Fvd/400e6
ratio4=abs(Ase-AseTeor)/AseTeor
Aplaca=mensula_cortaEHE08.getAreaNecApoyo(Fvd,fck/1.5)
AplacaTeor=Fvd/0.7/fck*1.5
ratio5=abs(Aplaca-AplacaTeor)/AplacaTeor


##print "dmin= ",dmin," m"
##print "ratio1= ",ratio1
##print "T1d= ",T1d/1e3," kN"
##print "ratio2= ",ratio2
##print "As1= ",As1*1e4," cm2"
##print "ratio3= ",ratio3
##print "Ase= ",Ase*1e4," cm2"
##print "ratio4= ",ratio4
##print "Aplaca= ",Aplaca," m2"
##print "ratio5= ",ratio5
  

if (ratio1<1e-12) and (ratio2<1e-7) and (ratio3<1e-7) and (ratio4<1e-15) and (ratio5<1e-15):
    print "test ménsula corta EHE-08: ok."
else:
    print "test ménsula corta EHE-08: ERROR."
  
