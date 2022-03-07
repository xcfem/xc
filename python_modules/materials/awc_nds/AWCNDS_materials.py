# -*- coding: utf-8 -*-
''' Wood construction materials according to
    National Design Specification for Wood Construction
    2018 Edition. 
VERIFICATION TESTS REQUIRED!!!.'''

from __future__ import division
from __future__ import print_function

import math
import sys
from enum import IntEnum
import scipy.interpolate
from misc_utils import log_messages as lmsg
from materials import wood_base
from materials import typical_materials
from materials.sections import section_properties as sp
from postprocess import def_vars_control as vc
from materials import typical_materials
from materials import steel_member_base
import pickle
from misc_utils import pickle_utils
import os
from model import model_inquiry

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

def getLoadCombinationDurationFactor(deadLoad= False, liveLoad= False, constructionLoad= False, snowLoad= False, windLoad= False):
    ''' Return the duration factor according to appendix B Section B.2
        of AWC-NDS2018 (Non-mandatory).

    :param deadLoad: if true the dead load is present in the load combination.
    :param liveLoad: if true the live load is present in the load combination.
    :param constructionLoad: if true the construction load (short live load) is present in the load combination.
    :param snowLoad: if true the snow load is present in the load combination.
    :param windLoad: if true the wind load is present in the load combination.
    '''
    retval= 1.0
    deadLoadOnly= deadLoad and not (constructionLoad or liveLoad or snowLoad or windLoad)
    if(deadLoadOnly):
        retval= 0.9
    elif(deadLoad and liveLoad and not (windLoad or snowLoad)):
        retval= 1.0
    elif(deadLoad and snowLoad and not (liveLoad or windLoad)):
        retval= 1.15
    elif(deadLoad and constructionLoad and not (liveLoad or windLoad or snowLoad)):
        retval= 1.25
    elif(deadLoad and windLoad and not (liveLoad or snowLoad)):
        retval= 1.6
    elif(deadLoad and liveLoad and snowLoad and not windLoad):
        retval= 1.15
    elif(deadLoad and liveLoad and windLoad and not snowLoad):
        retval= 1.6
    elif(deadLoad and snowLoad and windLoad and not liveLoad):
        retval= 1.6
    else:
        retval= 1.6
    return retval

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

class Wood(wood_base.Wood):
    '''Base class for wood materials according
       to AWC-NDS2018.

    ;ivar CD: load duration factor.
    '''
    def __init__(self, name, specificGravity= None, CD= 1.0):
        '''Constructor.

        :param name: wood name.
        :param specificGravity: specific gravity of the wood.   
        :param CD: load duration factor.
        '''
        super(Wood,self).__init__(name, specificGravity)
        self.CD= CD
        
    def getDowelBearingStrength(self, diameter, theta, endGrain= False):
        ''' Return the dowel bearing strength for Dowel-Type
            Fasteners in Wood Members according to Eq.
            12.3-11 and table 12.3.3 of National Design Specification of
            the American Wood Council

        :param diameter: diameter of the dowel-type fastener. 
        :param theta: angle between the direction of load and the
                      direction of grain (longitudinal axis of member).
        :param endGrain: true for dowel-type fasteners with D > 1/4" that are 
                         inserted into the end grain of the main member, 
                         with the fastener axis parallel to the wood fibers.
                         See clause 12.3.3.4 of NDS.
        '''
        retval= 0.0
        if(diameter<0.25*in2meter):
            retval= 16600.0*pow(self.specificGravity,1.84)
        else:
            Fe_perp= 6100.0*pow(self.specificGravity,1.84)/math.sqrt(diameter)
            if(endGrain):
                retval= Fe_perp
            else:
                Fe_parallel= 11200*self.specificGravity
                retval= Fe_parallel*Fe_perp/(Fe_parallel*math.sin(theta)**2+Fe_perp*math.cos(theta)**2)
        retval*= psi2Pa
        return retval


class LSL(Wood):
    ''' Base class for laminated strand lumber (LSL)'''
    def __init__(self, name= 'LSL'):
        ''' Constructor.

        :param name: material name.
        '''
        super(LSL,self).__init__(name)
        
    def getFb(self,depth):
        ''' Return the allowable bending stress given
            the member depth.

        :param depth: member depth.
        '''
        if(depth<3.5*in2meter):
            return 1.159*self.Fb_12
        else:
            return math.pow((12.0/depth),0.12)*self.Fb_12

    def get37_1c(self):
        ''' Return the c coefficient for equation 3.7-1 of
            AWC_NDS2018 (clause 3.7.1 column stability factor.'''
        return 0.9 # structural glued laminated timber,
                   # structural composite lumber, and cross-laminated timber

class LSL_135E(LSL):
    ''' LSL 1.35E.'''
    E= 1.35e6*psi2Pa # Elastic modulus (Pa)
    Emin= E
    Fb_12= 1730*psi2Pa # Bending stress for 12" depth.
    Fv= 410*psi2Pa # Shear stress.
    Fc_pll= 1650*psi2Pa # Compression stress (parallel to grain)
    Fc_perp= 750*psi2Pa # Compression stress (perpendicular to grain)
    xc_material_name= 'LSL_135E'
    def __init__(self, name= 'LSL_135E'):
        ''' Constructor.

        :param name: material name.
        '''
        super(LSL_135E,self).__init__(name)
        
    def getFb(self,depth):
        ''' Return the allowable bending stress given
            the member depth.

        :param depth: member depth.
        '''
        if(depth<3.5*in2meter):
            return 1.159*Fb_12
        else:
            return math.pow((12.0/depth),0.12)*Fb_12

class LSL_155E(LSL):
    ''' LSL 1.35E.'''
    E= 1.55e6*psi2Pa # Elastic modulus (Pa)
    Emin= E
    Fb_12= 2360*psi2Pa # Bending stress for 12" depth.
    Fv= 410*psi2Pa # Shear stress.
    Fc_pll= 2175*psi2Pa # Compression stress (parallel to grain)
    Fc_perp= 875*psi2Pa # Compression stress (perpendicular to grain)
    xc_material_name= 'LSL_155E'
    def __init__(self, name= 'LSL_135E'):
        ''' Constructor.

        :param name: material name.
        '''
        super(LSL_155E,self).__init__(name)

