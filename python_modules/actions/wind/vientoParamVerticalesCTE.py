# -*- coding: utf-8 -*-

import scipy.interpolate
import math

# Coeficientes de presión exterior en paramentos verticales según la tabla D.3 del CTE.

#    AA
#   A  A
#  A    A
#  A    A
#  AAAAAA
#  A    A
#  A    A

'''
Coeficiente de presión exterior (positiva hacia adentro) en zona A
paramento vertical para viento sensiblemente transversal (-45<theta<45) y
área mayor o igual a 10 metros cuadrados en función de h/d según la tabla D.3.
'''

x= []
y= []
x.append(.25); y.append(-1.2)
x.append(1); y.append(-1.2)
x.append(5); y.append(-1.2)
cpParamVertZonaAVTrsvAGE10= scipy.interpolate.interp1d(x,y)

'''
Coeficiente de presión exterior (positiva hacia adentro) en zona A
paramento vertical para viento sensiblemente transversal (-45<theta<45) y
área igual a 5 metros cuadrados en función de h/d según la tabla D.3.
'''

x= []
y= []
x.append(.25); y.append(-1.3)
x.append(1); y.append(-1.3)
x.append(5); y.append(-1.3)
cpParamVertZonaAVTrsvAEQ5= scipy.interpolate.interp1d(x,y)

'''
Coeficiente de presión exterior (positiva hacia adentro) en zona A
paramento vertical para viento sensiblemente transversal (-45<theta<45) y
área igual a 2 metros cuadrados en función de h/d según la tabla D.3.
'''

x= []
y= []
x.append(.25); y.append(-1.3)
x.append(1); y.append(-1.3)
x.append(5); y.append(-1.3)
x.append(1e6); y.append(-1.3)
cpParamVertZonaAVTrsvAEQ2= scipy.interpolate.interp1d(x,y)

'''
Coeficiente de presión exterior (positiva hacia adentro) en zona A
paramento vertical para viento sensiblemente transversal (-45<theta<45) y
área menor o igual a 1 metro cuadrado en función de h/d según la tabla D.3.
'''

x= []
y= []
x.append(0); y.append(-1.4)
x.append(.25); y.append(-1.4)
x.append(1); y.append(-1.4)
x.append(5); y.append(-1.4)
x.append(1e6); y.append(-1.4)
cpParamVertZonaAVTrsvALE1= scipy.interpolate.interp1d(x,y)

'''
Coeficiente de presión exterior (positiva hacia abajo) en 
paramento vertical zona A para viento sensiblemente 
transversal (-45<theta<45) según la tabla D.3.
'''
def cpParamVertZonaAVTrsv(A, h, d):
    ratio= h/d
    cpLE1= cpParamVertZonaAVTrsvALE1(ratio)
    cpEQ2= cpParamVertZonaAVTrsvAEQ2(ratio)
    cpEQ5= cpParamVertZonaAVTrsvAEQ5(ratio)
    cpGE10= cpParamVertZonaAVTrsvAGE10(ratio)


    x= []
    y= []
    x.append(0); y.append(cpLE1)
    x.append(1); y.append(cpLE1)
    x.append(2); y.append(cpEQ2)
    x.append(5); y.append(cpEQ5)
    x.append(10); y.append(cpGE10)
    x.append(100); y.append(cpGE10)
    interpolacion= scipy.interpolate.interp1d(x,y)
    return interpolacion(A)

#  BBBBB
#  B    B
#  B   B
#  BBBB
#  B    B
#  B   B 
#  BBBB

'''
Coeficiente de presión exterior (positiva hacia adentro) en zona B
paramento vertical para viento sensiblemente transversal (-45<theta<45) y
área mayor o igual a 10 metros cuadrados en función de h/d según la tabla D.3.
'''

x= []
y= []
x.append(.25); y.append(-0.8)
x.append(1); y.append(-0.8)
x.append(5); y.append(-0.8)
cpParamVertZonaBVTrsvAGE10= scipy.interpolate.interp1d(x,y)

