# -*- coding: utf-8 -*-

'''Verification of the computing of the ultimate load of an steel corbel
from the exercise 24.4.5 at page XXV-40 of the book Estructuras Metálicas
de Vicente Cudós Samblancat (url={https://books.google.ch/books?id=7UrJnQEACAAJ}).'''

from materials.eae import EAE_limit_state_checking
import math

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AOO)"
__copyright__= "Copyright 2015, LCPT and AOO"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

# Geometry
H= 0.2
l= 0.2
d= 0.2
tChapa= 8e-3 # Espesor de la chapa sobre la que apoya la carga.
tRig= 8e-3 # Espesor del rigidizador.

# Material properties
fy= 2600*9.81/1e-4 # Acero A42b
fyd= fy/1.0 # Strength reduction factor
Es= 2.1e6*9.81/1e-4 # Elastic modulus of the steel.

# Resultados parciales.
c= EAE_limit_state_checking.widthMax(tChapa,l,H)
lmbd= EAE_limit_state_checking.esbeltezAdim(c,tChapa,fy,Es)
CE= EAE_limit_state_checking.coefEscuadra(lmbd)
MplRd= EAE_limit_state_checking.momPlastRig(tRig,c,fy)
VRd2= EAE_limit_state_checking.ultimateLoadRig(CE,d,MplRd)

'''The difference between this value for cTeor and the one used here
is that this program DO accounts for the web thickess to compute c
as specified in the figure on the EAE standard and in the reference book
(see at the top of this file).'''
 
cTeor= (0.208*math.sqrt(2)/2)
ratio1= ((c-cTeor)/cTeor)
lambdaTeor= (0.0283*c/tRig)
ratio2= ((lmbd-lambdaTeor)/lambdaTeor)
ratio3= ((CE-1.8)/1.8)
MplRdTeor= (103381*9.81/100)
ratio4= ((MplRdTeor-MplRd)/MplRdTeor)
VRd2Teor= (CE*MplRdTeor/d)
ratio5= ((VRd2Teor-VRd2)/VRd2Teor)

'''
print "c= ",c
print "lambda= ",lmbd
print "CE= ",CE
print "MplRd= ",MplRd/1e3,"kN m\n"
print "VRd2= ",VRd2/1e3,"kN \n"

print "ratio1= ",ratio1
print "ratio2= ",ratio2
print "ratio3= ",ratio3
print "ratio4= ",ratio4
print "ratio5= ",ratio5'''
import os
from miscUtils import LogMessages as lmsg
fname= os.path.basename(__file__)
if (abs(ratio1)<0.1) & (abs(ratio2)<0.1) & (abs(ratio3)<0.1)& (abs(ratio4)<0.1) & (abs(ratio5)<0.1):
   print "test ",fname,": ok."
else:
  lmsg.error(fname+' ERROR.')

