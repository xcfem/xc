# -*- coding: utf-8 -*-

import scipy.interpolate
import math

'''
Coeficientes de presión exterior para una cubierta plana
según la tabla D.4 del CTE. En esta norma se consideran
cubiertas planas aquéllas que tienen una pendiente inferior
al cinco por ciento.
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
área mayor o igual a 10 metros cuadrados según la tabla D.4.
'''

x= []
y= []
x.append(0); y.append(-1.8)
x.append(0.025); y.append(-1.6)
x.append(0.05); y.append(-1.4)
x.append(0.1); y.append(-1.2)
cpPlanaZonaFVTrsvAGrande= scipy.interpolate.interp1d(x,y)


'''
Coeficiente de presión exterior (positiva hacia abajo) en zona F de la cubierta 
para viento sensiblemente transversal (-45<theta<45) y
área menor o igual a 1 metro cuadrado según la tabla D.4.
'''

x= []
y= []
x.append(0); y.append(-2.5)
x.append(0.025); y.append(-2.2)
x.append(0.05); y.append(-2.0)
x.append(0.1); y.append(-1.8)
cpPlanaZonaFVTrsvAPeq= scipy.interpolate.interp1d(x,y)

'''
Coeficiente de presión exterior (positiva hacia abajo) en zona F de la cubierta 
para viento sensiblemente transversal (-45<theta<45) según la tabla D.4.
'''
def cpPlanaZonaFVTrsv(A, hp, h):
    cpPeq= cpPlanaZonaFVTrsvAPeq(hp/h)
    cpGrande= cpPlanaZonaFVTrsvAGrande(hp/h)
    return ifte(A<1,cpPeq,ifte(A>10,cpGrande,interpolaCoefsPresion(A,cpPeq,cpGrande)))

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
área mayor o igual a 10 metros cuadrados según la tabla D.4.
'''

x= []
y= []
x.append(0); y.append(-1.2)
x.append(0.025); y.append(-1.2)
x.append(0.05); y.append(-0.9)
x.append(0.1); y.append(-0.8)
cpPlanaZonaGVTrsvAGrande= scipy.interpolate.interp1d(x,y)

'''
Coeficiente de presión exterior (positiva hacia abajo) en zona G de la cubierta 
para viento sensiblemente transversal (-45<theta<45) y
área menor o igual a 1 metro cuadrado según la tabla D.4.
'''

x= []
y= []
x.append(0); y.append(-2.0)
x.append(0.025); y.append(-1.8)
x.append(0.05); y.append(-1.6)
x.append(0.1); y.append(-1.4)
cpPlanaZonaGVTrsvAPeq= scipy.interpolate.interp1d(x,y)

'''
Coeficiente de presión exterior (positiva hacia abajo) en zona G de la cubierta 
para viento sensiblemente transversal (-45<theta<45) según la tabla D.4.
'''
def cpPlanaZonaGVTrsv(A, hp, h):
    cpPeq= cpPlanaZonaGVTrsvAPeq(hp/h)
    cpGrande= cpPlanaZonaGVTrsvAGrande(hp/h)
    return ifte(A<1,cpPeq,ifte(A>10,cpGrande,interpolaCoefsPresion(A,cpPeq,cpGrande)))

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
área mayor o igual a 10 metros cuadrados según la tabla D.4.
'''

x= []
y= []
x.append(0); y.append(-0.7)
x.append(0.025); y.append(-0.7)
x.append(0.05); y.append(-0.7)
x.append(0.1); y.append(-0.7)
cpPlanaZonaHVTrsvAGrande= scipy.interpolate.interp1d(x,y)

'''
Coeficiente de presión exterior (positiva hacia abajo) en zona H de la cubierta 
para viento sensiblemente transversal (-45<theta<45) y
área menor o igual a 1 metro cuadrado según la tabla D.4.
'''

x= []
y= []
x.append(0); y.append(-1.2)
x.append(0.025); y.append(-1.2)
x.append(0.05); y.append(-1.2)
x.append(0.1); y.append(-1.2)
cpPlanaZonaHVTrsvAPeq= scipy.interpolate.interp1d(x,y)

