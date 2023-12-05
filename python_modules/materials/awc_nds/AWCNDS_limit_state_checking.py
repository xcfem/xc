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
import geom
import xc
from misc_utils import log_messages as lmsg
from materials import member_base
from materials import wood_member_base
from materials import limit_state_checking_base as lsc
from materials.awc_nds import AWCNDS_materials
from postprocess import control_vars as cv
from misc_utils import units_utils
from model import predefined_spaces
from actions import load_cases as lcm
from solution import predefined_solutions

class Member(wood_member_base.Member):
    ''' Beam and column members according to AWC NDS-2018. This class
    exists to compute and update the values of the buckling reduction
    factors of the members (chiN, chiLT, FcE and FbE).

    :ivar unbracedLengthX: unbraced length for torsional buckling 
                           about the longitudinal axis.
    :ivar unbracedLengthY: unbraced length for flexural buckling 
                           about y-axis.
    :ivar unbracedLengthZ: unbraced length for flexural buckling 
                           about z-axis.
    :ivar Cr: repetitive member factor.
    :ivar connection: connection type at member ends.
    :ivar memberRestraint: Member restrain condition according  to clause 4.4.1.2 of AWC_NDS2018.
    :ivar memberLoadingCondition: parameters defining the member condition in order to obtain 
                                  its effective length accordint to table 3.3.3 of AWC NDS-2018.
    :ivar loadCombDurationFactorFunction: function that returns the load 
                                          duration factor corresponding to
                                          a load combination expression
                                          (e.g.: 1.0*deadLoad+0.7*windLoad).
    '''
    def __init__(self, name, section, unbracedLengthX, unbracedLengthY= None, unbracedLengthZ= None, Cr= 1.0, connection= member_base.MemberConnection(), memberRestraint= AWCNDS_materials.MemberRestraint.notApplicable, memberLoadingCondition= AWCNDS_materials.MemberLoadingCondition(), loadCombDurationFactorFunction= None, lstLines=None):
        ''' Constructor. 

        :param name: object name.
        :param section: timber cross-section.
        :param Cr: repetitive member factor.
        :param unbracedLengthX: unbraced length for torsional buckling 
                               about the longitudinal axis.
        :param unbracedLengthY: unbraced length for flexural buckling 
                               about y-axis.
        :param unbracedLengthZ: unbraced length for flexural buckling 
                               about z-axis.
        :param connection: connection type at member ends.
        :param memberRestraint: Member restrain condition according  to clause 4.4.1.2 of AWC_NDS2018.
        :param memberLoadingCondition: parameters defining the member condition in order to obtain 
                                       its effective length accordint to table 3.3.3 of AWC NDS-2018.
        :param loadCombDurationFactorFunction: function that returns the load 
                                          duration factor corresponding to
                                          a load combination expression
                                          (e.g.: 1.0*deadLoad+0.7*windLoad).
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
        self.crossSection.Cr= Cr
        self.connection= connection
        self.memberRestraint= memberRestraint
        self.memberLoadingCondition= memberLoadingCondition
        self.loadCombDurationFactorFunction= loadCombDurationFactorFunction
            
    def getFcAdj(self):
        ''' Return the adjusted value of Fc including the column stability
            factor.'''
        sectionFbAdj= self.crossSection.getFcAdj()
        CP= self.getColumnStabilityFactor()
        return CP*sectionFbAdj
    
    def getFbAdj(self, majorAxis= True):
        ''' Return the adjusted value of Fb including the beam stability factor.

        :param majorAxis: if true return adjusted Fb for bending around major axis.
        '''
        sectionFbAdj= self.crossSection.getFbAdj(majorAxis= majorAxis)
        CL= self.getBeamStabilityFactor()
        return CL*sectionFbAdj
    
    def getFtAdj(self):
        ''' Return the adjusted value of Ft.'''
        return self.crossSection.getFtAdj()    
    
    def getEffectiveBucklingLengthCoefficientRecommended(self):
        '''Return the column effective buckling length coefficients
           according to NDS 2018 appendix G'''
        return self.connection.getEffectiveBucklingLengthCoefficientRecommended()
    
    def getColumnSlendernessRatioB(self):
        ''' Return the slenderness ratio of the member working as
            column for bending in the H plane.'''
        Ke= self.getEffectiveBucklingLengthCoefficientRecommended()
        return Ke*self.unbracedLengthY/self.crossSection.b
    
    def getColumnSlendernessRatioH(self):
        ''' Return the slenderness ratio of the member working as
            column for bending in the H plane.'''
        Ke= self.getEffectiveBucklingLengthCoefficientRecommended()
        return Ke*self.unbracedLengthZ/self.crossSection.h

    def getColumnSlendernessRatioBH(self):
        ''' Return both the slenderness ratios of the member working as
            column.'''
        Ke= self.getEffectiveBucklingLengthCoefficientRecommended()
        srB= Ke*self.unbracedLengthY/self.crossSection.b
        srH= Ke*self.unbracedLengthZ/self.crossSection.h
        return (srB,srH)
        
        
    def getColumnSlendernessRatio(self):
        ''' Return the slenderness ratio of the member working as
            column.'''
        (srB, srH)= self.getColumnSlendernessRatioBH()
        return max(srB,srH)

    def getColumnStabilityFactor(self):
        ''' Return the column stability factor according
            to expressions 3.7-1 and 15.2-1 of NDS-2.018. 
        '''
        (srB, srH)= self.getColumnSlendernessRatioBH()
        sr= max(srB,srH)
        E_adj= self.crossSection.getEminAdj()
        FcE= 0.822*E_adj/((sr)**2)
        Fc_adj= self.crossSection.getFcAdj()
        ratio= FcE/Fc_adj
        c= self.crossSection.wood.get37_1c()
        tmp= (1+ratio)/2.0/c
        retval= tmp-math.sqrt(tmp**2-ratio/c)
        if(self.crossSection.isBuiltUpSection()): # regular section.
           if(self.crossSection.b<self.crossSection.h): # weak axis pll to H
               if(srB>srH): # weak-axis slenderness ratio governs.
                   retval*= 0.6 # Equation 15.2-1
        return retval
    
    def getFcE1(self):
        ''' Return the critical buckling design value for compression
            members (F_{cE1}) as defined in section 3.9.2 of NDS-2.018
            for buckling about the major axis.
        '''
        E_adj= self.crossSection.getEminAdj()
        if(self.crossSection.h>self.crossSection.b): # Wide side: H
            return 0.822*E_adj/(self.getColumnSlendernessRatioH())**2
        else: # Wide side B
            return 0.822*E_adj/(self.getColumnSlendernessRatioB())**2
        
    def getFcE2(self):
        ''' Return the critical buckling design value for compression
            members (F_{cE2}) as defined in section 3.9.2 of NDS-2.018
            for buckling about the minor axis.
        '''
        E_adj= self.crossSection.getEminAdj()
        if(self.crossSection.h<self.crossSection.b): # Narrow side: H
            return 0.822*E_adj/(self.getColumnSlendernessRatioH())**2
        else: # Narrow side B
            return 0.822*E_adj/(self.getColumnSlendernessRatioB())**2

    def getFcE(self):
        ''' Return the critical buckling design value for compression
            members (F_{cE}) as defined in section 3.9.2 of NDS-2.018
            for buckling about the major and minor axis.'''
        E_adj= self.crossSection.getEminAdj()
        (srB, srH)= self.getColumnSlendernessRatioBH()
        EH= 0.822*E_adj/(srH)**2
        EB= 0.822*E_adj/(srB)**2
        if(self.crossSection.h>self.crossSection.b): # Wide side: H
            return (EH, EB)
        else: # Wide side B
            return (EB, EH)
        
    def getConcentratedLoadsBucklingLength(self, unbracedLength):
        ''' Return the effective length coefficient of the member according to table
            3.3.3 of NDS-2018.

           :param unbracedLength: unbraced length for the bending axis.
        '''
        return self.memberLoadingCondition.getEffectiveLength(unbracedLength= unbracedLength, section= self.crossSection)

    def getBeamStabilityFactor(self, majorAxis= True):
        ''' Return the beam stability factor according to clauses 3.3.3 
            and 4.4.1.2 of AWC NDS-2018.

        :param majorAxis: if true return adjusted Fb for bending around major axis.
        '''
        if(majorAxis):
            ## Check if
            if(self.memberRestraint>self.crossSection.getRequiredRestraint()):
                retval= 1.0
            else: ## Equation 3.3-6
                FbE= self.getFbECriticalBucklingDesignValue()
                FbAdj= self.crossSection.getFbAdj(majorAxis= majorAxis)
                ratio= FbE/FbAdj
                A= (1+ratio)/1.9
                B= A**2
                C= ratio/0.95
                retval= A-math.sqrt(B-C)
        else:
            retval= 1.0
        return retval
    
    # def getFbECriticalBucklingDesignValue(self):
    #     ''' Return the critical bucking design value for bending according to 
    #         section 3.3.3.8 of NDS-2018.
    #     '''
    #     RB= self.getBendingSlendernessRatio()
    #     return 1.2*self.crossSection.wood.Emin/RB**2
        
    def getColumnEffectiveLength(self):
        ''' Return the effective length of the member working 
            as column in the H and B planes.'''
        Ke= self.getEffectiveBucklingLengthCoefficientRecommended()
        return (Ke*self.unbracedLengthZ, Ke*self.unbracedLengthY)
        
    def getColumnBendingSlendernessRatioH(self):
        ''' Return the slenderness ratio of the member working as
            column for bending in the H plane.'''
        Ke= self.getEffectiveBucklingLengthCoefficientRecommended()
        le= Ke*self.unbracedLengthZ
        return math.sqrt(le*self.crossSection.h/self.crossSection.b**2)
    
    def getColumnBendingSlendernessRatioB(self):
        ''' Return the slenderness ratio of the member working as
            column for bending in the B plane.'''
        Ke= self.getEffectiveBucklingLengthCoefficientRecommended()
        le= Ke*self.unbracedLengthY
        return math.sqrt(le*self.crossSection.b/self.crossSection.h**2)

    def getColumnBendingSlendernessRatioHB(self):
        ''' Return the slenderness ratio of the member working as
            column for bending in the H and B planes.'''
        leH, leB= self.getColumnEffectiveLength()
        return (math.sqrt(leH*self.crossSection.h/self.crossSection.b**2), math.sqrt(leB*self.crossSection.b/self.crossSection.h**2))
    
    def getColumnBendingSlendernessRatio(self):
        ''' Return the maximum slenderness ratio for bending between the
            H and B planes.'''
        srH, srB= self.getColumnBendingSlendernessRatioHB()
        return max(srH, srB)
    
    def getBeamEffectiveLength(self):
        ''' Return the effective length of the member working as beam 
            according to table 3.3.3 of NDS-2018.
        '''
        return (self.getConcentratedLoadsBucklingLength(self.unbracedLengthZ), self.getConcentratedLoadsBucklingLength(self.unbracedLengthY))
    
    def getBeamBendingSlendernessRatioHB(self):
        ''' Return the slenderness ratio according to equation
            3.3-5 of NDS-2018.
        '''
        (leH, leB)= self.getBeamEffectiveLength()
        return (math.sqrt(leH*self.crossSection.h/self.crossSection.b**2), math.sqrt(leB*self.crossSection.b/self.crossSection.h**2))
        
    def getFbECriticalBucklingDesignValueHB(self):
        ''' Return the critical bucking design value for bending according to 
            section 3.3.3.8 of NDS-2018.
        '''
        sr= self.getBeamBendingSlendernessRatioHB()
        E_adj= self.crossSection.getEminAdj()
        return (1.2*E_adj/sr[0]**2, 1.2*E_adj/sr[1]**2)
    
    def getFbECriticalBucklingDesignValue(self):
        ''' Return the critical buckling desing value for bending (F_{bE}) 
            as defined in section 3.9.2 of NDS-2.018.
        '''
        tmp= self.getFbECriticalBucklingDesignValueHB()
        return min(tmp[0], tmp[1])

    def getBiaxialBendingEfficiency(self, Nd, Myd, Mzd, Vyd= 0.0, chiN=1.0, chiLT= 1.0):
        '''Return biaxial bending efficiency according to clause 3.9 of AWC-NDS2018.

        :param Nd: required axial strength.
        :param Myd: required bending strength (minor axis).
        :param Mzd: required bending strength (major axis).
        :param Vyd: required shear strength (major axis)
        :param chiN: column stability factor clause 3.7.1 of AWC-NDS2018 (default= 1.0).
        :param chiLT: beam stability factor clause 3.3.3 of AWC-NDS2018 (default= 1.0).
        '''
        # Critical buckling design values for compression.
        FcE= self.getFcE()
        FbE= self.getFbECriticalBucklingDesignValue()
        return self.crossSection.getBiaxialBendingEfficiency(Nd= Nd, Myd= Myd, Mzd= Mzd, FcE= FcE, FbE= FbE, chiN= chiN, chiLT= chiLT)

    def updateLoadDurationFactor(self, loadCombExpr):
        '''Update the value of the load duration factors.'''
        if(self.loadCombDurationFactorFunction):
            CD= self.loadCombDurationFactorFunction(loadCombExpr)
            self.crossSection.updateLoadDurationFactor(CD)
    
    def updateReductionFactors(self):
        '''Update the value of the appropriate reduction factors.'''
        chiN= self.getColumnStabilityFactor()
        chiLT= self.getBeamStabilityFactor()
        FcE= self.getFcE()
        FbE= self.getFbECriticalBucklingDesignValue()
        for e in self.elemSet:
             e.setProp('chiLT',chiLT) # flexural strength reduction factor.
             e.setProp('chiN',chiN) # compressive strength reduction factor.
             e.setProp('FcE', FcE) # critical buckling design values for compression members (both axis).
             e.setProp('FbE', FbE) # critical buckling design value for bending members.
    
    def installULSControlRecorder(self, recorderType, chiN: float= 1.0, chiLT: float= 1.0, FcE= (0.0,0.0), FbE= 0.0, calcSet= None):
        '''Install recorder for verification of ULS criterion.

        :param recorderType: type of the recorder to install.
        :param chiN: compressive strength reduction factor.
        :param chiLT: flexural strength reduction factor.
        :param FcE: critical buckling design values for compression members (both axis).
        :param FbE: critical buckling design value for bending members.
        :param calcSet: set of elements to be checked (defaults to 'None' which 
                        means that this set will be created elsewhere). In not
                        'None' the member elements will be appended to this set.
        '''
        recorder= self.createRecorder(recorderType, calcSet)
        self.crossSection.setupULSControlVars(self.elemSet, chiN= chiN, chiLT= chiLT, FcE= FcE, FbE= FbE)
        nodHndlr= self.getPreprocessor().getNodeHandler        
        if(nodHndlr.numDOFs==3):
            recorder.callbackRecord= controlULSCriterion2D()
        else:
            recorder.callbackRecord= controlULSCriterion()
