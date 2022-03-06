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
from materials.awc_nds import AWCNDS_materials
from postprocess import control_vars as cv
from postprocess import limit_state_data as lsd

import xc_base
import geom
import xc

class Member(wood_member_base.Member):
    ''' Beam and column members according to AWC NDS-2018.

    :ivar unbracedLengthX: unbraced length for torsional buckling 
                           about the longitudinal axis.
    :ivar unbracedLengthY: unbraced length for flexural buckling 
                           about y-axis.
    :ivar unbracedLengthZ: unbraced length for flexural buckling 
                           about z-axis.
    '''
    def __init__(self, name, section, unbracedLengthX, unbracedLengthY= None, unbracedLengthZ= None, lstLines=None):
        ''' Constructor. 

        :param name: object name.
        :param section: timber cross-section.
        :param unbracedLengthX: unbraced length for torsional buckling 
                               about the longitudinal axis.
        :param unbracedLengthY: unbraced length for flexural buckling 
                               about y-axis.
        :param unbracedLengthZ: unbraced length for flexural buckling 
                               about z-axis.
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

class MemberBase(object):
    ''' Base class for beam and column members according to chapter 
        3 of NDS-2018.

    :ivar Cr: repetitive member factor.
    '''
    def __init__(self, unbracedLength, section, Cr= 1.0, connection= member_base.MemberConnection(), memberRestraint= AWCNDS_materials.MemberRestraint.notApplicable, memberLoadingCondition= AWCNDS_materials.MemberLoadingCondition()):
        ''' Constructor. 

        :param unbracedLength: length between bracing elements.
        :param section: member cross-section.
        :param Cr: repetitive member factor.
        :param connection: connection type at beam ends.
        :param memberLoadingCondition: parameters defining the member condition in order to obtain 
                                       its effective length accordint to table 3.3.3 of AWC NDS-2018.
        '''
        self.unbracedLength= unbracedLength
        self.crossSection= section
        self.Cr= Cr
        self.connection= connection
        self.memberRestraint= memberRestraint
        self.memberLoadingCondition= memberLoadingCondition
        
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
        sectionFbAdj= self.crossSection.getFbAdj(majorAxis= majorAxis, Cr= self.Cr)
        CL= self.getBeamStabilityFactor()
        return CL*sectionFbAdj

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
        retval= .001
        if(majorAxis):
            ## Check if
            if(self.memberRestraint>self.crossSection.getRequiredRestraint()):
                retval= 1.0
            else: ## Equation 3.3-6
                FbE= self.getFbECriticalBucklingDesignValue()
                FbAdj= self.crossSection.getFbAdj(majorAxis= majorAxis, Cr= self.Cr)
                ratio= FbE/FbAdj
                A= (1+ratio)/1.9
                B= A**2
                C= ratio/0.95
                retval= A-math.sqrt(B-C)
        else:
            retval= 1.0
        return retval
    
    def getFbECriticalBucklingDesignValue(self):
        ''' Return the critical bucking design value for bending according to 
            section 3.3.3.8 of NDS-2018.
        '''
        RB= self.getBendingSlendernessRatio()
        return 1.2*self.crossSection.wood.Emin/RB**2
           
class BeamMember(MemberBase):
    ''' Beam member according to chapter 3.3 of NDS-2018.

    '''
    def __init__(self, unbracedLength, section, connection= member_base.MemberConnection(), Cr= 1.0, memberRestraint= AWCNDS_materials.MemberRestraint.notApplicable, memberLoadingCondition= AWCNDS_materials.MemberLoadingCondition()):
        ''' Constructor. 

        :param unbracedLength: length between bracing elements.
        :param connection: connection type at beam ends.
        :param Cr: repetitive member factor.
        :param memberLoadingCondition: parameters defining the member condition in order to obtain 
                                       its effective length accordint to table 3.3.3 of AWC NDS-2018.
        '''
        super(BeamMember,self).__init__(unbracedLength= unbracedLength, section= section, Cr= Cr, connection= connection, memberRestraint= memberRestraint, memberLoadingCondition= memberLoadingCondition)        
        
    def getEffectiveLength(self):
        ''' Return the effective length of the beam according to table
            3.3.3 of NDS-2018.
        '''
        return self.getConcentratedLoadsBucklingLength(self.unbracedLength)
    
    def getBendingSlendernessRatio(self):
        ''' Return the slenderness ratio according to equation
            3.3-5 of NDS-2018.
        '''
        le= self.getEffectiveLength()
        return math.sqrt(le*self.crossSection.h/self.crossSection.b**2)
        
    def getFtAdj(self):
        ''' Return the adjusted value of Ft.'''
        return self.crossSection.getFtAdj()    
    
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

class ColumnMember(MemberBase):
    ''' Column member according to chapter 3.7 and 3.9 of NDS-2018.

    :ivar unbracedLengthB: unbraced lenght for bending about weak axis.
    '''
    def __init__(self, unbracedLengthB, unbracedLengthH, section, connection= member_base.MemberConnection(), memberRestraint= AWCNDS_materials.MemberRestraint.notApplicable, memberLoadingCondition= AWCNDS_materials.MemberLoadingCondition()):
        ''' Constructor. 

        :param unbracedLengthB: unbraced lenght for bending about weak axis (B<H).
        :param unbracedLengthH: unbraced lenght for bending about strong axis (HZB).
        :param section: member cross-section.
        :param connection: connection type.
        :param memberLoadingCondition: parameters defining the member condition in order to obtain 
                                       its effective length accordint to table 3.3.3 of AWC NDS-2018.
        '''
        super(ColumnMember,self).__init__(unbracedLength= unbracedLengthB, section= section, connection= connection, memberRestraint= memberRestraint, memberLoadingCondition= memberLoadingCondition)
        self.unbracedLengthH= unbracedLengthH

    def getUnbracedLengthB(self):
        ''' Return the B unbraced length a.'''
        return self.unbracedLength

    def getEffectiveBucklingLengthCoefficientRecommended(self):
        '''Return the column effective buckling length coefficients
           according to NDS 2018 appendix G'''
        return self.connection.getEffectiveBucklingLengthCoefficientRecommended()
    def getColumnSlendernessRatioB(self):
        ''' Return the slenderness ratio of the member working as
            column for bending in the H plane.'''
        Ke= self.getEffectiveBucklingLengthCoefficientRecommended()
        return Ke*self.getUnbracedLengthB()/self.crossSection.b
    
    def getColumnSlendernessRatioH(self):
        ''' Return the slenderness ratio of the member working as
            column for bending in the H plane.'''
        Ke= self.getEffectiveBucklingLengthCoefficientRecommended()
        return Ke*self.unbracedLengthH/self.crossSection.h
        
    def getColumnSlendernessRatio(self):
        ''' Return the slenderness ratio of the member working as
            column.'''
        Ke= self.getEffectiveBucklingLengthCoefficientRecommended()
        srB= Ke*self.getUnbracedLengthB()/self.crossSection.b
        srH= Ke*self.unbracedLengthH/self.crossSection.h
        return max(srB,srH)

    def getColumnStabilityFactor(self):
        ''' Return the column stability factor according
            to expression 3.7-1 of NDS-2.018. 
        '''
        sr= self.getColumnSlendernessRatio()
        E_adj= self.crossSection.wood.getEminAdj()
        FcE= 0.822*E_adj/((sr)**2)
        Fc_adj= self.crossSection.getFcAdj()
        ratio= FcE/Fc_adj
        c= self.crossSection.wood.get37_1c()
        tmp= (1+ratio)/2.0/c
        return tmp-math.sqrt(tmp**2-ratio/c)
    
    def getFcE1(self):
        ''' Return the critical buckling design value for compression
            members (F_{cE1}) as defined in section 3.9.2 of NDS-2.018
            for buckling about the major axis.
        '''
        E_adj= self.crossSection.wood.getEminAdj()
        if(self.crossSection.h>self.crossSection.b): # Wide side: H
            return 0.822*E_adj/(self.getColumnSlendernessRatioH())**2
        else: # Wide side B
            return 0.822*E_adj/(self.getColumnSlendernessRatioB())**2
        
    def getFcE2(self):
        ''' Return the critical buckling design value for compression
            members (F_{cE2}) as defined in section 3.9.2 of NDS-2.018
            for buckling about the minor axis.
        '''
        E_adj= self.crossSection.wood.getEminAdj()
        if(self.crossSection.h<self.crossSection.b): # Narrow side: H
            return 0.822*E_adj/(self.getColumnSlendernessRatioH())**2
        else: # Narrow side B
            return 0.822*E_adj/(self.getColumnSlendernessRatioB())**2

    def getFcE(self):
        ''' Return the critical buckling design value for compression
            members (F_{cE}) as defined in section 3.9.2 of NDS-2.018
            for buckling about the major and minor axis.'''
        E_adj= self.crossSection.wood.getEminAdj()
        EH= 0.822*E_adj/(self.getColumnSlendernessRatioH())**2
        EB= 0.822*E_adj/(self.getColumnSlendernessRatioB())**2
        if(self.crossSection.h>self.crossSection.b): # Wide side: H
            return (EH, EB)
        else: # Wide side B
            return (EB, EH)

    def getBeamEffectiveLength(self):
        ''' Return the effective length of the member working as beam 
            according to table 3.3.3 of NDS-2018.
        '''
        return (self.getConcentratedLoadsBucklingLength(self.unbracedLengthH), self.getConcentratedLoadsBucklingLength(self.getUnbracedLengthB()))
    
    def getBeamBendingSlendernessRatioHB(self):
        ''' Return the slenderness ratio according to equation
            3.3-5 of NDS-2018.
        '''
        (leH, leB)= self.getBeamEffectiveLength()
        return (math.sqrt(leH*self.crossSection.h/self.crossSection.b**2), math.sqrt(leB*self.crossSection.b/self.crossSection.h**2))
        
    def getColumnEffectiveLength(self):
        ''' Return the effective length of the member working 
            as column in the H and B planes.'''
        Ke= self.getEffectiveBucklingLengthCoefficientRecommended()
        return (Ke*self.unbracedLengthH, Ke*self.getUnbracedLengthB())
        
    def getColumnBendingSlendernessRatioH(self):
        ''' Return the slenderness ratio of the member working as
            column for bending in the H plane.'''
        Ke= self.getEffectiveBucklingLengthCoefficientRecommended()
        le= Ke*self.unbracedLengthH
        return math.sqrt(le*self.crossSection.h/self.crossSection.b**2)
    
    def getColumnBendingSlendernessRatioB(self):
        ''' Return the slenderness ratio of the member working as
            column for bending in the B plane.'''
        Ke= self.getEffectiveBucklingLengthCoefficientRecommended()
        le= Ke*self.getUnbracedLengthB()
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
    
    def getFbECriticalBucklingDesignValueHB(self):
        ''' Return the critical bucking design value for bending according to 
            section 3.3.3.8 of NDS-2018.
        '''
        sr= self.getBeamBendingSlendernessRatioHB()
        E_adj= self.crossSection.wood.getEminAdj()
        return (1.2*E_adj/sr[0]**2, 1.2*E_adj/sr[1]**2)
    
    def getFbECriticalBucklingDesignValue(self):
        ''' Return the critical buckling desing value for bending (F_{bE}) 
            as defined in section 3.9.2 of NDS-2.018.
        '''
        tmp= self.getFbECriticalBucklingDesignValueHB()
        return min(tmp[0], tmp[1])
    
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
    def __init__(self,limitStateLabel):
        super(BiaxialBendingNormalStressController,self).__init__(limitStateLabel)

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
    def __init__(self,limitStateLabel):
        super(ShearController,self).__init__(limitStateLabel)

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
