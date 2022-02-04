# -*- coding: utf-8 -*-
''' Classes and functions for limit state checking according to the
    National Design Specification of the American Wood Council.'''

from __future__ import print_function
from __future__ import division

__author__= "Ana Ortega (AO_O) Luis C. Pérez Tato (LCPT)"
__copyright__= "Copyright 2022 LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "ana.ortega@ciccp.es, l.pereztato@gmail.com"

import sys
import math
from misc_utils import log_messages as lmsg
from materials import member_base
from materials import wood_member_base
from materials import limit_state_checking_base as lsc
from postprocess import control_vars as cv
from postprocess import limit_state_data as lsd

import xc_base
import geom
import xc

class Member(wood_member_base.Member):
    ''' Beam and column members according to ANSI AISC 360-16.

    :ivar unbracedLengthX: unbraced length for torsional buckling 
                           about the longitudinal axis.
    :ivar unbracedLengthY: unbraced length for flexural buckling 
                           about y-axis.
    :ivar unbracedLengthZ: unbraced length for flexural buckling 
                           about z-axis.
    '''
    def __init__(self, name, section, unbracedLengthX, unbracedLengthY= None, unbracedLengthZ= None, Cb= None, lstLines=None):
        ''' Constructor. 

        :param name: object name.
        :param crossSection: timber cross-section.
        :param lstLines: ordered list of lines that make up the beam.
        '''
        super(Member,self).__init__(name, section, lstLines)
        self.unbracedLengthX= unbracedLengthX
        if(unbracedLengthY):
            self.unbracedLengthY= unbracedLengthY
        else:
            self.unbracedLengthY= unbracedLengthX
        if(unbracedLengthZ):
            self.unbracedLengthZ= unbracedLengthZ
        else:
            self.unbracedLengthZ= unbracedLengthX
            
    def installULSControlRecorder(self,recorderType):
        '''Install recorder for verification of ULS criterion.

        :param recorderType: type of the recorder to install.
        '''
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
        self.crossSection.setupULSControlVars(self.elemSet)
        if(nodes.numDOFs==3):
            recorder.callbackRecord= controlULSCriterion2D()
        else:
            recorder.callbackRecord= controlULSCriterion()
#        recorder.callbackRestart= "print(\"Restart method called.\")" #20181121
        return recorder

def controlULSCriterion():
    funcName= sys._getframe(0).f_code.co_name
    lmsg.error(funcName+': not implemented yet.')
    return ''
# '''recorder= self.getProp('ULSControlRecorder')
# nmbComb= recorder.getCurrentCombinationName
# self.getResistingForce()
# crossSection= self.getProp('crossSection')
# crossSection.checkBiaxialBendingForElement(self,nmbComb)
# crossSection.checkYShearForElement(self,nmbComb)
# crossSection.checkZShearForElement(self,nmbComb)'''

def controlULSCriterion2D():
    funcName= sys._getframe(0).f_code.co_name
    lmsg.error(funcName+': not implemented yet.')
    return ''
# '''recorder= self.getProp('ULSControlRecorder')
# nmbComb= recorder.getCurrentCombinationName
# self.getResistingForce()
# crossSection= self.getProp('crossSection')
# crossSection.checkUniaxialBendingForElement(self,nmbComb)
# crossSection.checkYShearForElement(self,nmbComb)'''

class MemberBase(object):
    ''' Base class for beam and column members according to chapter 
        3 of NDS-2018.
    '''
    def __init__(self, unbracedLength, section, connection= member_base.MemberConnection()):
        ''' Constructor. '''
        self.unbracedLength= unbracedLength
        self.section= section
        self.connection= connection
        