class LVL_2900Fb2E(LSL):
    ''' LVL 2900Fb 2.0E structural.'''
    E= 2.0e6*psi2Pa # Elastic modulus (Pa)
    Emin= E
    Fb_12= 2900*psi2Pa # Bending stress for 12" depth.
    Fv= 285*psi2Pa # Shear stress.
    Fc_pll= 3200*psi2Pa # Compression stress (parallel to grain)
    Fc_perp= 750*psi2Pa # Compression stress (perpendicular to grain)
    xc_material_name= 'LVL_2900Fb2E'
    def __init__(self, name= 'LVL_2900Fb2E'):
        ''' Constructor.

        :param name: material name.
        '''
        super(LVL_2900Fb2E,self).__init__(name)

class MemberRestraint(IntEnum):
    ''' Member restrain condition according  to clause 4.4.1.2 of
        AWC_NDS2018.'''
    notApplicable= -1 # No sawn lumber...
    noLateralSupport= 0 # 4.4.1.2 (a)
    endSupport= 1 # 4.4.1.2 (b)
    compressionEdgeSupport= 2 # 4.4.1.2 (c)
    fullDepthSolidBlocking= 3 # 4.4.1.2 (d)
    bothEdgeSupport= 4 # 4.4.1.2 (e)

class MemberLoadingCondition(object):
    ''' Parameters defining the member condition in order to obtain
        its effective length accordint to table 3.3.3 of
        AWC NDS-2018

    :ivar numberOfConcentratedLoads: number of concentrated loads equally
                                      spaced along the beam (0: uniform load).
    :ivar lateralSupport: if true beam has a lateral support on each load.
    :ivar cantilever: if true cantilever beam otherwise single span beam.    
    '''
    def __init__(self, numberOfConcentratedLoads= 0, lateralSupport= False, cantilever= False):
        ''' Constructor.

        :param numberOfConcentratedLoads: number of concentrated loads equally
                                          spaced along the beam (0: uniform load).
        :param lateralSupport: if true beam has a lateral support on each load.
        :param cantilever: if true cantilever beam otherwise single span beam.
        '''
        self.numberOfConcentratedLoads= numberOfConcentratedLoads
        self.lateralSupport= lateralSupport
        self.cantilever= cantilever

    def getEffectiveLength(self, unbracedLength, section, majorAxis= True):
        ''' Return the effective length of the member according to table
            3.3.3 of NDS-2018.

        :param unbracedLength: unbraced length for the bending axis.
        :param section: member cross-section.
        :param majorAxis: if true return the bending axis is the major axis.
        '''
        retval= unbracedLength
        h= section.h
        if(not majorAxis):
            h= section.b
        if(self.cantilever):
            if(self.numberOfConcentratedLoads==0):
                retval*= 1.33 # Uniform load
            else:
                retval*= 1.87 # Concentrated load at unsupported end.
        else:
            ratio= unbracedLength/h
            if(self.numberOfConcentratedLoads==0):
                if(ratio<7.0):
                    retval*= 2.06 # Uniform load
                else:
                    retval= 1.63*unbracedLength+3.0*h
            elif((self.numberOfConcentratedLoads==1) and (not self.lateralSupport)):
                if(ratio<7.0):
                    retval*= 1.80 # Uniform load
                else:
                    retval= 1.37*unbracedLength+3.0*h
            elif(self.numberOfConcentratedLoads==1):
                if(self.lateralSupport):
                    retval*=1.11
                else:
                    lmsg.error('Load condition case not implemented.')
                    retval*=10.0
            elif(self.numberOfConcentratedLoads==2):
                if(self.lateralSupport):
                    retval*=1.68
                else:
                    lmsg.error('Load condition case not implemented.')
                    retval*=10.0
            elif(self.numberOfConcentratedLoads==3):
                if(self.lateralSupport):
                    retval*=1.54
                else:
                    lmsg.error('Load condition case not implemented.')
                    retval*=10.0
            elif(self.numberOfConcentratedLoads==4):
                if(self.lateralSupport):
                    retval*=1.68
                else:
                    lmsg.error('Load condition case not implemented.')
                    retval*=10.0
            elif(self.numberOfConcentratedLoads==5):
                if(self.lateralSupport):
                    retval*=1.73
                else:
                    lmsg.error('Load condition case not implemented.')
                    retval*=10.0
            elif(self.numberOfConcentratedLoads==6):
                if(self.lateralSupport):
                    retval*=1.78
                else:
                    lmsg.error('Load condition case not implemented.')
                    retval*=10.0
            elif(self.numberOfConcentratedLoads==7):
                if(self.lateralSupport):
                    retval*=1.84
                else:
                    lmsg.error('Load condition case not implemented.')
                    retval*=10.0
        return retval

        
