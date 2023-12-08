# -*- coding: utf-8 -*-
'''Functions to compute thermal loads according to Eurocode 1: Actions on structures - Part 1-5: General actions - Thermal actions.'''

from __future__ import print_function
from __future__ import division

__author__= "Luis C. Pérez Tato (LCPT) Ana Ortega (A_OO) "
__copyright__= "Copyright 2023,  LCPT A_OO "
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com ana.Ortega.Ort@gmail.com"

import math
import scipy.interpolate
from misc_utils import log_messages as lmsg

# Bridge type.
# 1 steel deck: steel box girder, steel truss or plate girder
# 2 composite deck.
# 3 concrete deck.

steel_deck_type= 1
composite_deck_type= 2
concrete_deck_type= 3
concrete_box_girder= 3.1
concrete_beam= 3.2
concrete_slab= 3.3

# Figure 6.1
# Correlation between minimum/maximum shade air temperature
# (Tmin/Tmax) and minimum/maximum uniform bridge temperature component
# (Te.min/Te.max)

def getMinimumUniformBridgeTemperatureComponent(bridgeType:int, Tmin:float):
    ''' Return the minimum temperature on the bridge deck given the type of
        bridge and the minimum shade air temperature according to figure 6.1
        of EC1-1-5:2003.

    :param bridgeType: type of bridge: 1 steel deck, 2 composite deck or 3 concrete deck.
    :param Tmin: minimum shade air temperature (celsius degrees).
    '''
    retval= Tmin
    if(int(bridgeType)==concrete_deck_type): # type 3
        retval+= 8.0
    elif(bridgeType==composite_deck_type): # type 2
        retval+= 4.0
    elif(bridgeType==steel_deck_type): # type 1
        retval-= 3
    else:
        methodName= sys._getframe(0).f_code.co_name
        lmsg.error(methodName+"; bridge type: '"+str(bridgeType)+"' unknown.")
    return retval

def getMaximumUniformBridgeTemperatureComponent(bridgeType:int, Tmax:float):
    ''' Return the maximum temperature on the bridge deck given the type of
        bridge and the maximum shade air temperature according to figure 6.1
        of EC1-1-5:2003.

    :param bridgeType: type of bridge: 1 steel deck, 2 composite deck or 3 concrete deck.
    :param Tmax: maximum shade air temperature (celsius degrees).
    '''
    retval= Tmax
    if(int(bridgeType)==concrete_deck_type):
        retval+= 2.0
    elif(bridgeType==composite_deck_type):
        retval+= 4.0
    elif(bridgeType==steel_deck_type):
        retval+= 16
    return retval

# Minimum shade air temperature (Spanish National Annex).
climateZone= [1, 2, 3, 4, 5, 6, 7] # climate zone according to figure 4.3-b
height= [0.0, 200.0, 400.0, 600.0, 800.0, 1000.0, 1200.0, 1400.0, 1600.0, 1800.0, 2000.0] # height
# Minimum shade air temperature according to table AN.1 of the Spanish National Annex
minAnnualShadeAirTemp= [[-7.0, -11.0, -11.0, -6.0, -5.0, -6.0, 6.0],
                        [-10.0, -13.0, -12.0, -8.0, -8.0, -8.0, 5.0],
                        [-12.0, -15.0, -14.0, -10.0, -11.0, -9.0, 3.0],
                        [-15.0, -16.0, -15.0, -12.0, -14.0, -11.0, 2.0],
                        [-18.0, -18.0, -17.0, -14.0, -17.0, -13.0, 0.0],
                        [-20.0, -20.0, -19.0, -16.0, -20.0, -14.0, -2.0],
                        [-23.0, -21.0, -20.0, -18.0, -23.0, -16.0, -3.0],
                        [-26.0, -23.0, -22.0, -20.0, -26.0, -17.0, -5.0],
                        [-28.0, -25.0, -23.0, -22.0, -29.0, -19.0, -7.0],
                        [-31.0, -26.0, -25.0, -24.0, -32.0, -21.0, -8.0],
                        [-33.0, -28.0, -27.0, -26.0, -35.0, -22.0, -10.0]]
# Minimum shade air temperature interpolation.
fMinAnnualShadeAirTemp= scipy.interpolate.interp2d(climateZone, height, minAnnualShadeAirTemp, kind='linear')