'''
Coeficiente de presión exterior (positiva hacia abajo) en zona H de la cubierta 
para viento sensiblemente transversal (-45<theta<45) según la tabla D.4.
'''
def cpPlanaZonaHVTrsv(A, hp, h):
    cpPeq= cpPlanaZonaHVTrsvAPeq(hp/h)
    cpGrande= cpPlanaZonaHVTrsvAGrande(hp/h)
    return ifte(A<1,cpPeq,ifte(A>10,cpGrande,interpolaCoefsPresion(A,cpPeq,cpGrande)))

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
área mayor o igual a 10 metros cuadrados según la tabla D.4.
'''

x= []
y= []
x.append(0); y.append(0.2)
x.append(0.025); y.append(0.2)
x.append(0.05); y.append(0.2)
x.append(0.1); y.append(0.2)
cpPlanaZonaIVTrsvAGrande= scipy.interpolate.interp1d(x,y)

'''
Coeficiente de presión exterior (positiva hacia abajo) en zona I de la cubierta 
para viento sensiblemente transversal (-45<theta<45) y
área menor o igual a 1 metro cuadrado según la tabla D.4.
'''

x= []
y= []
x.append(0); y.append(-0.2)
x.append(0.025); y.append(-0.2)
x.append(0.05); y.append(-0.2)
x.append(0.1); y.append(-0.2)
cpPlanaZonaIVTrsvAPeq= scipy.interpolate.interp1d(x,y)

'''
Coeficiente de presión exterior (positiva hacia abajo) en zona I de la cubierta 
para viento sensiblemente transversal (-45<theta<45) según la tabla D.4.
'''
def cpPlanaZonaIVTrsv(A, hp, h):
    cpPeq= cpPlanaZonaIVTrsvAPeq(hp/h)
    cpGrande= cpPlanaZonaIVTrsvAGrande(hp/h)
    return ifte(A<1,cpPeq,ifte(A>10,cpGrande,interpolaCoefsPresion(A,cpPeq,cpGrande)))


#  FFFFFFF
#  F
#  F
#  FFFF
#  F
#  F
#  F



'''
Coeficiente de presión exterior (positiva hacia abajo) en zona F de la cubierta 
para viento sensiblemente longitudinal (45<theta<135) según la tabla D.4.
'''
def cpPlanaZonaFVLong(A, hp, h):
    cpPeq= cpPlanaZonaFVTrsvAPeq(hp/h)
    cpGrande= cpPlanaZonaFVTrsvAGrande(hp/h)
    return ifte(A<1,cpPeq,ifte(A>10,cpGrande,interpolaCoefsPresion(A,cpPeq,cpGrande)))

#  GGGGGGG
#  G
#  G
#  G   GGG
#  G     G
#  G     G
#  GGGGGGG


'''
Coeficiente de presión exterior (positiva hacia abajo) en zona G de la cubierta 
para viento sensiblemente longitudinal (45<theta<135) según la tabla D.4.
'''
def cpPlanaZonaGVLong(A, hp, h):
    cpPeq= cpPlanaZonaGVTrsvAPeq(hp/h)
    cpGrande= cpPlanaZonaGVTrsvAGrande(hp/h)
    return ifte(A<1,cpPeq,ifte(A>10,cpGrande,interpolaCoefsPresion(A,cpPeq,cpGrande)))

#  H     H
#  H     H
#  H     H
#  HHHHHHH
#  H     H
#  H     H
#  H     H



'''
Coeficiente de presión exterior (positiva hacia abajo) en zona H de la cubierta 
para viento sensiblemente longitudinal (45<theta<135) según la tabla D.4.
'''
def cpPlanaZonaHVLong(A, hp, h):
    cpPeq= cpPlanaZonaHVTrsvAPeq(hp/h)
    cpGrande= cpPlanaZonaHVTrsvAGrande(hp/h)
    return ifte(A<1,cpPeq,ifte(A>10,cpGrande,interpolaCoefsPresion(A,cpPeq,cpGrande)))

#  IIIII
#    I
#    I
#    I
#    I
#    I
#  IIIII



'''
Coeficiente de presión exterior (positiva hacia abajo) en zona I de la cubierta 
para viento sensiblemente longitudinal (45<theta<135) según la tabla D.4.
'''
def cpPlanaZonaIVLong(A, hp, h):
    cpPeq= cpPlanaZonaIVTrsvAPeq(hp/h)
    cpGrande= cpPlanaZonaIVTrsvAGrande(hp/h)
    return ifte(A<1,cpPeq,ifte(A>10,cpGrande,interpolaCoefsPresion(A,cpPeq,cpGrande)))

