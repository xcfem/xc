# -*- coding: utf-8 -*-

'''Verificación de la obtención de la carga última de una ménsula corta a
partir del ejercicio 24.4.5 de la página XXV-40 del libro de metálicas 
de Vicente Cudós Samblancat.'''

from materials.eae import comprobacion_casquillo_rigidizado
import math


# Geometry
H= 0.2
l= 0.2
d= 0.2
tChapa= 8e-3 # Espesor de la chapa sobre la que apoya la carga.
tRig= 8e-3 # Espesor del rigidizador.

# Material properties
fy= 2600*9.81/1e-4 # Acero A42b
fyd= fy/1.0 # Minoración límite elástico
Es= 2.1e6*9.81/1e-4 # Elastic modulus del acero.

# Resultados parciales.
c= comprobacion_casquillo_rigidizado.widthMax(tChapa,l,H)
lmbd= comprobacion_casquillo_rigidizado.esbeltezAdim(c,tChapa,fy,Es)
CE= comprobacion_casquillo_rigidizado.coefEscuadra(lmbd)
MplRd= comprobacion_casquillo_rigidizado.momPlastRig(tRig,c,fy)
VRd2= comprobacion_casquillo_rigidizado.cargaUltRig(CE,d,MplRd)

'''La diferencia entre este valor de cTeor y el que emplea este programa
es que el programa SI considera el espesor del ala del casquillo para el
cálculo de c tal y como se desprende de la figura de la EAE y del libro
de la UNED.'''
 
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

if (abs(ratio1)<0.1) & (abs(ratio2)<0.1) & (abs(ratio3)<0.1)& (abs(ratio4)<0.1) & (abs(ratio5)<0.1):
  print "test comprobacion_casquillo: ok."
else:
  print "test comprobacion_casquillo: ERROR."