def getMinAnnualShadeAirTemp(climateZn: int, height: float):
    ''' Return the minimal annual shade air temperature for a return period of 50 years according to table AN.1 of the Spanish National Annex.

    :param climateZn: climate zone according to figure AN.2 of the Spanish National Annes (identical to figure 4.3-b of IAP-11).
    :param height: height of the bridge location.
    '''
    return float(fMinAnnualShadeAirTemp(climateZn, height))

# Uniform temperature component on bridges.
def getMinAnnualUniformBridgeTemp(climateZn: int, height: float, returnPeriod: float= 100):
    '''Return the minimal annual temperature according to table AN.1 of the Spansh National Annex (identical to table 4.3-a of IAP-11).

    :param climateZn: climate zone according to figure AN.2 of the Spanish National Annes (identical to figure 4.3-b of IAP-11).
    :param height: height of the bridge location.
    :parem returnPeriod: return period of the temperature minimum in years.
    '''
    retval= getMinAnnualShadeAirTemp(climateZn= climateZn, height= height)
    # Adjust for a different return period if needed.
    if(returnPeriod!=50.0):
        k3= 0.393
        k4= -0.156
        p= 1.0/returnPeriod
        retval*= (k3+k4*math.log(-math.log(1-p)))
    return retval

def getMaxAnnualUniformBridgeTemp(tMax: float, returnPeriod: float= 100):
    '''Return the maximal annual temperature according to Spanish National Annex (or IAP-11).

    :param tMax: annual maximum temperature according to (figure AN.1 of the Spanish National Annex identical to figure 4.3-a of IAP-11).
    :parem returnPeriod: return period of the temperature minimum in years.
    '''
    retval= tMax
    if(returnPeriod!=50.0):
        k1= 0.781
        k2= 0.056
        p= 1.0/returnPeriod
        retval*= (k1-k2*math.log(-math.log(1-p)))
    return retval

# Temperature difference components on bridges.
## k_sur top warmer than bottom
bridgeType= [1, 2, 3] # bridge type 1 steel deck, 2 composite deck or 3 concrete deck
surfacingThickness= [0.0, .05, .1, .15, .75] # surfacing thickness (m).
## Recommended values of k sur to account for different surfacing thickness
### Top warmer than bottom.
kSurTopWarmerThanBottom=[[0.70, 0.90, 0.80],
                         [1.00, 1.00, 1.00],
                         [0.70, 1.00, 0.70],
                         [0.70, 1.00, 0.50],
                         [0.60, 0.80, 0.60]]

#### k_sur value interpolation for top warmer than bottom
fKSurTopWarmerThanBottom= scipy.interpolate.interp2d(bridgeType, surfacingThickness, kSurTopWarmerThanBottom, kind='linear')
### Bottom warmer than top
kSurBottomWarmerThanTop=[[0.90, 1.00, 1.10], 
                         [1.00, 1.00, 1.00], 
                         [1.20, 1.00, 1.00], 
                         [1.20, 1.00, 1.00], 
                         [1.40, 1.20, 1.00]]

#### k_sur value interpolation for top warmer than bottom
fKSurBottomWarmerThanTop= scipy.interpolate.interp2d(bridgeType, surfacingThickness, kSurBottomWarmerThanTop, kind='linear')


