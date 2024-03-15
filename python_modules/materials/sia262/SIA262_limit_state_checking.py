# -*- coding: utf-8 -*-

from __future__ import division
from __future__ import print_function

__author__= "Luis C. Pérez Tato (LCPT)"
__copyright__= "Copyright 2016, LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

import sys
import math
import geom
import scipy.interpolate
from materials.sia262 import SIA262_materials
from materials.sections import rebar_family as rf
from materials import limit_state_checking_base as lsc
from postprocess import control_vars as cv
from rough_calculations import ng_simple_bending_reinforcement
from materials.sections.fiber_section import fiber_sets
from misc_utils import log_messages as lmsg
from postprocess.reports import common_formats as fmt

# Returns adherence stress (Pa) for concrete type (tableau 19 SIA 262).
adherenceStress_x= [12e6 , 16e6,20e6 ,25e6 ,30e6 ,35e6 ,40e6 ,45e6 ,50e6]
adherenceStress_y= [1.5e6,1.8e6,2.1e6,2.4e6,2.7e6,3.0e6,3.3e6,3.6e6,3.8e6]
adherenceStress= scipy.interpolate.interp1d(adherenceStress_x,adherenceStress_y)

def getBasicAnchorageLength(phi,fck, fsd):
    """Returns anchorage length according to SIA 262.

    :param phi: bar diameter.
    :param fck: concrete characteristic compressive strength.
    :param fsd: steel design yield strength.
    """
    fbd= adherenceStress(abs(fck))
    return max(fsd/fbd/4.0,25.0)*phi
     
# SIA 262 section 4.4.2

def AsMinTensionStressLimit(concrete,sgAdm,t):
    '''Return minimun amount of bonded reinforcement to limit
       the stress in reinforcement under tension.

    :param concrete: concrete material.
    :param sgAdm: stress limit.
    :param t: smallest dimension of the tensioned member.
    '''
    fctm= concrete.fctm()
    kt= SIA262_materials.reductionFactorKT(t)
    return kt*fctm*t/sgAdm

def MinReinfAreaUnderTension(concrete,crackCRequirement,spacing,t):
    '''Return the minimun amount of bonded reinforcement to control cracking
       for reinforced concrete sections under tension.

    :param concrete: concrete material.
    :param crackCRequirement: crack control requirement.
    :param spacing: rebar spacing.
    :param t: smallest dimension of the tensioned member.
    '''
    fctm= concrete.fctm()
    kt= SIA262_materials.reductionFactorKT(t) # 4.4.1.3 SIA 262:2014
    sgAdm= SIA262_materials.stressLimits(crackCRequirement,spacing)
    return kt*fctm*t/sgAdm

def MinReinfAreaUnderFlexion(concrete,concreteCover,crackCRequirement,spacing,t):
    '''Return the minimun amount of bonded reinforcement to control cracking
       for reinforced concrete sections under flexion.

    :param concrete: concrete material.
    :param concreteCover: concrete cover of the tensioned bars.
    :param crackCRequirement: crack control requirement.
    :param spacing: rebar spacing.
    :param t: smallest dimension of the tensioned member.
    '''
    fctd= concrete.fctm()*SIA262_materials.reductionFactorKT(t/3)
    z= 0.9*(t-concreteCover)
    w= 1/6.0*t**2
    sgAdm= SIA262_materials.stressLimits(crackCRequirement,spacing)
    return fctd*w/sgAdm/z