#        recorder.callbackRestart= "print(\"Restart method called.\")" #20181121
        return recorder

def controlULSCriterion():
    return '''recorder= self.getProp('ULSControlRecorder')
nmbComb= recorder.getCurrentCombinationName
crossSection= self.getProp('crossSection')
crossSection.checkBiaxialBendingForElement(self,nmbComb)
crossSection.checkYShearForElement(self,nmbComb)
crossSection.checkZShearForElement(self,nmbComb)'''

def controlULSCriterion2D():
    return '''recorder= self.getProp('ULSControlRecorder')
nmbComb= recorder.getCurrentCombinationName
crossSection= self.getProp('crossSection')
crossSection.checkUniaxialBendingForElement(self,nmbComb)
crossSection.checkYShearForElement(self,nmbComb)'''

class BeamMember(Member):
    ''' Beam member according to chapter 3.3 of NDS-2018.

    '''
    def __init__(self, unbracedLength, section, connection= member_base.MemberConnection(), Cr= 1.0, memberRestraint= AWCNDS_materials.MemberRestraint.notApplicable, memberLoadingCondition= AWCNDS_materials.MemberLoadingCondition()):
        ''' Constructor. 

        :param unbracedLengthX: unbraced length for torsional buckling 
                                about the longitudinal axis.
        :param connection: connection type at beam ends.
        :param Cr: repetitive member factor.
        :param memberLoadingCondition: parameters defining the member condition in order to obtain 
                                       its effective length accordint to table 3.3.3 of AWC NDS-2018.
        '''
        super(BeamMember,self).__init__(name= None, unbracedLengthX= unbracedLength, section= section, Cr= Cr, connection= connection, memberRestraint= memberRestraint, memberLoadingCondition= memberLoadingCondition)        
        
    def getEffectiveLength(self):
        ''' Return the effective length of the beam according to table
            3.3.3 of NDS-2018.
        '''
        return self.getConcentratedLoadsBucklingLength(self.unbracedLengthX)
    
    def getBendingSlendernessRatio(self):
        ''' Return the slenderness ratio according to equation
            3.3-5 of NDS-2018.
        '''
        le= self.getEffectiveLength()
        return math.sqrt(le*self.crossSection.h/self.crossSection.b**2)
        
    def getBiaxialBendingEfficiency(self, Nd, Myd, Mzd, Vyd= 0.0, chiN=1.0, chiLT= 1.0):
        '''Return biaxial bending efficiency according to clause 3.9 of AWC-NDS2018.

        :param Nd: required axial strength.
        :param Myd: required bending strength (minor axis).
        :param Mzd: required bending strength (major axis).
        :param Vyd: required shear strength (major axis)
        :param chiN: column stability factor clause 3.7.1 of AWC-NDS2018 (default= 1.0).
        :param chiLT: beam stability factor clause 3.3.3 of AWC-NDS2018 (default= 1.0).
        '''
        # Critical buckling design values for compression.
        return self.crossSection.getBiaxialBendingEfficiency(Nd= Nd, Myd= Myd, Mzd= Mzd, chiN= chiN, chiLT= chiLT)