class WoodSection(object):
    ''' Wood structural cross-section.

    :ivar name: section name.
    :ivar xc_wood_material: section wood material.
    :ivar xc_section: XC section material.
    :ivar Cr: repetitive member factor.
    '''
    def __init__(self, name, Cr= 1.0):
        ''' Constructor.

        :param Cr: repetitive member adjustment factor
        '''
        self.name= name
        self.xc_wood_material= None
        self.xc_section= None
        self.Cr= Cr
        
    def updateLoadDurationFactor(self, CD):
        '''Update the value of the load duration factor.'''
        # Does nothing. Redefine when necessary.
        
    def setupULSControlVars(self, elems, chiN=1.0, chiLT=1.0, FcE=(0.0,0.0), FbE= 0.0):
        '''For each element creates the variables
           needed to check ultimate limit state criterion to satisfy.

        :param elems: elements to define properties on.
        :param chiN: column stability factor clause 3.7.1 of AWC-NDS2018 (default= 1.0).
        :param chiLT: beam stability factor clause 3.3.3 of AWC-NDS2018 (default= 1.0).
        :param FcE: Critical buckling design values for compression.
        :param FbE: Critical buckling design value for bending.
        '''
        vc.defVarsEnvelopeInternalForcesBeamElems(elems)
        for e in elems:
            e.setProp('chiLT',chiLT) # Lateral torsional buckling reduction factor.
            e.setProp('chiN',chiN) # Axial strength reduction factor.
            e.setProp('FcE', FcE) # Critical buckling design values for compression.
            e.setProp('FbE', FbE) # Critical buckling design value for bending.
            e.setProp('FCTNCP',[-1.0,-1.0]) #Normal stresses efficiency.
            e.setProp('FCVCP',[-1.0,-1.0]) #Shear stresses efficiency.
            e.setProp('crossSection', self)
            
    def getCompressiveStrength(self):
        ''' Return the value of the compressive strength of the section.
        '''
        return self.getFcAdj()*self.A()
    
    def getTensileStrength(self):
        ''' Return the value of the tensile strength of the section.

        '''
        return self.getFtAdj()*self.A()
        
    def getFlexuralStrength(self, majorAxis= True, chiLT= 1.0):
        ''' Return the value of the flexural strength of the section.

        :param majorAxis: if true, return the major flexural strength.
        :param chiLT: beam stability factor clause 3.3.3 of AWC-NDS2018 (default= 1.0).
        '''
        retval= self.getFbAdj(majorAxis= majorAxis)*chiLT
        if(majorAxis):
            retval*= self.getElasticSectionModulusZ()
        else:
            retval*= self.getElasticSectionModulusY()
        return retval

    def getShearStrength(self, majorAxis= True):
        ''' Return the value of the flexural strength of the section.
        '''
        return self.getFvAdj()*self.A()/1.5

    def getAxialEfficiency(self, Nd, chiN):
        '''Return axial efficiency according to AWC-NDS2018.

        :param Nd: required axial strength.
        :param chiN: column stability factor clause 3.7.1 of AWC-NDS2018 (default= 1.0).
        '''
        ratioN= 0.0
        if(Nd<=0): # compression
            NcRd= chiN*self.getCompressiveStrength() # available axial strength.
            ratioN=  abs(Nd)/NcRd
        else:
            NcRd= chiN*self.getTensileStrength() # available axial strength.
            ratioN= Nd/NcRd
        return ratioN
    
    def getFlexuralEfficiency(self, Md , chiLT, majorAxis= True):
        '''Return biaxial bending efficiency according to clause
           3.9 of AWC-NDS2018.

        :param Md: required bending strength.
        :param chiLT: beam stability factor clause 3.3.3 of AWC-NDS2018 (default= 1.0).
        :param majorAxis: if true, return the efficiency about the major axis.
        '''
        McRd= self.getFlexuralStrength(majorAxis= majorAxis, chiLT= chiLT) # available flexural strength.
        return abs(Md)/McRd

        
    def getBiaxialBendingEfficiency(self, Nd, Myd, Mzd, FcE= None, FbE= None, chiN=1.0, chiLT=1.0):
        '''Return biaxial bending efficiency according to clause
           3.9 of AWC-NDS2018.

        :param Nd: required axial strength.
        :param Myd: required bending strength (minor axis).
        :param Mzd: required bending strength (major axis).
        :param FcE: critical buckling design value for compression
                    members (F_{cE}) as defined in section 3.9.2 of NDS-2.018
        :param FbE: critical bucking design value for bending according to 
                    section 3.3.3.8 of NDS-2018.
        :param chiN: column stability factor clause 3.7.1 of AWC-NDS2018 (default= 1.0).
        :param chiLT: beam stability factor clause 3.3.3 of AWC-NDS2018 (default= 1.0).
        '''
        CF= None # Capacity factor (efficiency).
        ratioN= self.getAxialEfficiency(Nd, chiN)
        if(Myd==0.0 and Mzd==0.0):
            CF= ratioN
        else:
            stressTreshold= 1.0/self.A() # Small stress.
            axialStress= Nd/self.A() # ft (if Nd>0) or fc (if Nd<0)
            Sz= self.getElasticSectionModulusZ()
            fb1= abs(Mzd)/Sz # Bending stress (major axis)
            Sy= self.getElasticSectionModulusY()
            fb2= abs(Myd)/Sy # Bending stress (minor axis)
            if(abs(Myd)>abs(Mzd)): # bending on weak axis
                if(Nd>0):
                    CF= ratioN+fb2/Fb2_aster # equation 3.9-1
                    if(fb2>stressTreshold): # not so small stress. 
                        Fb2_aster= self.getFlexuralStrength(majorAxis= False, chiLT= 1.0)/Sy # No CL adjustement
                        Fb2_aster2= Fb2_aster*chiLT # CL adjustement; F'b2 in equation 3.9-3
                        eq392= (fb2-axialStress)/Fb2_aster2 # equation 3.9-2
                        CF= max(CF, eq392)
                elif(Nd<0):
                    # Equation 3.9-3
                    CF= ratioN**2
                    FcE2= FcE[1] # minor axis.
                    eq394= abs(axialStress)/FcE2
                    if(fb2>stressTreshold): # not so small stress. 
                        Fb2_aster= self.getFlexuralStrength(majorAxis= False, chiLT= 1.0)/Sy # No CL adjustement
                        Fb2_aster2= Fb2_aster*chiLT # CL adjustement; F'b2 in equation 3.9-3
                        CF+= fb2/(Fb2_aster2*(1+axialStress/FcE2)) # axialStress is negative so -*-=+
                        eq394+= (fb2/FbE)**2
                    if(fb1>stressTreshold): # not so small stress. 
                        FcE1= FcE[0] # major axis.
                        Fb1_aster= self.getFlexuralStrength(majorAxis= True, chiLT= 1.0)/Sz # No CL adjustement
                        Fb1_aster2= Fb1_aster*chiLT # CL adjustement; F'b1 in equation 3.9-3    
                        CF+= fb1/(Fb1_aster2*(1+axialStress/FcE1-(fb2/FbE)**2)) # axialStress is negative so -*-=+
                    CF= max(CF, eq394)
                else: # Nd==0
                    CF= self.getFlexuralEfficiency(Md= Myd, majorAxis= False, chiLT= chiLT) # available flexural strength minor axis.
            else: # bending on string axis.
                if(Nd>0):
                    CF= ratioN # equation 3.9-1 part 1
                    if(fb1>stressTreshold): # not so small stress.
                        Fb1_aster= self.getFlexuralStrength(majorAxis= True, chiLT= 1.0)/Sz # No CL adjustement
                        Fb1_aster2= Fb1_aster*chiLT # CL adjustement; F'b1 in equation 3.9-3    
                        CF+= fb1/Fb1_aster # equation 3.9-1 part 2
                        eq392= (fb1-axialStress)/Fb1_aster2 # equation 3.9-2
                        CF= max(CF, eq392)
                elif(Nd<0):
                    # Equation 3.9-3
                    CF= ratioN**2
                    if(fb1>stressTreshold): # not so small stress. 
                        FcE1= FcE[0] # major axis.
                        Fb1_aster= self.getFlexuralStrength(majorAxis= True, chiLT= 1.0)/Sz # No CL adjustement
                        Fb1_aster2= Fb1_aster*chiLT # CL adjustement; F'b1 in equation 3.9-3    
                        CF+= fb1/(Fb1_aster2*(1+axialStress/FcE1)) # axialStress is negative so -*-=+
                    if(fb2>stressTreshold): # not so small stress. 
                        FcE2= FcE[1] # minor axis.
                        Fb2_aster= self.getFlexuralStrength(majorAxis= False, chiLT= 1.0)/Sy # No CL adjustement
                        Fb2_aster2= Fb2_aster*chiLT # CL adjustement; F'b2 in equation 3.9-3
                        CF+= fb2/(Fb2_aster2*(1+axialStress/FcE2-(fb1/FbE)**2)) # axialStress is negative so -*-=+
                else: # Nd==0
                    CF= self.getFlexuralEfficiency(Md= Mzd, majorAxis= True, chiLT= chiLT) # reference flexural strength major axis.
        return (CF,)    

    
    def getYShearEfficiency(self, Vy):
        '''Return major axis shear efficiency according to AISC-360-16.

        :param Vy: required shear strength (major axis)
        '''
        return abs(Vy)/self.getShearStrength(majorAxis= True)
    
    def getZShearEfficiency(self, Vz):
        '''Return major axis shear efficiency according to AISC-360-16.

        :param Vz: required shear strength (minor axis)
        '''
        return Vz/self.getShearStrength(majorAxis= False)

    def checkBiaxialBendingForElement(self, elem, nmbComb):
        '''Called in every commit to check biaxial bending criterion 
            (bars in 3D problems).

        :param elem: finite element to check.
        :param nmbComb: name of the load combination.
        '''
        elem.getResistingForce()
        chiLT= elem.getProp('chiLT') # beam stability factor.
        chiN= elem.getProp('chiN') # column stability factor.
        FcE= elem.getProp('FcE') # critical buckling design value for compression
        FbE= elem.getProp('FbE') # critical buckling design value for bending
        [[N1, My1, Mz1, Vy1], [N2, My2, Mz2, Vy2]]= model_inquiry.getValuesAtNodes(elem, ['N', 'My', 'Mz', 'Vy'], silent= False)
        FCTN1= self.getBiaxialBendingEfficiency(Nd= N1, Myd= My1, Mzd= Mz1, FcE= FcE, FbE= FbE, chiN= chiN, chiLT= chiLT)[0]
        FCTN2= self.getBiaxialBendingEfficiency(Nd= N2, Myd= My2, Mzd= Mz2, FcE= FcE, FbE= FbE, chiN= chiN, chiLT= chiLT)[0]
        fctn= elem.getProp("FCTNCP")
        if(FCTN1 > fctn[0]):
            fctn[0]= FCTN1
            elem.setProp("HIPCPTN1",nmbComb)
        if(FCTN2 > fctn[1]):
            fctn[1]= FCTN2
            elem.setProp("HIPCPTN2",nmbComb)
        elem.setProp("FCTNCP",fctn)
        vc.updateEnvelopeInternalForcesBeamElem(elem)
        
    def checkYShearForElement(self,elem,nmbComb):
        '''Called in every commit to y shear criterion.

        :param elem: finite element to check.
        :param nmbComb: name of the load combination.
        '''
        elem.getResistingForce()
        [[Vy1], [Vy2]]= model_inquiry.getValuesAtNodes(elem,['Vy'], False)
        FCV1= self.getYShearEfficiency(Vy1)
        FCV2= self.getYShearEfficiency(Vy2)
        fcv= elem.getProp("FCVCP")
        if(FCV1 > fcv[0]):
            fcv[0]= FCV1
            elem.setProp("HIPCPV1",nmbComb)
        if(FCV2 > fcv[1]):
            fcv[1]= FCV2
            elem.setProp("HIPCPV2",nmbComb)
        elem.setProp("FCVCP",fcv)
        
    def checkZShearForElement(self,elem,nmbComb):
        '''Called in every commit to y shear criterion.

        :param elem: finite element to check.
        :param nmbComb: name of the load combination.
        '''
        elem.getResistingForce()
        [[Vz1], [Vz2]]= model_inquiry.getValuesAtNodes(elem,['Vz'], False)
        FCV1= self.getZShearEfficiency(Vz1)
        FCV2= self.getZShearEfficiency(Vz2)
        fcv= elem.getProp("FCVCP")
        if(FCV1 > fcv[0]):
            fcv[0]= FCV1
            elem.setProp("HIPCPV1",nmbComb)
        if(FCV2 > fcv[1]):
            fcv[1]= FCV2
            elem.setProp("HIPCPV2",nmbComb)
        elem.setProp("FCVCP",fcv)
        
    def getRequiredRestraint(self):
        ''' Return the required restrains according to clause 4.4.1.2 of
            AWC_NDS2018.'''
        return MemberRestraint.notApplicable