class RebarController(lsc.RebarController):
    '''Control of some parameters as development length 
       minimum reinforcement and so on.'''

    def __init__(self, crackCRequirement, concreteCover= 35e-3, spacing= 150e-3):
        '''Constructor.'''
        super(RebarController,self).__init__(concreteCover= concreteCover, spacing= spacing)
        self.crackControlRequirement= crackCRequirement

    def getBasicAnchorageLength(self, concrete, phi, steel):
        """Returns anchorage length according to SIA 262.

        :param concrete: concrete material.
        :param phi: bar diameter.
        :param steel: reinforcement steel.
        """
        return getBasicAnchorageLength(phi,-concrete.fck, steel.fyd())

    # SIA 262 section 4.4.2

    def getAsMinTensionStressLimit(self, concrete, sgAdm,t):
        '''Return minimun amount of bonded reinforcement to limit
           the stress in reinforcement under tension.

        :param concrete: concrete material.
        :param sgAdm: stress limit.
        :param t: smallest dimension of the tensioned member.
        '''
        return AsMinTensionStressLimit(concrete,sgAdm,t)

    def getMinReinfAreaInTension(self, concrete, spacing, t, memberType= None):
        '''Return the minimun amount of bonded reinforcement to control cracking
           for reinforced concrete sections under tension.

        :param concrete: concrete material.
        :param spacing: rebar spacing.
        :param t: smallest dimension of the tensioned member.
        :param memberType: member type; slab, wall, beam or column.
        '''
        return MinReinfAreaUnderTension(concrete,self.crackControlRequirement,spacing,t)

    def getMinReinfAreaInBending(self, concrete, concreteCover, spacing,t, memberType= None):
        '''Return the minimun amount of bonded reinforcement to control cracking
           for reinforced concrete sections under flexion.

        :param concrete: concrete material.
        :param concreteCover: concrete cover of the tensioned bars.
        :param spacing: rebar spacing.
        :param t: smallest dimension of the tensioned member.
        :param memberType: member type; slab, wall, beam or column.
        '''
        return MinReinfAreaUnderFlexion(concrete,concreteCover,self.crackControlRequirement,spacing,t)
  

#Check normal stresses limit state.

class BiaxialBendingNormalStressController(lsc.BiaxialBendingNormalStressControllerBase):
    '''Object that controls normal stresses limit state.'''

    def __init__(self, limitStateLabel):
        ''' Constructor.
        
        :param limitStateLabel: label that identifies the limit state.
        '''
        super(BiaxialBendingNormalStressController,self).__init__(limitStateLabel)

class UniaxialBendingNormalStressController(lsc.UniaxialBendingNormalStressControllerBase):
    '''Object that controls normal stresses limit state (uniaxial bending).'''

    def __init__(self, limitStateLabel):
        ''' Constructor.
        
        :param limitStateLabel: label that identifies the limit state.
        '''
        super(UniaxialBendingNormalStressController,self).__init__(limitStateLabel)

# Shear checking.

def VuNoShearRebars(concrete,steel,Nd,Md,AsTrac,b,d, z= None):
    '''Section shear capacity without shear reinforcement.

       :param concrete: concrete material.
       :param steel: steel material of the tensioned reinforcement.
       :param Nd: axial internal force on the section.
       :param Md: bending moment on the section.
       :param AsTrac: area of tensioned reinforcement.
       :param b: section width.
       :param d: section effective depth.
       :param z: inner lever arm (if None z= 0.9*d).
    '''
    Mu= ng_simple_bending_reinforcement.Mu(AsTrac,concrete.fcd(),steel.fyd(),b,d, z= z, c_depth= concrete.getCDepth()) 
    return VuNoShearRebarsSIA262(concrete,Nd,Md,Mu,b,d)

def VuNoShearRebarsSIA262(concrete,Nd,Md,Mu,b,d):
    '''Section shear capacity without shear reinforcement. 
       Simplified method according to document: «Dalles sans étriers soumises
       à l'effort tranchant» par Aurelio Muttoni (file: beton-VD1V2.pdf).'''
    h=d/0.9   #mejorar
    if Nd<=0:
        mDd=-Nd*(h/2-d/3)
    else:
        mDd=-Nd*(h/2-(h-d))
    kv= 2.2*(Md-mDd)/(Mu-mDd)
    kd= 1/(1+kv*d)
    taucd= concrete.taucd()
    return kd*taucd*b*d

def VuShearRebars90SIA262(Asw,s,steel,z,alpha= math.radians(30)):
    '''Shear capacity of shear reinforcement.'''  
    cot_alpha= 1.0/math.tan(alpha)
    retval= Asw/s*z*steel.fyd()*cot_alpha
    return retval

def VuWithShearRebarsSIA262(concrete,steel,Nd,Md,Mu,b,d,Asw,s,z,alpha= math.radians(30)):
    '''Section shear capacity with shear reinforcement.'''  
    Vcu= VuNoShearRebarsSIA262(concrete,Nd,Md,Mu,b,d)
    Vsu= VuShearRebars90SIA262(Asw,s,steel,z,alpha)
    return Vcu+Vsu

