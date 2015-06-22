# -*- coding: utf-8 -*-

#Test de funcionamiento del cálculo de Vsu.
#El resultado se compara con el obtenidos del ejercicio ER-CA-02 de
#www.areadecalculo.com. Según la norma EHE-08».

import sys

from materials.ehe import comprobVEHE08
from materials.ehe import areaBarrasEHE
import math

#Coeficientes de seguridad.
gammas= 1.15 #Coeficiente de minoración de la resistencia del acero.

numBarrasEstribo=4
areaRamasEstribo=numBarrasEstribo*areaBarrasEHE.Fi6
sepEstribos=0.2
AsTrsv=areaRamasEstribo/sepEstribos
z=0.9*0.55
alpha=math.pi/2
theta=math.pi/4
fyd=500e6/gammas
Vsu=comprobVEHE08.getVsuEHE08(z,alpha,theta,AsTrsv,fyd)
ratio1=abs(Vsu-111.966e3)/111.966e3

##print "areaRamasEstribo= ",AsTrsv*1e4," cm2"
##print "Vsu= ",Vsu/1e3," kN"
##print "ratio1= ",ratio1," kN"
  

if ratio1<0.01:
    print "test cortante EHE-08 03: ok."
else:
    print "test cortante EHE-08 03: ERROR."
  
