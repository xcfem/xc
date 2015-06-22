# -*- coding: utf-8 -*-

import scipy.interpolate
import math

# Coeficientes de presión exterior para una marquesina a un agua según la tabla D.10 del CTE.


'''
Presión exterior hacia abajo en zona A de marquesina a 1 agua 
en función de la pendiente según la tabla D.10.
'''

x= []
y= []
x.append(math.radians(0)); y.append(0.5)
x.append(math.radians(5)); y.append(0.8)
x.append(math.radians(10)); y.append(1.2)
x.append(math.radians(15)); y.append(1.4)
x.append(math.radians(20)); y.append(1.7)
x.append(math.radians(25)); y.append(2.0)
x.append(math.radians(30)); y.append(2.2)
cpMarq1AguaAbajoZonaA= scipy.interpolate.interp1d(x,y)

'''
Presión exterior hacia arriba en zona A de marquesina a 1 agua
con factor de obstrucción nulo en función de la pendiente
según la tabla D.10.
'''

x= []
y= []
x.append(math.radians(0)); y.append(-0.6)
x.append(math.radians(5)); y.append(-1.1)
x.append(math.radians(10)); y.append(-1.5)
x.append(math.radians(15)); y.append(-1.8)
x.append(math.radians(20)); y.append(-2.2)
x.append(math.radians(25)); y.append(-2.6)
x.append(math.radians(30)); y.append(-3.0)
cpMarq1AguaArribaZonaAFi0= scipy.interpolate.interp1d(x,y)

'''
Presión exterior hacia arriba en zona A de marquesina a 1 agua
con factor de obstrucción 1 en función de la pendiente
según la tabla D.10.
'''

x= []
y= []
x.append(math.radians(0)); y.append(-1.5)
x.append(math.radians(5)); y.append(-1.6)
x.append(math.radians(10)); y.append(-2.1)
x.append(math.radians(15)); y.append(-1.6)
x.append(math.radians(20)); y.append(-1.6)
x.append(math.radians(25)); y.append(-1.5)
x.append(math.radians(30)); y.append(-1.5)
cpMarq1AguaArribaZonaAFi1= scipy.interpolate.interp1d(x,y)

'''
Presión exterior hacia arriba en zona A de marquesina a 1 agua
con factor de obstrucción cualquiera en función de la pendiente
según la tabla D.10.
'''
def cpMarq1AguaArribaZonaA(alpha, fi):
    v0= cpMarq1AguaArribaZonaAFi0(alpha)
    v1= cpMarq1AguaArribaZonaAFi1(alpha)
    return fi*(v1-v0)+v0