class WoodRectangularSection(WoodSection, sp.RectangularSection):
    ''' Wood structural cross-section.'''
    def __init__(self, name, b, h, Cr= 1.0):
        ''' Constructor.

        :param name: section name.
        :param b: section width.
        :param h: section height.
        :param Cr: repetitive member adjustment factor
        '''
        WoodSection.__init__(self, name, Cr= Cr)
        sp.RectangularSection.__init__(self, name= name, b= b, h= h)

        
class WoodPanelSection(WoodRectangularSection):
    ''' Wood structural panel.

    :ivar shearConstant: shear constant
    '''
    def __init__(self, name, b, h, shear_constant):
        ''' Constructor.

        :param name: section name.
        :param b: section width.
        :param h: section height.
        :param shear_constant: shear constant.
        '''
        super(WoodPanelSection,self).__init__(name, b, h)
        self.shearConstant= shear_constant
        
    def getArealDensity(self):
        ''' Return the mass per unit area (thickness= h).'''
        return self.rho*self.h
    
    def getSpanRating(self):
        ''' Return the span rating from the panel thickness according
            to the table B of the documents:
            "Design Capacities for Oriented Strand Board V1.0 – 01/2008"
            and "Design Capacities for Structural Plywood V1.0 – 01/2008"
            both from PFS TECO • 1507 Matt Pass, Cottage Grove, WI 53527, USA
        '''
        spanRating= 'unknown'
        if(self.name=='3/8'):
            spanRating= '24/0'
        elif(self.name=='7/16'):
            spanRating= '24/16'
        elif(self.name=='15/32'):
            spanRating= '32/16'
        elif(self.name=='1/2'):
            spanRating= '32/16'
        elif(self.name=='19/32'):
            spanRating= '40/20'
        elif(self.name=='5/8'):
            spanRating= '40/20'
        elif(self.name=='23/32'):
            spanRating= '48/24'
        elif(self.name=='3/4'):
            spanRating= '48/24'
        elif(self.name=='7/8'):
            spanRating= '48/24'
        elif(self.name=='1'):
            spanRating= '48/24'
        elif(self.name=='1-1/8'):
            spanRating= '48/24'
        else:
            lmsg.error('name: ', self.name, 'unknown')
        return spanRating
    
    def getAPARatedSturdIFloor(self):
        ''' Return the APA rated Sturd-I-Floor from the panel thickness 
            according to the table B of the documents:
            "Design Capacities for Oriented Strand Board V1.0 – 01/2008"
            and "Design Capacities for Structural Plywood V1.0 – 01/2008"
            both from PFS TECO • 1507 Matt Pass, Cottage Grove, WI 53527, USA
        '''
        spanRating= 'unknown'
        if(self.name=='3/8'):
            spanRating= '0oc'
        elif(self.name=='7/16'):
            spanRating= '0oc'
        elif(self.name=='15/32'):
            spanRating= '0oc'
        elif(self.name=='1/2'):
            spanRating= '0oc'
        elif(self.name=='19/32'):
            spanRating= '20oc'
        elif(self.name=='5/8'):
            spanRating= '20oc'
        elif(self.name=='23/32'):
            spanRating= '24oc'
        elif(self.name=='3/4'):
            spanRating= '24oc'
        elif(self.name=='7/8'):
            spanRating= '32oc'
        elif(self.name=='1'):
            spanRating= '32oc'
        elif(self.name=='1-1/8'):
            spanRating= '48oc'
        else:
            lmsg.error('name: ', self.name, 'unknown')
        return spanRating


        # def_vars_control.defVarsControlTensRegElastico3d(elems)
        # Wz= self.Wzel()
        # Wy= self.Wyel()
        # for e in elems:
        #     e.setProp("fyd",self.fyd)
        #     e.setProp("fydV",self.taud)
        #     e.setProp("Wyel",Wy)
        #     e.setProp("AreaQy",0.9*self.A())
        #     e.setProp("Wzel",Wz)
        #     e.setProp("AreaQz",self.A()-e.getProp("AreaQy"))

        
