# -*- coding: utf-8 -*-
''' Classes and functions for limit state checking according to Eurocode 3. '''
from __future__ import print_function
from __future__ import division

__author__= "Luis C. Pérez Tato (LCPT) Ana Ortega (AO_O)"
__copyright__= "Copyright 2016 LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com ana.ortega@ciccp.es"

import math
from misc_utils import log_messages as lmsg


class SectionClasif(Enum):
    '''Classification of sections for local buckling.'''
    compact= 0
    noncompact= 1
    slender= 2
    too_slender= 3

class Member(object):
    ''' Beam and column members according to ANSI AISC 360-16.

    :ivar Kx: effective length factor for torsional buckling 
              about the longitudinal axis.
    :ivar Ky: effective length factor for flexural buckling 
              about y-axis.
    :ivar Kz: effective length factor for flexural buckling 
              about z-axis
    '''
    def __init__(self, section, unbracedLengthX, unbracedLengthY= None, unbracedLengthZ= None, kx= 1.0, ky= 1.0, kz= 1.0, sectionClasif= SectionClasif.compact):
        ''' Constructor. '''
        self.section= section
        self.unbracedLengthX= unbracedLengthX
        if(unbracedLengthY):
            self.unbracedLengthY= unbracedLengthY
        else:
            self.unbracedLengthY= unbracedLengthX
        if(unbracedLengthZ):
            self.unbracedLengthZ= unbracedLengthZ
        else:
            self.unbracedLengthZ= unbracedLengthX            
        self.Kx= kx
        self.Ky= ky
        self.Kz= kz
        self.sectionClasif= sectionClasif

    def getEffectiveLengthX(self):
        ''' Return the effective length of member for buckling 
            about longitudinal axis: L_cx.'''
        return self.Kx*self.unbracedLengthX
    
    def getEffectiveLengthY(self):
        ''' Return the effective length of member for buckling 
            about y axis: L_cy.'''
        return self.Ky*self.unbracedLengthY

    def getEffectiveLengthZ(self):
        ''' Return the effective length of member for buckling 
            about y axis: L_cy.'''
        return self.Kz*self.unbracedLengthZ

    def getFlexuralSlendernessRatio(self):
        ''' Return the flexural buckling slenderness ratio of the member.

        '''
        retval= self.getEffectiveLengthZ()/self.section.get('iz')
        return max(retval,self.getEffectiveLengthY()/self.section.get('iy'))
        
    def getFlexuralCriticalSlendernessRatio(self):
        ''' Return the critical value of the flexural buckling 
            slenderness ratio of the member.
        '''
        sr= self.getFlexuralSlendernessRatio()
        E= self.section.get('E')
        Fy= self.section.steelType.fy
        return sr/math.pi*math.sqrt(Fy/E)

    def getFlexuralElasticBucklingStress(self):
        ''' Return the flexural elastic buckling stress of the member according
            to equation E3-4 of AISC-360-16.
        '''
        sr= self.getFlexuralSlendernessRatio()
        E= self.section.get('E')
        return math.pi**2*E/sr**2

    def getFlexuralElasticBucklingStressY(self):
        ''' Return the flexural elastic buckling stress of the member according
            to equation E4-5 of AISC-360-16.
        '''
        sry= self.getEffectiveLengthY()/self.section.get('iy') # Slenderness ratio
        E= self.section.get('E')
        return math.pi**2*E/sry**2

    def getFlexuralElasticBucklingStressZ(self):
        ''' Return the flexural elastic buckling stress of the member according
            to equation E4-6 of AISC-360-16.
        '''
        srz= self.getEffectiveLengthY()/self.section.get('iz') # Slenderness ratio
        E= self.section.get('E')
        return math.pi**2*E/srz**2
    
    def getCriticalStress(self, Fe):
        ''' Return the critical stress of the member according
            to equations E3-2 and E3-3 of AISC-360-16.

        :param Fe: flexural or torsional elastic buckling stress.
        '''
        retval= 0.0
        if(self.sectionClasif<2):
            sr= self.getFlexuralSlendernessRatio()
            E= self.section.get('E')
            Fy= self.section.steelType.fy
            Fratio= Fy/Fe
            thresholdA= 4.71*math.sqrt(E/Fy)
            if((sr<=thresholdA) or (Fratio<=2.25)):
                retval= math.pow(0.658,Fratio)*Fy # (E3-2)
            else:
                retval= 0.877*Fe
        else:
            lmsg.error('Critical stress of slender members not implemented yet.')
        return retval
    
    def getFlexuralCriticalStress(self):
        ''' Return the flexural critical stress of the member according
            to equations E3-2 and E3-3 of AISC-360-16.
        '''
        Fe= self.getFlexuralElasticBucklingStress()
        return self.getCriticalStress(Fe)

    def getTorsionalElasticBucklingStress(self):
        ''' Return the torsional or flexural-torsional elastic buckling stress
            of the member according to equations E4-2, E4-3 and E4-4 of 
            AISC-360-16.
        '''
        lmsg.error('Torsional and flexural-torsional elastic buckling stress of slender members not implemented yet.')
        return 0.0
    
    def getTorsionalCriticalStress(self):
        ''' Return the torsional critical stress of the member according
            to equations E4-2, E4-3 and E4-4 of AISC-360-16.
        '''
        Fe= self.getTorsionalElasticBucklingStress()
        return self.getCriticalStress(Fe)

    
    def getNominalCompressiveStrength(self):
        ''' Return the nominal compressive strength of the member
            according to equation E3-1 of AISC-360-16.
        '''
        Ag= self.section.get('A') # Gross area of member
        Lcx= self.getEffectiveLengthX() # Torsional effective length
        Lcy= self.getEffectiveLengthY() # Y bending effective length
        Lcz= self.getEffectiveLengthZ() # Z bending effective length
        if(Lcx<= max(Lcy,Lcz)):
            return self.getFlexuralCriticalStress()*Ag
        else:
            return self.getTorsionalCriticalStress()*Ag
        
       