class ShearController(lsc.ShearControllerBase):
    '''Object that controls shear limit state according to SIA 262.'''

    ControlVars= cv.SIATypeRCShearControlVars
    
    def __init__(self, limitStateLabel, solutionProcedureType= lsc.defaultStaticLinearSolutionProcedure):
        ''' Constructor.
        
        :param limitStateLabel: label that identifies the limit state.
        :param solutionProcedureType: type of the solution procedure to use
                                      when computing load combination results.
        '''
        super(ShearController,self).__init__(limitStateLabel= limitStateLabel, fakeSection= True, solutionProcedureType= solutionProcedureType)

    def setSection(self,rcSection):
        self.concrete= rcSection.getConcreteType() #Arreglar
        self.steel= rcSection.getReinfSteelType()
        self.width= rcSection.b
        self.effectiveDepth= 0.9* rcSection.h
        self.mechanicLeverArm= 0.9*self.effectiveDepth #Mejorar
        shReinf= rcSection.getShearReinfZ()
        self.AsTrsv= shReinf.getAs()
        self.s= shReinf.shReinfSpacing
        self.Vcu= 0.0 # Concrete contribution to the shear strength.
        self.Vsu= 0.0 # Rebar contribution to the shear strength.

    def getVuNoShearRebars(self,Nd,Md,AsTrac,b,d):
        '''Section shear capacity without shear reinforcement.

           :param Nd: axial internal force on the section.
           :param Md: bending moment on the section.
           :param AsTrac: area of tensioned reinforcement.
        '''
        return VuNoShearRebars(self.concrete, self.steel,Nd,Md,AsTrac,self.width,self.effectiveDepth)
    
    def calcVcu(self, Nd, Md, Mu):
        ''' Computes the shear strength of the section without 
            shear reinforcement.
        '''
        self.Vcu= VuNoShearRebarsSIA262(self.concrete,Nd,abs(Md),abs(Mu),self.width,self.effectiveDepth)
        return self.Vcu

    def calcVsu(self):
        ''' Computes the shear strength of the section without shear reinforcement.
            s= 1.0 because AsTrsv ya incorpora todas las ramas en un metro.'''
        self.Vsu= VuShearRebars90SIA262(self.AsTrsv,1.0,self.steel,self.mechanicLeverArm)

    def calcVu(self, Nd, Md, Mu, Vd):
        '''  Computes section ultimate shear strength.'''
        self.Vcu= 0.0
        self.Vsu= 0.0
        self.calcVcu(Nd,abs(Md),abs(Mu))
        self.calcVsu()
        return self.Vu()

    def Vu(self):
        return self.Vcu+self.Vsu

    def check(self,elements,nmbComb):
        '''
        Check the shear strength of the RC section.
           XXX Orientation of the transverse reinforcement is not
           taken into account.
        '''
        lmsg.info("Postprocesing combination: "+nmbComb)
        # XXX torsional deformation ingnored.

        for e in elements:
            e.getResistingForce()
            scc= e.getSection()
            idSection= e.getProp("idSection")
            section= scc.getProp('sectionData')
            self.setSection(section)
            shReinf= section.getShearReinfY()
            #AsTrsv= shReinf.getAs()
            #alpha= shReinf.angAlphaShReinf
            theta= shReinf.angThetaConcrStruts

            VuTmp= section.getRoughVcuEstimation() 
            NTmp= scc.getStressResultantComponent("N")
            MyTmp= scc.getStressResultantComponent("My")
            momentThreshold= VuTmp/1e6
            if(abs(MyTmp)<momentThreshold): #bending moment too small.
                MyTmp= momentThreshold
            MzTmp= scc.getStressResultantComponent("Mz")
            if(abs(MzTmp)<momentThreshold): #bending moment too small.
                MzTmp= momentThreshold
            VyTmp= scc.getStressResultantComponent("Vy")
            if(abs(VyTmp)>VuTmp/5.0): #We "eliminate" very small shear forces.
                posEsf= geom.Pos3d(NTmp,MyTmp,MzTmp)
                diagInt= e.getProp("diagInt")
                intersection= diagInt.getIntersection(posEsf)
                Mu= intersection.z
                VuTmp= self.calcVu(NTmp,MzTmp, Mu, VyTmp) #Mz associated with Vy
            else:
                #Fictitious ultimate moment.
                Mu= self.concrete.Ecm()*section.getIz_RClocalZax()*1e-3/section.h
                if(abs(MzTmp)>Mu):
                    errMsg= 'Fictitious ultimate moment too low;'
                    errMsg+= ' Mu= '+ str(Mu) + ' MzTmp= ' + str(MzTmp)
                    lmsg.error(errMsg)
            #13.02.2018 right-justified in order to be run only if VyTmp>VuTmp/5.0
            if(VuTmp!=0.0):
                FCtmp= abs(VyTmp)/VuTmp
            else:
                FCtmp= 10
            if(FCtmp>=e.getProp(self.limitStateLabel).CF):
                VzTmp= scc.getStressResultantComponent("Vz")
                e.setProp(self.limitStateLabel,self.ControlVars(idSection= idSection, combName= nmbComb, CF= FCtmp, N= NTmp, My= MyTmp, Mz= MzTmp, Mu= Mu, Vy= VyTmp, Vz= VzTmp, theta= theta, Vcu= self.Vcu, Vsu= self.Vsu, Vu=VuTmp)) # Worst case
            #13.02.2018 End of changes


