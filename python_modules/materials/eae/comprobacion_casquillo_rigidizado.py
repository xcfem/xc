# -*- coding: utf-8 -*-

from __future__ import division

import math

# Valor máximo admisible de la reacción a transmitir por
# un casquillo rigidizado (mensula corta). Norma EAE, artículo
# 61.5.1, pág 324. Ver también libro metálicas UNED apartado
# 25.3.3

def widthMax(tChapa,l,H):
    '''
    Devuelve el "width máximo" c del rigidizador (ver figura 61.1.5.b
    pág 325 EAE). Siendo:
    tChapa: Espesor de la chapa sobre la que apoya la carga.
    l: Longitud del cateto horizontal del rigidizador.
    H: Longitud del cateto vertical del rigidizador. 
    '''
    theta=math.atan2(l,H)
    return l*math.cos(theta)+tChapa*math.sin(theta)
  

  
def esbeltezAdim(c,tRig,fy,Es):
    '''
    Devuelve la esbeltez adimensional del rigidizador, siendo:
    c: "width máximo" rigidizador.
    tRig: Espesor del rigidizador.
    fy: Límite elástico del acero.
    Es: Módulo elástico del acero.
    '''
    return 0.805*c/tRig*math.sqrt(fy/Es) 


  
def coefEscuadra(coefLambda):
    '''
    Devuelve la el coeficiente de escuadra del rigidizador, siendo:
    coefLambda: Esbeltez adimensional del rigidizador.
    '''
    return 0.14*(coefLambda)**2-1.07*coefLambda+2.3 

  
def momPlastRig(tRig,c,fy):
    '''
    Devuelve el momento plástico de la sección del rigidizador de depth c,
    siendo:
    tRig: Espesor del rigidizador.
    c: "width máximo" rigidizador.
    fy: Límite elástico del acero.
    '''
    return tRig*c**2*fy/4 

def cargaUltRig(CE,d,MplRd):
    '''
    Devuelve la carga última del casquillo rigidizado debida al colapso del rigidizador, siendo:
    CE: Coeficiente de escuadra.
    d: Brazo de la carga.
    MplRd: Momento plástico de la sección del rigidizador de depth c. 
    '''
    return CE*MplRd/d 

#Soldaduras

 
def cargaUltCord1(a1,l,H,fu,betaW,gammaM2):
    '''
    Devuelve la carga última del casquillo rigidizado debida al agotamiento de los cordones (1)
    (ver figura 61.1.5.b pág 325 EAE) que unen el rigidizador al ala del casquillo, siendo:
    a1: Espesor de garganta de los cordones (1).
    l: Longitud del cateto horizontal del rigidizador.
    H: Longitud del cateto vertical del rigidizador.
    fu: Resistencia a tracción del acero (tabla 59.8.2 pág 304 EAE).
    betaW: Coeficiente de correlación (tabla 59.8.2 pág 304 EAE).
    gammaM2: Partial safety factor for steel (article 15.3 page 34 EAE).
     '''
    theta=math.atan2(l,H)
    return (2.0*a1*l*fu)/(betaW*math.sqrt(2+3*math.tan(theta)**2)*gammaM2)
  

  
def cargaUltCord2(a2,l,H,fu,betaW,gammaM2):
    '''
    Devuelve la carga última del casquillo rigidizado debida al colapso de los cordones (2)
    (ver figura 61.1.5.b pág 325 EAE) que unen el rigidizador al ala del casquillo, siendo:
    a2: Espesor de garganta de los cordones (2).
    l: Longitud del cateto horizontal del rigidizador.
    H: Longitud del cateto vertical del rigidizador.
    fu: Resistencia a tracción del acero (tabla 59.8.2 pág 304 EAE).
    betaW: Coeficiente de correlación (tabla 59.8.2 pág 304 EAE).
    gammaM2: Partial safety factor for steel (article 15.3 page 34 EAE).
    '''
    theta=math.atan2(l,H)
    return (2.0*a2*H*fu)/(betaW*math.sqrt(3+2*math.tan(theta)**2)*gammaM2)
  

  
def cargaUltCord3(a3,b,l,H,fu,betaW,gammaM2):
    '''
    Devuelve la carga última del casquillo rigidizado debida al colapso de los cordones (3)
    (ver figura 61.1.5.b pág 325 EAE) que unen el rigidizador al ala del casquillo, siendo:
    a3: Espesor de garganta de los cordones (3).
    b: Ancho del ala del casquillo (longitud de los cordones 3).
    l: Longitud del cateto horizontal del rigidizador.
    H: Longitud del cateto vertical del rigidizador.
    fu: Resistencia a tracción del acero (tabla 59.8.2 pág 304 EAE).
    betaW: Coeficiente de correlación (tabla 59.8.2 pág 304 EAE).
    gammaM2: Partial safety factor for steel (article 15.3 page 34 EAE).
    '''
    theta=math.atan2(l,H)
    return (math.sqrt(2)*b*a3*fu)/(betaW*math.tan(theta)*gammaM2)
  
