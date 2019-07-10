# -*- coding: utf-8 -*-
''' Wood construction materials according to
    National Design Specification for Wood Construction
    2018 Edition. 
VERIFICATION TESTS REQUIRED!!!.'''

import math
import scipy.interpolate
from miscUtils import LogMessages as lmsg

__author__= "Luis C. Pérez Tato (LCPT) , Ana Ortega (AO_O) "
__copyright__= "Copyright 2016, LCPT, AO_O"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@ciccp.es, ana.ortega@ciccp.es "

tenMinutes= 10/(365.25*24*60)
sevenDays= 7/365.25
twoMonths= 2/12
x= [0,tenMinutes,sevenDays,twoMonths,10,1000]
y= [2.0,1.6,1.25,1.15,1.0,0.9]

table232= scipy.interpolate.interp1d(x,y)

def getLoadDurationFactor(duration):
    '''Return load duration factor according to table 2.3.2
       of AWC-NDS2018.

       :param duration: duration of the load (years)
    '''
    return table232(duration)

def convertToFahrenheit(celsius):
    '''Convert to Fahrenheit degrees.'''
    return celsius * 9 / 5 + 32
    
    print '%r Celsius, converted to Fahrenheit, is: %r Fahrenheit.' % c, f


def convertToCelsius(fahrenheit):
    '''Convert to Celsius degrees.'''
    return (fahrenheit - 32) * 5 / 9

def getWetServiceFactor(refValue, moistureContent, sawnLumber= False):
    '''Return the wet service factor according to AWC-NDS2018.

       :param refValue: reference design values (Ft,E,Emin,Fb,Fv,Fc and FcT)
       :param moistureContent: moisture content (%)
       :param sawnLumber: True or False
    '''
    retval= 1.0
    if(sawnLumber):
        if(moistureContent>=19):
            if(refValue in ['Fb','Ft','Fv','E','Emin']):
                retval= 1.00
            elif(refValue=='FcT'):
                retval= 0.67
            elif(refValue=='Fc'):
                retval= 0.91
            else:
                retval= 1.0
    else:
        if(moistureContent>=16):
            if(refValue in ['Fb','Ft']):
                retval= 0.8
            elif(refValue=='Fv'):
                retval= 0.875
            elif(refValue=='FcT'):
                retval= 0.53
            elif(refValue=='Fc'):
                retval= 0.73
            elif(refValue in ['E','Emin']):
                retval= 0.833
            else:
                retval= 1.0
    return retval

def getTemperatureFactor(refValue, moisture, fahrenheit):
    '''Return the temperature factor according to table 2.3.3
       of AWC-NDS2018.

       :param refValue: reference design values (Ft,E,Emin,Fb,Fv,Fc and FcT)
       :param moisture: moisture (dry or wet)
       :param fahrenheit: temperature (fahrenheit degrees)
    '''
    retval= 1.0
    dry= (moisture=='dry')
    line1= False
    if(refValue in ['Ft','E','Emin']):
        line1= True
    if(fahrenheit>100.0):
        if(fahrenheit<=125.0):
            if(line1):
                retval= 0.9
            else:
                if(dry):
                    retval= 0.8
                else:
                    retval= 0.7
        elif(fahrenheit<=150.0):
            if(line1):
                retval= 0.9
            else:
                if(dry):
                    retval= 0.7
                else:
                    retval= 0.5
        else:
            lmsg.error('Temperature too high: '+fahrenheit)
            retval= 0.01
    return retval
       
def getFormatConversionFactor(refValue, connection= False):
    '''Return the format conversion factor (LRFD) according to table 2.3.5
       of AWC-NDS2018.

      :param refValue: reference design values (Ft,Emin,Fb,Fv,
                       Frt,Fs,Fc and FcT)
      :param connection: connection design (True or False)
    '''
    retval= 3.32
    if(not connection):
        if(refValue=='Fb'):
            retval= 2.54
        elif(refValue=='Ft'):
            retval= 2.70
        elif(refValue in ['Ft','Frt','Fs']):
            retval= 2.88
        elif(refValue=='Fc'):
            retval= 2.40
        elif(refValue=='FcT'):
            retval= 1.67
        elif(refValue=='Emin'):
            retval= 1.76
    return retval

def getResistanceFactor(refValue, connection= False):
    '''Return the format conversion factor (LRFD) according to table 2.3.6
       of AWC-NDS2018.

      :param refValue: reference design values (Ft,Emin,Fb,Fv,
                       Frt,Fs,Fc and FcT)
      :param connection: connection design (True or False)
    '''
    retval= 0.65
    if(not connection):
        if(refValue=='Fb'):
            retval= 0.85
        elif(refValue=='Ft'):
            retval= 0.80
        elif(refValue in ['Ft','Frt','Fs']):
            retval= 0.75
        elif(refValue in ['Fc','FcT']):
            retval= 0.90
        elif(refValue=='Emin'):
            retval= 0.85
    return retval

def getVolumeFactor(L,b,h,southernPine= False):
    '''Return the volume factor according to clause 5.3.6
       of AWC-NDS2018.

      :param L: length of bending member between points
                of zero moment (m).
      :param b: width of bending member (m).
      :param d: depth of bending member (m).
    '''
    x= 1/10.0
    if(southernPine):
        x= 1/20.0
    bmax= min(b,10.75*0.0254)
    retval= math.pow(21*0.3048/L,x)*math.pow(12*0.0254/h,x)*math.pow(5.125*0.0254/bmax,x)
    retval= min(retval,1.0)
    return retval

def getFireDesignAdjustementFactor(refValue):
    '''Return the format conversion factor (LRFD) according to table 16.2.2
       of AWC-NDS2018.

      :param refValue: reference design values (Fb, FbE, Ft,Fc,
                       and FcE)
    '''
    retval= 2.03
    if(refValue=='Fb'):
        retval= 2.85
    elif(refValue=='FbE'):
        retval= 2.03
    elif(refValue=='Ft'):
        retval= 2.85
    elif(refValue=='Fv'):
        retval= 2.75
    elif(refValue=='Fc'):
        retval= 2.58
    elif(refValue=='FcE'):
        retval= 2.03
    else:
        retval= 2.03
    return retval
