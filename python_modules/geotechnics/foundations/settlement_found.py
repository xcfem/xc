# -*- coding: utf-8 -*-

__author__= "Luis C. Pérez Tato (LCPT)"
__copyright__= "Copyright 2022, LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

import math


def deltaSigmaSoilLayer(p,area,zmean):
    '''Return the increase in the total vertical pressure, in the center of a soil-layer, in the vertical of the center of the foundation, created by the load applied to it (used to calculate the average elastic modulus and poisson coefficient).(Guía dimentaciones OC, 4.8.1)

    :param p: average pressure transmitted by the foundation
    :param area: area of the foundation
    :param zmean: average depth of the soil-layer
    '''
    a=math.sqrt(area/math.pi) # radius of the equivalent circular foundation
    ang=math.atan(a/zmean)
    deltaSg=p*(1-(math.cos(ang))**3)
    return deltaSg

def deltaSigmaSoilLayerLongFound(p,B,zmean):
    '''Return the increase in the total vertical pressure, in long rectangular foundations (L/B>=10) in the center of a soil-layer, in the vertical of the center of the foundation, created by the load applied to it (used to calculate the average elastic modulus and poisson coefficient).(Guía dimentaciones OC, 4.8.1)

    :param p: average pressure transmitted by the foundation
    :param B: width of the long foundation
    :param zmean: average depth of the soil-layer
    '''
    eps=math.atan(B/2/zmean)
    deltaSg=p*(2*eps+math.sin(2*eps))/math.pi
    return deltaSg



def meanEquivE(soilLayers,area,p,widhtLongFound=None):
    '''Equivalent average modulus of elasticity. (Guía dimentaciones OC, 4.8.1)
        
    :param soilLayers: dictionary that defines the soil-layers geothecnical parameters
                example: soils={'soil01':{'zTop': , 'zBottom', 'E':, 'nu':}, ...}
                where:
                     zTop: depth of the soil-layer top face (depths positive)
                     zBottom: depth of the soil-layer bottom face
                     E: elastic modulues of the soil
                     nu= poisson coefficient of the layer
    :param area: area of the foundation
    :param p: average pressure transmitted by the foundation
    :param widhtLongFound: is the width of the rectangular foundation when B<10L
     '''
    num=0;den=0
    for k in soilLayers.keys():
        soil=soilLayers[k]
        zmean=(soil['zTop']+soil['zBottom'])/2.
        hi=soil['zBottom']-soil['zTop']
        Ei=soil['E']
        if widhtLongFound:
            deltaSgi=deltaSigmaSoilLayerLongFound(p,widhtLongFound,zmean)
        else:
            deltaSgi=deltaSigmaSoilLayer(p,area,zmean)
        num+=deltaSgi*hi
        den+=deltaSgi/Ei*hi
    Eavrg=num/den
    return Eavrg
        
def meanEquivNu(soilLayers,area,p,widhtLongFound=None):
    '''Equivalent average modulus of Poisson (Guía dimentaciones OC, 4.8.1)
        
    :param soilLayers: dictionary that defines the soil-layers geothecnical parameters
                example: soils={'soil01':{'zTop': , 'zBottom', 'E':, 'nu':}, ...}
                where:
                     zTop: depth of the soil-layer top face (depths positive)
                     zBottom: depth of the soil-layer bottom face
                     E: elastic modulues of the soil
                     nu= poisson coefficient of the layer
    :param area: area of the foundation
    :param p: average pressure transmitted by the foundation
    :param widhtLongFound: is the width of the rectangular foundation when B<10L
     '''
    num=0;den=0
    for k in soilLayers.keys():
        soil=soilLayers[k]
        zmean=(soil['zTop']+soil['zBottom'])/2.
        hi=soil['zBottom']-soil['zTop']
        nui=soil['nu']
        if widhtLongFound:
            deltaSgi=deltaSigmaSoilLayerLongFound(p,widhtLongFound,zmean)
        else:
            deltaSgi=deltaSigmaSoilLayer(p,area,zmean)
        num+=nui*deltaSgi*hi
        den+=deltaSgi*hi
    nuavrg=num/den
    return nuavrg


def settlementFlexibleRectFound(soilLayers,L,B,p):
    ''' Return the settlement in a flexible rectangular foundation
    (Guia de cimentaciones en obras de carretera, 4.8.1, fig. 4.10
    :param soilLayers: dictionary that defines the soil-layers geothecnical parameters
                example: soils={'soil01':{'zTop': , 'zBottom', 'E':, 'nu':}, ...}
                where:
                     zTop: depth of the soil-layer top face (depths positive)
                     zBottom: depth of the soil-layer bottom face
                     E: elastic modulues of the soil
                     nu= poisson coefficient of the layer
    :param L: length of the foundation
    :param B: width of the foundation
    :param p: average pressure transmitted by the foundation
     '''
    if len(list(soilLayers.keys()))>1:
        area=L*B
        if B>L/10:
            E=meanEquivE(soilLayers,area,p)
            nu=meanEquivNu(soilLayers,area,p)
        else: # long foundation
            E=meanEquivE(soilLayers,area,p,B)
            nu=meanEquivNu(soilLayers,area,p,B)
    else:
        soil=list(soilLayers.keys())[0]
        E=soilLayers[soil]['E']
        nu=soilLayers[soil]['nu']
    R=B*math.asinh(L/B)+L*math.asinh(B/L)
    settlmt=2/math.pi*R*p*(1.0-nu**2)/E
    return settlmt

def settlementFlexibleCircFound(soilLayers,Rad,p):
    ''' Return the settlement in a flexible circular foundation
    (Guia de cimentaciones en obras de carretera, 4.8.1, fig. 4.10
    :param soilLayers: dictionary that defines the soil-layers geothecnical parameters
                example: soils={'soil01':{'zTop': , 'zBottom', 'E':, 'nu':}, ...}
                where:
                     zTop: positive depth of the soil-layer top face taken from the foundation bottom face
                     zBottom: positive depth of the soil-layer bottom face taken from the foundation bottom face
                     E: elastic modulues of the soil
                     nu= poisson coefficient of the layer
    :param Rad: radius of the foundation
    :param p: average pressure transmitted by the foundation
     '''
    if len(list(soilLayers.keys()))>1:
        area=math.pi*Rad**2
        E=meanEquivE(soilLayers,area,p)
        nu=meanEquivNu(soilLayers,area,p)
    else:
        soil=list(soilLayers.keys())[0]
        E=soilLayers[soil]['E']
        nu=soilLayers[soil]['nu']
    settlmt=2*p*Rad*(1-nu**2)/E
    return settlmt



   
        
        




