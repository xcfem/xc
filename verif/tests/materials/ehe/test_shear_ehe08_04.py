# -*- coding: utf-8 -*-

'''Verification test of the computation of strain in the web
according to article 44.2.3.2.2 of EHE. The point is to verify that
the expression is correctly written in Python.'''
from __future__ import division

import sys

from materials.ehe import comprobVEHE08
import math

__author__= "Luis C. Pérez Tato (LCPT)"
__copyright__= "Copyright 2014, LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

#Coeficientes de seguridad.
gammaC= 1.5

#All values are fictional, they are used only to check
#   that the expression is written correctly.

Nd=0
Md=0 #Valor absoluto del momento de cálculo.
Vd=0 #Valor absoluto del cortante efectivo de cálculo (artículo 42.2.2).
Td=0 #Torsor de cálculo.
z=10 #Brazo mecánico.
AsPas=1/4 #Area of passive longitudinal reinforcement anchored at a distance greater than the effective depth of the section.
AsAct=1/8 #Area of active (prestressed) longitudinal reinforcement anchored at a distance greater than the effective depth of the section.
Es=1000 #Elastic modulus of the steel de la reinforcement pasiva (AQUI FICTICIO).
Ep=2000 #Elastic modulus of the steel de la reinforcement activa (AQUI FICTICIO).
Fp=0 #Prestressing force in the cross-section (positive if in tension).
Ae=0.01 #Area enclosed by the mid-line of the effective hollow area.
ue=1 #Perimeter of the mid-line of the effective hollow area.

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

import os
from miscUtils import LogMessages as lmsg
fname= os.path.basename(__file__)
if (ratio1<1e-5) and (ratio2<1e-5) and (ratio3<1e-5) and (ratio4<1e-5) and (ratio5<1e-5) and (ratio6<1e-5):
  print "test ",fname,": ok."
else:
  lmsg.error(fname+' ERROR.')

  
