# -*- coding: utf-8 -*-

import sys
import math

def esfuerzoPunzonamiento(qk,A):
    #Estimación del esfuerzo de punzonamiento en la losa sobre
    #un pilar (HL.3 números gordos)
    #Datos:
    #qk: carga uniforme total característica sobre la losa o forjado
    #A: área de influencia del pilar
    return 1.6*qk*A

def punzMaximo(fck,d,a,b):
    #Estimación de la resistencia de las bielas en la sección de
    #intersección del contorno del pilar con el forjado (HL.3 números gordos)
    #(Interpreto que este es el punzonamiento máximo si no vamos a disponer
    #armadura de punzonamiento)
    #Si el esfuerzo de punzonamiento es mayor habrá que:
    #  Aumentar la escuadría del pilar (lo más barato)
    #  Aumentar el depth de la losa (lo más efectivo)
    #  Mejorar la resistencia del hormigón
    #Datos:
    #fck: resistencia característica del hormigón (N/m2)
    #d: depth útil del forjado (m)
    #a,b: escuadría del pilar (m)
    #El resultado se expresa en N
    fcd=fck*10/1.5/1e6  #resistencia de cálculo del hormigón (kp/cm2)
    return 1.5*math.sqrt(fcd)*2*d*(a+b)*100e3

def armaduraPunz(Vd,fck,d,a,b,h,fyd):
    #Estimación de la armadura necesaria por punzonamiento
    #comprobación en la superficie crítica de punzonamiento 
    #situada a d/2 del perímetro del pilar (HL.3 números gordos)
    #Vd: esfuerzo de cálculo de punzomiento (N)
    #fck: resistencia característica del hormigón (N/m2)
    #fyd: límite elástico de cálculo del acero de armas (N/m2)
    #d: depth útil del forjado (m)
    #h: depth del forjado (m)
    #a,b: escuadría del pilar (m)
    #El resultado de la armadura se expresa en m2/m
    fcd=fck*10/1.5/1e6   #resistencia de cálculo del hormigón (kp/cm2)
    Vcu=math.sqrt(fcd)*2*d*(a+b+2*d)*100e3 #resistencia del hormigón a punzonamiento (kN)
    if Vd < Vcu:
        return 0
    else:
        fyalfad=min(400,fyd/1e6)
        Aalfa=(Vd-0.5*Vcu)/0.8/h/fyalfad*10/1e7
        return Aalfa