# Properties of Plywood structural panels taken from:
# http://www.pfsteco.com/techtips/pdf/tt_plywooddesigncapacities
# table C.

class PlywoodPanelSection(WoodPanelSection):
    ''' Plywood structural panel.'''
    rho= 577.941243312 # density kg/m3
    def __init__(self, name, b, h, shear_constant):
        ''' Constructor.

        :param name: section name.
        :param b: section width.
        :param h: section height.
        :param shear_constant: shear constant.
        '''
        super(PlywoodPanelSection,self).__init__(name, b, h, shear_constant)

# Oriented strand board panels according to document:
# "Panel design specification" Form No. D510C/Revised May 2012/0300

class OSBPanelSection(WoodPanelSection):
    ''' Oriented strand board panelaccording to document:
        "Panel design specification" 
        Form No. D510C/Revised May 2012/0300.'''
    rho= 632.62 # average density kg/m3 Table 12
    
    def __init__(self, name, b, h, shear_constant):
        ''' Constructor.

        :param name: section name.
        :param b: section width.
        :param h: section height.
        :param shear_constant: shear constant.
        '''
        super(OSBPanelSection,self).__init__(name, b, h, shear_constant)
        
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

        :param angle: angle of the stress with the strength axis.
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
    
    def defElasticShearSection2d(self,preprocessor, angle= math.pi/2.0, overrideRho= None):
        ''' Defines a elastic shear section for two-dimensional
            problems.

        :param preprocessor: pre-processor for the finite element problem.
        :param angle: angle of the stress with the strength axis.
        :param overrideRho: if defined (not None), override the value of 
                            the material density.
        '''
        matName= self.name+'_'+'osbMaterial'
        rho= self.rho
        if(overrideRho!=None):
            rho= overrideRho
        if(not self.xc_wood_material):
            self.xc_wood_material= typical_materials.MaterialData(name= matName,E= self.getE(angle), nu=0.2, rho= rho)
        retval= super(OSBPanelSection,self).defElasticShearSection2d(preprocessor, self.xc_wood_material, overrideRho= overrideRho)
        return retval

