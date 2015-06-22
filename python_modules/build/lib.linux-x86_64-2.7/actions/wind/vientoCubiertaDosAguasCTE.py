# -*- coding: utf-8 -*-

import scipy.interpolate
import math
import vientoCTE

'''
Coeficientes de presión exterior para una cubierta a dos aguas
según la tabla D.6 del CTE.
'''

#  FFFFFFF
#  F
#  F
#  FFFF
#  F
#  F
#  F

'''
Coeficiente de presión exterior (positiva hacia abajo) en zona F de la cubierta 
para viento sensiblemente transversal (-45<theta<45) y
área mayor o igual a 10 metros cuadrados según la tabla D.6.
'''
x= []
y= []
x.append(math.radians(-45)); y.append(-0.6)
x.append(math.radians(-30)); y.append(-1.1)
x.append(math.radians(-15)); y.append(-2.5)
x.append(math.radians(-5)); y.append(-2.3)
x.append(math.radians(5)); y.append(-1.7)
x.append(math.radians(15)); y.append(-0.9)
x.append(math.radians(30)); y.append(-0.5)
x.append(math.radians(45)); y.append(-0.0)
x.append(math.radians(60)); y.append(0.7)
x.append(math.radians(75)); y.append(0.8)

cpDosAguasZonaFVTrsvAGrande= scipy.interpolate.interp1d(x,y)


'''
Coeficiente de presión exterior (positiva hacia abajo) en zona F de la cubierta 
para viento sensiblemente transversal (-45<theta<45) y
área menor o igual a 1 metro cuadrado según la tabla D.6.
'''

x= []
y= []
x.append(math.radians(-45)); y.append(-0.6)
x.append(math.radians(-30)); y.append(-2)
x.append(math.radians(-15)); y.append(-2.8)
x.append(math.radians(-5)); y.append(-2.5)
x.append(math.radians(5)); y.append(-2.5)
x.append(math.radians(15)); y.append(-2)
x.append(math.radians(30)); y.append(-1.5)
x.append(math.radians(45)); y.append(-0.0)
x.append(math.radians(60)); y.append(0.7)
x.append(math.radians(75)); y.append(0.8)
cpDosAguasZonaFVTrsvAPeq= scipy.interpolate.interp1d(x,y)

def ifte(cond,v1,v2):
  if(cond):
    return v1
  else:
    return v2

'''
Coeficiente de presión exterior (positiva hacia abajo) en zona F de la cubierta 
para viento sensiblemente transversal (-45<theta<45) según la tabla D.6.
'''
def cpDosAguasZonaFVTrsv(A, alpha):
    cpPeq= cpDosAguasZonaFVTrsvAPeq(alpha)
    cpGrande= cpDosAguasZonaFVTrsvAGrande(alpha)
    return ifte(A<1,cpPeq,ifte(A>10,cpGrande,vientoCTE.interpolaCoefsPresion(A,cpPeq,cpGrande)))

#  GGGGGGG
#  G
#  G
#  G   GGG
#  G     G
#  G     G
#  GGGGGGG

'''
Coeficiente de presión exterior (positiva hacia abajo) en zona G de la cubierta 
para viento sensiblemente transversal (-45<theta<45) y
área mayor o igual a 10 metros cuadrados según la tabla D.6.
'''

x= []
y= []
x.append(math.radians(-45)); y.append(-0.6)
x.append(math.radians(-30)); y.append(-0.8)
x.append(math.radians(-15)); y.append(-1.3)
x.append(math.radians(-5)); y.append(-1.2)
x.append(math.radians(5)); y.append(-1.2)
x.append(math.radians(15)); y.append(-0.8)
x.append(math.radians(30)); y.append(-0.5)
x.append(math.radians(45)); y.append(-0.0)
x.append(math.radians(60)); y.append(0.7)
x.append(math.radians(75)); y.append(0.8)
cpDosAguasZonaGVTrsvAGrande= scipy.interpolate.interp1d(x,y)

'''
Coeficiente de presión exterior (positiva hacia abajo) en zona G de la cubierta 
para viento sensiblemente transversal (-45<theta<45) y
área menor o igual a 1 metro cuadrado según la tabla D.6.
'''

x= []
y= []
x.append(math.radians(-45)); y.append(-0.6)
x.append(math.radians(-30)); y.append(-1.5)
x.append(math.radians(-15)); y.append(-2)
x.append(math.radians(-5)); y.append(-2)
x.append(math.radians(5)); y.append(-2)
x.append(math.radians(15)); y.append(-1.5)
x.append(math.radians(30)); y.append(-1.5)
x.append(math.radians(45)); y.append(-0.0)
x.append(math.radians(60)); y.append(0.7)
x.append(math.radians(75)); y.append(0.8)
cpDosAguasZonaGVTrsvAPeq= scipy.interpolate.interp1d(x,y)