'''
Coeficiente de presión exterior (positiva hacia adentro) en zona B
paramento vertical para viento sensiblemente transversal (-45<theta<45) y
área igual a 5 metros cuadrados en función de h/d según la tabla D.3.
'''

x= []
y= []
x.append(.25); y.append(-0.9)
x.append(1); y.append(-0.9)
x.append(5); y.append(-0.9)
cpParamVertZonaBVTrsvAEQ5= scipy.interpolate.interp1d(x,y)

'''
Coeficiente de presión exterior (positiva hacia adentro) en zona B
paramento vertical para viento sensiblemente transversal (-45<theta<45) y
área igual a 2 metros cuadrados en función de h/d según la tabla D.3.
'''

x= []
y= []
x.append(.25); y.append(-1)
x.append(1); y.append(-1)
x.append(5); y.append(-1)
cpParamVertZonaBVTrsvAEQ2= scipy.interpolate.interp1d(x,y)

'''
Coeficiente de presión exterior (positiva hacia adentro) en zona B
paramento vertical para viento sensiblemente transversal (-45<theta<45) y
área menor o igual a 1 metro cuadrado en función de h/d según la tabla D.3.
'''

x= []
y= []
x.append(.25); y.append(-1.1)
x.append(1); y.append(-1.1)
x.append(5); y.append(-1.1)
cpParamVertZonaBVTrsvALE1= scipy.interpolate.interp1d(x,y)

'''
Coeficiente de presión exterior (positiva hacia abajo) en 
paramento vertical zona B para viento sensiblemente 
transversal (-45<theta<45) según la tabla D.3.
'''
def cpParamVertZonaBVTrsv(A, h, d):
    ratio= h/d
    cpLE1= cpParamVertZonaBVTrsvALE1(ratio)
    cpEQ2= cpParamVertZonaBVTrsvAEQ2(ratio)
    cpEQ5= cpParamVertZonaBVTrsvAEQ5(ratio)
    cpGE10= cpParamVertZonaBVTrsvAGE10(ratio)

    x= []
    y= []
    x.append(0); y.append(cpLE1)
    x.append(1); y.append(cpLE1)
    x.append(2); y.append(cpEQ2)
    x.append(5); y.append(cpEQ5)
    x.append(10); y.append(cpGE10)
    x.append(100); y.append(cpGE10)
    interpolacion= scipy.interpolate.interp1d(x,y)
    return interpolacion(A)

#   CCC
#  C   C
#  C   
#  C
#  C    
#  C   C 
#   CCC

'''
Coeficiente de presión exterior (positiva hacia adentro) en zona C
paramento vertical para viento sensiblemente transversal (-45<theta<45) y
área mayor o igual a 10 metros cuadrados en función de h/d según la tabla D.3.
'''

x= []
y= []
x.append(.25); y.append(-0.5)
x.append(1); y.append(-0.5)
x.append(5); y.append(-0.5)
cpParamVertZonaCVTrsvAGE10= scipy.interpolate.interp1d(x,y)

'''
Coeficiente de presión exterior (positiva hacia adentro) en zona C
paramento vertical para viento sensiblemente transversal (-45<theta<45) y
área igual a 5 metros cuadrados en función de h/d según la tabla D.3.
'''

x= []
y= []
x.append(.25); y.append(-0.5)
x.append(1); y.append(-0.5)
x.append(5); y.append(-0.5)
cpParamVertZonaCVTrsvAEQ5= scipy.interpolate.interp1d(x,y)

'''
Coeficiente de presión exterior (positiva hacia adentro) en zona C
paramento vertical para viento sensiblemente transversal (-45<theta<45) y
área igual a 2 metros cuadrados en función de h/d según la tabla D.3.
'''

x= []
y= []
x.append(.25); y.append(-0.5)
x.append(1); y.append(-0.5)
x.append(5); y.append(-0.5)
cpParamVertZonaCVTrsvAEQ2= scipy.interpolate.interp1d(x,y)