class HeaderSection(WoodRectangularSection):
    ''' Structural beam/header.'''
    nu= 0.2
    
    def __init__(self, name, b, h, Ms, Vs, linearDensity, wood):
        ''' Constructor.

        :param name: section name.
        :param b: section width.
        :param h: section height.
        :param Ms: allowable moment.
        :param Vs: allowable shear.
        :param linearDensity: mass per unit length.
        :param wood: timber material.
        '''
        super(HeaderSection,self).__init__(name, b, h)
        self.Ms= Ms # Allowable moment.
        self.Vs= Vs # Allowable shear.
        self.rho= linearDensity/b/h
        self.wood= wood
        
    def getLinearDensity(self):
        ''' Return the mass per unit length.'''
        return self.rho*self.b*self.h
    
    def getFb(self):
        ''' Return the allovable bending stress.'''
        return self.getVolumeFactor()*self.wood.Fb_12

    def getFbAdj(self, majorAxis= True):
        ''' Return the adjusted value of the allovable bending stress.

        :param majorAxis: if true return adjusted Fb for bending around major axis.
        '''
        retval= self.getFb()
        if(not majorAxis):
            className= type(self).__name__
            methodName= sys._getframe(0).f_code.co_name
            lmsg.error(className+'.'+methodName+'; not implemented for minor axis.')
            retval/=1000.0
        return retval
    
    def getFcAdj(self):
        ''' Return the adjusted value of the allovable compressive stress.'''
        return self.wood.Fc_pll

    def getFv(self):
        ''' Return the allovable shear stress.'''
        return self.wood.Fv
    
    def getFvAdj(self):
        ''' Return the adjusted value of the allovable shear stress.'''
        return self.getFv()
    
    def defXCMaterial(self, overrideRho= None):
        '''Defines the material in XC.

        :param overrideRho: if defined (not None), override the value of 
                            the material density.
        '''
        rho= self.rho
        if(overrideRho!=None):
            rho= overrideRho
        if(not self.xc_wood_material):
            self.xc_wood_material= typical_materials.MaterialData(name= self.wood.xc_material_name,E=self.wood.E,nu=self.nu,rho= rho)
        return self.xc_wood_material
    
    def defElasticShearSection2d(self, preprocessor, overrideRho= None):
        ''' Defines a elastic shear section for two-dimensional
            problems.

        :param preprocessor: pre-processor for the finite element problem.
        :param overrideRho: if defined (not None), override the value of 
                            the material density.
        '''
        mat= self.defXCMaterial(overrideRho= overrideRho)
        self.xc_section= super(HeaderSection,self).defElasticShearSection2d(preprocessor, mat, overrideRho)
        return self.xc_section
    
    def defElasticShearSection3d(self, preprocessor, overrideRho= None):
        ''' Defines a elastic shear section for three-dimensional
            problems.

        :param preprocessor: pre-processor for the finite element problem.
        :param overrideRho: if defined (not None), override the value of 
                            the material density.
        '''
        mat= self.defXCMaterial(overrideRho= overrideRho)
        self.xc_section= super(HeaderSection,self).defElasticShearSection3d(preprocessor,mat, overrideRho)
        return self.xc_section

# Properties of LSL beams and headers taken from:
# LP SolidStart LSL Beam & Header Technical Guide

class LSLHeaderSection(HeaderSection):
    ''' LSL structural beam/header.'''
    def __init__(self, name, b, h, Ms, Vs, linearDensity, wood):
        ''' Constructor.

        :param name: section name.
        :param b: section width.
        :param h: section height.
        :param Ms: allowable moment.
        :param Vs: allowable shear.
        :param linearDensity: mass per unit length.
        :param wood: timber material.
        '''
        super(LSLHeaderSection,self).__init__(name, b, h, Ms, Vs, linearDensity, wood)
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

class LSL_135E_HeaderSection(LSLHeaderSection):
    ''' LSL 1.35E structural beam/header.'''
    def __init__(self, name, b, h, Ms, Vs, linearDensity):
        ''' Constructor.

        :param name: section name.
        :param b: section width.
        :param h: section height.
        :param Ms: allowable moment.
        :param Vs: allowable shear.
        :param linearDensity: mass per unit length.
        '''
        super(LSL_135E_HeaderSection,self).__init__(name, b, h, Ms, Vs, linearDensity, wood= LSL_135E())

class LSL_155E_HeaderSection(LSLHeaderSection):
    ''' LSL 1.55E structural beam/header.'''
    def __init__(self, name, b, h, Ms, Vs, linearDensity):
        ''' Constructor.

        :param name: section name.
        :param b: section width.
        :param h: section height.
        :param Ms: allowable moment.
        :param Vs: allowable shear.
        :param linearDensity: mass per unit length.
        '''
        super(LSL_155E_HeaderSection,self).__init__(name, b, h, Ms, Vs, linearDensity, wood= LSL_155E())

# Properties of LVL beams and headers taken from:
# LP SolidStart LVL Beam & Header Technical Guide

class LVLHeaderSection(HeaderSection):
    ''' LVL structural beam/header.'''
    def __init__(self, name, b, h, Ms, Vs, linearDensity, wood):
        ''' Constructor.

        :param name: section name.
        :param b: section width.
        :param h: section height.
        :param Ms: allowable moment.
        :param Vs: allowable shear.
        :param linearDensity: mass per unit length.
        :param wood: timber material.
        '''
        super(LVLHeaderSection,self).__init__(name, b, h, Ms, Vs, linearDensity, wood)
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

class LVL_2900Fb2E_HeaderSection(LVLHeaderSection):
    ''' LVL 2900Fb 2.0E structural beam/header.'''
    def __init__(self, name, b, h, Ms, Vs, linearDensity):
        ''' Constructor.

        :param name: section name.
        :param b: section width.
        :param h: section height.
        :param Ms: allowable moment.
        :param Vs: allowable shear.
        :param linearDensity: mass per unit length.
        '''
        super(LVL_2900Fb2E_HeaderSection,self).__init__(name, b, h, Ms, Vs, linearDensity, wood= LVL_2900Fb2E())
        
