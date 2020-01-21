# -*- coding: utf-8 -*-
''' Wood construction materials according to
    National Design Specification for Wood Construction
    2018 Edition. 
VERIFICATION TESTS REQUIRED!!!.'''

from __future__ import division
from __future__ import print_function

import math
import scipy.interpolate
from miscUtils import LogMessages as lmsg
from materials.sections import section_properties as sp
from materials import typical_materials
from materials import buckling_base

__author__= "Luis C. Pérez Tato (LCPT) , Ana Ortega (AO_O) "
__copyright__= "Copyright 2016, LCPT, AO_O"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@ciccp.es, ana.ortega@ciccp.es "

# ENHANCE: it will be great if we can store
# the interpolation in a file to avoid repeating
# computations every time the module is loaded.
tenMinutes= 10/(365.25*24*60)
sevenDays= 7/365.25
twoMonths= 2/12
x= [0,tenMinutes,sevenDays,twoMonths,10,1000]
y= [2.0,1.6,1.25,1.15,1.0,0.9]

table232= scipy.interpolate.interp1d(x,y)

in2meter= 0.0254 # inch to meter conversion factor.
psi2Pa= 6894.76 # pounds per square inch to Pascal conversion factor.
pound2N= 4.44822 # pound to Newton conversion factor.
foot2meter= 0.3048 # foot to meter conversion factor.
pound2kg= 0.453592 # pound to kilogram conversion factor.

def getLoadDurationFactor(duration):
    '''Return load duration factor according to table 2.3.2
       of AWC-NDS2018.

       :param duration: duration of the load (years)
    '''
    return table232(duration)

def convertToFahrenheit(celsius):
    '''Convert to Fahrenheit degrees.'''
    return celsius * 9 / 5 + 32

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
    bmax= min(b,10.75*in2meter)
    retval= math.pow(21*0.3048/L,x)*math.pow(12*in2meter/h,x)*math.pow(5.125*in2meter/bmax,x)
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

class Wood(object):
    '''Base class for wood materials according
       to AWC-NDS2018.'''
    def __init__(self, name):
        self.name= name

class LSL_135E(Wood):
    ''' LSL 1.35E.'''
    E= 1.35e6*psi2Pa # Elastic modulus (Pa)
    Fb_12= 1730*psi2Pa # Bending stress for 12" depth.
    Fv= 410*psi2Pa # Shear stress.
    Fc_pll= 1650*psi2Pa # Compression stress (parallel to grain)
    Fc_perp= 750*psi2Pa # Compression stress (perpendicular to grain)
    xc_material_name= 'LSL_135E'
    def __init__(self, name= 'LSL_135E'):
        super(LSL_135E,self).__init__(name)

class LSL_155E(Wood):
    ''' LSL 1.35E.'''
    E= 1.55e6*psi2Pa # Elastic modulus (Pa)
    Fb_12= 2360*psi2Pa # Bending stress for 12" depth.
    Fv= 410*psi2Pa # Shear stress.
    Fc_pll= 2175*psi2Pa # Compression stress (parallel to grain)
    Fc_perp= 875*psi2Pa # Compression stress (perpendicular to grain)
    xc_material_name= 'LSL_155E'
    def __init__(self, name= 'LSL_135E'):
        super(LSL_155E,self).__init__(name)

class LVL_2900Fb2E(Wood):
    ''' LVL 2900Fb 2.0E structural.'''
    E= 2.0e6*psi2Pa # Elastic modulus (Pa)
    Fb_12= 2900*psi2Pa # Bending stress for 12" depth.
    Fv= 285*psi2Pa # Shear stress.
    Fc_pll= 3200*psi2Pa # Compression stress (parallel to grain)
    Fc_perp= 750*psi2Pa # Compression stress (perpendicular to grain)
    xc_material_name= 'LVL_2900Fb2E'
    def __init__(self, name= 'LVL_2900Fb2E'):
        super(LVL_2900Fb2E,self).__init__(name)

class Member(object):
    ''' Beam/Column according to chapter 3.7 and 3.9 of NDS-2018.'''
    def __init__(self, unbracedLengthB, unbracedLengthH, section, connection= buckling_base.MemberConnection()):
        ''' Constructor. '''
        self.unbracedLengthB= unbracedLengthB
        self.unbracedLengthH= unbracedLengthH
        self.section= section
        self.connection= connection

    def getEffectiveBucklingLengthCoefficientRecommended(self):
        '''Return the column effective buckling length coefficients
           according to NDS 2018 appendix G'''
        return self.connection.getEffectiveBucklingLengthCoefficientRecommended()
    def getBSlendernessRatio(self):
        ''' Return the slenderness ratio for the B dimension.'''
        Ke= self.getEffectiveBucklingLengthCoefficientRecommended()
        return Ke*self.unbracedLengthB/self.section.b
    
    def getHSlendernessRatio(self):
        ''' Return the slenderness ratio for the H dimension.'''
        Ke= self.getEffectiveBucklingLengthCoefficientRecommended()
        return Ke*self.unbracedLengthH/self.section.h
        
    def getSlendernessRatio(self):
        ''' Return the slenderness ratio.'''
        Ke= self.getEffectiveBucklingLengthCoefficientRecommended()
        srB= Ke*self.unbracedLengthB/self.section.b
        srH= Ke*self.unbracedLengthH/self.section.h
        return max(srB,srH)

    def getColumnStabilityFactor(self, c, E_adj, Fc_adj):
        ''' Return the column stability factor according
            to expression 3.7-1 of NDS-2.018. 

        :param E_adj: adjusted modulus of elasticity for beam 
                      stability and column stability calculations.
        :param Fc_adj: adjusted compression stress design value parallel 
                       to grain.
        :param c: 0.8 for sawn lumber, 0.85 for round timber poles 
                  and piles and 0.9 for structural glued laminated
                  timber structural composite lumber, and 
                  cross-laminated timber.
        '''
        sr= self.getSlendernessRatio()
        FcE= 0.822*E_adj/((sr)**2)
        ratio= FcE/Fc_adj
        tmp= (1+ratio)/2.0/c
        return tmp-math.sqrt(tmp**2-ratio/c)
    def getFcE1(self, E_adj):
        ''' Return the value of F_{cE1} as defined in section
            3.9.2 of NDS-2.018.

        :param E_adj: adjusted modulus of elasticity for beam 
                      stability and column stability calculations.
        '''
        if(self.section.h>self.section.b): # Wide side: H
            return 0.822*E_adj/(self.getHSlendernessRatio())**2
        else: # Wide side B
            return 0.822*E_adj/(self.getBSlendernessRatio())**2
    def getFcE2(self, E_adj):
        ''' Return the value of F_{cE2} as defined in section
            3.9.2 of NDS-2.018.

        :param E_adj: adjusted modulus of elasticity for beam 
                      stability and column stability calculations.
        '''
        if(self.section.h<self.section.b): # Narrow side: H
            return 0.822*E_adj/(self.getHSlendernessRatio())**2
        else: # Narrow side B
            return 0.822*E_adj/(self.getBSlendernessRatio())**2
    def getBendingSlendernessRatioH(self):
        ''' Return the slenderness ratio for bending in
            the h plane.'''
        Ke= self.getEffectiveBucklingLengthCoefficientRecommended()
        le= Ke*self.unbracedLengthH
        return math.sqrt(le*self.section.h/self.section.b**2)
    def getBendingSlendernessRatioB(self):
        ''' Return the slenderness ratio for bending in the
            B plane.'''
        Ke= self.getEffectiveBucklingLengthCoefficientRecommended()
        le= Ke*self.unbracedLengthB
        return math.sqrt(le*self.section.b/self.section.h**2)
    def getFbE(self, E_adj):
        ''' Return the value of F_{bE} as defined in section
            3.9.2 of NDS-2.018.

        :param E_adj: adjusted modulus of elasticity for beam 
                      stability and column stability calculations.
        '''
        sr= 1.0
        if(self.section.h<self.section.b): # Narrow side: H
            sr= self.getBendingSlendernessRatioB()
        else: # Narrow side B
            sr= self.getBendingSlendernessRatioH()
        return 1.2*E_adj/sr**2
    def getCapacityFactor(self, E_adj, Fc_adj, Fb1_adj, Fb2_adj, fc,fb1, fb2):
        ''' Return the capacity factor for members subjected to a 
            combination of bending about one or both principal axes 
            and axial compression according to section 3.9.2 of
            NDS-2.018.

        :param E_adj: adjusted modulus of elasticity for beam 
                      stability and column stability calculations.
        :param Fc_adj: adjusted value of reference compression stress.
        :param Fb1_adj: adjusted value of reference bending stress (for
                        bending load applied to narrow face of member).
        :param Fb2_adj: adjusted value of reference bending stress (for
                        bending load applied to wide face of member).
        :param fc: compression stress.
        :param fb1: bending stress (bending load applied to narrow face
                    of member).
        :param fb2: bending stress (bending load applied to wide face
                    of member).
        '''
        val393= (fc/Fc_adj)**2 #Equation 3-9-3
        FcE1= self.getFcE1(E_adj)
        FcE2= self.getFcE2(E_adj)
        FbE= self.getFbE(E_adj)
        val393+= fb1/(Fb1_adj*(1-min(fc/FcE1,1.0)))
        val393+= fb2/(Fb2_adj*(1-min(fc/FcE2,1.0)-min(fb1/FbE,1.0)**2))
        val394= fc/FcE2+(fb1/FbE)**2 #Equation 3-9-4
        return max(val393,val394)