'''
Coeficiente de presión exterior (positiva hacia adentro) en zona C
paramento vertical para viento sensiblemente transversal (-45<theta<45) y
área menor o igual a 1 metro cuadrado en función de h/d según la tabla D.3.
'''

x= []
y= []
x.append(.25); y.append(-0.5)
x.append(1); y.append(-0.5)
x.append(5); y.append(-0.5)
cpParamVertZonaCVTrsvALE1= scipy.interpolate.interp1d(x,y)

'''
Coeficiente de presión exterior (positiva hacia abajo) en 
paramento vertical zona C para viento sensiblemente 
transversal (-45<theta<45) según la tabla D.3.
'''
def cpParamVertZonaCVTrsv(A, h, d):
    ratio= h/d

    cpLE1= cpParamVertZonaCVTrsvALE1(ratio)
    cpEQ2= cpParamVertZonaCVTrsvAEQ2(ratio)
    cpEQ5= cpParamVertZonaCVTrsvAEQ5(ratio)
    cpGE10= cpParamVertZonaCVTrsvAGE10(ratio)


    x= []
    y= []
    x.append(0); y.append(cpLE1)
    x.append(1); y.append(cpLE1)
    x.append(2); y.append(cpEQ2)
    x.append(5); y.append(cpEQ5)
    x.append(10); y.append(cpGE10)
    x.append(100); y.append(cpGE10)
    interpolacion= scipy.interpolate.interp1d(x,y)
    return interpolacion(A)

#  DDDD
#  D   D
#  D   D
#  D   D
#  D   D
#  D   D 
#  DDDD

'''
Coeficiente de presión exterior (positiva hacia adentro) en zona D
paramento vertical para viento sensiblemente transversal (-45<theta<45) y
área mayor o igual a 10 metros cuadrados en función de h/d según la tabla D.3.
'''

x= []
y= []
x.append(0); y.append(0.7)
x.append(.25); y.append(0.7)
x.append(1); y.append(0.8)
x.append(5); y.append(0.8)
cpParamVertZonaDVTrsvAGE10= scipy.interpolate.interp1d(x,y)

'''
Coeficiente de presión exterior (positiva hacia adentro) en zona D
paramento vertical para viento sensiblemente transversal (-45<theta<45) y
área igual a 5 metros cuadrados en función de h/d según la tabla D.3.
'''

x= []
y= []
x.append(0); y.append(0.8)
x.append(.25); y.append(0.8)
x.append(1); y.append(0.9)
x.append(5); y.append(0.9)
cpParamVertZonaDVTrsvAEQ5= scipy.interpolate.interp1d(x,y)

'''
Coeficiente de presión exterior (positiva hacia adentro) en zona D
paramento vertical para viento sensiblemente transversal (-45<theta<45) y
área igual a 2 metros cuadrados en función de h/d según la tabla D.3.
'''

x= []
y= []
x.append(0); y.append(0.7)
x.append(.25); y.append(0.7)
x.append(1); y.append(0.9)
x.append(5); y.append(0.9)
cpParamVertZonaDVTrsvAEQ2= scipy.interpolate.interp1d(x,y)

'''
Coeficiente de presión exterior (positiva hacia adentro) en zona D
paramento vertical para viento sensiblemente transversal (-45<theta<45) y
área menor o igual a 1 metro cuadrado en función de h/d según la tabla D.3.
'''

x= []
y= []
x.append(.25); y.append(1)
x.append(1); y.append(1)
x.append(5); y.append(1)
cpParamVertZonaDVTrsvALE1= scipy.interpolate.interp1d(x,y)