'''
Coeficiente de presión exterior (positiva hacia abajo) en zona G de la cubierta 
para viento sensiblemente transversal (-45<theta<45) según la tabla D.6.
'''
def cpDosAguasZonaGVTrsv(A, alpha):
    cpPeq= cpDosAguasZonaGVTrsvAPeq(alpha)
    cpGrande= cpDosAguasZonaGVTrsvAGrande(alpha)
    return ifte(A<1,cpPeq,ifte(A>10,cpGrande,vientoCTE.interpolaCoefsPresion(A,cpPeq,cpGrande)))

#  H     H
#  H     H
#  H     H
#  HHHHHHH
#  H     H
#  H     H
#  H     H

'''
Coeficiente de presión exterior (positiva hacia abajo) en zona H de la cubierta 
para viento sensiblemente transversal (-45<theta<45) y
área mayor o igual a 10 metros cuadrados según la tabla D.6.
'''

x= []
y= []
x.append(math.radians(-45)); y.append(-0.8)
x.append(math.radians(-30)); y.append(-0.8)
x.append(math.radians(-15)); y.append(-0.9)
x.append(math.radians(-5)); y.append(-0.8)
x.append(math.radians(5)); y.append(-0.6)
x.append(math.radians(15)); y.append(-0.3)
x.append(math.radians(30)); y.append(-0.2)
x.append(math.radians(45)); y.append(-0.0)
x.append(math.radians(60)); y.append(0.7)
x.append(math.radians(75)); y.append(0.8)
cpDosAguasZonaHVTrsvAGrande= scipy.interpolate.interp1d(x,y)

'''
Coeficiente de presión exterior (positiva hacia abajo) en zona H de la cubierta 
para viento sensiblemente transversal (-45<theta<45) y
área menor o igual a 1 metro cuadrado según la tabla D.6.
'''

x= []
y= []
x.append(math.radians(-45)); y.append(-0.8)
x.append(math.radians(-30)); y.append(-0.8)
x.append(math.radians(-15)); y.append(-1.2)
x.append(math.radians(-5)); y.append(-1.2)
x.append(math.radians(5)); y.append(-1.2)
x.append(math.radians(15)); y.append(-0.3)
x.append(math.radians(30)); y.append(-0.2)
x.append(math.radians(45)); y.append(-0.0)
x.append(math.radians(60)); y.append(0.7)
x.append(math.radians(75)); y.append(0.8)
cpDosAguasZonaHVTrsvAPeq= scipy.interpolate.interp1d(x,y)

'''
Coeficiente de presión exterior (positiva hacia abajo) en zona H de la cubierta 
para viento sensiblemente transversal (-45<theta<45) según la tabla D.6.
'''
def cpDosAguasZonaHVTrsv(A, alpha):
    cpPeq= cpDosAguasZonaHVTrsvAPeq(alpha)
    cpGrande= cpDosAguasZonaHVTrsvAGrande(alpha)
    return ifte(A<1,cpPeq,ifte(A>10,cpGrande,vientoCTE.interpolaCoefsPresion(A,cpPeq,cpGrande)))

#  IIIII
#    I
#    I
#    I
#    I
#    I
#  IIIII

'''
Coeficiente de presión exterior (positiva hacia abajo) en zona I de la cubierta 
para viento sensiblemente transversal (-45<theta<45) y
área mayor o igual a 10 metros cuadrados según la tabla D.6.
'''

x= []
y= []
x.append(math.radians(-45)); y.append(-0.7)
x.append(math.radians(-30)); y.append(-0.6)
x.append(math.radians(-15)); y.append(-0.5)
x.append(math.radians(-5)); y.append(0.2)
x.append(math.radians(5)); y.append(0.2)
x.append(math.radians(15)); y.append(-0.4)
x.append(math.radians(30)); y.append(-0.4)
x.append(math.radians(45)); y.append(-0.2)
x.append(math.radians(60)); y.append(-0.2)
x.append(math.radians(75)); y.append(-0.2)
cpDosAguasZonaIVTrsvAGrande= scipy.interpolate.interp1d(x,y)

'''
Coeficiente de presión exterior (positiva hacia abajo) en zona I de la cubierta 
para viento sensiblemente transversal (-45<theta<45) y
área menor o igual a 1 metro cuadrado según la tabla D.6.
'''