class WoodPanel(sp.RectangularSection):
    ''' Plywood structural panel.'''
    def __init__(self, name, b, h, shear_constant):
        super(WoodPanel,self).__init__(name, b, h)
        self.shearConstant= shear_constant
    def getSpanRating(self):
        ''' Return the span rating from the panel thickness according
            to the table B of the documents:
            "Design Capacities for Oriented Strand Board V1.0 – 01/2008"
            and "Design Capacities for Structural Plywood V1.0 – 01/2008"
            both from PFS TECO • 1507 Matt Pass, Cottage Grove, WI 53527, USA
        '''
        spanRating= 'unknown'
        if(self.sectionName=='3/8'):
            spanRating= '24/0'
        elif(self.sectionName=='7/16'):
            spanRating= '24/16'
        elif(self.sectionName=='15/32'):
            spanRating= '32/16'
        elif(self.sectionName=='1/2'):
            spanRating= '32/16'
        elif(self.sectionName=='19/32'):
            spanRating= '40/20'
        elif(self.sectionName=='5/8'):
            spanRating= '40/20'
        elif(self.sectionName=='23/32'):
            spanRating= '48/24'
        elif(self.sectionName=='3/4'):
            spanRating= '48/24'
        elif(self.sectionName=='7/8'):
            spanRating= '48/24'
        elif(self.sectionName=='1'):
            spanRating= '48/24'
        elif(self.sectionName=='1-1/8'):
            spanRating= '48/24'
        else:
            lmsg.error('name: ', self.sectionName, 'unknown')
        return spanRating
    def getAPARatedSturdIFloor(self):
        ''' Return the APA rated Sturd-I-Floor from the panel thickness 
            according to the table B of the documents:
            "Design Capacities for Oriented Strand Board V1.0 – 01/2008"
            and "Design Capacities for Structural Plywood V1.0 – 01/2008"
            both from PFS TECO • 1507 Matt Pass, Cottage Grove, WI 53527, USA
        '''
        spanRating= 'unknown'
        if(self.sectionName=='3/8'):
            spanRating= '0oc'
        elif(self.sectionName=='7/16'):
            spanRating= '0oc'
        elif(self.sectionName=='15/32'):
            spanRating= '0oc'
        elif(self.sectionName=='1/2'):
            spanRating= '0oc'
        elif(self.sectionName=='19/32'):
            spanRating= '20oc'
        elif(self.sectionName=='5/8'):
            spanRating= '20oc'
        elif(self.sectionName=='23/32'):
            spanRating= '24oc'
        elif(self.sectionName=='3/4'):
            spanRating= '24oc'
        elif(self.sectionName=='7/8'):
            spanRating= '32oc'
        elif(self.sectionName=='1'):
            spanRating= '32oc'
        elif(self.sectionName=='1-1/8'):
            spanRating= '48oc'
        else:
            lmsg.error('name: ', self.sectionName, 'unknown')
        return spanRating


# Properties of Plywood structural panels taken from:
# http://www.pfsteco.com/techtips/pdf/tt_plywooddesigncapacities
# table C.

class PlywoodPanel(WoodPanel):
    ''' Plywood structural panel.'''
    rho= 577.941243312 # density kg/m3
    def __init__(self, name, b, h, shear_constant):
        super(PlywoodPanel,self).__init__(name, b, h, shear_constant)
    def getArealDensity(self):
        return self.rho*self.h


PlywoodPanels= dict()

PlywoodPanels['3/8']= PlywoodPanel('3/8',b= 1.0, h= 0.375*in2meter, shear_constant= 3.00) #,  'CrossSectionalArea':4.500, 'MomentOfInertia':0.053, 'SectionModulus':0.281, 'StaticalMoment':0.211
PlywoodPanels['7/16']= PlywoodPanel('7/16',b= 1.0, h= 0.437*in2meter, shear_constant= 3.50) #,  'CrossSectionalArea':5.250, 'MomentOfInertia':0.084, 'SectionModulus':0.383, 'StaticalMoment':0.287
PlywoodPanels['15/32']= PlywoodPanel('15/32',b= 1.0, h= 0.469*in2meter, shear_constant= 3.75) #,  'CrossSectionalArea':5.625, 'MomentOfInertia':0.103, 'SectionModulus':0.440, 'StaticalMoment':0.330
PlywoodPanels['1/2']= PlywoodPanel('1/2',b= 1.0, h= 0.500*in2meter, shear_constant= 4.00) #,  'CrossSectionalArea':6.000, 'MomentOfInertia':0.125, 'SectionModulus':0.500, 'StaticalMoment':0.375
PlywoodPanels['19/32']= PlywoodPanel('19/32',b= 1.0, h= 0.594*in2meter, shear_constant= 4.75) #,  'CrossSectionalArea':7.125, 'MomentOfInertia':0.209, 'SectionModulus':0.705, 'StaticalMoment':0.529
PlywoodPanels['5/8']= PlywoodPanel('5/8',b= 1.0, h= 0.625*in2meter, shear_constant= 5.00) #,  'CrossSectionalArea':7.500, 'MomentOfInertia':0.244, 'SectionModulus':0.781, 'StaticalMoment':0.586
PlywoodPanels['23/32']= PlywoodPanel('23/32',b= 1.0, h= 0.719*in2meter, shear_constant= 5.75) #,  'CrossSectionalArea':8.625, 'MomentOfInertia':0.371, 'SectionModulus':1.033, 'StaticalMoment':0.775
PlywoodPanels['3/4']= PlywoodPanel('3/4',b= 1.0, h= 0.750*in2meter, shear_constant= 6.00) #,  'CrossSectionalArea':9.000, 'MomentOfInertia':0.422, 'SectionModulus':1.125, 'StaticalMoment':0.844
PlywoodPanels['7/8']= PlywoodPanel('7/8',b= 1.0, h= 0.875*in2meter, shear_constant= 7.00) #, 'CrossSectionalArea':10.500, 'MomentOfInertia':0.670, 'SectionModulus':1.531, 'StaticalMoment':1.148
PlywoodPanels['1']= PlywoodPanel('1',b= 1.0, h= 1.000*in2meter, shear_constant= 8.00) #,  'CrossSectionalArea':12.000, 'MomentOfInertia':1.000, 'SectionModulus':2.000, 'StaticalMoment':1.500
PlywoodPanels['1-1/8']= PlywoodPanel('1-1/8',b= 1.0, h= 1.125*in2meter, shear_constant= 9.00) #,  'CrossSectionalArea':13.500, 'MomentOfInertia':1.424, 'SectionModulus':2.531, 'StaticalMoment':1.898