def getKSur(bridgeType:int, topWarmerThanBottom= True, surfacingDepth= 50e-3, ballast= False, waterproofedOnly= False):
    ''' Return the recommended value of k_sur to account for different 
        surfacing thickness according to table 6.2 of EC1-1-5:2003.

    :param bridgeType: type of bridge: 1 steel deck, 2 composite deck or 3.1 concrete box girder or 3.2 concrete beam or 3.3 concrete slab.
    :param topWarmerThanBottom: true if the user requires the increment when the top surface is warmer than the deck bottom (deck heated), otherwise the bottom warmer than the top (deck cooled) is returned.
    :param surfacingDepth: depth of surfacing.
    :param ballast: true if the bridge deck is covered with ballast (0.75 m thick).
    :param waterproofedOnly: true if the bridge deck is only waterproofed with a darck color waterproofing layer.
    '''
    if(ballast):
        if(int(bridgeType)==concrete_deck_type): # type 3
            if(topWarmerThanBottom):
                retval= 0.6
            else:
                retval= 1.0
        elif(bridgeType==composite_deck_type): # type 2
            if(topWarmerThanBottom):
                retval= 0.8
            else:
                retval= 1.2
        elif(bridgeType==steel_deck_type): # type 1
            if(topWarmerThanBottom):
                retval= 0.6
            else:
                retval= 1.4
        else:
            methodName= sys._getframe(0).f_code.co_name
            lmsg.error(methodName+"; bridge type: '"+str(bridgeType)+"' unknown.")
    elif(waterproofedOnly):
        if(int(bridgeType)==concrete_deck_type): # type 3
            if(topWarmerThanBottom):
                retval= 1.5
            else:
                retval= 1.0
        elif(bridgeType==composite_deck_type): # type 2
            if(topWarmerThanBottom):
                retval= 1.1
            else:
                retval= 0.9
        elif(bridgeType==steel_deck_type): # type 1
            if(topWarmerThanBottom):
                retval= 1.6
            else:
                retval= 0.6
        else:
            methodName= sys._getframe(0).f_code.co_name
            lmsg.error(methodName+"; bridge type: '"+str(bridgeType)+"' unknown.")

    else:
        if(topWarmerThanBottom):
            retval= float(fKSurTopWarmerThanBottom(bridgeType, surfacingDepth))
        else:
            retval= float(fKSurBottomWarmerThanTop(bridgeType, surfacingDepth))
    return retval
        
def getLinearTemperatureDifferenceComponent(bridgeType:int, topWarmerThanBottom= True, surfacingDepth= 50e-3, ballast= False, waterproofedOnly= False):
    ''' Return the linear temperature difference component for different types 
        of bridge decks for road, foot and railway bridges
        according to table 6.1 of EC1-1-5:2003.

    :param bridgeType: type of bridge: 1 steel deck, 2 composite deck or 3.1 concrete box girder or 3.2 concrete beam or 3.3 concrete slab.
    :param topWarmerThanBottom: true if the user requires the increment when the top surface is warmer than the deck bottom (deck heated), otherwise the bottom warmer than the top (deck cooled) is returned.
    :param surfacingDepth: depth of surfacing.
    :param ballast: true if the bridge deck is covered with ballast (0.75 m thick).
    :param waterproofedOnly: true if the bridge deck is only waterproofed with a darck color waterproofing layer.
    '''
    k_sur= getKSur(bridgeType= bridgeType, surfacingDepth= surfacingDepth, ballast= ballast, waterproofedOnly= waterproofedOnly)
    retval= k_sur
    if(topWarmerThanBottom):
        if(bridgeType==concrete_deck_type):
            lmsg.warning(methodName+"; bridge type: '"+str(bridgeType)+"' is ambiguous choose between:  3.1 concrete box girder or 3.2 concrete beam or 3.3 concrete slab.")
            retval*= 15.0
        elif(bridgeType==concrete_beam or bridgeType==concrete_slab):
            retval*= 15.0
        elif(bridge_type==concrete_box_girder):
            retval*= 10
        elif(bridgeType==composite_deck_type):
            retval*= 15.0
        elif(bridgeType==steel_deck_type):
            retval*= 18.0
        else:
            methodName= sys._getframe(0).f_code.co_name
            lmsg.error(methodName+"; bridge type: '"+str(bridgeType)+"' unknown.")
    else:
        if(bridgeType==concrete_deck_type):
            lmsg.warning(methodName+"; bridge type: '"+str(bridgeType)+"' is ambiguous choose between:  3.1 concrete box girder or 3.2 concrete beam or 3.3 concrete slab.")
            retval*= 8.0
        elif(bridgeType==concrete_beam or bridgeType==concrete_slab):
            retval*= 8.0
        elif(bridge_type==concrete_box_girder):
            retval*= 5.0
        elif(bridgeType==composite_deck_type):
            retval*= 18.0
        elif(bridgeType==steel_deck_type):
            retval*= 13.0
        else:
            methodName= sys._getframe(0).f_code.co_name
            lmsg.error(methodName+"; bridge type: '"+str(bridgeType)+"' unknown.")

    return retval