x= []
y= []
x.append(math.radians(-45)); y.append(-0.7)
x.append(math.radians(-30)); y.append(-0.6)
x.append(math.radians(-15)); y.append(-0.5)
x.append(math.radians(-5)); y.append(0.2)
x.append(math.radians(5)); y.append(0.2)
x.append(math.radians(15)); y.append(-0.4)
x.append(math.radians(30)); y.append(-0.4)
x.append(math.radians(45)); y.append(-0.2)
x.append(math.radians(60)); y.append(-0.2)
x.append(math.radians(75)); y.append(-0.2)
cpDosAguasZonaIVTrsvAPeq= scipy.interpolate.interp1d(x,y)

'''
Coeficiente de presión exterior (positiva hacia abajo) en zona I de la cubierta 
para viento sensiblemente transversal (-45<theta<45) según la tabla D.6.
'''
def cpDosAguasZonaIVTrsv(A, alpha):
    cpPeq= cpDosAguasZonaIVTrsvAPeq(alpha)
    cpGrande= cpDosAguasZonaIVTrsvAGrande(alpha)
    return ifte(A<1,cpPeq,ifte(A>10,cpGrande,vientoCTE.interpolaCoefsPresion(A,cpPeq,cpGrande)))


#    JJJJ
#      J
#      J
#      J
#      J
#   J  J
#    JJ

'''
Coeficiente de presión exterior (positiva hacia abajo) en zona J de la cubierta 
para viento sensiblemente transversal (-45<theta<45) y
área mayor o igual a 10 metros cuadrados según la tabla D.6.
'''

x= []
y= []
x.append(math.radians(-45)); y.append(-1)
x.append(math.radians(-30)); y.append(-0.8)
x.append(math.radians(-15)); y.append(-0.7)
x.append(math.radians(-5)); y.append(0.2)
x.append(math.radians(5)); y.append(0.2)
x.append(math.radians(15)); y.append(-1)
x.append(math.radians(30)); y.append(-0.5)
x.append(math.radians(45)); y.append(-0.3)
x.append(math.radians(60)); y.append(-0.3)
x.append(math.radians(75)); y.append(-0.3)
cpDosAguasZonaJVTrsvAGrande= scipy.interpolate.interp1d(x,y)

'''
Coeficiente de presión exterior (positiva hacia abajo) en zona J de la cubierta 
para viento sensiblemente transversal (-45<theta<45) y
área menor o igual a 1 metro cuadrado según la tabla D.6.
'''

x= []
y= []
x.append(math.radians(-45)); y.append(-1.5)
x.append(math.radians(-30)); y.append(-1.4)
x.append(math.radians(-15)); y.append(-1.2)
x.append(math.radians(-5)); y.append(0.2)
x.append(math.radians(5)); y.append(0.2)
x.append(math.radians(15)); y.append(-1.5)
x.append(math.radians(30)); y.append(-0.5)
x.append(math.radians(45)); y.append(-0.3)
x.append(math.radians(60)); y.append(-0.3)
x.append(math.radians(75)); y.append(-0.3)
cpDosAguasZonaJVTrsvAPeq= scipy.interpolate.interp1d(x,y)

'''
Coeficiente de presión exterior (positiva hacia abajo) en zona J de la cubierta 
para viento sensiblemente transversal (-45<theta<45) según la tabla D.6.
'''
def cpDosAguasZonaJVTrsv(A, alpha):
    cpPeq= cpDosAguasZonaJVTrsvAPeq(alpha)
    cpGrande= cpDosAguasZonaJVTrsvAGrande(alpha)
    return ifte(A<1,cpPeq,ifte(A>10,cpGrande,vientoCTE.interpolaCoefsPresion(A,cpPeq,cpGrande)))


#  FFFFFFF
#  F
#  F
#  FFFF
#  F
#  F
#  F

'''
Coeficiente de presión exterior (positiva hacia abajo) en zona F de la cubierta 
para viento sensiblemente longitudinal (45<theta<135) y
área mayor o igual a 10 metros cuadrados según la tabla D.6.
'''

x= []
y= []
x.append(math.radians(-45)); y.append(-1.4)
x.append(math.radians(-30)); y.append(-1.5)
x.append(math.radians(-15)); y.append(-1.9)
x.append(math.radians(-5)); y.append(-1.8)
x.append(math.radians(5)); y.append(-1.6)
x.append(math.radians(15)); y.append(-1.3)
x.append(math.radians(30)); y.append(-1.1)
x.append(math.radians(45)); y.append(-1.1)
x.append(math.radians(60)); y.append(-1.1)
x.append(math.radians(75)); y.append(-1.1)
cpDosAguasZonaFVLongAGrande= scipy.interpolate.interp1d(x,y)