class BeamMember(MemberBase):
    ''' Beam member according to chapter 3.3 of NDS-2018.'''
    def __init__(self, unbracedLength, section, connection= member_base.MemberConnection()):
        ''' Constructor. '''
        super(BeamMember,self).__init__(unbracedLength, section, connection)
    def getEffectiveLength(self,numberOfConcentratedLoads= 0, lateralSupport= False, cantilever= False):
        ''' Return the effective length of the beam according to table
            3.3.3 of NDS-2018.

           :param numberOfConcentratedLoads: number of concentrated loads equally
                                             spaced along the beam (0: uniform load).
           :param lateralSupport: if true beam has a lateral support on each load.
           :param cantilever: if true cantilever beam otherwise single span beam.
        '''
        retval= self.unbracedLength
        if(cantilever):
            if(numberOfConcentratedLoads==0):
                retval*= 1.33 # Uniform load
            else:
                retval*= 1.87 # Concentrated load at unsupported end.
        else:
            ratio= self.unbracedLength/self.section.h
            if(numberOfConcentratedLoads==0):
                if(ratio<7.0):
                    retval*= 2.06 # Uniform load
                else:
                    retval= 1.63*self.unbracedLength+3.0*self.section.h
            elif((numberOfConcentratedLoads==1) and (not lateralSupport)):
                if(ratio<7.0):
                    retval*= 1.80 # Uniform load
                else:
                    retval= 1.37*self.unbracedLength+3.0*self.section.h
            elif(numberOfConcentratedLoads==1):
                if(lateralSupport):
                    retval*=1.11
                else:
                    lmsg.error('Load case not implemented.')
                    retval*=10.0
            elif(numberOfConcentratedLoads==2):
                if(lateralSupport):
                    retval*=1.68
                else:
                    lmsg.error('Load case not implemented.')
                    retval*=10.0
            elif(numberOfConcentratedLoads==3):
                if(lateralSupport):
                    retval*=1.54
                else:
                    lmsg.error('Load case not implemented.')
                    retval*=10.0
            elif(numberOfConcentratedLoads==4):
                if(lateralSupport):
                    retval*=1.68
                else:
                    lmsg.error('Load case not implemented.')
                    retval*=10.0
            elif(numberOfConcentratedLoads==5):
                if(lateralSupport):
                    retval*=1.73
                else:
                    lmsg.error('Load case not implemented.')
                    retval*=10.0
            elif(numberOfConcentratedLoads==6):
                if(lateralSupport):
                    retval*=1.78
                else:
                    lmsg.error('Load case not implemented.')
                    retval*=10.0
            elif(numberOfConcentratedLoads==7):
                if(lateralSupport):
                    retval*=1.84
                else:
                    lmsg.error('Load case not implemented.')
                    retval*=10.0
        return retval
    def getBendingSlendernessRatio(self,numberOfConcentratedLoads= 0, lateralSupport= False, cantilever= False):
        ''' Return the slenderness ratio according to equation
            3.3-5 of NDS-2018.

           :param numberOfConcentratedLoads: number of concentrated loads equally
                                             spaced along the beam (0: uniform load).
           :param lateralSupport: if true beam has a lateral support on each load.
           :param cantilever: if true cantilever beam otherwise single span beam.
        '''
        le= self.getEffectiveLength(numberOfConcentratedLoads, lateralSupport, cantilever)
        return math.sqrt(le*self.section.h/self.section.b**2)
    def getFbECriticalBucklingDesignValue(self,numberOfConcentratedLoads= 0, lateralSupport= False, cantilever= False):
        ''' Return the critical bucking design value for bending according to 
            section 3.3.3.8 of NDS-2018.

           :param numberOfConcentratedLoads: number of concentrated loads equally
                                             spaced along the beam (0: uniform load).
           :param lateralSupport: if true beam has a lateral support on each load.
           :param cantilever: if true cantilever beam otherwise single span beam.
        '''
        RB= self.getBendingSlendernessRatio(numberOfConcentratedLoads, lateralSupport, cantilever)
        return 1.2*self.section.wood.Emin/RB**2
    def getBeamStabilityFactor(self,numberOfConcentratedLoads= 0, lateralSupport= False, cantilever= False):
        ''' Return the beam stability factor according to equation 
            3.3.6 of NDS-2018.

           :param numberOfConcentratedLoads: number of concentrated loads equally
                                             spaced along the beam (0: uniform load).
           :param lateralSupport: if true beam has a lateral support on each load.
           :param cantilever: if true cantilever beam otherwise single span beam.
        '''
        FbE= self.getFbECriticalBucklingDesignValue(numberOfConcentratedLoads, lateralSupport, cantilever)
        ratio= FbE/self.section.wood.getFb(self.section.h)
        A= (1+ratio)/1.9
        B= A**2
        C= ratio/0.95
        return A-math.sqrt(B-C)

class ColumnMember(MemberBase):
    ''' Column member according to chapter 3.7 and 3.9 of NDS-2018.'''
    def __init__(self, unbracedLengthB, unbracedLengthH, section, connection= member_base.MemberConnection()):
        ''' Constructor. '''
        super(ColumnMember,self).__init__(unbracedLengthB, section, connection)
        self.unbracedLengthH= unbracedLengthH

    def getUnbracedLengthB(self):
        ''' Return the B unbraced length.'''
        return self.unbracedLength

    def getEffectiveBucklingLengthCoefficientRecommended(self):
        '''Return the column effective buckling length coefficients
           according to NDS 2018 appendix G'''
        return self.connection.getEffectiveBucklingLengthCoefficientRecommended()
    def getBSlendernessRatio(self):
        ''' Return the slenderness ratio for the B dimension.'''
        Ke= self.getEffectiveBucklingLengthCoefficientRecommended()
        return Ke*self.getUnbracedLengthB()/self.section.b
    
    def getHSlendernessRatio(self):
        ''' Return the slenderness ratio for the H dimension.'''
        Ke= self.getEffectiveBucklingLengthCoefficientRecommended()
        return Ke*self.unbracedLengthH/self.section.h
        
    def getSlendernessRatio(self):
        ''' Return the slenderness ratio.'''
        Ke= self.getEffectiveBucklingLengthCoefficientRecommended()
        srB= Ke*self.getUnbracedLengthB()/self.section.b
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
        le= Ke*self.getUnbracedLengthB()
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
        FcE1= self.getFcE1(E_adj) #Critical buckling design values.
        FcE2= self.getFcE2(E_adj)
        FbE= self.getFbE(E_adj)
        almostOne= 1-1e-15
        val393+= fb1/(Fb1_adj*(1-min(fc/FcE1,almostOne)))
        val393+= fb2/(Fb2_adj*(1-min(fc/FcE2,almostOne)-min(fb1/FbE,almostOne)**2))
        val394= fc/FcE2+(fb1/FbE)**2 #Equation 3-9-4
        return max(val393,val394)
