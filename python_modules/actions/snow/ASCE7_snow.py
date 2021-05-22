# -*- coding: utf-8 -*-
from __future__ import division
# Snow loads according to SIA-261 (2003).

import math

def importanceFactor(riskCategory: int):
    ''' Return the importance foctor for snow load according ASCE 7-16.

    :param riskCategory: risk category of the structure.
    '''
    retval= 1.2 # pesimistic asumption
    if(riskCategory==1):
        retval= 0.8
    elif(riskCategory==2):
        retval= 1.0
    elif(riskCategory==3):
        retval= 1.1
    elif(riskCategory==4):
        retval= 1.2
    else:
        lmsg.error('risk category: '+str(riskCategory)+' unknown.')
    return retval

def exposureCoefficient(terrainCategory: str,  roofExposure: str):
    ''' Return the exposure coefficient for snow according ASCE 7-16.

    :param terrainCategory: terrain category ('A'-> large city center,
                            'B'-> urban and suburban areas, 
                            'C'-> open terrain with scattered obstructions,
                            'D'-> unobstructed areas with wind over open water,
                            'E'-> above the tree line in windswept mountainous areas,
                            'F'-> Alaska in areas with trees not within 2 miles of the site).
    :param roofExposure: exposure of the roof ('fully', 'partially', 'sheltered').
    '''
    retval= 1.0
    if(terrainCategory=='A'):
        if(roofExposure=='fully'):
            lmsg.error('not applicable.')
            retval= None
        elif(roofExposure=='partially'):
            retval= 1.1
        else:
            retval= 1.3
    elif(terrainCategory=='B'):
        if(roofExposure=='fully'):
            retval= 0.9
        elif(roofExposure=='partially'):
            retval= 1.0
        else:
            retval= 1.2
    elif(terrainCategory=='C'):
        if(roofExposure=='fully'):
            retval= 0.9
        elif(roofExposure=='partially'):
            retval= 1.0
        else:
            retval= 1.1
    elif(terrainCategory=='D'):
        if(roofExposure=='fully'):
            retval= 0.8
        elif(roofExposure=='partially'):
            retval= 0.9
        else:
            retval= 1.0
    elif(terrainCategory=='E' or terrainCategory=='F'):
        if(roofExposure=='fully'):
            retval= 0.7
        elif(roofExposure=='partially'):
            retval= 0.8
        else:
            lmsg.error('not applicable.')
            retval= None
    else:
        lmsg.error('terrain category: '+str(terrainCategory)+' unknown.')
    return retval

def thermalFactor(thermalCondition: str):
    ''' Return the thermal factor according to ASCE 7-16.

    :param thermalCondition: thermal condition of the structure
                             ('A'-> all structures except as indicated below,
                            'B'-> structures kept just above freezing,
                            'C'-> unheated and open air structures,
                            'D'-> structures intentionally kept below freezing,
                            'E'-> continuously heated greenhouses).
    '''
    retval= 1.3 # pessimistic assumption.
    if(thermalCondition=='A'):
        retval= 1.0
    elif(thermalCondition=='B'):
        retval= 1.1
    elif(thermalCondition=='C'):
        retval= 1.2
    elif(thermalCondition=='D'):
        retval= 1.3
    elif(thermalCondition=='E'):
        retval= 0.85
    else:
        lmsg.error('thermal condition: '+str(thermalCondition)+' unknown.')
    return retval
    
  
def designSnowLoadFlatRoof(terrainCategory: str,  roofExposure: str, thermalCondition: str, riskCategory: int, p_g: float):
    ''' Return the design snow loads for flat roofs
        according ASCE 7-16.

    :param terrainCategory: terrain category ('A'-> large city center,
                            'B'-> urban and suburban areas, 
                            'C'-> open terrain with scattered obstructions,
                            'D'-> unobstructed areas with wind over open water,
                            'E'-> above the tree line in windswept mountainous areas,
                            'F'-> Alaska in areas with trees not within 2 miles of the site).
    :param roofExposure: exposure of the roof ('fully', 'partially', 'sheltered').
    :param thermalCondition: thermal condition of the structure
                             ('A'-> all structures except as indicated below,
                            'B'-> structures kept just above freezing,
                            'C'-> unheated and open air structures,
                            'D'-> structures intentionally kept below freezing,
                            'E'-> continuously heated greenhouses).
    :param riskCategory: risk category of the structure.
    :param p_g: ground snow load.
    '''
    Ce= exposureCoefficient(terrainCategory,  roofExposure)
    Ct= thermalFactor(thermalCondition)
    I= importanceFactor(riskCategory)
    return 0.7*Ce*Ct*I*p_g

def designSnowLoadSlopedRoof(terrainCategory: str,  roofExposure: str, thermalCondition: str, riskCategory: int, p_g: float, Cs: float):
    ''' Return the design snow loads for sloped roofs
        according ASCE 7-16.

    :param terrainCategory: terrain category ('A'-> large city center,
                            'B'-> urban and suburban areas, 
                            'C'-> open terrain with scattered obstructions,
                            'D'-> unobstructed areas with wind over open water,
                            'E'-> above the tree line in windswept mountainous areas,
                            'F'-> Alaska in areas with trees not within 2 miles of the site).
    :param roofExposure: exposure of the roof ('fully', 'partially', 'sheltered').
    :param thermalCondition: thermal condition of the structure
                             ('A'-> all structures except as indicated below,
                            'B'-> structures kept just above freezing,
                            'C'-> unheated and open air structures,
                            'D'-> structures intentionally kept below freezing,
                            'E'-> continuously heated greenhouses).
    :param riskCategory: risk category of the structure.
    :param p_g: ground snow load.
    :param Cs: slope factor.
    '''
    p_f= designSnowLoadFlatRoof(terrainCategory, roofExposure, thermalCondition, riskCategory, p_g)
    return Cs*p_f