'''
Coeficiente de presión exterior (positiva hacia abajo) en zona F de la cubierta 
para viento sensiblemente longitudinal (45<theta<135) y
área menor o igual a 1 metro cuadrado según la tabla D.6.
'''

x= []
y= []
x.append(math.radians(-45)); y.append(-2.0)
x.append(math.radians(-30)); y.append(-2.1)
x.append(math.radians(-15)); y.append(-2.5)
x.append(math.radians(-5)); y.append(-2.5)
x.append(math.radians(5)); y.append(-2.2)
x.append(math.radians(15)); y.append(-2)
x.append(math.radians(30)); y.append(-1.5)
x.append(math.radians(45)); y.append(-1.5)
x.append(math.radians(60)); y.append(-1.5)
x.append(math.radians(75)); y.append(-1.5)
cpDosAguasZonaFVLongAPeq= scipy.interpolate.interp1d(x,y)

'''
Coeficiente de presión exterior (positiva hacia abajo) en zona F de la cubierta 
para viento sensiblemente longitudinal (45<theta<135) según la tabla D.6.
'''
def cpDosAguasZonaFVLong(A, alpha):
    cpPeq= cpDosAguasZonaFVLongAPeq(alpha)
    cpGrande= cpDosAguasZonaFVLongAGrande(alpha)
    return ifte(A<1,cpPeq,ifte(A>10,cpGrande,vientoCTE.interpolaCoefsPresion(A,cpPeq,cpGrande)))

#  GGGGGGG
#  G
#  G
#  G   GGG
#  G     G
#  G     G
#  GGGGGGG

'''
Coeficiente de presión exterior (positiva hacia abajo) en zona G de la cubierta 
para viento sensiblemente longitudinal (45<theta<135) y
área mayor o igual a 10 metros cuadrados según la tabla D.6.
'''

x= []
y= []
x.append(math.radians(-45)); y.append(-1.2)
x.append(math.radians(-30)); y.append(-1.2)
x.append(math.radians(-15)); y.append(-1.2)
x.append(math.radians(-5)); y.append(-1.2)
x.append(math.radians(5)); y.append(-1.3)
x.append(math.radians(15)); y.append(-1.3)
x.append(math.radians(30)); y.append(-1.4)
x.append(math.radians(45)); y.append(-1.4)
x.append(math.radians(60)); y.append(-1.2)
x.append(math.radians(75)); y.append(-1.2)
cpDosAguasZonaGVLongAGrande= scipy.interpolate.interp1d(x,y)

'''
Coeficiente de presión exterior (positiva hacia abajo) en zona G de la cubierta 
para viento sensiblemente longitudinal (45<theta<135) y
área menor o igual a 1 metro cuadrado según la tabla D.6.
'''

x= []
y= []
x.append(math.radians(-45)); y.append(-2)
x.append(math.radians(-30)); y.append(-2)
x.append(math.radians(-15)); y.append(-2)
x.append(math.radians(-5)); y.append(-2)
x.append(math.radians(5)); y.append(-2)
x.append(math.radians(15)); y.append(-2)
x.append(math.radians(30)); y.append(-2)
x.append(math.radians(45)); y.append(-2)
x.append(math.radians(60)); y.append(-2)
x.append(math.radians(75)); y.append(-2)
cpDosAguasZonaGVLongAPeq= scipy.interpolate.interp1d(x,y)

'''
Coeficiente de presión exterior (positiva hacia abajo) en zona G de la cubierta 
para viento sensiblemente longitudinal (45<theta<135) según la tabla D.6.
'''
def cpDosAguasZonaGVLong(A, alpha):
    cpPeq= cpDosAguasZonaGVLongAPeq(alpha)
    cpGrande= cpDosAguasZonaGVLongAGrande(alpha)
    return ifte(A<1,cpPeq,ifte(A>10,cpGrande,vientoCTE.interpolaCoefsPresion(A,cpPeq,cpGrande)))

#  H     H
#  H     H
#  H     H
#  HHHHHHH
#  H     H
#  H     H
#  H     H

'''
Coeficiente de presión exterior (positiva hacia abajo) en zona H de la cubierta 
para viento sensiblemente longitudinal (45<theta<135) y
área mayor o igual a 10 metros cuadrados según la tabla D.6.
'''