# Oriented strand board panels according to document:
# "Panel design specification" Form No. D510C/Revised May 2012/0300

class OSBPanel(WoodPanel):
    ''' Oriented strand board panel.'''
    rho= 632.62 # average density kg/m3 Table 12
    def __init__(self, name, b, h, shear_constant):
        super(OSBPanel,self).__init__(name, b, h, shear_constant)
    def getArealDensity(self):
        return self.rho*self.h
    def getFb(self, angle= math.pi/2.0):
        ''' Return the bending stress Fb or the panel according
            to the table A of the document:
            "Design Capacities for Oriented Strand Board V1.0 – 01/2008"
            from PFS TECO • 1507 Matt Pass, Cottage Grove, WI 53527, USA

            angle: angle of the stress with the strength axis.
        '''
        spanRating= self.getAPARatedSturdIFloor()
        FbS= 0.0
        if((angle!=0.0) and (angle!=math.pi/2.0)):
            lmsg.error('angle must be 0 or PI/2')
        if(spanRating!='unknown'):
            if(spanRating=='16oc'):
                if(angle==0.0):
                    FbS= 500.0
                else:
                    FbS= 180.0
            elif(spanRating=='20oc'):
                if(angle==0.0):
                    FbS= 575.0
                else:
                    FbS= 250.0
            elif(spanRating=='24oc'):
                if(angle==0.0):
                    FbS= 770.0
                else:
                    FbS= 385.0                
            elif(spanRating=='32oc'):
                if(angle==0.0):
                    FbS= 1050.0
                else:
                    FbS= 685.0                
            elif(spanRating=='48oc'):
                if(angle==0.0):
                    FbS= 1900.0
                else:
                    FbS= 1200.0                
        else:
            spanRating= self.getSpanRating()
            if(spanRating=='24/0'):
                if(angle==0.0):
                    FbS= 300.0
                else:
                    FbS= 97.0
            elif(spanRating=='24/16'):
                if(angle==0.0):
                    FbS= 385.0
                else:
                    FbS= 115.0
            elif(spanRating=='32/16'):
                if(angle==0.0):
                    FbS= 445.0
                else:
                    FbS= 165.0                
            elif(spanRating=='40/20'):
                if(angle==0.0):
                    FbS= 750.0
                else:
                    FbS= 270.0                
            elif(spanRating=='48/24'):
                if(angle==0.0):
                    FbS= 1000.0
                else:
                    FbS= 405.0
        Fb= FbS/self.Wzel()*4.44822*0.0254/0.3048
        return Fb
    def getFv(self):
        ''' Return the bending stress Fb or the panel according
            to the table A of the document:
            "Design Capacities for Oriented Strand Board V1.0 – 01/2008"
            from PFS TECO • 1507 Matt Pass, Cottage Grove, WI 53527, USA
        '''
        spanRating= self.getAPARatedSturdIFloor()
        Fvtv= 0.0
        if(spanRating!='unknown'):
            if(spanRating=='16oc'):
                Fvtv= 170.0
            elif(spanRating=='20oc'):
                Fvtv= 195.0
            elif(spanRating=='24oc'):
                Fvtv= 215.0
            elif(spanRating=='32oc'):
                Fvtv= 230.0
            elif(spanRating=='48oc'):
                Fvtv= 305.0
        else:
            spanRating= self.getSpanRating()
            if(spanRating=='24/0'):
                Fvtv= 155.0
            elif(spanRating=='24/16'):
                Fvtv= 165.0
            elif(spanRating=='32/16'):
                Fvtv= 180.0
            elif(spanRating=='40/20'):
                Fvtv= 195.0
            elif(spanRating=='48/24'):
                Fvtv= 220.0
        Fv= Fvtv/self.h*4.44822/0.3048
        return Fv
    def getE(self, angle= math.pi/2.0):
        ''' Return the bending stress Fb or the panel according
            to the table A of the document:
            "Design Capacities for Oriented Strand Board V1.0 – 01/2008"
            from PFS TECO • 1507 Matt Pass, Cottage Grove, WI 53527, USA

            angle: angle of the stress with the strength axis.
        '''
        spanRating= self.getAPARatedSturdIFloor()
        EI= 0.0
        if((angle!=0.0) and (angle!=math.pi/2.0)):
            lmsg.error('angle must be 0 or PI/2')
        if(spanRating!='unknown'):
            if(spanRating=='16oc'):
                if(angle==0.0):
                    EI= 150e3
                else:
                    EI= 34e3
            elif(spanRating=='20oc'):
                if(angle==0.0):
                    EI= 210e3
                else:
                    EI= 40.5e3
            elif(spanRating=='24oc'):
                if(angle==0.0):
                    EI= 300e3
                else:
                    EI= 80.5e3
            elif(spanRating=='32oc'):
                if(angle==0.0):
                    EI= 650e3
                else:
                    EI= 235e3
            elif(spanRating=='48oc'):
                if(angle==0.0):
                    EI= 1150e3
                else:
                    EI= 495e3
        else:
            spanRating= self.getSpanRating()
            if(spanRating=='24/0'):
                if(angle==0.0):
                    EI= 60e3
                else:
                    EI= 11e3
            elif(spanRating=='24/16'):
                if(angle==0.0):
                    EI= 78e3
                else:
                    EI= 16e3
            elif(spanRating=='32/16'):
                if(angle==0.0):
                    EI= 115e3
                else:
                    EI= 25e3
            elif(spanRating=='40/20'):
                if(angle==0.0):
                    EI= 225e3
                else:
                    EI= 56e3
            elif(spanRating=='48/24'):
                if(angle==0.0):
                    EI= 400e3
                else:
                    EI= 91.5e3
        E= EI/self.Iz()*4.44822*0.0254**2/0.3048
        return E

OSBPanels= dict()

OSBPanels['3/8']= OSBPanel('3/8',b= 1.0, h= 0.375*in2meter, shear_constant= 3.00)
OSBPanels['7/16']= OSBPanel('7/16',b= 1.0, h= 0.437*in2meter, shear_constant= 3.50)
OSBPanels['15/32']= OSBPanel('15/32',b= 1.0, h= 0.469*in2meter, shear_constant= 3.75)
OSBPanels['1/2']= OSBPanel('1/2',b= 1.0, h= 0.500*in2meter, shear_constant= 4.00)
OSBPanels['19/32']= OSBPanel('19/32',b= 1.0, h= 0.594*in2meter, shear_constant= 4.75)
OSBPanels['5/8']= OSBPanel('5/8',b= 1.0, h= 0.625*in2meter, shear_constant= 5.00)
OSBPanels['23/32']= OSBPanel('23/32',b= 1.0, h= 0.719*in2meter, shear_constant= 5.75)
OSBPanels['3/4']= OSBPanel('3/4',b= 1.0, h= 0.750*in2meter, shear_constant= 6.00)
OSBPanels['7/8']= OSBPanel('7/8',b= 1.0, h= 0.875*in2meter, shear_constant= 7.00)
OSBPanels['1']= OSBPanel('1',b= 1.0, h= 1.000*in2meter, shear_constant= 8.00)
OSBPanels['1-1/8']= OSBPanel('1-1/8',b= 1.0, h= 1.125*in2meter, shear_constant= 9.00)

class Header(sp.RectangularSection):
    ''' Structural beam/header.'''
    nu= 0.2
    def __init__(self, name, b, h, Ms, Vs, linearDensity, material):
        '''Constructor.'''
        super(Header,self).__init__(name, b, h)
        self.Ms= Ms # Allowable moment.
        self.Vs= Vs # Allowable shear.
        self.rho= linearDensity/b/h
        self.material= material
        self.xc_material= None
        self.xc_section= None
    def getFb(self):
        return self.getVolumeFactor()*self.material.Fb_12
    def defXCMaterial(self):
        '''Defines the material in XC.'''
        if(not self.xc_material):
            self.xc_material= typical_materials.MaterialData(name= self.material.xc_material_name,E=self.material.E,nu=self.nu,rho=self.rho)
        return self.xc_material
    def defElasticShearSection2d(self, preprocessor):
        mat= self.defXCMaterial()
        self.xc_section= super(Header,self).defElasticShearSection2d(preprocessor,mat)
        return self.xc_section
    def defElasticShearSection3d(self, preprocessor):
        mat= self.defXCMaterial()
        self.xc_section= super(Header,self).defElasticShearSection3d(preprocessor,mat)
        return self.xc_section

    

