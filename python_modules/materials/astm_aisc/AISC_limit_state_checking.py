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
from materials import buckling_base
import ASTM_materials as astm


# Unbraced segment ascii art:
#
#         A      B      C
#  x------+------+------+------x 
#  ^                           ^
#  0      1      2      3      4
#
# x: brace for lateral buckling
# +: control point.

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
    :ivar sectionClassif: classification of the section for local
                          buckling (defaults to compact).
    '''
    def __init__(self, name, section, unbracedLengthX, unbracedLengthY= None, unbracedLengthZ= None, kx= 1.0, ky= 1.0, kz= 1.0, sectionClassif= astm.SectionClassif.compact, lstLines=None, lstPoints=None):
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
        self.sectionClassif= sectionClassif

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
    
    def getNominalCompressiveStrength(self):
        ''' Return the nominal compressive strength of the member
            according to equation E3-1 of AISC-360-16.
        '''
        return self.shape.getNominalCompressiveStrength(effectiveLengthX= self.getEffectiveLengthX(), effectiveLengthY= self.getEffectiveLengthY(), effectiveLengthZ= self.getEffectiveLengthZ(), sectionClassif= self.sectionClassif)

    def getDesignCompressiveStrength(self):
        ''' Return the design compressive strength of the member
            according to section E1 of AISC-360-16.
        '''
        return self.shape.getDesignCompressiveStrength(effectiveLengthX= self.getEffectiveLengthX(), effectiveLengthY= self.getEffectiveLengthY(), effectiveLengthZ= self.getEffectiveLengthZ(), sectionClassif= self.sectionClassif)

    def getCompressiveStrengthReductionFactor(self):
        ''' Return the reduction factor of the compressive strength 
            of the member with respect to the reference compressive strength
            of its section.
        '''
        return self.getDesignFlexuralStrength()/self.shape.getReferenceFlexuralStrength()
    
    def getCompressiveStrengthReductionFactor(self):
        ''' Return the reduction factor of the compressive strength 
            of the member with respect to the plastic axial load of
            its section.
        '''
        return self.getDesignCompressiveStrength()/self.shape.getReferenceCompressiveStrength(sectionClassif= self.sectionClassif)

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
            according to section F of AISC-360-16.
        '''
        return 0.9*self.getNominalFlexuralStrength(majorAxis)
        
    def getFlexuralStrengthReductionFactor(self):
        ''' Return the reduction factor of the flexural strength 
            of the member with respect to the reference flexural strength 
            of its section.
        '''
        return self.getDesignFlexuralStrength()/self.shape.getReferenceFlexuralStrength()

    def getDesignShearStrength(self, majorAxis= True):
        ''' Return the design shear strength of the member
            according to section G of AISC-360-16.
        '''
        return self.shape.getDesignShearStrength(majorAxis= majorAxis)
    
    def getBiaxialBendingEfficiency(self,Nd,Myd,Mzd):
        '''Return biaxial bending efficiency according to section H1
           of AISC-360-16.

           Nd: axial design load (required axial strength).
           Myd: bending moment about weak axis (required flexural strength).
           Mzd: bending moment about strong axis (required flexural strength).
        '''
        # Compute axial load reduction factor.
        lrfN= self.getCompressiveStrengthReductionFactor()

        # Compute lateral buckling reduction factor.
        lrfLT= self.getFlexuralStrengthReductionFactor()

        return self.shape.getBiaxialBendingEfficiency(sectionClassif= self.sectionClassif, Nd= Nd, Myd= Myd, Mzd= Mzd, Vyd= 0.0, chiN= lrfN, chiLT= lrfLT)
 
