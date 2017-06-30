# -*- coding: utf-8 -*-


#Verification test of the cálculo del cortante de agotamiento por compresión oblicua del alma (Vu1) según el artículo 44.2.3.1 de EHE-08.
import sys

from materials.ehe import comprobVEHE08
import math

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AOO)"
__copyright__= "Copyright 2015, LCPT and AOO"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

fck=30e6
fcd=fck/1.5
Ncd=0
b=0.35
h=0.7
As=0.0
Ac=b*h-As
b0=b
d=h-0.041
Vu1=comprobVEHE08.getVu1EHE08(fck,fcd,Ncd,Ac,b0,d,math.radians(90),math.radians(45))

ratio1= abs((Vu1-1.3839e6)/1.3839e6)

'''
print "Vu1= ",Vu1/1e3," kN"
print "ratio1= ",ratio1
'''

if ratio1<1e-12:
    print "test cortante EHE-08 01: ok."
else:
    print "test cortante EHE-08 01: ERROR.\n"
  