class CrackControlSIA262(lsc.CrackControlBaseParameters):
    '''Crack control checking of a reinforced concrete section
     according to SIA 262.

    :ivar limitStress: limit value for rebar stresses.
    '''
    ControlVars= cv.CrackControlVars
    
    def __init__(self, limitStateLabel, limitStress):
        ''' Constructor.
        
        :param limitStateLabel: label that identifies the limit state.
        :param limitStress: limit value for rebar stresses.
        '''
        super(CrackControlSIA262,self).__init__(limitStateLabel)
        self.limitStress= limitStress #Limit value for rebar stresses.
        
    def calcRebarStress(self, scc):
        '''Returns average stress in rebars.'''
        section= scc.getProp('sectionData')
        concreteTag= section.getConcreteType().matTagK
        reinfMatTag= section.getReinfSteelType().matTagK
        if(not scc.hasProp("rcSets")):
          scc.setProp("rcSets", fiber_sets.fiberSectionSetupRC3Sets(scc,concreteTag,self.concreteFibersSetName,reinfMatTag,self.rebarFibersSetName))
        rcSets= scc.getProp("rcSets")
        concrFibers= rcSets.concrFibers.fSet
        #reinfFibers= rcSets.reinfFibers.fSet
        tensionedReinforcement= rcSets.tensionFibers

        self.claseEsfuerzo= scc.getStrClaseEsfuerzo(0.0)
        self.tensionedRebars.number= rcSets.getNumTensionRebars()
        if(self.tensionedRebars.number>0):
            scc.computeCovers(self.tensionedRebarsFiberSetName)
            scc.computeSpacement(self.tensionedRebarsFiberSetName)
            self.eps1= concrFibers.getStrainMax()
            self.eps2= max(concrFibers.getStrainMin(),0.0)
            self.tensionedRebars.setup(tensionedReinforcement)
        return self.tensionedRebars.averageStress

    def initControlVars(self,elements):
        ''' Initialize control variables over elements.

          :param elements: elements to define control variables in
        '''
        for e in elements:
            e.setProp(self.limitStateLabel, self.ControlVars(idSection= e.getProp('idSection')))

    def check(self,elements,nmbComb):
        '''Crack control checking.'''
        lmsg.warning("REWRITE NEEDED. See equivalent function in  CrackControlSIA262PlanB.")
        for e in elements:
          scc= e.getSection()
          # idSection= e.getProp("idSection")
          sigma_s= self.calcRebarStress(scc)
          if(sigma_s>e.getProp("sg_sCP")):
            e.setProp("sg_sCP",sigma_s) # Worst case
            e.setProp("HIPCP",nmbComb)
            Ntmp= scc.getStressResultantComponent("N")
            MyTmp= scc.getStressResultantComponent("My")
            MzTmp= scc.getStressResultantComponent("Mz")
            e.setProp("NCP",Ntmp)
            e.setProp("MyCP",MyTmp)
            e.setProp("MzCP",MzTmp)