# Properties of LSL beams and headers taken from:
# LP SolidStart LSL Beam & Header Technical Guide

class LSLHeader(Header):
    ''' LSL structural beam/header.'''
    def __init__(self, name, b, h, Ms, Vs, linearDensity, material):
        '''Constructor.'''
        super(LSLHeader,self).__init__(name, b, h, Ms, Vs, linearDensity, material)
    def getVolumeFactor(self):
        '''Return volumen factor.'''
        retval= 1.0
        in12= 12.0*in2meter
        in3_5= 3.5*in2meter
        if(self.h<in3_5):
            retval*= 1.159
        else:
            retval= pow(in12/self.h,0.12)
        return retval

class LSL_135E_Header(LSLHeader):
    ''' LSL 1.35E structural beam/header.'''
    def __init__(self, name, b, h, Ms, Vs, linearDensity):
        super(LSL_135E_Header,self).__init__(name, b, h, Ms, Vs, linearDensity, material= LSL_135E())

LSL135Headers= dict()

LSL135Headers['3.5x4-3/8']= LSL_135E_Header('3.5x4-3/8',b= 3.5*in2meter, h= (4+3/8.0)*in2meter, Ms= 1817*pound2N*foot2meter, Vs= 4185*pound2N, linearDensity= 4.7*pound2kg)
LSL135Headers['1.5x5-1/2']= LSL_135E_Header('1.5x5-1/2',b= 1.5*in2meter, h= 5.5*in2meter, Ms= 1197*pound2N*foot2meter, Vs= 2255*pound2N, linearDensity= 2.6*pound2kg)
LSL135Headers['3.5x5-1/2']= LSL_135E_Header('3.5x5-1/2',b= 3.5*in2meter, h= 5.5*in2meter, Ms= 2794*pound2N*foot2meter, Vs= 5262*pound2N, linearDensity= 5.9*pound2kg)
LSL135Headers['1.5x7-1/4']= LSL_135E_Header('1.5x7-1/4',b= 1.5*in2meter, h= 7.25*in2meter, Ms= 2013*pound2N*foot2meter, Vs= 2972*pound2N, linearDensity= 3.4*pound2kg)
LSL135Headers['3.5x7-1/4']= LSL_135E_Header('3.5x7-1/4',b= 3.5*in2meter, h= 7.25*in2meter, Ms= 4696*pound2N*foot2meter, Vs= 6936*pound2N, linearDensity= 7.8*pound2kg)

class LSL_155E_Header(LSLHeader):
    ''' LSL 1.55E structural beam/header.'''
    def __init__(self, name, b, h, Ms, Vs, linearDensity):
        super(LSL_155E_Header,self).__init__(name, b, h, Ms, Vs, linearDensity, material= LSL_155E())

LSL155Headers= dict()

LSL155Headers['1.5x5-1/2']= LSL_155E_Header('1.5x5-1/2', b= 1.5*in2meter, h= 5.5*in2meter, Ms= 1633*pound2N*foot2meter, Vs= 2255*pound2N, linearDensity= 2.6*pound2kg)
LSL155Headers['1.5x7-1/4']= LSL_155E_Header('1.5x7-1/4', b= 1.5*in2meter, h= 7.25*in2meter, Ms= 2745*pound2N*foot2meter, Vs= 2973*pound2N, linearDensity= 3.5*pound2kg)
LSL155Headers['1.5x9-1/4']= LSL_155E_Header('1.5x9-1/4', b= 1.5*in2meter, h= 9.25*in2meter, Ms= 4340*pound2N*foot2meter, Vs= 3793*pound2N, linearDensity= 4.4*pound2kg)
LSL155Headers['1.5x9-1/2']= LSL_155E_Header('1.5x9-1/2', b= 1.5*in2meter, h= 9.5*in2meter, Ms= 4563*pound2N*foot2meter, Vs= 3895*pound2N, linearDensity= 4.6*pound2kg)
LSL155Headers['1.5x11-1/4']= LSL_155E_Header('1.5x11-1/4', b= 1.5*in2meter, h= 11.25*in2meter, Ms= 6271*pound2N*foot2meter, Vs= 4613*pound2N, linearDensity= 5.4*pound2kg)
LSL155Headers['1.5x11-7/8']= LSL_155E_Header('1.5x11-7/8', b= 1.5*in2meter, h= 11.875*in2meter, Ms= 6942*pound2N*foot2meter, Vs= 4869*pound2N, linearDensity= 5.7*pound2kg)

LSL155Headers['1.75x5-1/2']= LSL_155E_Header('1.75x5-1/2', b= 1.75*in2meter, h= 5.5*in2meter, Ms= 1905*pound2N*foot2meter, Vs= 2631*pound2N, linearDensity= 3.1*pound2kg)
LSL155Headers['1.75x7-1/4']= LSL_155E_Header('1.75x7-1/4', b= 1.75*in2meter, h= 7.25*in2meter, Ms= 3203*pound2N*foot2meter, Vs= 3468*pound2N, linearDensity= 4.1*pound2kg)
LSL155Headers['1.75x9-1/4']= LSL_155E_Header('1.75x9-1/4', b= 1.75*in2meter, h= 9.25*in2meter, Ms= 5064*pound2N*foot2meter, Vs= 4425*pound2N, linearDensity= 5.2*pound2kg)
LSL155Headers['1.75x9-1/2']= LSL_155E_Header('1.75x9-1/2', b= 1.75*in2meter, h= 9.5*in2meter, Ms= 5324*pound2N*foot2meter, Vs= 4544*pound2N, linearDensity= 5.3*pound2kg)

LSL155Headers['1.75x11-7/8']= LSL_155E_Header('1.75x11-7/8', b= 1.75*in2meter, h= 11.875*in2meter, Ms= 8099*pound2N*foot2meter, Vs= 5680*pound2N, linearDensity= 6.6*pound2kg)
LSL155Headers['1.75x14']= LSL_155E_Header('1.75x14', b= 1.75*in2meter, h= 14*in2meter, Ms= 11037*pound2N*foot2meter, Vs= 6697*pound2N, linearDensity= 7.8*pound2kg)
LSL155Headers['1.75x16']= LSL_155E_Header('1.75x16', b= 1.75*in2meter, h= 16*in2meter, Ms= 14186*pound2N*foot2meter, Vs= 7653*pound2N, linearDensity= 8.9*pound2kg)
LSL155Headers['1.75x18']= LSL_155E_Header('1.75x18', b= 1.75*in2meter, h= 18*in2meter, Ms= 17702*pound2N*foot2meter, Vs= 8610*pound2N, linearDensity= 10.1*pound2kg)

LSL155Headers['3.5x5-1/2']= LSL_155E_Header('3.5x5-1/2', b= 3.5*in2meter, h= 5.5*in2meter, Ms= 3811*pound2N*foot2meter, Vs= 5262*pound2N, linearDensity= 6.1*pound2kg)
LSL155Headers['3.5x7-1/4']= LSL_155E_Header('3.5x7-1/4', b= 3.5*in2meter, h= 7.25*in2meter, Ms= 6406*pound2N*foot2meter, Vs= 6936*pound2N, linearDensity= 8.1*pound2kg)
LSL155Headers['3.5x9-1/4']= LSL_155E_Header('3.5x9-1/4', b= 3.5*in2meter, h= 9.25*in2meter, Ms= 10127*pound2N*foot2meter, Vs= 8849*pound2N, linearDensity= 10.3*pound2kg)
LSL155Headers['3.5x9-1/2']= LSL_155E_Header('3.5x9-1/2', b= 3.5*in2meter, h= 9.5*in2meter, Ms= 10648*pound2N*foot2meter, Vs= 9088*pound2N, linearDensity= 10.6*pound2kg)

