# -*- coding: utf-8 -*-
'''Verificación de la obtención de la resistencia de un cordón de
soldadura a partir del ejemplo de la página 57 de la monografia
«Estudio de los diferentes métodos de dimensionamiento para uniones atornilladas y soldadas de acuerdo con la normativa vigente» de Marc Masnou Malgosa.'''

from materials.eae import resistencia_cordon_soldadura
import math

# Tensiones sobre el plano de garganta abatido sobre la cara del cordón
tn= 72.07e6 # Tensión tangencial normal al eje del cordón.
ta= -33.22e6 # Tensión tangencial paralela al eje del cordón.
n= -252.4e6 # Tensión normal.

# Tensiones sobre la sección por el plano de garganta
sigmaN= resistencia_cordon_soldadura.getSigmaNPlanoGarganta(n,tn) 
tauN= resistencia_cordon_soldadura.getTauNPlanoGarganta(n,tn)
tauPll= ta 

# Tension de comparación
sigmaCo= resistencia_cordon_soldadura.getTensComparacionCordonAngulo(n,tn,ta)
sigmaUlt= resistencia_cordon_soldadura.getValorComparacionResistenciaCordon(430e6,275e6,1.25)
fcCond1= resistencia_cordon_soldadura.getFCCondicion1Cordon(n,tn,ta,430e6,275e6,1.25)
sigmaNUlt= resistencia_cordon_soldadura.getTensionNormalUltimaCordon(430e6,1.25)
fcCond2= resistencia_cordon_soldadura.getFCCondicion2Cordon(n,tn,430e6,1.25)

ratio1= ((sigmaN+229.44e6)/229.44e6)
ratio2= ((tauN+127.54e6)/127.54e6)
ratio3= ((sigmaCo-323.65e6)/323.65e6)
ratio4= ((sigmaUlt-404.706e6)/404.706e6)
ratio5= (fcCond1-sigmaCo/sigmaUlt)
ratio6= (fcCond2+sigmaN/sigmaNUlt)


'''
print "sigmaN= ",sigmaN
print "tauN= ",tauN
print "tauPll= ",tauPll
print "sigmaCo= ",sigmaCo
print "sigmaUlt= ",sigmaUlt
print "fcCond1= ",fcCond1
print "sigmaNUlt= ",sigmaNUlt
print "fcCond2= ",fcCond2

print "ratio1= ",ratio1
print "ratio2= ",ratio2
print "ratio3= ",ratio3
print "ratio3= ",ratio4
print "ratio5= ",ratio5
print "ratio6= ",ratio5
'''

if (abs(ratio1)<0.001) & (abs(ratio2)<0.001) & (abs(ratio3)<0.001) & (abs(ratio4)<0.001) & (abs(ratio5)<0.001) & (abs(ratio6)<0.001):
  print "test verif_resistencia_cordon: ok."
else:
  print "test verif_resistencia_cordon: ERROR."
