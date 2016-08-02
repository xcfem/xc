# -*- coding: utf-8 -*-

#Test de funcionamiento del cálculo de la deformación en el alma
#según el artículo 44.2.3.2.2 de EHE. Lo único que se pretende
#con el test es comprobar que la fórmula está bien escrita en la macro.

from __future__ import division
import sys

from materials.ehe import comprobVEHE08
import math

#Coeficientes de seguridad.
gammaC= 1.5

#Todos los valores son FICTICIOS, sólo se emplean para comprobar
#   que la fórmula está bien escrita en la macro.

Nd=0
Md=0 #Valor absoluto del momento de cálculo.
Vd=0 #Valor absoluto del cortante efectivo de cálculo (artículo 42.2.2).
Td=0 #Torsor de cálculo.
z=10 #Brazo mecánico.
AsPas=1/4 #Área de la de la reinforcement longitudinal pasiva traccionada anclada a una distancia mayor que el depth útil de la sección en estudio.
AsAct=1/8 #Área de la de la reinforcement longitudinal activa traccionada anclada a una distancia mayor que el depth útil de la sección en estudio.
Es=1000 #Elastic modulus del acero de la reinforcement pasiva (AQUI FICTICIO).
Ep=2000 #Elastic modulus del acero de la reinforcement activa (AQUI FICTICIO).
Fp=0 #Fuerza producida por el pretensado en la sección (positiva si es de tracción).
Ae=0.01 #Área encerrada por la línea media de la sección hueca eficaz.
ue=1 #Perímetro de la línea media de la sección hueca eficaz.

Md= 10
epsilonX01=comprobVEHE08.getEpsilonXEHE08(Nd,Md,Vd,Td,z,AsPas,AsAct,Es,Ep,Fp,Ae,ue)
ratio1=abs(epsilonX01-1e-3)/1e-3
Md= 0
Vd= 1
epsilonX02=comprobVEHE08.getEpsilonXEHE08(Nd,Md,Vd,Td,z,AsPas,AsAct,Es,Ep,Fp,Ae,ue)
ratio2=abs(epsilonX02-2e-3)/2e-3
Vd= 0
Td= 0.02
epsilonX03=comprobVEHE08.getEpsilonXEHE08(Nd,Md,Vd,Td,z,AsPas,AsAct,Es,Ep,Fp,Ae,ue)
ratio3=abs(epsilonX03-1e-3)/1e-3
Td= 0
Nd= -1
epsilonX04=comprobVEHE08.getEpsilonXEHE08(Nd,Md,Vd,Td,z,AsPas,AsAct,Es,Ep,Fp,Ae,ue)
ratio4=abs(epsilonX04)
Nd= 2
epsilonX05=comprobVEHE08.getEpsilonXEHE08(Nd,Md,Vd,Td,z,AsPas,AsAct,Es,Ep,Fp,Ae,ue)
ratio5=abs(epsilonX05-1e-3)/1e-3
Nd= 4
Fp= -1
epsilonX06=comprobVEHE08.getEpsilonXEHE08(Nd,Md,Vd,Td,z,AsPas,AsAct,Es,Ep,Fp,Ae,ue)
ratio6=abs(epsilonX06-1e-3)/1e-3

'''
print "epsilonX01= ",epsilonX01*1000," por mil"
print "ratio1= ",ratio1
print "epsilonX02= ",epsilonX02*1000," por mil"
print "ratio2= ",ratio2
print "epsilonX03= ",epsilonX03*1000," por mil"
print "ratio3= ",ratio3
print "epsilonX04= ",epsilonX04*1000," por mil"
print "ratio4= ",ratio4
print "epsilonX05= ",epsilonX05*1000," por mil"
print "ratio5= ",ratio5
print "epsilonX06= ",epsilonX06*1000," por mil"
print "ratio6= ",ratio6
'''

if (ratio1<1e-5) and (ratio2<1e-5) and (ratio3<1e-5) and (ratio4<1e-5) and (ratio5<1e-5) and (ratio6<1e-5):
    print "test cortante EHE-08 04: ok."
else:
    print "test cortante EHE-08 04: ERROR."
  