LSL155Headers['3.5x11-7/8']= LSL_155E_Header('3.5x11-7/8', b= 3.5*in2meter, h= 11.875*in2meter, Ms= 16198*pound2N*foot2meter, Vs= 11360*pound2N, linearDensity= 13.3*pound2kg)
LSL155Headers['3.5x14']= LSL_155E_Header('3.5x14', b= 3.5*in2meter, h= 14*in2meter, Ms= 22073*pound2N*foot2meter, Vs= 13393*pound2N, linearDensity= 15.7*pound2kg)
LSL155Headers['3.5x16']= LSL_155E_Header('3.5x16', b= 3.5*in2meter, h= 16*in2meter, Ms= 28372*pound2N*foot2meter, Vs= 15307*pound2N, linearDensity= 17.9*pound2kg)
LSL155Headers['3.5x18']= LSL_155E_Header('3.5x18', b= 3.5*in2meter, h= 18*in2meter, Ms= 35405*pound2N*foot2meter, Vs= 17220*pound2N, linearDensity= 20.1*pound2kg)

LSL155Headers['5.25x5-1/2']= LSL_155E_Header('5.25x5-1/2', b= 5.25*in2meter, h= 5.5*in2meter, Ms= 5716*pound2N*foot2meter, Vs= 7893*pound2N, linearDensity= 9.2*pound2kg)
LSL155Headers['5.25x7-1/4']= LSL_155E_Header('5.25x7-1/4', b= 5.25*in2meter, h= 7.25*in2meter, Ms= 9609*pound2N*foot2meter, Vs= 10404*pound2N, linearDensity= 12.2*pound2kg)
LSL155Headers['5.25x9-1/4']= LSL_155E_Header('5.25x9-1/4', b= 5.25*in2meter, h= 9.25*in2meter, Ms= 15191*pound2N*foot2meter, Vs= 13274*pound2N, linearDensity= 15.5*pound2kg)
LSL155Headers['5.25x9-1/2']= LSL_155E_Header('5.25x9-1/2', b= 5.25*in2meter, h= 9.5*in2meter, Ms= 15972*pound2N*foot2meter, Vs= 13633*pound2N, linearDensity= 15.9*pound2kg)

LSL155Headers['5.25x11-7/8']= LSL_155E_Header('5.25x11-7/8', b= 5.25*in2meter, h= 11.875*in2meter, Ms= 24297*pound2N*foot2meter, Vs= 17041*pound2N, linearDensity= 19.9*pound2kg)
LSL155Headers['5.25x14']= LSL_155E_Header('5.25x14', b= 5.25*in2meter, h= 14*in2meter, Ms= 33110*pound2N*foot2meter, Vs= 20090*pound2N, linearDensity= 23.5*pound2kg)
LSL155Headers['5.25x16']= LSL_155E_Header('5.25x16', b= 5.25*in2meter, h= 16*in2meter, Ms= 42558*pound2N*foot2meter, Vs= 22960*pound2N, linearDensity= 26.8*pound2kg)
LSL155Headers['5.25x18']= LSL_155E_Header('5.25x18', b= 5.25*in2meter, h= 18*in2meter, Ms= 53107*pound2N*foot2meter, Vs= 25830*pound2N, linearDensity= 30.2*pound2kg)

# Properties of LVL beams and headers taken from:
# LP SolidStart LVL Beam & Header Technical Guide

class LVLHeader(Header):
    ''' LVL structural beam/header.'''
    def __init__(self, name, b, h, Ms, Vs, linearDensity, material):
        '''Constructor.'''
        super(LVLHeader,self).__init__(name, b, h, Ms, Vs, linearDensity, material)
    def getVolumeFactor(self):
        '''Return volumen factor.'''
        retval= 1.0
        in12= 12.0*0.0254
        in3_5= 3.5*0.0254
        if(self.h>in12):
            retval*= pow((in12/self.h),0.143)
        else:
            if(self.h<in3_5):
                retval*= 1.147
            else:
                retval*= pow((in12/self.h),0.111)
        return retval

class LVL_2900Fb2E_Header(LVLHeader):
    ''' LVL 2900Fb 2.0E structural beam/header.'''
    def __init__(self, name, b, h, Ms, Vs, linearDensity):
        super(LVL_2900Fb2E_Header,self).__init__(name,b,h, Ms, Vs, linearDensity, material= LVL_2900Fb2E())

LVLHeaders= dict()

LVLHeaders['1.75x7-1/4']= LVL_2900Fb2E_Header('1.75x7-1/4', b= 1.75*in2meter, h= 7.25*in2meter, Ms= 3918*pound2N*foot2meter, Vs= 2411*pound2N, linearDensity= 3.6*pound2kg)
LVLHeaders['1.75x9-1/4']= LVL_2900Fb2E_Header('1.75x9-1/4', b= 1.75*in2meter, h= 9.25*in2meter, Ms= 6208*pound2N*foot2meter, Vs= 3076*pound2N, linearDensity= 4.6*pound2kg)
LVLHeaders['1.75x9-1/2']= LVL_2900Fb2E_Header('1.75x9-1/2', b= 1.75*in2meter, h= 9.5*in2meter, Ms= 6529*pound2N*foot2meter, Vs= 3159*pound2N, linearDensity= 4.8*pound2kg)
LVLHeaders['1.75x11-1/4']= LVL_2900Fb2E_Header('1.75x11-1/4', b= 1.75*in2meter, h= 11.25*in2meter, Ms= 8985*pound2N*foot2meter, Vs= 3741*pound2N, linearDensity= 5.6*pound2kg)
LVLHeaders['1.75x11-7/8']= LVL_2900Fb2E_Header('1.75x11-7/8', b= 1.75*in2meter, h= 11.875*in2meter, Ms= 9951*pound2N*foot2meter, Vs= 3948*pound2N, linearDensity= 5.9*pound2kg)
LVLHeaders['1.75x14']= LVL_2900Fb2E_Header('1.75x14', b= 1.75*in2meter, h= 14*in2meter, Ms= 13514*pound2N*foot2meter, Vs= 4655*pound2N, linearDensity= 7*pound2kg)
LVLHeaders['1.75x16']= LVL_2900Fb2E_Header('1.75x16', b= 1.75*in2meter, h= 16*in2meter, Ms= 17318*pound2N*foot2meter, Vs= 5320*pound2N, linearDensity= 8*pound2kg)
LVLHeaders['1.75x18']= LVL_2900Fb2E_Header('1.75x18', b= 1.75*in2meter, h= 18*in2meter, Ms= 21552*pound2N*foot2meter, Vs= 5985*pound2N, linearDensity= 9*pound2kg)

LVLHeaders['3.5x7-1/4']= LVL_2900Fb2E_Header('3.5x7-1/4', b= 3.5*in2meter, h= 7.25*in2meter, Ms= 7837*pound2N*foot2meter, Vs= 4821*pound2N, linearDensity= 7.3*pound2kg)
LVLHeaders['3.5x9-1/4']= LVL_2900Fb2E_Header('3.5x9-1/4', b= 3.5*in2meter, h= 9.25*in2meter, Ms= 12416*pound2N*foot2meter, Vs= 6151*pound2N, linearDensity= 9.3*pound2kg)
LVLHeaders['3.5x9-1/2']= LVL_2900Fb2E_Header('3.5x9-1/2', b= 3.5*in2meter, h= 9.5*in2meter, Ms= 13057*pound2N*foot2meter, Vs= 6318*pound2N, linearDensity= 9.5*pound2kg)
LVLHeaders['3.5x11-1/4']= LVL_2900Fb2E_Header('3.5x11-1/4', b= 3.5*in2meter, h= 11.25*in2meter, Ms= 17970*pound2N*foot2meter, Vs= 7481*pound2N, linearDensity= 11.3*pound2kg)
LVLHeaders['3.5x11-7/8']= LVL_2900Fb2E_Header('3.5x11-7/8', b= 3.5*in2meter, h= 11.875*in2meter, Ms= 19902*pound2N*foot2meter, Vs= 7897*pound2N, linearDensity= 11.9*pound2kg)
LVLHeaders['3.5x14']= LVL_2900Fb2E_Header('3.5x14', b= 3.5*in2meter, h= 14*in2meter, Ms= 27029*pound2N*foot2meter, Vs= 9310*pound2N, linearDensity= 14*pound2kg)
LVLHeaders['3.5x16']= LVL_2900Fb2E_Header('3.5x16', b= 3.5*in2meter, h= 16*in2meter, Ms= 34636*pound2N*foot2meter, Vs= 10640*pound2N, linearDensity= 16*pound2kg)
LVLHeaders['3.5x18']= LVL_2900Fb2E_Header('3.5x18', b= 3.5*in2meter, h= 18*in2meter, Ms= 43105*pound2N*foot2meter, Vs= 11970*pound2N, linearDensity= 18*pound2kg)

