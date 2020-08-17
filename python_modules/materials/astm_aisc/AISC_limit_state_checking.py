# -*- coding: utf-8 -*-
''' Classes and functions for limit state checking according to Eurocode 3. '''
from __future__ import print_function
from __future__ import division

__author__= "Luis C. Pérez Tato (LCPT) Ana Ortega (AO_O)"
__copyright__= "Copyright 2016 LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com ana.ortega@ciccp.es"

import enum
import math
from misc_utils import log_messages as lmsg
from materials import buckling_base
from materials import limit_state_checking_base as lsc
from postprocess import control_vars as cv
from postprocess import limit_state_data as lsd
import xc

class SectionClassif(enum.IntEnum):
    '''Classification of sections for local buckling.'''
    compact= 0
    noncompact= 1
    slender= 2
    too_slender= 3

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
        retval= 0.0
        mMax= max(self.Mi)
        if(mMax>0.0):
            denom= 2.5*mMax+3.0*self.Mi[1]+4.0*self.Mi[2]+3.0*self.Mi[3]
            retval= 12.5*mMax/denom
        return retval

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
    def __init__(self, name, section, unbracedLengthX, unbracedLengthY= None, unbracedLengthZ= None, kx= 1.0, ky= 1.0, kz= 1.0, sectionClassif= SectionClassif.compact, Cb= None, lstLines=None, lstPoints=None):
        ''' Constructor. 

        :param name: object name.
        :param ec3Shape: cross-section shape (e.g. IPNShape, IPEShape, ...)
        :param Cb: lateral-torsional buckling modification factor
                   (defaults to None).
        :param lstLines: ordered list of lines that make up the beam. 
                         Ignored if Cb is given (defaults to None).
        :param lstPoints: ordered list of points that make up the beam. 
                          Ignored if Cb or lstLines is given (defaults to None)
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
        self.Cb= Cb
        if(self.Cb):
            if(lstLines or lstPoints):
                lmsg.error('lateral-torsional buckling modification factor was specified, computation from finite element model will not be performed.')

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
        Cb= 1.0 # conservative value
        if(self.Cb):
            Cb= self.Cb
        else:
            if(self.lstLines or self.lstPoints):
                Cb= self.getLateralTorsionalBucklingModificationFactor()
            else:
                lmsg.error('Can\'t compute lateral-torsional buckling modification factor, taken as: '+str(Cb))
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
    
    def updateReductionFactors(self):
        '''Update the value of the appropriate reduction factors.'''
        chiN= self.getCompressiveStrengthReductionFactor()
        chiLT= self.getFlexuralStrengthReductionFactor()
        for e in self.elemSet:
             e.setProp('chiLT',chiLT) # flexural strength reduction factor.
             e.setProp('chiN',chiN) # compressive strength reduction factor.

    def installULSControlRecorder(self,recorderType, chiN= 1.0, chiLT=1.0):
        '''Install recorder for verification of ULS criterion.'''
        prep= self.getPreprocessor()
        nodes= prep.getNodeHandler
        domain= prep.getDomain
        recorder= domain.newRecorder(recorderType,None)
        if(not self.elemSet):
            self.createElementSet()
        eleTags= list()
        for e in self.elemSet:
            eleTags.append(e.tag)
            e.setProp('ULSControlRecorder',recorder)
        idEleTags= xc.ID(eleTags)
        recorder.setElements(idEleTags)
        self.shape.setupULSControlVars(self.elemSet,self.sectionClassif,chiN= chiN, chiLT= chiLT)
        if(nodes.numDOFs==3):
            recorder.callbackRecord= controlULSCriterion2D()
        else:
            recorder.callbackRecord= controlULSCriterion()
#        recorder.callbackRestart= "print(\"Restart method called.\")" #20181121
        return recorder

class BiaxialBendingNormalStressController(lsc.LimitStateControllerBase):
    '''Object that controls normal stresses limit state.'''

    def __init__(self,limitStateLabel):
        super(BiaxialBendingNormalStressController,self).__init__(limitStateLabel)

    def initControlVars(self,setCalc):
        '''Initialize control variables over elements.

        :param setCalc: set of elements to which define control variables
        '''
        for e in setCalc.elements:
            e.setProp(self.limitStateLabel+'Sect1',cv.AISCBiaxialBendingControlVars(idSection= 'Sect1'))
            e.setProp(self.limitStateLabel+'Sect2',cv.AISCBiaxialBendingControlVars(idSection= 'Sect2'))

    def checkSetFromIntForcFile(self,intForcCombFileName,setCalc=None):
        '''Launch checking.

        :param intForcCombFileName: name of the file to read the internal 
               force results
        :param setCalc: set of elements to check
        '''
        intForcItems= lsd.readIntForcesFile(intForcCombFileName,setCalc)
        internalForcesValues= intForcItems[2]
        for e in setCalc.elements:
            sh= e.getProp('crossSection')
            sc= e.getProp('sectionClass')
            elIntForc= internalForcesValues[e.tag]
            if(len(elIntForc)==0):
                lmsg.warning('No internal forces for element: '+str(e.tag)+' of type: '+e.type())
            for lf in elIntForc:
                CFtmp,NcRdtmp,McRdytmp,McRdztmp,MvRdztmp,MbRdztmp= sh.getBiaxialBendingEfficiency(sc,lf.N,lf.My,lf.Mz,lf.Vy,lf.chiN, lf.chiLT)
                if lf.idSection == 0:
                    label= self.limitStateLabel+'Sect1'
                    if(CFtmp>e.getProp(label).CF):
                        e.setProp(label,cv.AISCBiaxialBendingControlVars('Sect1',lf.idComb,CFtmp,lf.N,lf.My,lf.Mz,NcRdtmp,McRdytmp,McRdztmp,MvRdztmp,MbRdztmp,lf.chiLT, lf.chiN))
                else:
                    label= self.limitStateLabel+'Sect2'
                    if(CFtmp>e.getProp(label).CF):
                        e.setProp(label,cv.AISCBiaxialBendingControlVars('Sect2',lf.idComb,CFtmp,lf.N,lf.My,lf.Mz,NcRdtmp,McRdytmp,McRdztmp,MvRdztmp,MbRdztmp,lf.chiLT, lf.chiN))

class ShearController(lsc.LimitStateControllerBase):
    '''Object that controls shear limit state.'''

    def __init__(self,limitStateLabel):
        super(ShearController,self).__init__(limitStateLabel)

    def initControlVars(self,setCalc):
        '''Initialize control variables over elements.

        :param setCalc: set of elements to which define control variables
        '''
        for e in setCalc.elements:
            e.setProp(self.limitStateLabel+'Sect1',cv.ShearYControlVars())
            e.setProp(self.limitStateLabel+'Sect2',cv.ShearYControlVars())

    def checkSetFromIntForcFile(self,intForcCombFileName,setCalc=None):
        '''Launch checking.

        :param setCalc: set of elements to check
        '''
        intForcItems=lsd.readIntForcesFile(intForcCombFileName,setCalc)
        internalForcesValues=intForcItems[2]
        for e in setCalc.elements:
            sh=e.getProp('crossSection')
            sc=e.getProp('sectionClass')
            elIntForc=internalForcesValues[e.tag]
            if(len(elIntForc)==0):
                lmsg.warning('No internal forces for element: '+str(e.tag)+' of type: '+e.type())
            for lf in elIntForc:
                CFtmp= sh.getYShearEfficiency(sc,lf.Vy)
                if lf.idSection == 0:
                    if(CFtmp>e.getProp(self.limitStateLabel+'Sect1').CF):
                        e.setProp(self.limitStateLabel+'Sect1',cv.ShearYControlVars('Sects1',lf.idComb,CFtmp,lf.Vy))
                else:
                    if(CFtmp>e.getProp(self.limitStateLabel+'Sect2').CF):
                        e.setProp(self.limitStateLabel+'Sect2',cv.ShearYControlVars('Sects2',lf.idComb,CFtmp,lf.Vy))

                        
def controlULSCriterion():
    return '''recorder= self.getProp('ULSControlRecorder')
nmbComb= recorder.getCurrentCombinationName
self.getResistingForce()
crossSection= self.getProp('crossSection')
crossSection.checkBiaxialBendingForElement(self,nmbComb)
crossSection.checkYShearForElement(self,nmbComb)
crossSection.checkZShearForElement(self,nmbComb)'''

def controlULSCriterion2D():
    return '''recorder= self.getProp('ULSControlRecorder')
nmbComb= recorder.getCurrentCombinationName
self.getResistingForce()
crossSection= self.getProp('crossSection')
crossSection.checkUniaxialBendingForElement(self,nmbComb)
crossSection.checkYShearForElement(self,nmbComb)'''

