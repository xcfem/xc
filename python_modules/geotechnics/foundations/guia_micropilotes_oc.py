# -*- coding: utf-8 -*-
'''Micropile modelling according to the document: "Guía para el proyecto y
   la ejecución de micropilotes en obras de carretera», by «Ministerio de Fomento" (Spain) 2005.'''

import sys
from misc_utils import log_messages as lmsg

def alpha_p(soilType, soilState):
    ''' Return the value of the alpha_p coefficient according to the table
        A-6.5 of the "Guía..." document.

    :param soilType: "peat", "clay", "silt", "sand", "gravel" or "rock"
    :param soilState: "overConsolidated", "normallyConsolidated", 
                               "remolded", "highlyFractured",
                               "moderatelyFractured" (the two last only 
                               for rocks).
    '''
    retval= None
    # Check input values.
    if(not soilType in ['peat', 'clay', 'silt', 'sand', 'gravel', 'rock']):
        methodName= sys._getframe(0).f_code.co_name
        lmsg.warning(methodName+'; unknown soil type: '+str(soilType))
        return retval
    if(not soilState in ['overConsolidated', 'normallyConsolidated', 'remolded', 'highlyFractured', 'moderatelyFractured']):
        methodName= sys._getframe(0).f_code.co_name
        lmsg.warning(methodName+'; unknown soil consolidation state: '+str(soilState))
        return retval
    if(soilType=='peat'):
        if(soilState!='normallyConsolidated'):
            methodName= sys._getframe(0).f_code.co_name
            lmsg.warning(methodName+'; soil state: '+str(soilState)+' cannot be used with soil type: '+str(soilType))
        return retval
    if(soilType=='rock'):
        if(soilState not in ['highlyFractured', 'moderatelyFractured']):
            methodName= sys._getframe(0).f_code.co_name
            lmsg.warning(methodName+'; soil state: '+str(soilState)+' cannot be used with soil type: '+str(soilType))
            
            
    # Get value.
    if(soilState=='overConsolidated'):
        if(soilType=='peat'):
            retval= None
        elif(soilType=='clay'):
            retval= 1.0
        elif(soilType=='peat'):
            retval= 2/3.0
        elif(soilType=='sand'):
            retval= 1/2.0
        elif(soilType=='gravel'):
            retval= 1/3.0
    elif(soilState=='normallyConsolidated'):
        if(soilType=='peat'):
            retval= 1.0
        elif(soilType=='clay'):
            retval= 2/3.0
        elif(soilType=='peat'):
            retval= 1/2.0
        elif(soilType=='sand'):
            retval= 1/3.0
        elif(soilType=='gravel'):
            retval= 1/4.0
    elif(soilState=='remolded'): # remolded 
        if(soilType=='peat'):
            retval= None
        elif(soilType=='clay'):
            retval= 1/2.0
        elif(soilType=='peat'):
            retval= 1/2.0
        elif(soilType=='sand'):
            retval= 1/3.0
        elif(soilType=='gravel'):
            retval= 1/4.0
    elif(soilState=='highlyFractured'): # rock
        retval= 1/3.0
    else:
        retval= 2/3.0
    return retval

def pressuremeterModulusEstimation(relativeDensity= None, consistency= None):
    ''' Return an estimation of the pressuremeter modulus according to the table
        A-6.6 of the "Guía..." document.

    :param relativeDensity: if not None, relative density of the granular soil, 
                            expressed as 'veryLoose', 'loose', 'medium',
                            'dense' or 'veryDense'.
    :param consistency: if not None, consistency of the cohesive soil, expressed
                        as 'verySoft', 'soft', 'medium', 'stiff', veryStiff'
                        or 'hard'.
    '''
    retval= None
    if(relativeDensity): # soil is granular
        if(relativeDensity=='veryLoose'):
            retval= 0.0
        elif(relativeDensity=='loose'):
            retval= 3.5/2.0*1e6
        elif(relativeDensity=='medium'):
            retval= (12+3.5)/2.0*1e6
        elif(relativeDensity=='dense'):
            retval= (22.5+12)/2.0*1e6
        elif(relativeDensity=='veryDense'):
            retval= 22.5*1e6
        else:
            methodName= sys._getframe(0).f_code.co_name
            lmsg.warning(methodName+'; unknown relative density: '+str(relativeDensity))
            retval= None
    elif(consistency): # cohesive soil
        if(consistency=='verySoft'):
            retval= 0.0
        elif(consistency=='soft'):
            retval= 2.5/2.0*1e6
        elif(consistency=='medium'):
            retval= (5+2.5)/2.0*1e6
        elif(consistency=='stiff'):
            retval= (12+5)/2.0*1e6
        elif(consistency=='veryStiff'):
            retval= (25+12)/2.0*1e6
        elif(consistency=='hard'):
            retval= 25e6
        else:
            methodName= sys._getframe(0).f_code.co_name
            lmsg.warning(methodName+'; unknown consistency: '+str(consistency))
            retval= None
    return retval


def soilReactionModulus(soilType, soilState, relativeDensity= None, consistency= None):
    ''' Return the value of the soil reaction modulus according to the clause
        A-6.5.2 of the "Guía..." document.

    :param soilType: "peat", "clay", "silt", "sand", "gravel" or "rock"
    :param soilState: "overConsolidated", "normallyConsolidated", 
                               "remolded", "highlyFractured",
                               "moderatelyFractured" (the two last only 
                               for rocks).
    :param relativeDensity: if not None, relative density of the granular soil, 
                            expressed as 'veryLoose', 'loose', 'medium',
                            'dense' or 'veryDense'.
    :param consistency: if not None, consistency of the cohesive soil, expressed
                        as 'verySoft', 'soft', 'medium', 'stiff', veryStiff'
                        or 'hard'.
    '''
    EM= pressuremeterModulusEstimation(relativeDensity= relativeDensity, consistency= consistency)
    a_p= alpha_p(soilType= soilType, soilState= soilState)
    return EM*6/(4/3.0*pow(2.65,a_p)+a_p)

def soilReactionModulusSand(relativeDensity, depth, waterTableDepth):
    ''' Return the value of the soil reaction modulus according to the table
        A-6.7 of the "Guía..." document.

    :param relativeDensity: if not None, relative density of the granular soil, 
                            expressed as 'veryLoose', 'loose', 'medium',
                            'dense' or 'veryDense'.
    :param depth: depth of the point for which the reaction modulus will be
                  obtained.
    :param waterTableDepth: depth of the water table. 
    '''
    if(depth>waterTableDepth): # not submerged
        if(relativeDensity=='veryLoose'):
            retval= 1
        elif(relativeDensity=='loose'):
            retval= 2
        elif(relativeDensity=='medium'):
            retval= 5
        elif(relativeDensity=='dense'):
            retval= 10
        elif(relativeDensity=='veryDense'):
            retval= 20
        else:
            methodName= sys._getframe(0).f_code.co_name
            lmsg.warning(methodName+'; unknown relative density: '+str(relativeDensity))
            retval= None
    else: # submerged
        if(relativeDensity=='veryLoose'):
            retval= 0.6
        elif(relativeDensity=='loose'):
            retval= 1.2
        elif(relativeDensity=='medium'):
            retval= 3
        elif(relativeDensity=='dense'):
            retval= 6
        elif(relativeDensity=='veryDense'):
            retval= 12
        else:
            methodName= sys._getframe(0).f_code.co_name
            lmsg.warning(methodName+'; unknown relative density: '+str(relativeDensity))
            retval= None
    if(retval):
        retval*= depth*1e6
    return retval
        
    
# Commonly used tubes