'''
Coeficiente de presión exterior (positiva hacia abajo) en 
paramento vertical zona D para viento sensiblemente 
transversal (-45<theta<45) según la tabla D.3.
'''
def cpParamVertZonaDVTrsv(A, h, d):
    ratio= h/d

    cpLE1= cpParamVertZonaDVTrsvALE1(ratio)
    cpEQ2= cpParamVertZonaDVTrsvAEQ2(ratio)
    cpEQ5= cpParamVertZonaDVTrsvAEQ5(ratio)
    cpGE10= cpParamVertZonaDVTrsvAGE10(ratio)


    x= []
    y= []
    x.append(0); y.append(cpLE1)
    x.append(1); y.append(cpLE1)
    x.append(2); y.append(cpEQ2)
    x.append(5); y.append(cpEQ5)
    x.append(10); y.append(cpGE10)
    x.append(100); y.append(cpGE10)
    interpolacion= scipy.interpolate.interp1d(x,y)
    return interpolacion(A)

#  EEEEE
#  E   
#  E   
#  EEE
#  E   
#  E    
#  EEEEE

'''
Coeficiente de presión exterior (positiva hacia adentro) en zona E
paramento vertical para viento sensiblemente transversal (-45<theta<45) y
área mayor o igual a 10 metros cuadrados en función de h/d según la tabla D.3.
'''

x= []
y= []
x.append(0); y.append(-0.3)
x.append(.25); y.append(-0.3)
x.append(1); y.append(-0.5)
x.append(5); y.append(-0.7)
cpParamVertZonaEVTrsvAGE10= scipy.interpolate.interp1d(x,y)

'''
Coeficiente de presión exterior (positiva hacia adentro) en zona E
paramento vertical para viento sensiblemente transversal (-45<theta<45) y
área igual a 5 metros cuadrados en función de h/d según la tabla D.3.
'''

x= []
y= []
x.append(.25); y.append(-0.3)
x.append(0); y.append(-0.3)
x.append(1); y.append(-0.5)
x.append(5); y.append(-0.7)
cpParamVertZonaEVTrsvAEQ5= scipy.interpolate.interp1d(x,y)

'''Coeficiente de presión exterior (positiva hacia adentro) en zona E
paramento vertical para viento sensiblemente transversal (-45<theta<45) y
área igual a 2 metros cuadrados en función de h/d según la tabla D.3.
'''

x= []
y= []
x.append(0); y.append(-0.3)
x.append(.25); y.append(-0.3)
x.append(1); y.append(-0.5)
x.append(5); y.append(-0.7)
cpParamVertZonaEVTrsvAEQ2= scipy.interpolate.interp1d(x,y)

'''
Coeficiente de presión exterior (positiva hacia adentro) en zona E
paramento vertical para viento sensiblemente transversal (-45<theta<45) y
área menor o igual a 1 metro cuadrado en función de h/d según la tabla D.3.
'''

x= []
y= []
x.append(0); y.append(-0.3)
x.append(.25); y.append(-0.3)
x.append(1); y.append(-0.5)
x.append(5); y.append(-0.7)
cpParamVertZonaEVTrsvALE1= scipy.interpolate.interp1d(x,y)

'''
Coeficiente de presión exterior (positiva hacia abajo) en 
paramento vertical zona E para viento sensiblemente 
transversal (-45<theta<45) según la tabla D.3.
'''
def cpParamVertZonaEVTrsv(A, h, d):
    ratio= h/d

    cpLE1= cpParamVertZonaEVTrsvALE1(ratio)
    cpEQ2= cpParamVertZonaEVTrsvAEQ2(ratio)
    cpEQ5= cpParamVertZonaEVTrsvAEQ5(ratio)
    cpGE10= cpParamVertZonaEVTrsvAGE10(ratio)

    x= []
    y= []
    x.append(0); y.append(cpLE1)
    x.append(1); y.append(cpLE1)
    x.append(2); y.append(cpEQ2)
    x.append(5); y.append(cpEQ5)
    x.append(10); y.append(cpGE10)
    x.append(100); y.append(cpGE10)
    interpolacion= scipy.interpolate.interp1d(x,y)
    return interpolacion(A)