LVLHeaders['5.25x7-1/4']= LVL_2900Fb2E_Header('5.25x7-1/4', b= 5.25*in2meter, h= 7.25*in2meter, Ms= 11755*pound2N*foot2meter, Vs= 7232*pound2N, linearDensity= 10.9*pound2kg)
LVLHeaders['5.25x9-1/4']= LVL_2900Fb2E_Header('5.25x9-1/4', b= 5.25*in2meter, h= 9.25*in2meter, Ms= 18624*pound2N*foot2meter, Vs= 9227*pound2N, linearDensity= 13.9*pound2kg)
LVLHeaders['5.25x9-1/2']= LVL_2900Fb2E_Header('5.25x9-1/2', b= 5.25*in2meter, h= 9.5*in2meter, Ms= 19586*pound2N*foot2meter, Vs= 9476*pound2N, linearDensity= 14.3*pound2kg)
LVLHeaders['5.25x11-1/4']= LVL_2900Fb2E_Header('5.25x11-1/4', b= 5.25*in2meter, h= 11.25*in2meter, Ms= 26955*pound2N*foot2meter, Vs= 11222*pound2N, linearDensity= 16.9*pound2kg)
LVLHeaders['5.25x11-7/8']= LVL_2900Fb2E_Header('5.25x11-7/8', b= 5.25*in2meter, h= 11.875*in2meter, Ms= 29854*pound2N*foot2meter, Vs= 11845*pound2N, linearDensity= 17.8*pound2kg)
LVLHeaders['5.25x14']= LVL_2900Fb2E_Header('5.25x14', b= 5.25*in2meter, h= 14*in2meter, Ms= 40543*pound2N*foot2meter, Vs= 13965*pound2N, linearDensity= 21*pound2kg)
LVLHeaders['5.25x16']= LVL_2900Fb2E_Header('5.25x16', b= 5.25*in2meter, h= 16*in2meter, Ms= 51954*pound2N*foot2meter, Vs= 15960*pound2N, linearDensity= 24*pound2kg)
LVLHeaders['5.25x18']= LVL_2900Fb2E_Header('5.25x18', b= 5.25*in2meter, h= 18*in2meter, Ms= 64657*pound2N*foot2meter, Vs= 17955*pound2N, linearDensity= 27*pound2kg)

LVLHeaders['7x7-1/4']= LVL_2900Fb2E_Header('7x7-1/4', b= 7*in2meter, h= 7.25*in2meter, Ms= 15673*pound2N*foot2meter, Vs= 9643*pound2N, linearDensity= 14.5*pound2kg)
LVLHeaders['7x9-1/4']= LVL_2900Fb2E_Header('7x9-1/4', b= 7*in2meter, h= 9.25*in2meter, Ms= 24832*pound2N*foot2meter, Vs= 12303*pound2N, linearDensity= 18.5*pound2kg)
LVLHeaders['7x9-1/2']= LVL_2900Fb2E_Header('7x9-1/2', b= 7*in2meter, h= 9.5*in2meter, Ms= 26115*pound2N*foot2meter, Vs= 12635*pound2N, linearDensity= 19*pound2kg)
LVLHeaders['7x11-1/4']= LVL_2900Fb2E_Header('7x11-1/4', b= 7*in2meter, h= 11.25*in2meter, Ms= 35940*pound2N*foot2meter, Vs= 14963*pound2N, linearDensity= 22.5*pound2kg)
LVLHeaders['7x11-7/8']= LVL_2900Fb2E_Header('7x11-7/8', b= 7*in2meter, h= 11.875*in2meter, Ms= 39805*pound2N*foot2meter, Vs= 15794*pound2N, linearDensity= 23.8*pound2kg)
LVLHeaders['7x14']= LVL_2900Fb2E_Header('7x14', b= 7*in2meter, h= 14*in2meter, Ms= 54057*pound2N*foot2meter, Vs= 18620*pound2N, linearDensity= 28*pound2kg)
LVLHeaders['7x16']= LVL_2900Fb2E_Header('7x16', b= 7*in2meter, h= 16*in2meter, Ms= 69272*pound2N*foot2meter, Vs= 21280*pound2N, linearDensity= 32*pound2kg)
LVLHeaders['7x18']= LVL_2900Fb2E_Header('7x18', b= 7*in2meter, h= 18*in2meter, Ms= 86209*pound2N*foot2meter, Vs= 23940*pound2N, linearDensity= 36.1*pound2kg)

# Southern Pine reference design values based on SPIB grading rules
# and AWC National Design Specification

southern_pine_wide_values= [0.000, 0.038, 0.140, 0.184, 0.235, 0.286]
southern_pine_no_1_Fb= [10.30E+06, 10.30E+06, 9.30E+06, 8.60E+06, 7.20E+06, 6.90E+06]
southern_pine_no_1_Ft= [6.90E+06, 6.90E+06, 6.00E+06, 5.50E+06, 4.80E+06, 4.50E+06]
southern_pine_no_1_Fv= 1.2e6
southern_pine_no_1_Fct= 3.9e6
southern_pine_no_1_Fc= [11.40E+06, 11.40E+06, 10.70E+06, 10.30E+06, 10.00E+06, 9.70E+06]
southern_pine_no_1_E= 11e9
southern_pine_no_1_Emin= 4e9

southern_pine_no_1_dense_Fb= [11.40E+06, 11.40E+06, 10.30E+06, 9.30E+06, 8.30E+06, 7.60E+06]
southern_pine_no_1_dense_Ft= [7.60E+06, 7.60E+06, 6.90E+06, 6.20E+06, 5.50E+06, 5.20E+06]
southern_pine_no_1_dense_Fv= 1.2e6
southern_pine_no_1_dense_Fct= 4.6e6
southern_pine_no_1_dense_Fc= [12.10E+06, 12.10E+06, 11.40E+06, 11.00E+06, 10.70E+06, 10.30E+06]
southern_pine_no_1_dense_E= 12.4e9
southern_pine_no_1_dense_Emin= 4.6e9

southern_pine_no_1_non_dense_Fb= [9.00E+06, 9.00E+06, 8.30E+06, 7.60E+06, 6.60E+06, 6.20E+06]
southern_pine_no_1_non_dense_Ft= [6.00E+06, 6.00E+06, 5.30E+06, 4.80E+06, 4.30E+06, 4.00E+06]
southern_pine_no_1_non_dense_Fv= 1.2e6
southern_pine_no_1_non_dense_Fct= 3.3e6
southern_pine_no_1_non_dense_Fc= [10.70E+06, 10.70E+06, 10.00E+06, 9.70E+06, 9.70E+06, 9.30E+06]
southern_pine_no_1_non_dense_E= 9.7e9
southern_pine_no_1_non_dense_Emin= 3.5e9