class TJIJoistSection(WoodSection):
    ''' TJI joist section.'''
    E= 1.55e6*psi2Pa # Elastic modulus (Pa) arbitrarily chosen.
    def __init__(self, name:str, h:float, Ms:float, Vs:float, EI:float, linearRho:float= None):
        ''' Constructor.

        :param name: section name.
        :param h: section height.
        :param Ms: allowable moment.
        :param Vs: allowable shear.
        :param EI: bending stiffness-
        '''
        super(TJIJoistSection,self).__init__(name)
        self.h= h
        self.Ms= Ms # Allowable moment.
        self.Vs= Vs # Allowable shear.
        self.Iz= EI/self.E
        self.linearRho= linearRho

    def getCompressiveStrength(self):
        ''' Return the value of the compressive strength of the section.
        '''
        b= 12*self.Iz/self.h**3
        area= self.h*b
        return area # very small strength 1N/m2
    
    def getFlexuralStrength(self, majorAxis= True, chiLT= 1.0):
        ''' Return the value of the flexural strength of the section.

        :param chiLT: beam stability factor clause 3.3.3 of AWC-NDS2018 (default= 1.0).
        '''
        retval= self.Ms*chiLT
        if(not majorAxis):
            # className= type(self).__name__
            # methodName= sys._getframe(0).f_code.co_name
            # lmsg.error(className+'.'+methodName+'; not implemented for minor axis.')
            retval/=1e4 # very small strength
        return retval

    def getShearStrength(self, majorAxis= True):
        ''' Return the value of the flexural strength of the section.
        '''
        retval= self.Vs
        if(not majorAxis):
            # className= type(self).__name__
            # methodName= sys._getframe(0).f_code.co_name
            # lmsg.error(className+'.'+methodName+'; not implemented for minor axis.')
            retval/=1e4 # very small strength
        return retval

    def defElasticShearSection2d(self, preprocessor, overrideRho= None):
        ''' Defines a elastic shear section for two-dimensional
            problems.

        :param preprocessor: pre-processor for the finite element problem.
        :param overrideRho: if defined (not None), override the value of 
                            the material density.
        '''
        b= 12*self.Iz/self.h**3
        area= self.h*b
        G= self.E/(2*(1+0.3))
        lR= self.linearRho
        if(lR==None):
            lR= 632.62*b*self.h # average density kg/m3
        return typical_materials.defElasticShearSection2d(preprocessor,name= self.name, A= area,E= self.E,G= G, I= self.Iz,alpha= 5.0/6.0, linearRho= lR)
    
    def defElasticShearSection3d(self, preprocessor, overrideRho= None):
        ''' Defines a elastic shear section for two-dimensional
            problems.

        :param preprocessor: pre-processor for the finite element problem.
        :param overrideRho: if defined (not None), override the value of 
                            the material density.
        '''
        b= 12*self.Iz/self.h**3
        area= self.h*b
        G= self.E/(2*(1+0.3))
        lR= self.linearRho
        Iy= 1/12.0*self.h*b**3
        J= Iy/1000.0
        if(lR==None):
            lR= 632.62*b*self.h # average density kg/m3
        return typical_materials.defElasticShearSection3d(preprocessor,name= self.name, A= area,E= self.E, G= G,Iz= self.Iz, Iy= Iy, J= J, alpha_y= 5.0/6.0, alpha_z= 5/6.0, linearRho= lR)

class CustomLumberSection(WoodRectangularSection):
    ''' Section of a lumber member with custom dimensions.'''
    def __init__(self, name, b, h, woodMaterial, Cr= 1.0):
        ''' Constructor.

        :param name: section name.
        :param b: section width.
        :param h: section height.
        :param woodMaterial: timber material.
        :param Cr: repetitive member adjustment factor
        '''
        super(CustomLumberSection,self).__init__(name, b, h, Cr= Cr)
        self.wood= woodMaterial
        
    def getFb(self):
        return self.wood.getFb(b= self.b, h= self.h)

    def updateLoadDurationFactor(self, CD):
        '''Update the value of the load duration factor.'''
        self.wood.CD= CD

    def defElasticShearSection2d(self, preprocessor, overrideRho= None):
        ''' Defines a elastic shear section for two-dimensional
            problems.

        :param preprocessor: pre-processor for the finite element problem.
        :param overrideRho: if defined (not None), override the value of 
                            the material density.
        '''
        mat= self.wood.defXCMaterial(overrideRho= overrideRho)
        self.xc_section= super(CustomLumberSection,self).defElasticShearSection2d(preprocessor,mat, overrideRho= overrideRho)
        return self.xc_section
    
    def defElasticShearSection3d(self, preprocessor, overrideRho= None):
        ''' Defines a elastic shear section for three-dimensional
            problems.

        :param preprocessor: pre-processor for the finite element problem.
        :param overrideRho: if defined (not None), override the value of 
                            the material density.
        '''
        mat= self.wood.defXCMaterial(overrideRho= overrideRho)
        self.xc_section= super(CustomLumberSection,self).defElasticShearSection3d(preprocessor, mat, overrideRho= overrideRho)
        return self.xc_section
    
    def getBendingFlatUseFactor(self):
        ''' Return the flat use factor for the bending design
            value Fb.'''
        return self.wood.getBendingFlatUseFactor(self.b,self.h)
    
    def getBendingSizeFactor(self):
        ''' Return the size factor for the bending design
            value Fb.'''
        bNom= self.wood.getNominalThickness(self.b)
        hNom= self.wood.getNominalWidth(self.h)
        return self.wood.getBendingSizeFactor(b= bNom, h= hNom)
    
    def getTensionSizeFactor(self):
        ''' Return the size factor for the tension design
            value Ft.'''
        return self.wood.getTensionSizeFactor(self.b,self.h)
    
    def getCompressionSizeFactor(self):
        ''' Return the size factor for the compression design
            value Fc.'''
        return self.wood.getCompressionSizeFactor(self.b,self.h)
    
    def getFbAdj(self, majorAxis= True):
        ''' Return the adjusted value of Fb.

        :param majorAxis: if true return adjusted Fb for bending around major axis.
        '''
        b= self.b
        h= self.h
        if(not majorAxis):
            h, b = b, h
        return self.wood.getFbAdj(b= b, h= h, Cr= self.Cr)
    
    def getFtAdj(self):
        ''' Return the adjusted value of Ft.
        '''
        return self.wood.getFtAdj(self.b, self.h)
    
    def getFvAdj(self):
        ''' Return the adjusted value of Fv.'''
        return self.wood.getFvAdj()
    
    def getFc_perpAdj(self, Cb= 1.0):
        ''' Return the adjusted value of Fc_perp.

        :param Cb: bearing area factor
        '''
        return self.wood.getFc_perpAdj(Cb)
    
    def getFc(self):
        ''' Return the value of Fc.'''
        return self.wood.getFc(self.b,self.h)
    
    def getFcAdj(self):
        ''' Return the adjusted value of Fc.'''
        return self.wood.getFcAdj(self.b,self.h)
    
    def getEAdj(self):
        ''' Return the adjusted value of E.'''
        return self.wood.getEAdj()
    
    def getEminAdj(self):
        ''' Return the adjusted value of Emin.'''
        return self.wood.getEminAdj()
    
    def getRequiredRestraint(self):
        ''' Return the required restrains according to clause 4.4.1.2 of
            AWC_NDS2018.'''
        retval= MemberRestraint.bothEdgeSupport
        depthToBreadthRatio= self.h/self.b
        if(depthToBreadthRatio<=2.0):
            retval= MemberRestraint.noLateralSupport
        elif(depthToBreadthRatio<=4.0):
            retval= MemberRestraint.endSupport
        elif(depthToBreadthRatio<=5.0):
            retval= MemberRestraint.compressionEdgeSupport
        elif(depthToBreadthRatio<=6.0):
            retval= MemberRestraint.fullDepthSolidBlocking
        elif(depthToBreadthRatio<=7.0):
            retval= MemberRestraint.bothEdgeSupport
        return retval

