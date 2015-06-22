# -*- coding: utf-8 -*-
'''Obtención de la longitud de cordón necesaria para apoyar el extremo
de una viga mediante soldadura directa del alma, según el artículo 61.4.1
de la instrucción EAE.'''
# Comprobación mediante ejemplo página XXV-31 del libro de la UNE.

from materials.eae import apoyo_por_soldadura_alma
import math


# DATOS acciones
Vd= 8585*9.81 # Valor de cálculo de la carga

# DATOS geométricos
h= 270e-3 # Canto de la viga expresado en metros.
tw= 6.6e-3 # Espesor del alma expresado en metros.
a= 4e-3 # Espesor de garganta expresado en metros.
l= 5.15925e-2 # Longitud de los cordones.

# DATOS mecánicos
fy= 275e6 # Acero S-275-JR
fyd= fy/1.1 # Minoración límite elástico
fu= 430e6 # Resistencia a tracción  del acero de las piezas a soldar S-275-JR (tabla 59.8.2 pág 304 EAE).
Es= 2e11 # Módulo elástico del acero.

# DATOS seguridad
betaW= 0.8 # Coeficiente de correlación para S-275 (tabla 59.8.2 pág 304 EAE).
gammaMw= 1.0/math.sqrt(3) # Coeficiente de minoración del acero para uniones (artículo 59.8.2 página 304 EAE).



# Resultados parciales.
VRd= apoyo_por_soldadura_alma.VdSoldAlma(l,a,betaW,gammaMw,2600*9.81/1e-4)
lMax= apoyo_por_soldadura_alma.LongMaxSoldAlma(tw)

ratio1= abs(((VRd-Vd)/VRd))

'''
print "Longitud de los cordones l= ",l*1000,"mm \n"
print "Longitud máxima de los cordones lMax= ",lMax*1000," mm\n"
print "Espesor de garganta a= ",a*1000,"mm \n"
print "Cortante que solicita la Vd= ",Vd/1000," kN\n"
print "Cortante último de la unión VRd= ",VRd/1000," kN\n"
print "VRd/Vd= ",VRd/Vd
print "ratio1= ",ratio1
'''

if (abs(ratio1)<0.1):
  print "test verif_apoyo_por_soldadura_alma: ok.\n"
else:
  print "test verif_apoyo_por_soldadura_alma: ERROR.\n"