class CrackControlSIA262PlanB(CrackControlSIA262):

    ControlVars= cv.CrackControlVars
    def __init__(self, limitStateLabel, limitStress):
        ''' Constructor.
        
        :param limitStateLabel: label that identifies the limit state.
        :param limitStress: limit value for rebar stresses.
        '''
        super(CrackControlSIA262PlanB,self).__init__(limitStateLabel, limitStress)

    def check(self,elements,nmbComb):
        '''Crack control.'''
        for e in elements:
            e.getResistingForce()
            scc= e.getSection()
            idSection= e.getProp("idSection")
            Ntmp= scc.getStressResultantComponent("N")
            MyTmp= scc.getStressResultantComponent("My")
            MzTmp= scc.getStressResultantComponent("Mz")
            datosScc= scc.getProp('sectionData')
            stressCalc= datosScc.getStressCalculator()
            stressCalc.solve(Ntmp, MyTmp)
            sigma_sPos= stressCalc.sgs
            sigma_sNeg= stressCalc.sgsp
            CFPos= sigma_sPos/self.limitStress #Positive face capacity factor.
            CFNeg= sigma_sNeg/self.limitStress #Negative face capacity factor.
            elementControlVars= None
            if(e.hasProp(self.limitStateLabel)):
                elementControlVars= e.getProp(self.limitStateLabel)
            else:
                elementControlVars= self.ControlVars(idSection,cv.CrackControlBaseVars(nmbComb,CFPos,Ntmp,MyTmp,MzTmp,sigma_sPos),cv.CrackControlBaseVars(nmbComb,CFNeg,Ntmp,MyTmp,MzTmp,sigma_sNeg))
            if(CFPos>elementControlVars.crackControlVarsPos.CF):
                elementControlVars.crackControlVarsPos= cv.CrackControlBaseVars(nmbComb,CFPos,Ntmp,MyTmp,MzTmp,sigma_sPos)
            if(CFNeg>elementControlVars.crackControlVarsNeg.CF):
                elementControlVars.crackControlVarsNeg= cv.CrackControlBaseVars(nmbComb,CFNeg,Ntmp,MyTmp,MzTmp,sigma_sNeg)
            e.setProp(self.limitStateLabel,elementControlVars)


def procesResultVerifFISSIA262(preprocessor, nmbComb, limitStress, limitStateLabel= 'SLS_crackControl'):
    '''Crack control checking of reinforced concrete sections.

    :param preprocessor: preprocessor for the finite element problem.
    :param nmbComb: name of the load combination.    
    :param limitStress: limit value for rebar stresses.
    :param limitStateLabel: label identifying the limit state.
    '''
    lmsg.info("Postprocesing combination: "+nmbComb+"\n")

    secHAParamsFis= CrackControlSIA262(limitStateLabel= limitStateLabel, limitStress= limitStress)
    elements= preprocessor.getSets.getSet("total").elements
    secHAParamsFis.check(elements,nmbComb)

def procesResultVerifFISSIA262PlanB(preprocessor, nmbComb, limitStress, limitStateLabel= 'SLS_crackControl'):
    '''Crack control checking of reinforced concrete sections.

    :param preprocessor: preprocessor for the finite element problem.
    :param nmbComb: name of the load combination.
    :param limitStress: limit value for rebar stresses.
    :param limitStateLabel: label identifying the limit state.
    '''
    lmsg.info("Postprocesing combination: "+nmbComb+"\n")

    secHAParamsFis= CrackControlSIA262PlanB(limitStateLabel= limitStateLabel, limitStress= limitStress)
    elements= preprocessor.getSets.getSet("total").elements
    secHAParamsFis.check(elements,nmbComb)

# Control of fatigue limit state according to SIA 262.

def estimateSteelStress(sccData, N, M, As, y):
    eNC= sccData.depth/3.0
    denom= math.copysign(0.8*As*0.9*sccData.depth,y)
    retval= M/denom
    if(retval<0.0):
        f= M/(0.8*0.9*sccData.depth)
        retval= -f/(0.2**sccData.getAc())*10.0
    if(abs(N)>1e-6):
        exc= abs(M/N)
        if(exc<eNC):
            retval= N/sccData.getAc()*10.0
            retval+= M/sccData.getI()*y
    return retval

def estimateSteelStressPos(sccData, N, M):
    ''' Estimate positive stress on the steel.

    :param sccData: section geometry data.
    :param N: axial load.
    :param M: bending moment.
    '''
    As= sccData.getAsPos()
    y = sccData.getYAsPos()
    return estimateSteelStress(sccData, N, M, As, y)

