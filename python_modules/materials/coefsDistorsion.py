# -*- coding: utf-8 -*-
from __future__ import division

# Coeficientes de distorsión para diferentes tipos de secciones
# Ver "Cálculo de estructuras por el método de los elementos finitos"
#  de Eugenio Oñate página 122.}

alphaSeccRectangular=5/6
alphaSeccCircular=6/7
def alphaSeccAnilloCircular(R,r):
    c=r/R
    K=c/(1+c**2)
    return 6/(7+20*(K)**2)