x= []
y= []
x.append(math.radians(-45)); y.append(-1)
x.append(math.radians(-30)); y.append(-1)
x.append(math.radians(-15)); y.append(-0.8)
x.append(math.radians(-5)); y.append(-0.7)
x.append(math.radians(5)); y.append(-0.7)
x.append(math.radians(15)); y.append(-0.6)
x.append(math.radians(30)); y.append(-0.8)
x.append(math.radians(45)); y.append(-0.9)
x.append(math.radians(60)); y.append(-0.8)
x.append(math.radians(75)); y.append(-0.8)
cpDosAguasZonaHVLongAGrande= scipy.interpolate.interp1d(x,y)

'''
Coeficiente de presión exterior (positiva hacia abajo) en zona H de la cubierta 
para viento sensiblemente longitudinal (45<theta<135) y
área menor o igual a 1 metro cuadrado según la tabla D.6.
'''

x= []
y= []
x.append(math.radians(-45)); y.append(-1.3)
x.append(math.radians(-30)); y.append(-1.3)
x.append(math.radians(-15)); y.append(-1.2)
x.append(math.radians(-5)); y.append(-1.2)
x.append(math.radians(5)); y.append(-1.2)
x.append(math.radians(15)); y.append(-1.2)
x.append(math.radians(30)); y.append(-1.2)
x.append(math.radians(45)); y.append(-1.2)
x.append(math.radians(60)); y.append(-1.0)
x.append(math.radians(75)); y.append(-1.0)
cpDosAguasZonaHVLongAPeq= scipy.interpolate.interp1d(x,y)

'''
Coeficiente de presión exterior (positiva hacia abajo) en zona H de la cubierta 
para viento sensiblemente longitudinal (45<theta<135) según la tabla D.6.
'''
def cpDosAguasZonaHVLong(A, alpha):
    cpPeq= cpDosAguasZonaHVLongAPeq(alpha)
    cpGrande= cpDosAguasZonaHVLongAGrande(alpha)
    return ifte(A<1,cpPeq,ifte(A>10,cpGrande,vientoCTE.interpolaCoefsPresion(A,cpPeq,cpGrande)))

#  IIIII
#    I
#    I
#    I
#    I
#    I
#  IIIII

'''
Coeficiente de presión exterior (positiva hacia abajo) en zona I de la cubierta 
para viento sensiblemente longitudinal (45<theta<135) y
área mayor o igual a 10 metros cuadrados según la tabla D.6.
'''

x= []
y= []
x.append(math.radians(-45)); y.append(-0.9)
x.append(math.radians(-30)); y.append(-0.9)
x.append(math.radians(-15)); y.append(-0.8)
x.append(math.radians(-5)); y.append(-0.6)
x.append(math.radians(5)); y.append(-0.6)
x.append(math.radians(15)); y.append(-0.5)
x.append(math.radians(30)); y.append(-0.5)
x.append(math.radians(45)); y.append(-0.5)
x.append(math.radians(60)); y.append(-0.5)
x.append(math.radians(75)); y.append(-0.5)
cpDosAguasZonaIVLongAGrande= scipy.interpolate.interp1d(x,y)

'''
Coeficiente de presión exterior (positiva hacia abajo) en zona I de la cubierta 
para viento sensiblemente longitudinal (45<theta<135) y
área menor o igual a 1 metro cuadrado según la tabla D.6.
'''

x= []
y= []
x.append(math.radians(-45)); y.append(-1.2)
x.append(math.radians(-30)); y.append(-1.2)
x.append(math.radians(-15)); y.append(-1.2)
x.append(math.radians(-5)); y.append(-1.2)
x.append(math.radians(5)); y.append(-0.6)
x.append(math.radians(15)); y.append(-0.5)
x.append(math.radians(30)); y.append(-0.5)
x.append(math.radians(45)); y.append(-0.5)
x.append(math.radians(60)); y.append(-0.5)
x.append(math.radians(75)); y.append(-0.5)
cpDosAguasZonaIVLongAPeq= scipy.interpolate.interp1d(x,y)

'''
Coeficiente de presión exterior (positiva hacia abajo) en zona I de la cubierta 
para viento sensiblemente longitudinal (45<theta<135) según la tabla D.6.
'''
def cpDosAguasZonaIVLong(A, alpha):
    cpPeq= cpDosAguasZonaIVLongAPeq(alpha)
    cpGrande= cpDosAguasZonaIVLongAGrande(alpha)
    return ifte(A<1,cpPeq,ifte(A>10,cpGrande,vientoCTE.interpolaCoefsPresion(A,cpPeq,cpGrande)))