def estimateSteelStressNeg(sccData, N, M):
    ''' Estimate negative stress on the steel.

    :param sccData: section geometry data.
    :param N: axial load.
    :param M: bending moment.
    '''
    As= sccData.getAsNeg()
    y = sccData.getYAsNeg()
    return estimateSteelStress(sccData, N, M, As, y)

def estimateSigmaC(sccData, sigma_sPos, sigma_sNeg):
    sgMax= max(sigma_sPos,sigma_sNeg)
    retval= 0.0
    if(sgMax<0.0):
        retval= (sigma_sPos+sigma_sNeg)/2.0/10.0
    else:
        sgMin= min(sigma_sPos,sigma_sNeg)
        if(sgMin<=0):
            Ac= 0.1*sccData.getAc()
            if(sgMax==sigma_sPos):
                retval= -sgMax*sccData.getAsPos()/Ac
            else:
              retval= -sgMax*sccData.getAsNeg()/Ac
    return retval

def estimateSigmaCPlanB(sccData, N, M):
    Ac= 0.1*sccData.getAc()
    Ic= sccData.getI()
    sg1= N/Ac+M/Ic*sccData.depth/2.0
    sg2= N/Ac-M/Ic*sccData.depth/2.0
    sgc= min(min(sg1,sg2),0.0)
    return 2.0*sgc #Ver Jiménez Montoya 12.3 (page 244)

def getSteelFatigueLimitStress(barShape, barDiameter, overlappingConnectionsOnly= True):
    ''' Return the design value of the fatigue strength according to expression
        86 from clause 4.3.8.2.3 and table 13 of SIA 262 2013.

    :param barShape: 'straight' or 'stirrup'
    :param barDiameter: diameter of the reinforcement bar.
    :param overlappingConnectionsOnly: if false, bars are welded of coupled with
                                       mechanical splices and the fatigue 
                                       strength is accordingly reduced.
    '''
    DSigma_sd_fat= 55e6 # table 13 welded or coupled joints.
    if(overlappingConnectionsOnly): # no welds, no couplers
        if(barShape=='straight'):
            if(barDiameter<=20e-3): # small diameter
                DSigma_sd_fat= 145e6
            else:
                DSigma_sd_fat= 120e6
        elif(barShape=='stirrup'):
            if(barDiameter<=16e-3): # small diameter
                DSigma_sd_fat= 135e6
            else:
                funcName= sys._getframe(0).f_code.co_name
                lmsg.error(funcName+'; not implemented for stirrups with diameter greater than 16 mm')
    return 0.8*DSigma_sd_fat # Equation 86
    
def getConcreteLimitStress(sccData,kc, controlVars):
    '''4.3.8.3.1 SIA 262 2013.'''
    fcd= sccData.getConcreteType().fcd()
    concreteStresses= controlVars.getConcreteMaxMinStresses()
    #sg_max= concreteStresses[0]
    sg_min= concreteStresses[1]
    return min(-0.5*kc*fcd+0.45*abs(sg_min),-0.9*kc*fcd)

def getShearLimit(sccData,controlVars,vu):
    '''4.3.8.3.2 SIA 262 2013'''
    v_0= controlVars.state0.Vy
    v_1= controlVars.state1.Vy
    vd_min= min(v_0,v_1)
    vd_max= max(v_0,v_1)
    coc= vd_min
    if(abs(vd_max)<1e-12):
        if(vd_max<0):
          coc*=-1 #change sign.
    else:
        coc/= vd_max
    if(coc>=0):
        retval= min(0.5*vu+0.45*abs(vd_min),0.9*vu)
    else:
        retval= 0.5*vu-abs(vd_min)
    if(retval<0):
        methodName= sys._getframe(0).f_code.co_name
        lmsg.warning(methodName+'; negative limit:'+str(retval)+'.')
    return retval

def getShearCF(controlVars):
    '''Fatigue shear capacity factor.'''
    return max(abs(controlVars.state0.Vy),abs(controlVars.state1.Vy))/controlVars.shearLimit