southern_pine_no_2_Fb= [7.60E+06, 7.60E+06, 6.90E+06, 6.40E+06, 5.50E+06, 5.20E+06]
southern_pine_no_2_Ft= [4.70E+06, 4.70E+06, 4.10E+06, 3.80E+06, 3.30E+06, 3.10E+06]
southern_pine_no_2_Fv= 1.2e6
southern_pine_no_2_Fct= 3.9e6
southern_pine_no_2_Fc= [10.00E+06, 10.00E+06, 9.70E+06, 9.30E+06, 9.00E+06, 8.60E+06]
southern_pine_no_2_E= 9.7e9
southern_pine_no_2_Emin= 3.5e9

southern_pine_no_2_dense_Fb= [8.30E+06, 8.30E+06, 7.20E+06, 6.70E+06, 5.90E+06, 5.50E+06]
southern_pine_no_2_dense_Ft= [5.20E+06, 5.20E+06, 4.50E+06, 4.10E+06, 3.60E+06, 3.40E+06]
southern_pine_no_2_dense_Fv= 1.2e6
southern_pine_no_2_dense_Fct= 4.6e6
southern_pine_no_2_dense_Fc= [10.30E+06, 10.30E+06, 10.00E+06, 9.70E+06, 9.30E+06, 9.00E+06]
southern_pine_no_2_dense_E= 11e9
southern_pine_no_2_dense_Emin= 4e9

southern_pine_no_2_non_dense_Fb= [7.20E+06, 7.20E+06, 6.60E+06, 6.00E+06, 5.20E+06, 4.80E+06]
southern_pine_no_2_non_dense_Ft= [4.10E+06, 4.10E+06, 3.60E+06, 3.40E+06, 2.90E+06, 2.80E+06]
southern_pine_no_2_non_dense_Fv= 1.2e6
southern_pine_no_2_non_dense_Fct= 3.3e6
southern_pine_no_2_non_dense_Fc= [10.00E+06, 10.00E+06, 9.30E+06, 9.00E+06, 8.60E+06, 8.60E+06]
southern_pine_no_2_non_dense_E= 9e9
southern_pine_no_2_non_dense_Emin= 3.2e9

southern_pine_no_3_Fb= [4.50E+06, 4.50E+06, 4.00E+06, 3.60E+06, 3.30E+06, 3.10E+06]
southern_pine_no_3_Ft= [2.80E+06, 2.80E+06, 2.40E+06, 2.20E+06, 1.90E+06, 1.70E+06]
southern_pine_no_3_Fv= 1.2e6
southern_pine_no_3_Fct= [3.90E+06, 3.90E+06, 3.90E+06, 3.90E+06, 3.90E+06, 3.90E+06]
southern_pine_no_3_Fc= [5.90E+06, 5.90E+06, 5.50E+06, 5.30E+06, 5.20E+06, 5.00E+06]
southern_pine_no_3_E= 9e9
southern_pine_no_3_Emin= 3.2e9

southern_pine_stud_Fb= southern_pine_no_3_Fb
southern_pine_stud_Ft= southern_pine_no_3_Ft
southern_pine_stud_Fv= southern_pine_no_3_Fv
southern_pine_stud_Fct= southern_pine_no_3_Fct
southern_pine_stud_Fc= southern_pine_no_3_Fc
southern_pine_stud_E= southern_pine_no_3_E
southern_pine_stud_Emin= southern_pine_no_3_Emin

southern_pine_structural_Fb= [16.20E+06, 16.20E+06, 14.50E+06, 13.40E+06, 11.70E+06, 11.00E+06]
southern_pine_structural_Ft= [11.40E+06, 11.40E+06, 10.00E+06, 9.30E+06, 7.90E+06, 7.60E+06]
southern_pine_structural_Fv= 1.2e6
southern_pine_structural_Fct= 3.9e6
southern_pine_structural_Fc= [13.10E+06, 13.10E+06, 12.40E+06, 11.70E+06, 11.40E+06, 11.40E+06]
southern_pine_structural_E= 12.4e9
southern_pine_structural_Emin= 4.6e9

southern_pine_structural_dense_Fb= [18.60E+06, 18.60E+06, 16.50E+06, 15.20E+06, 13.40E+06, 12.40E+06]
southern_pine_structural_dense_Ft= [13.10E+06, 13.10E+06, 11.40E+06, 10.70E+06, 9.00E+06, 8.60E+06]
southern_pine_structural_dense_Fv= 1.2e6
southern_pine_structural_dense_Fct= 4.6e6
southern_pine_structural_dense_Fc= [14.10E+06, 14.10E+06, 13.10E+06, 12.80E+06, 12.40E+06, 12.10E+06]
southern_pine_structural_dense_E= 13.1e9
southern_pine_structural_dense_Emin= 4.8e9

southern_pine_structural_non_dense_Fb= [14.10E+06, 14.10E+06, 12.80E+06, 11.70E+06, 10.30E+06, 9.70E+06]
southern_pine_structural_non_dense_Ft= [10.00E+06, 10.00E+06, 9.00E+06, 8.30E+06, 7.20E+06, 6.70E+06]
southern_pine_structural_non_dense_Fv= 1.2e6
southern_pine_structural_non_dense_Fct= 3.3e6
southern_pine_structural_non_dense_Fc= [12.40E+06, 12.40E+06, 11.70E+06, 11.40E+06, 11.00E+06, 10.70E+06]
southern_pine_structural_non_dense_E= 11e9
southern_pine_structural_non_dense_Emin= 4e9

southern_pine_no_1_Fb_interp= scipy.interpolate.interp1d(southern_pine_wide_values,southern_pine_no_1_Fb)
southern_pine_no_1_Ft_interp= scipy.interpolate.interp1d(southern_pine_wide_values,southern_pine_no_1_Ft)
southern_pine_no_1_Fc_interp= scipy.interpolate.interp1d(southern_pine_wide_values,southern_pine_no_1_Fc)

southern_pine_no_1_dense_Fb_interp= scipy.interpolate.interp1d(southern_pine_wide_values,southern_pine_no_1_dense_Fb)
southern_pine_no_1_dense_Ft_interp= scipy.interpolate.interp1d(southern_pine_wide_values,southern_pine_no_1_dense_Ft)
southern_pine_no_1_dense_Fc_interp= scipy.interpolate.interp1d(southern_pine_wide_values,southern_pine_no_1_dense_Fc)

southern_pine_no_1_non_dense_Fb_interp= scipy.interpolate.interp1d(southern_pine_wide_values,southern_pine_no_1_non_dense_Fb)
southern_pine_no_1_non_dense_Ft_interp= scipy.interpolate.interp1d(southern_pine_wide_values,southern_pine_no_1_non_dense_Ft)
southern_pine_no_1_non_dense_Fc_interp= scipy.interpolate.interp1d(southern_pine_wide_values,southern_pine_no_1_non_dense_Fc)

southern_pine_no_2_Fb_interp= scipy.interpolate.interp1d(southern_pine_wide_values,southern_pine_no_2_Fb)
southern_pine_no_2_Ft_interp= scipy.interpolate.interp1d(southern_pine_wide_values,southern_pine_no_2_Ft)
southern_pine_no_2_Fc_interp= scipy.interpolate.interp1d(southern_pine_wide_values,southern_pine_no_2_Fc)

southern_pine_no_2_dense_Fb_interp= scipy.interpolate.interp1d(southern_pine_wide_values,southern_pine_no_2_dense_Fb)
southern_pine_no_2_dense_Ft_interp= scipy.interpolate.interp1d(southern_pine_wide_values,southern_pine_no_2_dense_Ft)
southern_pine_no_2_dense_Fc_interp= scipy.interpolate.interp1d(southern_pine_wide_values,southern_pine_no_2_dense_Fc)

southern_pine_no_2_non_dense_Fb_interp= scipy.interpolate.interp1d(southern_pine_wide_values,southern_pine_no_2_non_dense_Fb)
southern_pine_no_2_non_dense_Ft_interp= scipy.interpolate.interp1d(southern_pine_wide_values,southern_pine_no_2_non_dense_Ft)
southern_pine_no_2_non_dense_Fc_interp= scipy.interpolate.interp1d(southern_pine_wide_values,southern_pine_no_2_non_dense_Fc)

