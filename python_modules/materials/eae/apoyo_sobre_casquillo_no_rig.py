# -*- coding: utf-8 -*-
from __future__ import division

# Valor máximo admisible de la reacción a transmitir por
# un casquillo no rigidizado. Norma EAE, artículo
# 61.5, pág 322. Ver también libro metálicas UNED apartado
# 25.2.2

import math

def cargaUlt1(tf,r,tw,fy):
    '''
    Devuelve la resistencia de la viga a aplastamiento local
    tf: Espesor del ala de la viga.
    tw: Espesor del alma de la viga.
    r: Radio del acuerdo ala-alma.
    fy: Límite elástico del acero.
    '''
    return 2.5*(tf+r)*tw*fy
  

def cargaUlt2(b,a,fu,betaW,gammaM2):
    '''
    Devuelve la resistencia del cordón de soldadura
    b: Longitud del casquillo medida de dirección perpendicular a la viga.
    a: Espesor de garganta del cordón de soldadura.
    fu: Resistencia a tracción del acero (table 59.8.2 page 304 EAE).
    betaW: Coeficiente de correlación (tabla 59.8.2 pág 304 EAE).
    gammaM2: Partial safety factor for steel (article 15.3 page 34 EAE).
    '''
    return b*a*fu/betaW/math.sqrt(2.0)/gammaM2
  

def cargaUlt3(b,t,fy):
    '''
    Devuelve la resistencia a cortante del ala del angular
    b: Longitud del casquillo medida de dirección perpendicular a la viga.
    t: Espesor del ala del angular.
    fy: Límite elástico del acero.
    '''
    return b*t*fy/math.sqrt(3.0)
  

def cargaUlt(tf,tw,r,b,a,fy,fu,betaW,gammaM2):
    '''
    Devuelve la resistencia última del apoyo sobre casquillo de angular
    b: Longitud del casquillo medida de dirección perpendicular a la viga.
    t: Espesor del ala del angular.
    fy: Límite elástico del acero.
    '''
    return min(cargaUlt1(tf,r,tw,fy),min(cargaUlt2(b,a,fu,betaW,gammaM2),cargaUlt3(b,t,fy)))
  