threeQuarters= 0.75*in2meter
inchAndAHalf= 1.5*in2meter
twoInchAndAHalf= 2.5*in2meter
threeInchAndAHalf= 3.5*in2meter
fourInchAndAHalf= 4.5*in2meter
fiveInchAndAHalf= 5.5*in2meter
sevenInchAndAQuarter= 7.25*in2meter
nineInchAndAQuarter= 9.25*in2meter
elevenInchAndAQuarter= 11.25*in2meter

dimensionLumberSizes= dict()
dimensionLumberSizes['1x2']= (threeQuarters, inchAndAHalf)
dimensionLumberSizes['1x3']= (threeQuarters, twoInchAndAHalf)
dimensionLumberSizes['1x4']= (threeQuarters, threeInchAndAHalf)
dimensionLumberSizes['1x5']= (threeQuarters, fourInchAndAHalf)
dimensionLumberSizes['1x6']= (threeQuarters, fiveInchAndAHalf)
dimensionLumberSizes['1x8']= (threeQuarters, sevenInchAndAQuarter)
dimensionLumberSizes['1x10']= (threeQuarters, nineInchAndAQuarter)
dimensionLumberSizes['1x12']= (threeQuarters, elevenInchAndAQuarter)
dimensionLumberSizes['2x2']= (inchAndAHalf, inchAndAHalf)
dimensionLumberSizes['2x3']= (inchAndAHalf, twoInchAndAHalf)
dimensionLumberSizes['2x4']= (inchAndAHalf, threeInchAndAHalf)
dimensionLumberSizes['2x6']= (inchAndAHalf, fiveInchAndAHalf)
dimensionLumberSizes['2x8']= (inchAndAHalf, sevenInchAndAQuarter)
dimensionLumberSizes['2x10']= (inchAndAHalf, nineInchAndAQuarter)
dimensionLumberSizes['2x12']= (inchAndAHalf, elevenInchAndAQuarter)
dimensionLumberSizes['4x4']= (threeInchAndAHalf, threeInchAndAHalf)
dimensionLumberSizes['4x6']= (threeInchAndAHalf, fiveInchAndAHalf)
dimensionLumberSizes['6x6']= (fiveInchAndAHalf, fiveInchAndAHalf)
dimensionLumberSizes['2x1']= (inchAndAHalf, threeQuarters)
dimensionLumberSizes['3x1']= (twoInchAndAHalf, threeQuarters)
dimensionLumberSizes['4x1']= (threeInchAndAHalf, threeQuarters)
dimensionLumberSizes['5x1']= (fourInchAndAHalf, threeQuarters)
dimensionLumberSizes['6x1']= (fiveInchAndAHalf, threeQuarters)
dimensionLumberSizes['8x1']= (sevenInchAndAQuarter, threeQuarters)
dimensionLumberSizes['10x1']= (nineInchAndAQuarter, threeQuarters)
dimensionLumberSizes['12x1']= (elevenInchAndAQuarter, threeQuarters)
dimensionLumberSizes['3x2']= (twoInchAndAHalf, inchAndAHalf)
dimensionLumberSizes['4x2']= (threeInchAndAHalf, inchAndAHalf)
dimensionLumberSizes['6x2']= (fiveInchAndAHalf, inchAndAHalf)
dimensionLumberSizes['8x2']= (sevenInchAndAQuarter, inchAndAHalf)
dimensionLumberSizes['10x2']= (nineInchAndAQuarter, inchAndAHalf)
dimensionLumberSizes['12x2']= (elevenInchAndAQuarter, inchAndAHalf)
dimensionLumberSizes['6x4']= (fiveInchAndAHalf, threeInchAndAHalf)

class DimensionLumberSection(CustomLumberSection):
    ''' Section of a dimension lumber member.'''
    def __init__(self, name, woodMaterial):
        ''' Constructor.

        :param name: section name.
        :param woodMaterial: timber material.
        '''
        b= 0.0
        h= 0.0
        if(name in dimensionLumberSizes.keys()):
            size= dimensionLumberSizes[name]
            b= size[0]
            h= size[1]
        else:
            lmsg.error('section: \''+name+'\' doesn\'t exists.')
        super(DimensionLumberSection,self).__init__(name, b, h, woodMaterial)