southern_pine_no_3_Fb_interp= scipy.interpolate.interp1d(southern_pine_wide_values,southern_pine_no_3_Fb)
southern_pine_no_3_Ft_interp= scipy.interpolate.interp1d(southern_pine_wide_values,southern_pine_no_3_Ft)
southern_pine_no_3_Fc_interp= scipy.interpolate.interp1d(southern_pine_wide_values,southern_pine_no_3_Fc)

southern_pine_stud_Fb_interp= southern_pine_no_3_Fb_interp 
southern_pine_stud_Ft_interp= southern_pine_no_3_Ft_interp 
southern_pine_stud_Fc_interp= southern_pine_no_3_Fc_interp 

southern_pine_structural_Fb_interp= scipy.interpolate.interp1d(southern_pine_wide_values,southern_pine_structural_Fb)
southern_pine_structural_Ft_interp= scipy.interpolate.interp1d(southern_pine_wide_values,southern_pine_structural_Ft)
southern_pine_structural_Fc_interp= scipy.interpolate.interp1d(southern_pine_wide_values,southern_pine_structural_Fc)

southern_pine_structural_dense_Fb_interp= scipy.interpolate.interp1d(southern_pine_wide_values,southern_pine_structural_dense_Fb)
southern_pine_structural_dense_Ft_interp= scipy.interpolate.interp1d(southern_pine_wide_values,southern_pine_structural_dense_Ft)
southern_pine_structural_dense_Fc_interp= scipy.interpolate.interp1d(southern_pine_wide_values,southern_pine_structural_dense_Fc)

southern_pine_structural_non_dense_Fb_interp= scipy.interpolate.interp1d(southern_pine_wide_values,southern_pine_structural_non_dense_Fb)
southern_pine_structural_non_dense_Ft_interp= scipy.interpolate.interp1d(southern_pine_wide_values,southern_pine_structural_non_dense_Ft)
southern_pine_structural_non_dense_Fc_interp= scipy.interpolate.interp1d(southern_pine_wide_values,southern_pine_structural_non_dense_Fc)

southern_pine_no_1_E= 11e9
southern_pine_no_1_Emin= 4e9

class SouthernPineDimensionLumber(sp.RectangularSection):
    ''' Southern pine dimension lumber.'''
    nu= 0.2
    def __init__(self, name, b, h, grade= 'no_2', sub_grade= ''):
        '''Constructor.'''
        super(SouthernPineDimensionLumber,self).__init__(name, b, h)
        self.rho= (657+577)/2.0 # kg/m3
        self.xc_material_name= 'southern_pine_'+grade+'_'+sub_grade
        self.xc_material= None
        self.xc_section= None
        if(grade=='structural'):
            if(sub_grade==''):
                self.Fb= southern_pine_structural_Fb_interp
                self.Ft= southern_pine_structural_Ft_interp
                self.Fv= southern_pine_structural_Fv
                self.Fct= southern_pine_structural_Fct
                self.Fc= southern_pine_structural_Fc_interp
                self.E= southern_pine_structural_E
                self.Emin= southern_pine_structural_Emin
            elif(sub_grade=='dense'):
                self.Fb= southern_pine_structural_dense_Fb_interp
                self.Ft= southern_pine_structural_dense_Ft_interp
                self.Fv= southern_pine_structural_dense_Fv
                self.Fct= southern_pine_structural_dense_Fct
                self.Fc= southern_pine_structural_dense_Fc_interp
                self.E= southern_pine_structural_dense_E
                self.Emin= southern_pine_structural_dense_Emin
            elif(sub_grade=='non_dense'):
                self.Fb= southern_pine_structural_non_dense_Fb_interp
                self.Ft= southern_pine_structural_non_dense_Ft_interp
                self.Fv= southern_pine_structural_non_dense_Fv
                self.Fct= southern_pine_structural_non_dense_Fct
                self.Fc= southern_pine_structural_non_dense_Fc_interp
                self.E= southern_pine_structural_non_dense_E
                self.Emin= southern_pine_structural_non_dense_Emin
        elif(grade=='no_1'):
            if(sub_grade==''):
                self.Fb= southern_pine_no_1_Fb_interp
                self.Ft= southern_pine_no_1_Ft_interp
                self.Fv= southern_pine_no_1_Fv
                self.Fct= southern_pine_no_1_Fct
                self.Fc= southern_pine_no_1_Fc_interp
                self.E= southern_pine_no_1_E
                self.Emin= southern_pine_no_1_Emin
            elif(sub_grade=='dense'):
                self.Fb= southern_pine_no_1_dense_Fb_interp
                self.Ft= southern_pine_no_1_dense_Ft_interp
                self.Fv= southern_pine_no_1_dense_Fv
                self.Fct= southern_pine_no_1_dense_Fct
                self.Fc= southern_pine_no_1_dense_Fc_interp
                self.E= southern_pine_no_1_dense_E
                self.Emin= southern_pine_no_1_dense_Emin
            elif(sub_grade=='non_dense'):
                self.Fb= southern_pine_no_1_non_dense_Fb_interp
                self.Ft= southern_pine_no_1_non_dense_Ft_interp
                self.Fv= southern_pine_no_1_non_dense_Fv
                self.Fct= southern_pine_no_1_non_dense_Fct
                self.Fc= southern_pine_no_1_non_dense_Fc_interp
                self.E= southern_pine_no_1_non_dense_E
                self.Emin= southern_pine_no_1_non_dense_Emin
        elif(grade=='no_2'):
            if(sub_grade==''):
                self.Fb= southern_pine_no_2_Fb_interp
                self.Ft= southern_pine_no_2_Ft_interp
                self.Fv= southern_pine_no_2_Fv
                self.Fct= southern_pine_no_2_Fct
                self.Fc= southern_pine_no_2_Fc_interp
                self.E= southern_pine_no_2_E
                self.Emin= southern_pine_no_2_Emin
            elif(sub_grade=='dense'):
                self.Fb= southern_pine_no_2_dense_Fb_interp
                self.Ft= southern_pine_no_2_dense_Ft_interp
                self.Fv= southern_pine_no_2_dense_Fv
                self.Fct= southern_pine_no_2_dense_Fct
                self.Fc= southern_pine_no_2_dense_Fc_interp
                self.E= southern_pine_no_2_dense_E
                self.Emin= southern_pine_no_2_dense_Emin
            elif(sub_grade=='non_dense'):
                self.Fb= southern_pine_no_2_non_dense_Fb_interp
                self.Ft= southern_pine_no_2_non_dense_Ft_interp
                self.Fv= southern_pine_no_2_non_dense_Fv
                self.Fct= southern_pine_no_2_non_dense_Fct
                self.Fc= southern_pine_no_2_non_dense_Fc_interp
                self.E= southern_pine_no_2_non_dense_E
                self.Emin= southern_pine_no_2_non_dense_Emin
        elif(grade=='no_3' or grade=='stud'):
            if(sub_grade==''):
                self.Fb= southern_pine_no_3_Fb_interp
                self.Ft= southern_pine_no_3_Ft_interp
                self.Fv= southern_pine_no_3_Fv
                self.Fct= southern_pine_no_3_Fct
                self.Fc= southern_pine_no_3_Fc_interp
                self.E= southern_pine_no_3_E
                self.Emin= southern_pine_no_3_Emin
            else:
                lmsg.error('number 3 and stud grades have not sub grades.')
    def getFb(self):
        return self.Fb(self.h)
    def defXCMaterial(self):
        '''Defines the material in XC.'''
        if(not self.xc_material):
            self.xc_material= typical_materials.MaterialData(name= self.xc_material_name, E= self.E, nu= self.nu, rho= self.rho)
        return self.xc_material
    def defElasticShearSection2d(self, preprocessor):
        mat= self.defXCMaterial()
        self.xc_section= super(Header,self).defElasticShearSection2d(preprocessor,mat)
        return self.xc_section
    def defElasticShearSection3d(self, preprocessor):
        mat= self.defXCMaterial()
        self.xc_section= super(Header,self).defElasticShearSection3d(preprocessor,mat)
        return self.xc_section