class FatigueController(lsc.LimitStateControllerBase):
    '''Object that controls RC fatigue limit state.'''

    ControlVars= cv.FatigueControlVars
    def __init__(self,limitStateLabel, solutionProcedureType= lsc.defaultStaticLinearSolutionProcedure):
        ''' Constructor.
        
        :param limitStateLabel: label that identifies the limit state.
        :param solutionProcedureType: type of the solution procedure to use
                                      when computing load combination results.
        '''
        super(FatigueController,self).__init__(limitStateLabel= limitStateLabel, solutionProcedureType= solutionProcedureType)

    def initControlVars(self,elements):
        ''' Defines limit state control variables for all the elements.'''
        for e in elements:
            idS= e.getProp("idSection")
            e.setProp(self.limitStateLabel,self.ControlVars(idSection= idS))

    def check(self,elements, combNm):
        '''
        Checks the fatigue limit state for all the elements in the given set and
        for the  combination passed as a parameter. The verification is carried out
        following the procedure proposed in the standard SIA262 (art. 4.3.8)

        :param elements:  xc set of elements to be checked
        :param combNm:    combination name e.g.:

            - ELUF0: unloaded structure (permanent loads)
            - ELUF1: fatigue load in position 1.
            - ELUF2: fatigue load in position 2 (XXX not yet implemented!!!).
            - ELUF3: fatigue load in position 3 (XXX not yet implemented!!!).
            - ...
        '''
        lmsg.info('Controlling limit state: ',self.limitStateLabel, ' for load combination: ',combNm,"\n")


        index= int(combNm[-1])
        for e in elements:
            e.getResistingForce()
            scc= e.getSection()
            N= scc.getStressResultantComponent("N")
            My= scc.getStressResultantComponent("My")
            Mz= scc.getStressResultantComponent("Mz")
            Vy= scc.getStressResultantComponent("Vy")
            #Vz= scc.getStressResultantComponent("Vz")
            datosScc= scc.getProp('sectionData')
            stressCalc= datosScc.getStressCalculator()
            stressCalc.solve(N, My)
            sigma_sPos= stressCalc.sgs
            sigma_sNeg= stressCalc.sgsp
            sigma_c= stressCalc.sgc
            controlVars= e.getProp(self.limitStateLabel)
            resultsComb= cv.FatigueControlBaseVars(combNm,-1.0,N, My, Mz,Vy,sigma_sPos, sigma_sNeg,sigma_c)
            if(index==0):
                controlVars.state0= resultsComb
            else:
                controlVars.state1= resultsComb
                kc= 1.0 #XXX  SIA 262 4.2.1.7
                controlVars.concreteLimitStress= getConcreteLimitStress(datosScc,kc,controlVars)
                controlVars.concreteBendingCF= controlVars.getConcreteMinStress()/controlVars.concreteLimitStress

                section= scc.getProp('sectionData')
                concreteSectionShearParams= ShearController(self.limitStateLabel)
                concreteSectionShearParams.setSection(section)
                posEsf= geom.Pos3d(N,My,Mz)
                diagInt= e.getProp("diagInt")
                FCflex= diagInt.getCapacityFactor(posEsf)
                controlVars.Mu= My/FCflex
                controlVars.Vu= concreteSectionShearParams.calcVu(N,My, controlVars.Mu, Vy)

                controlVars.shearLimit= getShearLimit(section,controlVars,controlVars.Vu)
                controlVars.concreteShearCF= getShearCF(controlVars)
                e.setProp(self.limitStateLabel,controlVars)

##################
# Rebar families.#
##################

