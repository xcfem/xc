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
import enum
from misc_utils import log_messages as lmsg
from materials import buckling_base


class SectionClasif(enum.IntEnum):
    '''Classification of sections for local buckling.'''
    compact= 0
    noncompact= 1
    slender= 2
    too_slender= 3


# Unbraced segment ascii art:
#
#         A      B      C
#  +------+------+------+------+ 
#  ^                           ^
#  0      1      2      3      4
#

class LateralTorsionalBucklingModificationFactor(object):
    ''' Calculation of the Cb lateral-torsional buckling modification factor
    as defined in section F1 (c) of ANSI AISC 360-16.'''
    def __init__(self,Mi):
        ''' Constructor.

        :param Mi: bending moment list as follows:
                   *Mi[0]: Value of moment at the back end of the unbraced segment.
                   *Mi[1]: Value of moment at quarter point of the unbraced segment (MA in equation F1-1).
                   *Mi[2]: Value of moment at centerline of the unbraced segment (MB in equation F1-1).
                   *Mi[3]: Value of moment at three-quarter point of the unbraced segment (MC in equation F1-1).
                   *Mi[4]: Value of moment at the front end of the unbraced segment.
        '''
        self.Mi= list()
        for m in Mi:
            self.Mi.append(abs(m)) # absolute value of moments.

    def getLateralTorsionalBucklingModificationFactor(self):
        ''' Return the lateral-torsional buckling modification factor
            according to equation F1-1 of ANSI AISC 360-16.'''
        mMax= max(self.Mi)
        denom= 2.5*mMax+3.0*self.Mi[1]+4.0*self.Mi[2]+3.0*self.Mi[3]
        return 12.5*mMax/denom

class Member(buckling_base.MemberBase):
    ''' Beam and column members according to ANSI AISC 360-16.

    :ivar unbracedLengthX: unbraced length for torsional buckling 
                           about the longitudinal axis.
    :ivar unbracedLengthY: unbraced length for flexural buckling 
                           about y-axis.
    :ivar unbracedLengthZ: unbraced length for flexural buckling 
                           about z-axis.
    :ivar Kx: effective length factor for torsional buckling 
              about the longitudinal axis.
    :ivar Ky: effective length factor for flexural buckling 
              about y-axis.
    :ivar Kz: effective length factor for flexural buckling 
              about z-axis.
    :ivar sectionClasif: classification of the section for local
                          buckling (defaults to compact).
    '''
    def __init__(self, name, section, unbracedLengthX, unbracedLengthY= None, unbracedLengthZ= None, kx= 1.0, ky= 1.0, kz= 1.0, sectionClasif= SectionClasif.compact, lstLines=None, lstPoints=None):
        ''' Constructor. 

        :param name: object name.
        :param ec3Shape: cross-section shape (e.g. IPNShape, IPEShape, ...)
        :param lstLines: ordered list of lines that make up the beam 
                        (defaults to None).
        :param lstPoints: ordered list of points that make up the beam. 
                          Ignored if lstLines is given (defaults to None)
        '''
        super(Member,self).__init__(name, section, lstLines, lstPoints)
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
        ''' Return the effective length of member for torsional buckling 
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
        sc= self.shape.slendernessCheck()
        if(sc>1.01):
            lmsg.warning('Member section has slender members. Results are not valid.')
        retval= self.getEffectiveLengthZ()/self.shape.get('iz')
        return max(retval,self.getEffectiveLengthY()/self.shape.get('iy'))
        
    def getFlexuralCriticalSlendernessRatio(self):
        ''' Return the critical value of the flexural buckling 
            slenderness ratio of the member.
        '''
        sr= self.getFlexuralSlendernessRatio()
        E= self.shape.get('E')
        Fy= self.shape.steelType.fy
        return sr/math.pi*math.sqrt(Fy/E)

    def getFlexuralElasticBucklingStress(self):
        ''' Return the flexural elastic buckling stress of the member according
            to equation E3-4 of AISC-360-16.
        '''
        sr= self.getFlexuralSlendernessRatio()
        E= self.shape.get('E')
        return math.pi**2*E/sr**2

    def getFlexuralElasticBucklingStressY(self):
        ''' Return the flexural elastic buckling stress of the member according
            to equation E4-5 of AISC-360-16.
        '''
        sry= self.getEffectiveLengthY()/self.shape.get('iy') # Slenderness ratio
        E= self.shape.get('E')
        return math.pi**2*E/sry**2

    def getFlexuralElasticBucklingStressZ(self):
        ''' Return the flexural elastic buckling stress of the member according
            to equation E4-6 of AISC-360-16.
        '''
        srz= self.getEffectiveLengthY()/self.shape.get('iz') # Slenderness ratio
        E= self.shape.get('E')
        return math.pi**2*E/srz**2
    
    def getCriticalStress(self, Fe):
        ''' Return the critical stress of the member according
            to equations E3-2 and E3-3 of AISC-360-16.

        :param Fe: flexural or torsional elastic buckling stress.
        '''
        retval= 0.0
        if(self.sectionClasif<SectionClasif.slender):
            sr= self.getFlexuralSlendernessRatio()
            E= self.shape.get('E')
            Fy= self.shape.steelType.fy
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
        retval= 0.0
        symmetry= self.shape.getSymmetry()
        Lc= self.getEffectiveLengthX()
        if(symmetry=='double'):
            retval= self.shape.getTorsionalElasticBucklingStress(Lc)
        elif(symmetry=='simple'):
            Fex= self.shape.getTorsionalElasticBucklingStress(Lc) # E4-7
            Fez= getFlexuralElasticBucklingStressZ() #E4-5
            H= self.shape.getFlexuralConstant() # E4-8
            retval= (Fex+Fez)/2/H*(1-math.sqrt(1-(4.0*Fex*Fez*H)/(Fex+Fez)**2))
        else: # no simmetry: E4-4
            lmsg.error('Torsional elastic buckling stress for unsymmetric members not implemented yet.')
            
        return retval
    
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
        Ag= self.shape.get('A') # Gross area of member
        Lcx= self.getEffectiveLengthX() # Torsional effective length
        Lcy= self.getEffectiveLengthY() # Y bending effective length
        Lcz= self.getEffectiveLengthZ() # Z bending effective length
        if(Lcx<= max(Lcy,Lcz)):
            return self.getFlexuralCriticalStress()*Ag
        else:
            return self.getTorsionalCriticalStress()*Ag

    def getDesignCompressiveStrength(self):
        ''' Return the design compressive strength of the member
            according to section E1 of AISC-360-16.
        '''
        return 0.9*self.getNominalCompressiveStrength()

    def getLateralTorsionalBucklingModificationFactor(self):
        ''' Return the lateral-torsional buckling modification factor
            according to equation F1-1 of ANSI AISC 360-16.'''
        Mi= self.getBendingMomentsAtControlPoints()
        mf= LateralTorsionalBucklingModificationFactor(Mi)
        return mf.getLateralTorsionalBucklingModificationFactor()


    def getNominalFlexuralStrength(self, majorAxis= True):
        ''' Return the nominal compressive strength of the member
            according to chapter F of AISC-360-16.
        '''
        lateralUnbracedLength= self.getEffectiveLengthX()
        Cb= self.getLateralTorsionalBucklingModificationFactor()
        return self.shape.getNominalFlexuralStrength(lateralUnbracedLength, Cb, majorAxis)

    def getDesignFlexuralStrength(self, majorAxis= True):
        ''' Return the design flexural strength of the member
            according to section F1 of AISC-360-16.
        '''
        return 0.9*self.getNominalFlexuralStrength(majorAxis)