class ColumnMember(Member):
    ''' Column member according to chapter 3.7 and 3.9 of NDS-2018.

    :ivar unbracedLengthB: unbraced lenght for bending about weak axis.
    '''
    def __init__(self, unbracedLengthB, unbracedLengthH, section, repetitiveMemberFactor= 1.0, connection= member_base.MemberConnection(), memberRestraint= AWCNDS_materials.MemberRestraint.notApplicable, memberLoadingCondition= AWCNDS_materials.MemberLoadingCondition(), loadCombDurationFactorFunction= None):
        ''' Constructor. 

        :param unbracedLengthB: unbraced lenght for bending about weak axis (B<H).
        :param unbracedLengthH: unbraced lenght for bending about strong axis (HZB).
        :param section: member cross-section.
        :param repetitiveMemberFactor: repetitive member adjustment factor.
        :param connection: connection type.
        :param memberLoadingCondition: parameters defining the member condition in order to obtain 
                                       its effective length accordint to table 3.3.3 of AWC NDS-2018.
        :param loadCombDurationFactorFunction: function that returns the load 
                                          duration factor corresponding to
                                          a load combination expression
                                          (e.g.: 1.0*deadLoad+0.7*windLoad).
        '''
        super(ColumnMember,self).__init__(name= None, unbracedLengthX= unbracedLengthB, unbracedLengthZ= unbracedLengthH, Cr= repetitiveMemberFactor, section= section, connection= connection, memberRestraint= memberRestraint, memberLoadingCondition= memberLoadingCondition, loadCombDurationFactorFunction= loadCombDurationFactorFunction)

    
    def getCapacityFactor(self, Fc_adj, Fb1_adj, Fb2_adj, fc,fb1, fb2):
        ''' Return the capacity factor for members subjected to a 
            combination of bending about one or both principal axes 
            and axial compression according to section 3.9.2 of
            NDS-2.018.

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
        (FcE1, FcE2)= self.getFcE() #Critical buckling design values.
        FbE= self.getFbECriticalBucklingDesignValue()
        almostOne= 1-1e-15
        val393+= fb1/(Fb1_adj*(1-min(fc/FcE1,almostOne)))
        val393+= fb2/(Fb2_adj*(1-min(fc/FcE2,almostOne)-min(fb1/FbE,almostOne)**2))
        val394= fc/FcE2+(fb1/FbE)**2 #Equation 3-9-4
        return max(val393,val394)

def defineBuiltUpColumnMember(dimensions, unbracedLengthB, unbracedLengthH, wood, rotate= False):
    ''' Creates the column membert to be checked.

    :param dimensions: column dimensions (e.g.: '3x2x6').
    :param unbracedLength: column unbraced length.
    :param wood: wood material for dimensional lumber.
    :param rotate: if true swap B and H dimensions.
    '''
    dim= dimensions.split('x')
    nPieces= int(dim[0]) # number of pieces.
    # dim[1] thicknness of the piece. 
    # dim[2] height of the piece.
    (bb, hh)= AWCNDS_materials.dimensionLumberSizes[dim[1]+'x'+dim[2]]
    memberSection= None
    if(rotate):
        memberSection= AWCNDS_materials.BuiltUpLumberSection(name= dimensions, b= hh, h= nPieces*bb, woodMaterial= wood)
    else:
        memberSection= AWCNDS_materials.BuiltUpLumberSection(name= dimensions, b=nPieces*bb, h= hh, woodMaterial= wood)
    return ColumnMember(unbracedLengthB= unbracedLengthB, unbracedLengthH= unbracedLengthH, section= memberSection)


class StudArrangement(object):
    def __init__(self, name, studSection, studSpacing, wallHeight, loadCombDurationFactorFunction):
        '''Constructor.

        :param name: stud arrangement name.
        :param studSection: stud cross-section.
        :param studSpacing: distance between consecutive studs.
        :param wallHeight: height of the bearing wall.
        :param loadCombDurationFactorFunction: function that returns the load 
                                          duration factor corresponding to
                                          a load combination expression
                                          (e.g.: 1.0*deadLoad+0.7*windLoad).
        '''
        self.name= name
        self.studSpacing= studSpacing
        self.wallHeight= wallHeight
        self.studHeight= self.wallHeight-3*2*units_utils.inchToMeter
        self.stud= ColumnMember(unbracedLengthB= 0.3, unbracedLengthH= self.studHeight, section= studSection, repetitiveMemberFactor= 1.15, loadCombDurationFactorFunction= loadCombDurationFactorFunction)
        
    def updateLoadDurationFactor(self, loadCombExpr):
        '''Update the value of the load duration factors.'''
        self.stud.updateLoadDurationFactor(loadCombExpr)
        
    def checkStud(self, N, M):
        ## stresses
        fc= N/self.stud.crossSection.A()
        #fb1= M/self.stud.crossSection.getElasticSectionModulusZ()
        #fb2= 0.0

        #Fc_adj= self.stud.crossSection.getFcAdj()
        #Fb_adj= self.stud.crossSection.getFbAdj()
        #Checking
        CP= self.stud.getColumnStabilityFactor()
        FcE1= self.stud.getFcE1()
        FcE2= self.stud.getFcE2()
        RB= self.stud.getColumnSlendernessRatioB()
        FbE= self.stud.getFbECriticalBucklingDesignValue()
        CF= self.stud.getBiaxialBendingEfficiency(Nd= N, Myd= 0.0, Mzd= M, chiN= CP)
        results= dict()
        results['N']= N
        results['M']= M
        results['fc']= fc
        results['CP']= CP
        results['CD']= self.stud.crossSection.wood.CD # load duration factor.
        results['FcE1']= FcE1
        results['FcE2']= FcE2
        results['RB']= RB
        results['FbE=']= FbE
        results['CF']= CF
        return results

    def check(self, loadDict, loadCombinations):
        ''' Check the stud arrangement for the load arguments.

        :param loadDict: dictionary containing load values.
        :param loadCombinations: load combinations.
        '''
        
        worstCase= None
        worstCaseCF= 0.0
        # Read loads values from dictionary they will be used
        # in expr (see eval command in the for loop).
        deadLoad= loadDict['deadLoad']
        liveLoad= loadDict['liveLoad']
        snowLoad= loadDict['snowLoad']
        windLoad= loadDict['windLoad']
        if(__debug__): # Mostly to avoid "not used" warning from pyflakes3
            if(not deadLoad or not liveLoad or not snowLoad or not windLoad):
                AssertionError('Can\'t get the loads.')
        results= dict()
        # Checking
        for loadCombName in loadCombinations:
            expr= loadCombinations[loadCombName].expr
            # loads values are used here inside 'expr'
            value= eval(expr)
            N= value[1]*self.studSpacing
            M= value[0]*self.studHeight**2/8.0            

            self.updateLoadDurationFactor(expr)
            results[loadCombName]= self.checkStud(N= N, M= M)
            CF= results[loadCombName]['CF'][0]
            if(CF>worstCaseCF):
                worstCaseCF= CF
                worstCase= loadCombName
        return results, worstCase
    
class WallTopPlates(object):
    '''Plates on the top of a bearing wall.

    :ivar prb: XC finite element problem.
    :ivar plateSection: plate section.
    :ivar nodes: node handler.
    :ivar modelSpace: structural mechanics two-dimensional model of the plates.
    :ivar studSpacing: spacing of the studs that support the plates.
    :ivar trussSpacing: spacing of the trusses supported by the plates.
    :ivar pointLoadFactor: ??
    :ivar loadCombDurationFactorFunction: function that returns the load 
                                          duration factor corresponding to
                                          a load combination expression
                                          (e.g.: 1.0*deadLoad+0.7*windLoad).
    '''
    def __init__(self, title, plateSection, studSpacing, trussSpacing, pointLoadFactor, loadCombDurationFactorFunction= None):
        '''Constructor.

        :param title: problem title.
        :param plateSection: plate section.
        :param nodes: node handler.
        :param modelSpace: structural mechanics two-dimensional model of the plates.
        :param studSpacing: spacing of the studs that support the plates.
        :param trussSpacing: spacing of the trusses supported by the plates.
        :param pointLoadFactor: ??
        :param loadCombDurationFactorFunction: function that returns the load 
                                              duration factor corresponding to
                                              a load combination expression
                                              (e.g.: 1.0*deadLoad+0.7*windLoad).
        '''
        self.prb= xc.FEProblem()
        self.prb.title= title
        self.plateSection = plateSection
        preprocessor= self.prb.getPreprocessor   
        self.nodes= preprocessor.getNodeHandler
        self.modelSpace= predefined_spaces.StructuralMechanics2D(self.nodes)
        self.studSpacing= studSpacing
        self.trussSpacing= trussSpacing
        self.pointLoadFactor= pointLoadFactor
        self.loadCombDurationFactorFunction= loadCombDurationFactorFunction
        
    def genMesh(self):
        ''' Create the finite element mesh.'''
        prep= self.modelSpace.preprocessor
        pointHandler= prep.getMultiBlockTopology.getPoints
        infPoints= list()
        supPoints= list()
        for i in range(0,14):
            x= i*self.studSpacing
            infPoints.append(pointHandler.newPoint(geom.Pos3d(x,0.0,0.0)))
            supPoints.append(pointHandler.newPoint(geom.Pos3d(x,self.plateSection.h,0.0)))

        lines= prep.getMultiBlockTopology.getLines
        self.infSet= prep.getSets.defSet("inf")
        infLines= list()
        p0= infPoints[0]
        for p in infPoints[1:]:
            l= lines.newLine(p0.tag,p.tag)
            infLines.append(l)
            self.infSet.getLines.append(l)
            p0= p
        self.supSet= prep.getSets.defSet("sup")
        supLines= list()
        p0= supPoints[0]
        for p in supPoints[1:]:
            l= lines.newLine(p0.tag,p.tag)
            supLines.append(l)
            self.supSet.getLines.append(l)
            p0= p
        self.infSet.fillDownwards()
        self.supSet.fillDownwards()

        # Mesh
        section= self.plateSection.defElasticShearSection2d(prep)
        trfs= prep.getTransfCooHandler
        lin= trfs.newLinearCrdTransf2d("lin")
        seedElemHandler= prep.getElementHandler.seedElemHandler
        seedElemHandler.defaultMaterial= section.name
        seedElemHandler.defaultTransformation= lin.name
        elem= seedElemHandler.newElement("ElasticBeam2d",xc.ID([0,0]))
        if __debug__:
            if(not elem):
                AssertionError('Can\'t create the element.')
        infSetMesh= self.infSet.genMesh(xc.meshDir.I)
        if __debug__:
            if(not infSetMesh):
                AssertionError('Can\'t create the mesh.')
        self.infSet.fillDownwards()
        supSetMesh= self.supSet.genMesh(xc.meshDir.I)
        if __debug__:
            if(not supSetMesh):
                AssertionError('Can\'t create the mesh.')
        self.supSet.fillDownwards()

        ## Loaded nodes.
        self.loadedNodes= list()
        pos= supPoints[0].getPos+geom.Vector3d(self.studSpacing/2.0,0,0) #Position of the first loaded node
        xLast= supPoints[-1].getPos.x
        while(pos.x<xLast):
            self.loadedNodes.append(self.supSet.getNearestNode(pos))
            pos+= geom.Vector3d(self.trussSpacing,0.0,0.0)

        ## Loaded elements.
        self.loadedElements= list()
        for e in self.supSet.elements:
            self.loadedElements.append(e)

        # Constraints
        self.supportedNodes= list()
        for p in infPoints:
            n= p.getNode()
            self.modelSpace.fixNode00F(n.tag)
            self.supportedNodes.append(n)

        for n in self.supSet.nodes:
            pos= n.getInitialPos3d
            nInf= self.infSet.getNearestNode(pos)
            self.modelSpace.constraints.newEqualDOF(nInf.tag,n.tag,xc.ID([1]))

        for p in supPoints[1:-1]:
            n= p.getNode()
            pos= n.getInitialPos3d
            nInf= self.infSet.getNearestNode(pos)
            self.modelSpace.constraints.newEqualDOF(nInf.tag,n.tag,xc.ID([0]))
    
    def applyLoads(self, load):
        ''' Apply load to nodes and elements.'''
        uniformLoadFactor= 1.0-self.pointLoadFactor
        pointLoad= self.pointLoadFactor*load
        for n in self.loadedNodes:
            n.newLoad(xc.Vector([0.0,-pointLoad,0.0]))
        uniformLoad= uniformLoadFactor*load/self.trussSpacing
        for e in self.loadedElements:
            e.vector2dUniformLoadGlobal(xc.Vector([0.0,-uniformLoad]))
            
    def defineLoads(self, loadDict):
        ''' Define the loads from the values stored in the dictionary argument.

        :param loadDict: Python dictionary storing the load values.
        '''
        # Actions
        ## Define load cases
        loadCaseManager= lcm.LoadCaseManager(self.modelSpace.preprocessor)
        loadCaseNames= list(loadDict.keys())
        loadCaseManager.defineSimpleLoadCases(loadCaseNames)

        ## Define load values.
        for lcName in loadDict:
            value= loadDict[lcName]
            cLC= loadCaseManager.setCurrentLoadCase(lcName)
            if __debug__:
                if(not cLC):
                    AssertionError('Can\'t create the load case.')
            self.applyLoads(value)
            
    def checkPlates(self, loadDurationFactor):
        ''' Check the structural integrity of the plates.

        :param loadDurationFactor: load duration factor.
        '''
        results= dict()
        self.plateSection.updateLoadDurationFactor(loadDurationFactor)
        results['CD']= loadDurationFactor
        ## Bending stiffness
        uYMax= -1e6
        for n in self.infSet.nodes:
            uY= -n.getDisp[1]
            uYMax= max(uY,uYMax)

        results['uYMax']= uYMax

        ## Bending strength
        sgMax= -1e6
        for e in self.supSet.elements:
            e.getResistingForce()
            m1= e.getM1
            sg1= abs(m1/self.plateSection.xc_material.sectionProperties.I*self.plateSection.h/2)
            sgMax= max(sgMax,sg1)
            m2= e.getM2
            sg2= abs(m2/self.plateSection.xc_material.sectionProperties.I*self.plateSection.h/2)
            sgMax= max(sgMax,sg2)

        Fb_adj= self.plateSection.getFbAdj()
        FbCF= sgMax/Fb_adj
        results['sgMax']= sgMax
        results['Fb_adj']= Fb_adj
        results['bendingCF']= FbCF

        ## Shear strength
        tauMax= -1e6
        for e in self.supSet.elements:
            e.getResistingForce()
            v1= e.getV1
            tau1= abs(v1/self.plateSection.xc_material.sectionProperties.A)
            tauMax= max(tauMax,tau1)
            v2= e.getV2
            tau2= abs(v2/self.plateSection.xc_material.sectionProperties.A)
            tauMax= max(tauMax,tau2)

        tauMax*= (self.studSpacing-self.plateSection.h)/self.studSpacing
        Fv_adj= self.plateSection.getFvAdj()
        FvCF= tauMax/Fv_adj
        results['tauMax']= tauMax
        results['Fv_adj']= Fv_adj
        results['shearCF']= FvCF

        ## Compression perpendicular to grain

        ### Reactions
        preprocessor= self.infSet.getPreprocessor
        preprocessor.getNodeHandler.calculateNodalReactions(False,1e-7)
        RMax= -1e12;
        for n in self.supportedNodes:
            RMax= max(RMax,abs(n.getReaction[1]))
        sgMax= RMax/self.plateSection.A()
        lb= (self.plateSection.h)/0.0254 # Length in bearing parallel to the grain of the wood.
        Cb= (lb+0.375)/lb
        Fc_perp= self.plateSection.getFc_perpAdj(Cb) #Perpendicular to grain
        Fc_perpCF= sgMax/Fc_perp
        results['RMax']= RMax
        results['sgMax']= sgMax
        results['Fc_perp']= Fc_perp
        results['Fc_perpCF']= Fc_perpCF
        
        return results

    def checkCombination(self, comb):
        ''' Compute the internal forces corresponding to the load combination
            and check the structural integrity of the plates for those
            internal forces.

        :param comb: load combination.
        '''
        preprocessor= self.prb.getPreprocessor
        preprocessor.resetLoadCase()
        preprocessor.getLoadHandler.addToDomain(comb)
        # Solution
        solution= predefined_solutions.SimpleStaticLinear(self.prb)
        solution.setup()
        analysis= solution.analysis
        result= analysis.analyze(1)
        if(result!=0):
            className= type(self).__name__
            methodName= sys._getframe(0).f_code.co_name
            lmsg.error(className+'.'+methodName+'; can\'t solve for load case: '+str(comb)+'.')
        loadDurationFactor= self.loadCombDurationFactorFunction(comb)
        results= self.checkPlates(loadDurationFactor= loadDurationFactor)
        preprocessor.getLoadHandler.removeFromDomain(comb)
        return results

    def checkCombinations(self, combContainer):
        ''' Compute the internal forces corresponding to the load combinations
            and check the structural integrity of the plates.


        :param combContainer: load combination container.
        '''
        results= dict()
        for comb in combContainer:
            results[comb]= self.checkCombination(comb)
        # Search for worst cases.
        worstDeflectionCase= None
        worstDeflectionValue= 0.0
        worstBendingCase= None
        worstBendingCF= 0.0
        worstShearCase= None
        worstShearCF= 0.0
        worstPerpComprCase= None
        worstPerpComprCF= 0.0
        for lcName in results:
            deflectionValue= abs(results[lcName]['uYMax'])
            if(deflectionValue>worstDeflectionValue):
                worstDeflectionValue= deflectionValue
                worstDeflectionCase= lcName
                
            bendingCF= results[lcName]['bendingCF']
            if(bendingCF>worstBendingCF):
                worstBendingCF= bendingCF
                worstBendingCase= lcName
                
            shearCF= results[lcName]['shearCF']
            if(shearCF>worstShearCF):
                worstShearCF= shearCF
                worstShearCase= lcName
                
            perpComprCF= results[lcName]['Fc_perpCF']
            if(perpComprCF>worstPerpComprCF):
                worstPerpComprCF= perpComprCF
                worstPerpComprCase= lcName
        # Store the values in a dictionary.
        worstResults= {'worstDeflectionCase':worstDeflectionCase, 'worstDeflectionValue':worstDeflectionValue, 'worstBendingCase':worstBendingCase, 'worstBendingCF':worstBendingCF, 'worstShearCase':worstShearCase, 'worstShearCF':worstShearCF, 'worstPerpComprCase':worstPerpComprCase, 'worstPerpComprCF':worstPerpComprCF}
        return results, worstResults

    def check(self, loadDict, combContainer):
        ''' Check the plates for the load arguments.

        :param loadDict: dictionary containing the load values.
        :param combContainer: load combinations.
        '''
        # Create model
        self.genMesh()
        # Define loads.
        self.defineLoads(loadDict)
        # Define load combinations.
        combContainer.dumpCombinations(self.modelSpace.preprocessor)
        # Checking
        return self.checkCombinations(combContainer.SLS.qp)
    
class AWCNDSBiaxialBendingControlVars(cv.BiaxialBendingStrengthControlVars):
    '''Control variables for biaxial bending normal stresses LS 
    verification in steel-shape elements according to AISC.

    :ivar FcE: critical buckling design value for compression members (both axis).
    '''
    def __init__(self,idSection= 'nil',combName= 'nil',CF= -1.0,N= 0.0,My= 0.0,Mz= 0.0,Ncrd=0.0,McRdy=0.0,McRdz=0.0, FcE= (0.0,0.0), FbE= 0.0, chiLT=1.0, chiN= 1.0):
        '''
        Constructor.

        :param idSection: section identifier
        :param combName: name of the load combinations to deal with
        :param CF:       capacity factor (efficiency) (defaults to -1)
        :param N:        axial force (defaults to 0.0)
        :param My:       bending moment about Y axis (defaults to 0.0)
        :param Mz:       bending moment about Z axis (defaults to 0.0)
        :param Ncrd:     design strength to axial compression
        :param McRdy:    design moment strength about Y (weak) axis
        :param McRdz:    design moment strength about Z (strong) axis
        :param FcE: critical buckling design value for compression members (both axis).
        :param FbE: critical bucking design value for bending according to 
                    section 3.3.3.8 of NDS-2018.
        :param chiLT:    reduction factor for lateral-torsional buckling (defaults to 1)
        :param chiN:     reduction factor for compressive strength (defaults to 1)
        '''
        super(AWCNDSBiaxialBendingControlVars,self).__init__(idSection,combName,CF,N,My,Mz,Ncrd=Ncrd,McRdy=McRdy,McRdz=McRdz, chiLT=chiLT, chiN= chiN)
        self.FcE= FcE
        self.FbE= FbE
        
    def getDict(self):
        ''' Return a dictionary containing the object data.'''
        retval= super(AWCNDSBiaxialBendingControlVars,self).getDict()
        retval.update({'FcE':self.FcE, 'FbE':self.FbE})
        return retval
       
    def setFromDict(self,dct):
        ''' Set the data values from the dictionary argument.'''
        super(AWCNDSBiaxialBendingControlVars,self).setFromDict(dct)
        self.FcE= dct['FcE']
        self.FbE= dct['FbE']

class BiaxialBendingNormalStressController(lsc.LimitStateControllerBase2Sections):
    '''Object that controls normal stresses limit state.'''

    ControlVars= AWCNDSBiaxialBendingControlVars
    
    def __init__(self, limitStateLabel, solutionProcedureType= lsc.defaultStaticLinearSolutionProcedure):
        ''' Constructor.

        :param limitStateLabel: label that identifies the limit state.
        :param solutionProcedureType: type of the solution procedure to use
                                      when computing load combination results.
        '''
        super(BiaxialBendingNormalStressController,self).__init__(limitStateLabel= limitStateLabel, solutionProcedureType= solutionProcedureType)

    def updateEfficiency(self, elem, elementInternalForces):
        ''' Compute the efficiency of the material of the element
            subjected to the internal forces argument and update
            its value if its bigger than the previous one.

        :param elem: finite element whose material will be checked.
        :param elementInternalForces: internal forces acting on the cross-section.
        '''
        # Get section properties.
        crossSection= elem.getProp('crossSection')
        if(not crossSection):
            className= type(self).__name__
            methodName= sys._getframe(0).f_code.co_name
            lmsg.warning(className+'.'+methodName+'; undefined "crossSection" property for element: '+str(elem.tag)+'; nothing done.')
        else:
            # Check each element section.
            for lf in elementInternalForces:
                # Compute efficiency.
                CFtmp= crossSection.getBiaxialBendingEfficiency(Nd= lf.N, Myd= lf.My, Mzd= lf.Mz, FcE= lf.FcE, FbE= lf.FbE, chiN= lf.chiN, chiLT= lf.chiLT)[0]
                sectionLabel= self.getSectionLabel(lf.idSection)
                label= self.limitStateLabel+sectionLabel
                # Update efficiency.
                if(CFtmp>elem.getProp(label).CF):
                    elem.setProp(label,self.ControlVars(idSection= sectionLabel, combName= lf.idComb, CF= CFtmp, N= lf.N, My= lf.My, Mz= lf.Mz, FcE= lf.FcE, FbE= lf.FbE, chiN= lf.chiN, chiLT= lf.chiLT))
                
class ShearController(lsc.LimitStateControllerBase2Sections):
    '''Object that controls shear limit state.'''

    ControlVars= cv.ShearYControlVars
    
    def __init__(self, limitStateLabel, solutionProcedureType= lsc.defaultStaticLinearSolutionProcedure):
        ''' Constructor.

        :param limitStateLabel: label that identifies the limit state.
        :param solutionProcedureType: type of the solution procedure to use
                                      when computing load combination results.
        '''
        super(ShearController,self).__init__(limitStateLabel= limitStateLabel, solutionProcedureType= solutionProcedureType)

    def updateEfficiency(self, elem, elementInternalForces):
        ''' Compute the efficiency of the element steel shape
            subjected to the internal forces argument and update
            its value if its bigger than the previous one.

        :param elem: finite element whose section will be checked.
        :param elementInternalForces: internal forces acting on the steel shape.
        '''
        # Get section properties.
        crossSection= elem.getProp('crossSection')
        if(not crossSection):
            className= type(self).__name__
            methodName= sys._getframe(0).f_code.co_name
            lmsg.warning(className+'.'+methodName+'; undefined "crossSection" property for element: '+str(elem.tag)+'; nothing done.')
        else:
            # Check each element section.
            for sectionIForces in elementInternalForces:
                # Compute efficiency.
                CFtmp= crossSection.getYShearEfficiency(sectionIForces.Vy)
                sectionLabel= self.getSectionLabel(sectionIForces.idSection)
                label= self.limitStateLabel+sectionLabel
                # Update efficiency.
                if(CFtmp>elem.getProp(label).CF):
                    elem.setProp(label,self.ControlVars(sectionLabel+'s',sectionIForces.idComb,CFtmp,sectionIForces.Vy))