class SIARebarFamily(rf.RebarFamily):
    ''' Family or reinforcement bars with checking according to SIA 262.

      :ivar crackControlRequirement: crack control requirement A B or C.
    '''
    minDiams= 50
    def __init__(self,steel,diam,spacing,concreteCover, crackControlRequirement= 'B'):
        ''' Constructor.

        :param steel: reinforcing steel material.
        :param diam: diameter of the bars.
        :param spacing: spacing of the bars.
        :param concreteCover: concrete cover of the bars.
        :param crackControlRequirement: crack control requierement A, B or C.
        '''
        super(SIARebarFamily,self).__init__(steel,diam,spacing,concreteCover)
        self.crackControlRequirement= crackControlRequirement
      
    def getCopy(self):
        ''' Virtual constructor.'''
        return SIARebarFamily(steel= self.steel, diam= self.diam, spacing= self.spacing, concreteCover= self.concreteCover, crackControlRequirement= self.crackControlRequirement)
      
    def getRebarController(self):
        return RebarController(self.crackControlRequirement)
      
    def getBasicAnchorageLength(self,concrete):
        ''' Return the basic anchorage length of the bars.'''
        rebarController= self.getRebarController()
        return max(rebarController.getBasicAnchorageLength(concrete,self.getDiam(),self.steel),self.minDiams*self.diam)
    
    def getCrackControlRequirement(self):
        ''' Return the crack control requirement as in clause 4.4.2.2.3
            of SIA 262:2014.'''
        return self.crackControlRequirement
    
    def getMinReinfAreaInBending(self, concrete, thickness):
        '''Return the minimun amount of bonded reinforcement to control cracking
           for reinforced concrete sections under flexion.

        :param concrete: concrete material.
        :param thickness: thickness of the bended member.
        '''
        return self.getRebarController().getMinReinfAreaInBending(concrete,self.getEffectiveCover(),self.spacing,thickness)

    def getMinReinfAreaInTension(self,concrete,thickness):
        '''Return the minimun amount of bonded reinforcement to control cracking
           for reinforced concrete sections under tension.

        :param concrete: concrete material.
        :param thickness: thickness of the tensioned member.
        '''
        return self.getRebarController().getMinReinfAreaInTension(concrete,self.spacing,thickness)

    def getVR(self,concrete,Nd,Md,b,thickness):
        '''Return the shear resistance of the (b x thickness) rectangular section.

        :param concrete: concrete material.
        :param Nd: design axial force.
        :param Md: design bending moment.
        :param b: width of the rectangular section.
        :param thickness: height of the rectangular section.
        '''
        return VuNoShearRebars(concrete,self.steel,Nd,Md,self.getAs(),b,self.d(thickness))

class SIAFamNBars(SIARebarFamily):
    n= 2 #Number of bars.
    def __init__(self,steel,n,diam,spacing,concreteCover):
        super(SIAFamNBars, self).__init__(self,steel,diam,spacing,concreteCover)
        self.n= int(n)
    def __repr__(self):
        return str(self.n) + " x " + self.steel.name + ", diam: " + str(int(self.diam*1e3)) + " mm, e= " + str(int(self.spacing*1e3))
    def writeDef(self,outputFile,concrete):
        outputFile.write("  n= "+str(self.n)+" diam: "+ fmt.Diam.format(self.getDiam()*1000) + " mm, spacing: "+ fmt.Diam.format(self.spacing*1e3)+ " mm")
        reinfDevelopment= self.getBasicAnchorageLength(concrete)
        outputFile.write("  reinf. development L="+ fmt.Length.format(reinfDevelopment) + " m ("+ fmt.Diam.format(reinfDevelopment/self.getDiam())+ " diameters).\\\\\n")

class SIADoubleRebarFamily(rf.DoubleRebarFamily):
    ''' Two reinforcement bars families.'''
    
    def getCopy(self):
        return SIADoubleRebarFamily(self.f1, self.f2)
    
    def getRebarController(self):
        return RebarController(self.getCrackControlRequirement())
    
    def getCrackControlRequirement(self):
        ''' Return the crack control requirement as in clause 4.4.2.2.3
            of SIA 262:2014.'''
        retval= self.f1.crackControlRequirement
        if(retval!=self.f2.crackControlRequirement):
            className= type(self).__name__
            methodName= sys._getframe(0).f_code.co_name
            lmsg.error(className+'.'+methodName+'; different specifications for crack control.')
        return retval
    
    def getVR(self,concrete,Nd,Md,b,thickness):
        '''Return the shear resistance of the (b x thickness) rectangular section.
        :param concrete: concrete material.
        :param Nd: design axial force.
        :param Md: design bending moment.
        :param b: width of the rectangular section.
        :param thickness: height of the rectangular section.
        '''
        assert self.f1.steel==self.f2.steel
        return VuNoShearRebars(concrete,self.f1.steel,Nd,Md,self.getAs(),b,self.d(thickness))  


def copy_rebar_family(reinfToCopy, newCrackingRequirement):
    ''' Return a copy of the reinforcement argument changing the
        cracking requirement.
    '''
    retval= reinfToCopy.getCopy()
    retval.crackControlRequirement= newCrackingRequirement
    return retval
    
