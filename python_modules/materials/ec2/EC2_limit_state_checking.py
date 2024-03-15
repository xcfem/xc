# -*- coding: utf-8 -*-
''' Classes and functions for limit state checking according to Eurocode 2. '''

from __future__ import print_function

__author__= "Ana Ortega (AO_O) "
__copyright__= "Copyright 2016, AO_O"
__license__= "GPL"
__version__= "3.0"
__email__= "ana.ortega@ciccp.es "

import sys
import math
import scipy.interpolate
from materials import limit_state_checking_base as lscb
from materials.ec2 import EC2_materials
from materials.sections.fiber_section import fiber_sets
from postprocess import control_vars as cv
from postprocess import limit_state_data as lsd
from postprocess.config import file_names as fn
from misc_utils import log_messages as lmsg
from materials import concrete_base
from materials.sections import rebar_family as rf

class RebarController(lscb.EURebarController):
    '''Control of some parameters as development length 
       minimum reinforcement and so on.

    :ivar eta1: coefficient related to the quality of the bond condition 
                 and the position of the bar during concreting.
                 eta1= 1,0 when 'good' conditions are obtained and
                 eta1= 0,7 for all other cases.
    '''

    def __init__(self, concreteCover= 35e-3, spacing= 150e-3, eta1= 0.7, compression= True, alpha_1= 1.0, alpha_3= 1.0, alpha_4= 1.0, alpha_5= 1.0):
        '''Constructor.

        :param concreteCover: the distance from center of a bar or wire to 
                             nearest concrete surface.
        :param spacing: center-to-center spacing of bars or wires being 
                       developed, in.
        :param eta1: coefficient related to the quality of the bond condition 
                     and the position of the bar during concreting.
                     eta1= 1,0 when 'good' conditions are obtained and
                     eta1= 0,7 for all other cases.
        :param compression: true if reinforcement is compressed.
        :param alpha_1: effect of the form of the bars assuming adequate cover.
        :param alpha_3: effect of confinement by transverse reinforcement.
        :param alpha_4: influence of one or more welded transverse bars along 
                        the design anchorage length.
        :param alpha_5: effect of the pressure transverse to the plane of 
                        splitting along the design anchorage length.
        '''
        super(RebarController,self).__init__(concreteCover= concreteCover, spacing= spacing, compression= compression, alpha_1= alpha_1, alpha_3= alpha_3, alpha_4= alpha_4, alpha_5= alpha_5)
        self.eta1= eta1

    def getBasicAnchorageLength(self, concrete, rebarDiameter, steel, steelEfficiency= 1.0):
        '''Returns basic required anchorage length in tension according to 
           clause 8.4.3 of EC2:2004 (expression 8.3).

        :param concrete: concrete material.
        :param rebarDiameter: nominal diameter of the bar.
        :param steel: reinforcement steel.
        :param steelEfficiency: working stress of the reinforcement that it is 
                                intended to anchor, on the most unfavourable 
                                load hypothesis, in the section from which 
                                the anchorage length will be determined divided
                                by the steel design yield 
                                strength: (sigma_sd/fyd).
        '''
        return steel.getBasicAnchorageLength(concrete= concrete, rebarDiameter= rebarDiameter, eta1= self.eta1, steelEfficiency= steelEfficiency)

    def getConcreteMinimumCoverEffect(self, rebarDiameter, barShape= 'bent', lateralConcreteCover= None):
        ''' Return the value of the alpha_2 factor that introduces the effect
            of concrete minimum cover according to figure 8.3 and table 8.2
            of EC2:2004.

        :param rebarDiameter: nominal diameter of the bar.
        :param barShape: 'straight' or 'bent' or 'looped'.
        :param lateralConcreteCover: lateral concrete cover (c1 in figure 8.3
                                     of EC2:2004). If None make it equal to
                                     the regular concrete cover.
        '''
        return super(RebarController, self).getConcreteMinimumCoverEffect(rebarDiameter= rebarDiameter, barShape= barShape, lateralConcreteCover= lateralConcreteCover)
    
    def getDesignAnchorageLength(self, concrete, rebarDiameter, steel, steelEfficiency= 1.0, barShape= 'bent', lateralConcreteCover= None):
        '''Returns design  anchorage length according to clause 8.4.4
           of EC2:2004 (expression 8.4).

        :param concrete: concrete material.
        :param rebarDiameter: nominal diameter of the bar.
        :param steel: reinforcement steel.
        :param steelEfficiency: working stress of the reinforcement that it is 
                                intended to anchor, on the most unfavourable 
                                load hypothesis, in the section from which 
                                the anchorage length will be determined divided
                                by the steel design yield 
                                strength: (sigma_sd/fyd).
        :param barShape: 'straight' or 'bent' or 'looped'.
        :param lateralConcreteCover: lateral concrete cover (c1 in figure 8.3
                                     of EC2:2004). If None make it equal to
                                     the regular concrete cover.
        '''
        alpha_2= self.getConcreteMinimumCoverEffect(rebarDiameter, barShape= barShape, lateralConcreteCover= lateralConcreteCover)
        return steel.getDesignAnchorageLength(concrete= concrete, rebarDiameter= rebarDiameter, eta1= self.eta1, steelEfficiency= steelEfficiency, compression= self.compression, alpha_1= self.alpha_1, alpha_2= alpha_2, alpha_3= self.alpha_3, alpha_4= self.alpha_4, alpha_5= self.alpha_5)

    def getLapLength(self ,concrete, rebarDiameter, steel, steelEfficiency= 1.0, ratioOfOverlapedTensionBars= 1.0, lateralConcreteCover= None):
        '''Returns the value of the design lap length according to clause
           8.7.3 of EC2:2004 (expression 8.10).

        :param concrete: concrete material.
        :param rebarDiameter: nominal diameter of bar, wire, or prestressing strand.
        :param steel: reinforcement steel.
        :param distBetweenNearestSplices: distance between the nearest splices
                                          according to figure 69.5.2.2.a.
        :param steelEfficiency: working stress of the reinforcement that it is 
                                intended to anchor, on the most unfavourable 
                                load hypothesis, in the section from which 
                                the anchorage length will be determined divided
                                by the steel design yield 
                                strength: (sigma_sd/fyd).
        :param ratioOfOverlapedTensionBars: ratio of overlapped tension bars 
                                            in relation to the total steel
                                            section.
        :param lateralConcreteCover: lateral concrete cover (c1 in figure 8.3
                                     of EC2:2004). If None make it equal to
                                     the regular concrete cover.
        '''
        alpha_2= self.getConcreteMinimumCoverEffect(rebarDiameter= rebarDiameter, barShape= 'straight', lateralConcreteCover= lateralConcreteCover)
        return steel.getLapLength(concrete= concrete, rebarDiameter= rebarDiameter, eta1= self.eta1, steelEfficiency= steelEfficiency, ratioOfOverlapedTensionBars= ratioOfOverlapedTensionBars, alpha_1= self.alpha_1, alpha_2= alpha_2, alpha_3= self.alpha_3, alpha_5= self.alpha_5)

# EC2:2004 part 1-1. Clause 7.3.3. Control of cracking without direct calculation.

table7_2NSteelStresses= [160e6, 200e6, 240e6, 280e6, 320e6, 360e6, 400e6, 450e6]
table7_2NMaxBarSize04mm= [40e-3, 32e-3, 20e-3, 16e-3, 12e-3, 10e-3, 8e-3, 6e-3]
table7_2NColumn04mm= scipy.interpolate.interp1d(table7_2NSteelStresses, table7_2NMaxBarSize04mm, fill_value='extrapolate')
table7_2NMaxBarSize03mm= [32e-3, 25e-3, 16e-3, 12e-3, 10e-3, 8e-3, 6e-3, 5e-3]
table7_2NColumn03mm= scipy.interpolate.interp1d(table7_2NSteelStresses, table7_2NMaxBarSize03mm, fill_value='extrapolate')
table7_2NMaxBarSize02mm= [25e-3, 16e-3, 12e-3, 8e-3, 6e-3, 5e-3, 4e-3, 1e-6]
table7_2NColumn02mm= scipy.interpolate.interp1d(table7_2NSteelStresses, table7_2NMaxBarSize02mm, fill_value='extrapolate')

def getMaximumBarDiameterForCrackControl(steelStress, wk= 0.3e-3):
    ''' Return the maximum diameter of the bar according to table 7.2N of
        of EC2:2004 part 1-1. (clause 7.3.3).

    :param steelStress: maximum stress permitted in the reinforcement
                        immediately after formation of the crack.
    :param wk: crack width (m)
    '''
    if (steelStress<table7_2NSteelStresses[0]) or (steelStress>table7_2NSteelStresses[-1]):
        methodName= sys._getframe(0).f_code.co_name
        lmsg.warning(methodName+'; maximum stress permitted in the reinforcement immediately after formation of the crack. steelStress= '+str(steelStress/1e6)+' MPa. Out of range ('+str(table7_2NSteelStresses[0]/1e6)+','+str(table7_2NSteelStresses[-1]/1e6)+') MPa. Value computed by EXTRAPOLATION.')
    if (wk<=0.4e-3) and (wk>0.3e-3): # 0.4 and 0.3 columns.
        diam04= float(table7_2NColumn04mm(steelStress))
        diam03= float(table7_2NColumn03mm(steelStress))
        retval= (diam04-diam03)*1e4*(wk-0.3e-3)+diam03
    elif (wk<=0.3e-3) and (wk>=0.2e-3): # 0.3 and 0.2 columns.
        diam03= float(table7_2NColumn03mm(steelStress))
        diam02= float(table7_2NColumn02mm(steelStress))
        retval= (diam03-diam02)*1e4*(wk-0.2e-3)+diam02
    else:
        retval= None
        methodName= sys._getframe(0).f_code.co_name
        lmsg.error(methodName+'; value of crack width: wk= '+str(wk*1e3)+' mm. Out of range (0.2e-3,0.4e-3) meters.')
    return retval

table7_3NSteelStresses= [160e6, 200e6, 240e6, 280e6, 320e6, 360e6]
table7_3NMaxBarSpacing04mm= [0.30, 0.30, 0.25, 0.2, 0.15, 0.1]
table7_3NColumn04mm= scipy.interpolate.interp1d(table7_3NSteelStresses, table7_3NMaxBarSpacing04mm, fill_value='extrapolate')
table7_3NMaxBarSpacing03mm= [0.30, 0.25, 0.20, 0.15, 0.10, 0.05]
table7_3NColumn03mm= scipy.interpolate.interp1d(table7_3NSteelStresses, table7_3NMaxBarSpacing03mm, fill_value='extrapolate')
table7_3NMaxBarSpacing02mm= [0.20, 0.15, 0.10, 0.05, 1e-3, 1e-4]
table7_3NColumn02mm= scipy.interpolate.interp1d(table7_3NSteelStresses, table7_3NMaxBarSpacing02mm, fill_value='extrapolate')

def getMaximumBarSpacingForCrackControl(steelStress, wk= 0.3e-3):
    ''' Return the maximum bar spacing according to table 7.3N of
        of EC2:2004 part 1-1. (clause 7.3.3).

    :param steelStress: maximum stress permitted in the reinforcement
                        immediately after formation of the crack.
    :param wk: crack width (m)
    '''
    if (steelStress<table7_3NSteelStresses[0]) or (steelStress>table7_3NSteelStresses[-1]):
        methodName= sys._getframe(0).f_code.co_name
        lmsg.warning(methodName+'; maximum stress permitted in the reinforcement immediately after formation of the crack. steelStress= '+str(steelStress/1e6)+' MPa. Out of range ('+str(table7_3NSteelStresses[0]/1e6)+','+str(table7_3NSteelStresses[-1]/1e6)+') MPa. Value computed by EXTRAPOLATION.')
    if (wk<=0.4e-3) and (wk>0.3e-3): # 0.4 and 0.3 columns.
        spacing04= float(table7_3NColumn04mm(steelStress))
        spacing03= float(table7_3NColumn03mm(steelStress))
        retval= (spacing04-spacing03)*1e4*(wk-0.3e-3)+spacing03
    elif (wk<=0.3e-3) and (wk>=0.2e-3): # 0.3 and 0.2 columns.
        spacing03= float(table7_3NColumn03mm(steelStress))
        spacing02= float(table7_3NColumn02mm(steelStress))
        retval= (spacing03-spacing02)*1e4*(wk-0.2e-3)+spacing02
    else:
        retval= None
        methodName= sys._getframe(0).f_code.co_name
        lmsg.error(methodName+'; value of crack width: wk= '+str(wk*1e3)+' mm. Out of range (0.2e-3,0.4e-3) meters.')
    return retval
    
#These functions  should disappear               
def h_c_eff(depth_tot,depht_eff,depth_neutral_axis):
    '''
    Returns the depth of the effective area of concrete in tension surrounding
    the reinforcement or prestressing tendons, according to EC2

    :param depth_tot: overall depth of the cross-section [h]
    :param depht_eff: effective depth of the cross-section [d]
    :param depth_neutral_axis: depht of the neutral axis[x]
    '''
    h,d,x=depth_tot,depht_eff,depth_neutral_axis
    retval=min(2.5*(h-d),(h-x)/3.,h/2.)
    return retval

def ro_eff(A_s, width, h_c_eff):
    '''
    Returns the effective reinforcement ratio [A_s/A_ceff] depth of the 
    effective area of concrete in tension surrounding
    the reinforcement or prestressing tendons, according to EC2

    :param A_s: area of reinforcment steel
    :param width: width of the RC cross-section
    :param ,h_c_eff: depth of the effective area of concrete in 
    tension surrounding the reinforcement or prestressing tendons
    '''
    retval=A_s/width/h_c_eff
    return retval

def s_r_max(k1,k2,k3,k4,cover,fiReinf,ro_eff):
    ''' Returns the maximum crack spacing, according to expresion 7.11 of
    clause 7.34 of EC2:2004
 
    :param k1: coefficient which takes account of the bond properties
               of the bonded reinforcement:

               - =0.8 for high bond bars
               - =1.6 for bars with an effectively plain surface (e.g. 
                      prestressing tendons)

    :param k2: coefficient that takes account of the distribution of strain:

               - =0.5 for bending
               - =1.0 for pure tension
               - for cases of eccentric tension or for local areas, intermediate values of k2 should be used (see clause 7.3.4 EC2)
    :param k3: recommended k3=3.4
    :param k4: recommended k4=0.425 
    :param cover: cover of the longitudinal reinforcement
    :param fiReinf: bar diameter. Where a mixture of bar diameters is used in a
                    section, an equivalent diameter is used (see clause 7.3.4 EC2)
    :param ro_eff: effective reinforcement ratio
    '''
    retval= k3*cover + k1*k2*k4*fiReinf/ro_eff
    return retval

class CrackController(lscb.LimitStateControllerBase):
    '''Object that verifies the cracking serviceability limit state according 
    to EC2.

    :ivar k1: coefficient that takes account of the bound properties of the 
          bonded reinforcement. k1=0.8 for high bond bars, k1=1.0 for bars with
          effectively plain surface (e.g. prestressing tendons). Defaults to 0.8
    :ivar k3: defaults to the value recommended by EC2 and in EHE k3=3.4
    :ivar k4: defaults to the value recommended by EC2 and EHE k4=0.425
    :ivar shortTermLoading: if true, consider short therm loading 
                            (k_t= 0.6), otherwise consider long term 
                            loading (k_t= 0.4).
    '''
    
    ControlVars= cv.RCCrackControlVars
    
    def __init__(self, limitStateLabel, wk_lim= 0.3e-3, k1= 0.8, shortTermLoading= False, solutionProcedureType= lscb.defaultStaticNonLinearSolutionProcedure):
        ''' Constructor.
        
        :param limitStateLabel: label that identifies the limit state.
        :param wk_lim: maximum allowable crack width. 
        :param k1: coefficient which takes account of the bond properties
                   of the bonded reinforcement:

               - = 0.8 for high bond bars
               - = 1.6 for bars with an effectively plain surface (e.g. 
                      prestressing tendons)
        :param shortTermLoading: if true, consider short therm loading 
                                 (k_t= 0.6), otherwise consider long term 
                                 loading (k_t= 0.4).
        :param solutionProcedureType: type of the solution procedure to use
                                      when computing load combination results.
        '''
        super(CrackController,self).__init__(limitStateLabel= limitStateLabel, fakeSection= False, solutionProcedureType= solutionProcedureType)
        self.wk_lim= wk_lim
        self.k1= k1
        self.k3= 3.4
        self.k4= 0.425
        self.shortTermLoading= shortTermLoading

    def EC2_k2(self, eps1, eps2):
        '''Return the coefficient k2 involved in the calculation of the mean 
        crack distance according to EC2. This coefficient represents the 
        effect of the tension diagram in the section.

        :param eps1: maximum deformation calculated in the section at the 
                     limits of the tension zone
        :param eps2: minimum deformation calculated in the section at the
                     limits of the tension zone
        '''
        k2= (eps1+eps2)/(2.0*eps1)
        return k2

    def EC2_hceff(self, h, d, x):
        '''Return the maximum height to be considered in the calculation of the 
        concrete effective area in tension. See paragraph (3) of clause 7.3.2 
        of EC2.
     
        :param width: section width
        :param h: lever arm
        :param d: effective depth of the cross-section 
        :param x: depth of the neutral fiber.

        '''
        hceff= min(2.5*(h-d),abs(h+x)/3.,h/2.)
        return hceff

    def s_r_max(self, k2, cover, reinfPhi, spacing , ro_eff, h= None, x= None):
        ''' Returns the maximum crack spacing, according to expressions 7.11 
            and 7.14 of clause 7.3.4 of EC2:2004 part 1.

        :param k2: coefficient that takes account of the distribution of strain:

                   - =0.5 for bending
                   - =1.0 for pure tension
                   - for cases of eccentric tension or for local areas, 
                     intermediate values of k2 should be used 
                     (see clause 7.3.4 EC2)
        :param cover: cover of the longitudinal reinforcement
        :param reinfPhi: bar diameter. Where a mixture of bar diameters is used
                        in a section, an equivalent diameter is used (see 
                        clause 7.3.4 EC2).
        :param spacing: spacing of the bonded reinforcement.
        :param ro_eff: effective reinforcement ratio.
        :param h: overall depth of the section.
        :param x: depth of the neutral axis.
        '''
        influenceWidth= 5.0*(cover+reinfPhi/2.0)
        if(reinfPhi==0.0) or (spacing>influenceWidth):
            retval= 1.3*(h-x) # Expression 7.14
        else:
            retval= self.k3*cover + self.k1*k2*self.k4*reinfPhi/ro_eff
        return retval

    def meanStrainDifference(self, sigma_s, steel, concrete, ro_eff):
        ''' Returns the mean strain difference according to expression 7.9 of
            clause 7.3.4 of EC2:2004.

        :param sigma_s: stress in the tension reinforcement assuming a cracked
                        section. For pretensioned members, as may be replaced 
                         by Delta(sigma_p) the stress variation in prestressing
                         tendons from the state of zero strain of the concrete
                         at the same level.
        :param steel: steel type of the reinforcement.
        :param As: steel reinforcement area 
        :param concrete: concrete of the section.
        :param ro_eff: effective reinforcement ratio computed according to
                       expression 7.10 of EC2:2004 part 1.
        '''
        kt= 0.4
        if(self.shortTermLoading):
            kt= 0.6
        fct_eff= concrete.fctm() # mean value of the tensile strength of the
                                 # concrete effective at the time when the
                                 # cracks may first be expected to occur.
        Es= steel.Es # steel elastic modulus.
        alpha_e= Es/concrete.Ecm() # ratio between elastic moduli.
        retval= (sigma_s - kt*fct_eff/ro_eff*(1+alpha_e*ro_eff))/Es
        retval= max(retval, 0.6*sigma_s/Es)
        return retval

    def computeWk(self, sigma_s, steel, concrete, ro_eff, k2, cover, reinfPhi, spacing, h= None, x= None):
        '''Computes the characteristic value of the crack width according to the
           expression 7.8 of EC2:2004 part 1.

        :param sigma_s: stress in the tension reinforcement assuming a cracked
                        section. For pretensioned members, as may be replaced 
                        by Delta(sigma_p) the stress variation in prestressing
                        tendons from the state of zero strain of the concrete
                        at the same level.
        :param steel: steel type of the reinforcement.
        :param As: steel reinforcement area 
        :param concrete: concrete of the section.
        :param ro_eff: effective reinforcement ratio computed according to
                       expression 7.10 of EC2:2004 part 1.
        :param k2: coefficient that takes account of the distribution of strain:

                   - =0.5 for bending
                   - =1.0 for pure tension
                   - for cases of eccentric tension or for local areas, 
                     intermediate values of k2 should be used 
                     (see clause 7.3.4 EC2)
        :param cover: cover of the longitudinal reinforcement
        :param reinfPhi: bar diameter. Where a mixture of bar diameters is used
                        in a section, an equivalent diameter is used (see 
                        clause 7.3.4 EC2).
        :param spacing: spacing of the bonded reinforcement.
        :param h: overall depth of the section.
        :param x: depth of the neutral axis.
        '''
        s_r_max= self.s_r_max(k2= k2, cover= cover, reinfPhi= reinfPhi, spacing= spacing, ro_eff= ro_eff)
        meanStrainDifference= self.meanStrainDifference(sigma_s= sigma_s, steel= steel, concrete= concrete, ro_eff= ro_eff, shortTermLoading= self.shortTermLoading)
        return meanStrainDifference*s_r_max
    
    def check(self, elements, loadCombinationName):
        ''' For each element in the 'elememts' container passed as first 
        parameter and  the resulting internal forces for the load combination
       'loadCombinationName'  passed as second parameter, this method 
        calculates all the variables involved in the crack-SLS checking and 
        obtains the crack width.
        In the case that the calculated crack width is greater than the 
        biggest obtained for the element in previous load combinations, this 
        value is saved in the element results record. 

        Elements processed are those belonging to the phantom model, that is to 
        say, of type xc.ZeroLengthSection. As we have defined the variable 
        fakeSection as False, a reinfoced concrete fiber section is generated
        for each of these elements.

        :param elements: elements to check.
        :param loadCombinationName: name of the load combination.
      '''
        if(self.verbose):
            className= type(self).__name__
            methodName= sys._getframe(0).f_code.co_name
            lmsg.log(className+'.'+methodName+"; postprocessing combination: "+loadCombinationName)
        for e in elements:
            Aceff=0  #initial  value
            R= e.getResistingForce()
            sct= e.getSection()
            sctCrkProp= lscb.FibSectLSProperties(sct)
            sctCrkProp.setupStrghCrackDist()
            sigma_s= sctCrkProp.getMaxReinforcementTensileStress()
            if(sctCrkProp.eps1<=0): # No tensile strains.
                s_rmax= 0.0
                wk= 0.0
            else:
                concrete= sctCrkProp.sctProp.getConcreteType()
                flexuralTensileStrain= concrete.getFlexuralTensileStrain(h= sctCrkProp.h)
                if(sctCrkProp.eps1<flexuralTensileStrain): # Very small tensile strain => no cracking
                    s_rmax= 0.0
                    wk= 0.0
                else:
                    hceff= self.EC2_hceff(sctCrkProp.h,sctCrkProp.d,sctCrkProp.x)
                    Aceff= sct.getNetEffectiveConcreteArea(hceff,'tensSetFb',15.0)
                    if(Aceff<=0):
                        s_rmax= 0.0
                        wk= 0.0
                    else:
                        rfSteel= sctCrkProp.sctProp.getReinfSteelType()
                        k2= self.EC2_k2(sctCrkProp.eps1, sctCrkProp.eps2)
                        ro_s_eff= sctCrkProp.As/Aceff #effective ratio of reinforcement
                        s_rmax= self.s_r_max(k2= k2, cover= sctCrkProp.cover, reinfPhi= sctCrkProp.fiEqu, spacing= sctCrkProp.spacing, ro_eff= ro_s_eff)
                        meanStrainDifference= self.meanStrainDifference(sigma_s= sigma_s, steel= rfSteel, concrete= concrete, ro_eff= ro_s_eff)
                        wk= meanStrainDifference*s_rmax
            if(wk>=e.getProp(self.limitStateLabel).wk):
                CF= wk/self.wk_lim # Capacity factor.
                sigma_c= sctCrkProp.sigma_c
                e.setProp(self.limitStateLabel, self.ControlVars(idSection=e.getProp('idSection'), combName=loadCombinationName, N=-R[0], My=-R[4], Mz=-R[5], s_rmax= s_rmax, sigma_s= sigma_s, sigma_c= sigma_c, wk= wk, CF= CF))

class CrackStraightController(CrackController):
    ''' Object that verifies the cracking serviceability limit state according 
    to EC2 when considering a concrete stress-strain diagram that takes 
    account of the effects of tension  stiffening.

    :ivar k1: coefficient that takes account of the bound properties of the 
          bonded reinforcement. k1=0.8 for high bond bars, k1=1.0 for bars with
          effectively plain surface (e.g. prestressing tendons). Defaults to 0.8
    :ivar k3: defaults to the value recommended by EC2 and in EHE k3=3.4
    :ivar k4: defaults to the value recommended by EC2 and EHE k4=0.425
    '''
    ControlVars= cv.RCCrackStraightControlVars
    def __init__(self, limitStateLabel, solutionProcedureType= lscb.defaultStaticLinearSolutionProcedure):
        ''' Constructor.
        
        :param limitStateLabel: label that identifies the limit state.
        :param solutionProcedureType: type of the solution procedure to use
                                      when computing load combination results.
        '''
        super(CrackStraightController,self).__init__(limitStateLabel= limitStateLabel, solutionProcedureType= solutionProcedureType)

    def check(self, elements, loadCombinationName):
        ''' For each element in the 'elememts' container passed as first 
        parameter and  the resulting internal forces for the load combination
       'loadCombinationName'  passed as second parameter, this method 
        calculates all the variables involved in the crack-SLS checking and 
        obtains the crack width.
        In the case that the calculated crack width is greater than the 
        biggest obtained for the element in previous load combinations, this 
        value is saved in the element results record. 

        Elements processed are those belonging to the phantom model, that is to 
        say, of type xc.ZeroLengthSection. As we have defined the variable 
        fakeSection as False, a reinfoced concrete fiber section is generated
        for each of these elements.

        :param elements: elements to check.
        :param loadCombinationName: name of the load combination.
        '''
        if(self.verbose):
          lmsg.log('Postprocessing combination: '+loadCombinationName)
        for e in elements:
            Aceff=0  #initial  value
            R= e.getResistingForce()
            sct= e.getSection()
            sctCrkProp= lscb.FibSectLSProperties(sct)
            sctCrkProp.setupStrghCrackDist()
            hceff= self.EC2_hceff(sctCrkProp.h,sctCrkProp.d,sctCrkProp.x)
            # Acgross=sct.getGrossEffectiveConcreteArea(hceff)
            Aceff= sct.getNetEffectiveConcreteArea(hceff,'tensSetFb',15.0)
            concrete= EC2_materials.concrOfName[sctCrkProp.concrName]
            rfSteel= EC2_materials.steelOfName[sctCrkProp.rsteelName]
            k2= self.EC2_k2(sctCrkProp.eps1, sctCrkProp.eps2)
            if(Aceff<=0):
                s_rmax=0
            else:
                ro_s_eff= sctCrkProp.As/Aceff #effective ratio of reinforcement
                s_rmax= self.k3*sctCrkProp.cover+self.k1*k2*self.k4*sctCrkProp.fiEqu/ro_s_eff
                #Parameters for tension stiffening of concrete
                paramTS= concrete_base.paramTensStiffness(concrMat=concrete,reinfMat=rfSteel,reinfRatio=ro_s_eff,diagType='K')
                concrete.tensionStiffparam=paramTS #parameters for tension
                #stiffening are assigned to concrete
                ftdiag=concrete.tensionStiffparam.pointOnsetCracking()['ft']                    #stress at the adopted point for concrete onset cracking
                Etsdiag=abs(concrete.tensionStiffparam.regresLine()['slope'])
                fiber_sets.redefTensStiffConcr(setOfTenStffConcrFibSect=sctCrkProp.setsRC.concrFibers,ft=ftdiag,Ets=Etsdiag)
            e.setProp('ResF',R)   #vector resisting force
            e.setProp('s_rmax',s_rmax)  #maximum crack distance
        self.preprocessor.getDomain.revertToStart()
        self.solutionProcedure.solveComb(loadCombinationName)
        for e in elements:
            sct=e.getSection()
            rfset=sct.getFiberSets()['reinfSetFb']
            eps_sm=rfset.getStrainMax()
            srmax= e.getProp('s_rmax')
            # eps_cm=concrete.fctm()/2.0/concrete.E0()
            # wk=srmax*(eps_sm-eps_cm)
            wk= srmax*eps_sm
            if (wk>e.getProp(self.limitStateLabel).wk):
                R=e.getProp('ResF')
                e.setProp(self.limitStateLabel, self.ControlVars(idSection=e.getProp('idSection'),combName=loadCombinationName,N=-R[0],My=-R[4],Mz=-R[5],s_rmax=srmax,eps_sm=eps_sm,wk=wk))
                
###################
# Rebar families. #
###################

class EC2RebarFamily(rf.RebarFamily):
    ''' Family or reinforcement bars with checking according to EC2.

       :ivar pos: reinforcement position according to clause 66.5.1
                  of EC2.
    '''
    def __init__(self, steel, diam, spacing, concreteCover, pos= 'II', nationalAnnex= None):
        ''' Constructor.

        :param steel: reinforcing steel material.
        :param diam: diameter of the bars.
        :param spacing: spacing of the bars.
        :param concreteCover: concrete cover of the bars.
        :param pos: reinforcement position according to clause 66.5.1
                   of EC2.
        '''
        super(EC2RebarFamily,self).__init__(steel,diam,spacing,concreteCover)
        self.pos= pos
        self.nationalAnnex= nationalAnnex

    def getCopy(self):
        return EC2RebarFamily(steel= self.steel, diam= self.diam, spacing= self.spacing, concreteCover= self.concreteCover, pos= self.pos)
    
    def getRebarController(self):
        return RebarController(concreteCover= self.concreteCover, spacing= self.spacing)
    
    def getBasicAnchorageLength(self,concrete):
      ''' Return the basic anchorage length of the bars.'''
      rebarController= self.getRebarController()
      return rebarController.getBasicAnchorageLength(concrete,self.getDiam(),self.steel)
    def getMinReinfAreaInBending(self, concrete, thickness, b= 1.0, steelStressLimit= 450e6, memberType= None, sigmaC= 0.0, effectiveCover= 45e-3):
        '''Return the minimun amount of bonded reinforcement to control cracking
           for reinforced concrete sections under flexion per unit length 
           according to clauses 7.3.2, 9.2.1.1 (beams), 9.3.1.1 (slabs),
           9.5.2 (columns) and 9.6.2 (walls).

        :param thickness: gross thickness of concrete section (doesn't include 
                          the area of the voids).
        :param b: width of concrete section.
        :param memberType: member type; slab, wall, beam or column.
        :param concrete: concrete material.
        :param steelStressLimit: maximum stress permitted in the reinforcement 
                                 immediately after formation of the crack. This
                                 may be taken as the yield strength of the 
                                 reinforcement, fyk. A lower value may, 
                                 however, be needed to satisfy the crack width 
                                 limits according to the maximum bar size or
                                 spacing.
        :param sigmaC: is the mean stress of the concrete acting on the part 
                       of the section under consideration: sigmaC= NEd/(bh).
        :param effectiveCover: the distance between the exposed concrete surface
                               to the centroid of the main reinforcement.
        '''
        fct_eff= concrete.fctm() # mean value of the tensile strength of the
        # concrete effective at the time when the cracks may first be expected
        # to occur: fct,eff = fctm or lower, (fctm(t)), if cracking is expected
        # earlier than 28 days
        
        # Clause 7.3.2 minimum reinforcement areas:
        steelStressLimit= min(self.steel.fyk, steelStressLimit)
        # area of concrete within tensile zone computed using (Jones Method)
        # and assuming there is only one reinforcement layer (zero vertical
        # spacing between reinforcement bars).
        hc_eff= 2.5*effectiveCover
        Ac_t= hc_eff*b
        retval= getAsMinCrackControl(concrete= concrete, reinfSteel= self.steel, h= thickness, Act= Ac_t, sigmaC= sigmaC, sigma_s= steelStressLimit)
        d= thickness-effectiveCover # effective depth.
        if(memberType=='beam' or memberType=='slab'): # Clauses 9.2.1.1 and 9.3.1.1 (1)
            
            Asmin= getAsMinBeams(concrete= concrete, reinfSteel= self.steel, h= thickness, bt= b, d= d, z= 0.9*d, nationalAnnex= self.nationalAnnex) 
            retval= max(Asmin, retval)
        elif(memberType=='column'):
            if(b>4*thickness):
                className= type(self).__name__
                methodName= sys._getframe(0).f_code.co_name
                errMsg= className+'.'+methodName+"; column too wide; b= " + str(b) + ", h= "+str(thickness)+'\n'
                lmsg.error(errMsg)
            Ac= thickness*b
            NEd= sigmaC*Ac
            Asmin= getAsMinColumns(concrete= concrete, reinfSteel= self.steel, NEd= NEd, Ac= Ac, nationalAnnex= self.nationalAnnex)
            retval= max(Asmin, retval)
        elif(memberType=='wall' or memberType=='short_wall'): # Clause 9.6.2
            # We assume that only the vertical reinforcement is subjected
            # to bending.
            Asmin= getAsMinWalls(concrete= concrete, reinfSteel= self.steel, Ac= thickness*b, thickness= thickness, vertical= True, compressedSide= False, verticalReinforcementArea= None, nationalAnnex= self.nationalAnnex)
            retval= max(Asmin, retval)
        elif(memberType=='footing'):
            pass # Nothing to add here.        
        else:
            className= type(self).__name__
            methodName= sys._getframe(0).f_code.co_name
            errMsg= className+'.'+methodName+"; member type: " + str(memberType) + " not implemented.\n"
            lmsg.error(errMsg)
        return retval
    
    def getMinReinfAreaInTension(self, concrete, thickness, b = 1.0, stressLimit= 350e6, memberType= None):
        '''Return the minimun amount of bonded reinforcement to control cracking
           for reinforced concrete sections under tension according to
           clauses 9.6.3 and expressionn (7.1) in clause 7.3.2 of EC2:2004 part 1.

        :param concrete: concrete material.
        :param thickness: gross thickness of concrete section.
        :param b: width of concrete section.
        :param stressLimit: limit 
        :param memberType: member type; slab, wall, beam or column.
        '''
        if(memberType=='wall'):
            if(self.nationalAnnex=='Spain'): # 9.6.3 (1) Cuantías mínimas de la armadura horizontal en muros
                if(self.steel.fyk<500e6):
                    retval= 0.004*min(thickness,0.5)*b
                elif(self.steel.fyk==500e6):
                    retval= 0.0032*min(thickness,0.5)*b
                else:
                    className= type(self).__name__
                    methodName= sys._getframe(0).f_code.co_name
                    errMsg= className+'.'+methodName+"; steel fyk= : " + str(self.steel.fyk) + " not implemented.\n"
                    lmsg.error(errMsg)
            else:
                retval= 0.002*thickness*b # 0.001 for each side.
        elif(memberType=='short_wall'):
            if(self.nationalAnnex=='Spain'): # 9.6.3 (1) Cuantías mínimas de la armadura horizontal en muros
                retval= 0.002*min(thickness,0.5)*b
            else: # 9.6.3 Horizontal reinforcement.
                retval= 0.002*min(thickness,0.5)*b # 0.001 for each side.
        elif(memberType=='footing'):
            # EC2 establishes a minimum diameter but there is no easy
            # way to implement this here.
            retval= 0.0005*thickness*b
        else: # expressionn (7.1) in clause 7.3.2 of EC2:2004 part 1.
            kc= 1 # pure tension.
            k= 1.0 # coefficient which allows for the effect of non-uniform
                   # self-equilibrating stresses, which lead to a reduction of
                   # restraint forces.
            if(thickness>=0.8):
                k= 0.65
            elif(thickness>0.3):
                k= -0.7*thickness+1.21 # linear interpolation.
            Act= thickness*b  # All the section is in tension.
            fct_eff= concrete.fctm() # mean value of the tensile strength of the
            # concrete effective at the time when the cracks may first be expected
            # to occur: fct,eff = fctm or lower, (fctm(t)), if cracking is expected
            # earlier than 28 days
            retval= kc*k*fct_eff*Act/min(stressLimit, self.steel.fyk)
        return retval
     
    def getVR(self, concrete, Nd, Md, b, thickness):
        '''Return the shear resistance carried by the concrete on a
           (b x thickness) rectangular section according to expressions 6.2.a 
           and 6.2.b of EC2:2004.

        :param concrete: concrete material.
        :param Nd: design axial force.
        :param Md: design bending moment.
        :param b: width of the rectangular section.
        :param thickness: height of the rectangular section.
        '''
        return getShearResistanceCrackedNoShearReinf(concrete= concrete, NEd= Nd, Ac=thickness*b, Asl= self.getAs(), bw= b, d=0.8*thickness, nationalAnnex= self.nationalAnnex)
    

def define_rebar_families(steel, cover, diameters= [8e-3, 10e-3, 12e-3, 14e-3, 16e-3, 20e-3, 25e-3, 32e-3], spacings= [0.1, 0.15, 0.2]):
    ''' Creates a dictionary with predefined rebar families.

    :param steel: rebars steel material.
    :param cover: concrete cover for the families.
    :param diameters: list of diameters.
    :param spacings: list of spacings between bars. 
    '''
    retval= dict()
    for diameter in diameters:
        diameterText= str(int(diameter*1e3))
        for spacing in spacings:
            spacingText= str(int(spacing*1e2))
            familyName= 'A'+diameterText+'_'+spacingText
            retval[familyName]= EC2RebarFamily(steel= steel, diam= diameter, spacing= spacing, concreteCover= cover)
    return retval


# EC2:2004 6.2 Shear

# EC2:2004 6.2.2 Members not requiring design shear reinforcement
def getShearResistanceCrackedNoShearReinf(concrete, NEd, Ac, Asl, bw, d, nationalAnnex= None):
    ''' Return the design value of the shear resistance VRdc for cracked 
        sections subjected to bending moment, according to expressions 6.2.a 
        and 6.2.b of EC2:2004.

    :param concrete: concrete material.
    :param NEd: axial force in the cross-section due to loading or prestressing.
    :param Ac: area of concrete cross-section. 
    :param Asl: area of the tensile reinforcement, which extends beyond
                the section considered (see clause 6.2.2 of EC2:2004).
    :param bw: smallest width of the cross-section in the tensile area.
    :param d: effective depth of the cross-section.
    :param nationalAnnex: identifier of the national annex.
    '''
    k= min(1.0+math.sqrt(0.2/d),2) # d in meters.
    ro_l= min(Asl/bw/d,.02)
    fcdMPa= -concrete.fcd()/1e6 # design value of concrete compressive strength (MPa).
    sigma_cp= min(-NEd/Ac/1e6,.2*fcdMPa)
    gamma_c= concrete.gmmC
    CRdc= 0.18/gamma_c # Recommended value and Spanish national annex.
    k1= 0.15
    fckMPa= -concrete.fck/1e6 # concrete characteristic compressive strength (MPa).
    if(nationalAnnex=='Spain'):
        v_min= 0.075/gamma_c*math.pow(k,1.5)*math.sqrt(min(fckMPa,60))
    else:
        v_min= 0.035*math.pow(k,1.5)*math.sqrt(fckMPa)
    
    VRdc_62a= (CRdc*k*math.pow(100.0*ro_l*fckMPa,1/3.0)+k1*sigma_cp)*bw*d*1e6
    VRdc_62b= (v_min+k1*sigma_cp)*bw*d*1e6
    return max(VRdc_62a, VRdc_62b)
    
def getShearResistanceNonCrackedNoShearReinf(concrete, I, S, NEd, Ac, bw, alpha_l= 1.0):
    ''' Return the design value of the shear resistance VRdc for non-cracked 
        sections subjected to bending moment, according to expression 6.4 of
        EC2:2004.

    :param concrete: concrete material.
    :param I: second moment of area.
    :param S: first moment of area above and about the centroidal axis.
    :param NEd: axial force in the cross-section due to loading or prestressing.
    :param Ac: area of concrete cross-section. 
    :param bw: smallest width of the cross-section in the tensile area.
    :param alpha_l: see expression 6.4 in EC2:2004.
    '''
    sigma_cp= -NEd/Ac/1e6 # concrete compressive stress at the centroidal
                          # axis due to axial loading and/or prestressing
    fctdMPa= concrete.fctd()/1e6 #design tensile strength (MPa)
    return (I/S*bw)*math.sqrt(fctdMPa**2+alpha_l*sigma_cp*fctdMPa)*1e6

# EC2:2004 6.2.3 Members requiring design shear reinforcement.
def getWebStrutAngleForSimultaneousCollapse(concrete, bw, s, Asw, shearReinfSteel, shearReinfAngle= math.pi/2.0, nationalAnnex= None):
    ''' Return the web strut angle that makes web concrete collapse at the same
        time that the shear reinforcement (V_{Rd,s}= V_{Rd,max})

    :param concrete: concrete material.
    :param bw: smallest width of the cross-section in the tensile area.
    :param s: spacing of the stirrups.
    :param Asw: cross-sectional area of the shear reinforcement.
    :param shearReinfSteel: reinforcing steel material.
    :param shearReinfAngle: (alpha) angle between shear reinforcement and the beam axis perpendicular to the shear force.
    :param nationalAnnex: identifier of the national annex.
    '''
    # nu1: strength reduction factor for concrete cracked in shear
    nu= concrete.getShearStrengthReductionFactor(nationalAnnex)
    fcd= -concrete.fcd() # design value of concrete compressive strength (MPa).
    fywd= shearReinfSteel.fyd() # design yield strength of the shear reinforcement
    ratio= (bw*s*nu*fcd)/(Asw*fywd*math.sin(shearReinfAngle))
    if(ratio<1):
        methodName= sys._getframe(0).f_code.co_name
        lmsg.error(methodName+'; Warning, cross-sectional area of the shear reinforcement too big: '+str(Asw*1e6)+' mm2. Returning math.pi/4.0.')
        ratio= 2.0
    cotgTheta= math.sqrt(ratio-1)
    return math.atan(1.0/cotgTheta)   

def getMaximumShearWebStrutCrushing(concrete, NEd, Ac, bw, z, shearReinfAngle= math.pi/2.0, webStrutAngle= math.pi/4.0, nationalAnnex= None):
    ''' Return the maximum shear force due to diagonal compression in the web
        (strut crushing) according to expression 6.14 and 6.9 of EC2:2004.

    :param concrete: concrete material.
    :param NEd: axial force in the cross-section due to loading or prestressing.
    :param Ac: area of concrete cross-section. 
    :param bw: smallest width of the cross-section in the tensile area.
    :param z: internal lever arm.
    :param shearReinfAngle: (alpha) angle between shear reinforcement and the beam axis perpendicular to the shear force.
    :param webStrutAngle: (theta) angle between the concrete compression strut and the beam axis perpendicular to the shear force.
    :param nationalAnnex: identifier of the national annex.
    '''
    # alpha_cw: coefficient taking account of the state of the stress in the compression chord
    alpha_cw= concrete.getAlphaCw(NEd, Ac, nationalAnnex)
    # nu1: strength reduction factor for concrete cracked in shear
    nu1= concrete.getShearStrengthReductionFactor(nationalAnnex)
    fcd= -concrete.fcd() # design value of concrete compressive strength (MPa).
    webStrutAngle= checkWebStrutAngleLimits(webStrutAngle, nationalAnnex)
    cotgTheta= 1/math.tan(webStrutAngle)
    cotgAlpha= 1/math.tan(shearReinfAngle)
    return alpha_cw*bw*z*nu1*fcd*(cotgTheta+cotgAlpha)/(1+cotgTheta**2)

def getMaximumEffectiveShearReinforcement(concrete, NEd, Ac, bw, s, shearReinfSteel, shearReinfAngle= math.pi/2.0, nationalAnnex= None):
    ''' Return the maximum effective shear reinforcement according to expression
        6.15 and 6.12 of EC2:2004.

    :param concrete: concrete material.
    :param NEd: axial force in the cross-section due to loading or prestressing.
    :param Ac: area of concrete cross-section. 
    :param bw: smallest width of the cross-section in the tensile area.
    :param s: spacing of the stirrups.
    :param Asw: cross-sectional area of the shear reinforcement.
    :param shearReinfSteel: reinforcing steel material.
    :param shearReinfAngle: (alpha) angle between shear reinforcement and the beam axis perpendicular to the shear force.
    :param nationalAnnex: identifier of the national annex.
    '''
    # alpha_cw: coefficient taking account of the state of the stress in the compression chord
    alpha_cw= concrete.getAlphaCw(NEd, Ac, nationalAnnex)
    # nu1: strength reduction factor for concrete cracked in shear
    nu1= concrete.getShearStrengthReductionFactor(nationalAnnex)
    fcd= -concrete.fcd() # design value of concrete compressive strength (MPa).
    fywd= shearReinfSteel.fyd()
    return 0.5*alpha_cw*nu1*fcd/fywd*bw*s

def getWebStrutAngleLimits(nationalAnnex= None):
    ''' Return the limits specified in the expression 6.7N of EC2:2004
        for the web strut angle.

    :param nationalAnnex: identifier of the national annex.
    '''
    if(nationalAnnex=='Spain'): #EN 1992-1-1:2004 Apartado 6.2.3 (2)
        limSup= math.atan(1/0.5)
        limInf= math.atan(1/2.0)
    else:
        limSup= math.pi/4.0 # math.atan(1.0)
        limInf= math.atan(1/2.5)
    return limInf, limSup

def checkWebStrutAngleLimits(webStrutAngle, nationalAnnex= None):
    ''' Check that the strut angle is inside the limits specified
        in the expression 6.7N of EC2:2004. Otherwise, issue a
        warning and return a suitable strut angle.

    :param webStrutAngle: (theta) angle between the concrete compression web strut and the beam axis perpendicular to the shear force.
    :param nationalAnnex: identifier of the national annex.
    '''
    retval= webStrutAngle
    limInf, limSup= getWebStrutAngleLimits(nationalAnnex)
    if((webStrutAngle<limInf) or (webStrutAngle>limSup)): # eq 6.7N
        methodName= sys._getframe(0).f_code.co_name
        errString= methodName+'; strut angle: '+str(math.degrees(webStrutAngle))+' out of range: ['+str(math.degrees(limInf))+','+str(math.degrees(limSup))+']'
        if(webStrutAngle<limInf):
            retval= limInf
        elif(webStrutAngle>limSup):
            retval= limSup
        errString+= ' using '+str(math.degrees(retval))
        lmsg.warning(errString)
    return retval

def getShearResistanceShearReinf(concrete, NEd, Ac, bw, Asw, s, z, shearReinfSteel, shearReinfAngle= math.pi/2.0, webStrutAngle= math.pi/4.0, nationalAnnex= None):
    ''' Return the design value of the shear resistance VRds for shear 
        reinforced members according to expressions 6.7N, 6.13 and 6.14 of
        EC2:2004.

    :param concrete: concrete material.
    :param NEd: axial force in the cross-section due to loading or prestressing.
    :param Ac: area of concrete cross-section. 
    :param bw: smallest width of the cross-section in the tensile area.
    :param Asw: cross-sectional area of the shear reinforcement.
    :param s: spacing of the stirrups.
    :param z: inner lever arm, for a member with constant depth, corresponding to the bending moment in the element under consideration.
    :param shearReinfSteel: reinforcing steel material.
    :param shearReinfAngle: (alpha) angle between shear reinforcement and the beam axis perpendicular to the shear force.
    :param webStrutAngle: (theta) angle between the concrete compression web strut and the beam axis perpendicular to the shear force.
    :param nationalAnnex: identifier of the national annex.
    '''
    webStrutAngle= checkWebStrutAngleLimits(webStrutAngle, nationalAnnex)
    cotgTheta= 1/math.tan(webStrutAngle)
    cotgAlpha= 1/math.tan(shearReinfAngle)
    sinAlpha= math.sin(shearReinfAngle)
    fywd= shearReinfSteel.fyd()
    cotgThetaPluscotgAlpha= cotgTheta+cotgAlpha
    VRds= Asw/s*z*fywd*cotgThetaPluscotgAlpha*sinAlpha
    # nu1: strength reduction factor for concrete cracked in shear
    nu1= concrete.getShearStrengthReductionFactor(nationalAnnex)
    
    # if(sigma_reinf<0.8*shearReinfSteel.fyk):
    #     if(fckMPa<=60):
    #         nu1= 0.6
    #     else:
    #         nu1= max(0.9-fckMPa/200,0.5)
    
    # alpha_cw: coefficient taking account of the state of the stress in the compression chord
    alpha_cw= concrete.getAlphaCw(NEd, Ac, nationalAnnex)
    fcdMPa= -concrete.fcd()/1e6 # design value of concrete compressive strength (MPa).
    VRdmax= alpha_cw*bw*z*nu1*fcdMPa*cotgThetaPluscotgAlpha/(1+cotgTheta**2)*1e6
    return min(VRds, VRdmax)

def getMinShearReinforcementArea(concrete, shearReinfSteel, s, bw, shearReinfAngle= math.pi/2.0, nationalAnnex= None):
    ''' Return the cross-sectional area of the shear reinforcement 
        according to expression 9.4 of EC2:2004.

    :param concrete: concrete material.
    :param shearReinfSteel: reinforcing steel material.
    :param s: spacing of the stirrups.
    :param bw: smallest width of the cross-section in the tensile area.
    :param shearReinfAngle: (alpha) angle between shear reinforcement and the beam axis perpendicular to the shear force.
    :param nationalAnnex: identifier of the national annex.
    '''
    # minimum shear reinforcement ratio
    ro_w= concrete.getMinShearReinfRatio(shearReinfSteel, nationalAnnex)
    return ro_w*s*bw*math.sin(shearReinfAngle)

def getAdditionalTensileForceMainReinf(VEd, shearReinfAngle= math.pi/2.0, webStrutAngle= math.pi/4.0):
    ''' Return the additional tensile force, in the longitudinal reinforcement
        due to shear VEd according to expression 6.18 of EC2:2004.

    :param VEd: design value of the applied shear force.
    :param shearReinfAngle: (alpha) angle between shear reinforcement and the beam axis perpendicular to the shear force.
    :param webStrutAngle: (theta) angle between the concrete web compression strut and the beam axis perpendicular to the shear force.
    '''
    return 0.5*VEd*(1.0/math.tan(webStrutAngle)-1.0/math.tan(shearReinfAngle))

# EC2:2004 6.2.4 Shear between web and flanges.
def getFlangeStrutAngleLimits(compressionFlange= True, nationalAnnex= None):
    ''' Return the limits specified in the clause 6.2.4(4) of EC2:2004 for
        the angle of the struts in the flange.

    :param compressionFlange: true if flange is compressed.
    :param nationalAnnex: identifier of the national annex.
    '''
    if(compressionFlange):
        limSup= math.pi/4.0 # math.atan(1.0)
        limInf= math.atan(1/2.0) # 26.5º
    else:
        limSup= math.pi/4.0 # math.atan(1.0)
        limInf= math.atan(1/1.25) # 38.6º
    return limInf, limSup

def checkFlangeStrutAngleLimits(flangeStrutAngle, compressionFlange= True, nationalAnnex= None):
    ''' Check that the strut angle is inside the limits specified
        in the expression 6.7N of EC2:2004. Otherwise, issue a
        warning and return a suitable strut angle.

    :param flangeStrutAngle: (theta_f) angle between the concrete flange compression strut and the shear plane (see figure 6.7 on EC2:2004).
    :param compressionFlange: true if flange is compressed.
    :param nationalAnnex: identifier of the national annex.
    '''
    retval= flangeStrutAngle
    limInf, limSup= getFlangeStrutAngleLimits(compressionFlange, nationalAnnex)
    if((flangeStrutAngle<limInf) or (flangeStrutAngle>limSup)): # eq 6.7N
        methodName= sys._getframe(0).f_code.co_name
        errString= methodName+'; flange strut angle: '+str(math.degrees(flangeStrutAngle))+' out of range: ['+str(math.degrees(limInf))+','+str(math.degrees(limSup))+']'
        if(flangeStrutAngle<limInf):
            retval= limInf
        elif(flangeStrutAngle>limSup):
            retval= limSup
        errString+= ' using '+str(math.degrees(retval))
        lmsg.warning(errString)
    return retval

def getMaximumShearFlangeStrutCrushingStress(concrete, flangeStrutAngle= math.pi/4.0, compressionFlange= True, nationalAnnex= None):
    ''' Return the maximum shear force due to diagonal compression in the web
        (strut crushing) according to expression 6.22 of EC2:2004.

    :param concrete: concrete material.
    :param flangeStrutAngle: (theta_f) angle between the concrete flange compression strut and the shear plane (see figure 6.7 on EC2:2004).
    :param compressionFlange: true if flange is compressed.
    :param nationalAnnex: identifier of the national annex.
    '''
    # nu1: strength reduction factor for concrete cracked in shear
    nu1= concrete.getShearStrengthReductionFactor(nationalAnnex)
    fcd= -concrete.fcd() # design value of concrete compressive strength (MPa).
    flangeStrutAngle= checkFlangeStrutAngleLimits(flangeStrutAngle, compressionFlange, nationalAnnex)
    return nu1*fcd*math.sin(flangeStrutAngle)*math.cos(flangeStrutAngle)

def getConcreteFlangeShearStrength(concrete, hf, DeltaX, nationalAnnex= None):
    ''' Return the shear stress resisted by plain concrete according to
        clause 6.2.4 (6) of EC2:2004.

    :param concrete: concrete material.
    :param hf: flange thickness at the shear plane.
    :param DeltaX: length under consideration (see figure 6.7 on EC2:2004).
    :param nationalAnnex: identifier of the national annex.
    '''
    return concrete.getConcreteFlangeShearStressStrength(nationalAnnex)*DeltaX*hf

def getFlangeShearResistanceShearReinfStress(concrete, hf, Asf, sf, shearReinfSteel, flangeStrutAngle= math.pi/4.0, compressionFlange= True, nationalAnnex= None):
    ''' Return the design value of the flange shear resistance  
        according to expressions 6.21 of EC2:2004.

    :param concrete: concrete material.
    :param hf: flange thickness at the shear plane.
    :param Asf: cross-sectional area of the flange transverse reinforcement.
    :param sf: spacing of the reinforcement.
    :param shearReinfSteel: reinforcing steel material.
    :param flangeStrutAngle: (theta_f) angle between the concrete flange compression strut and the shear plane (see figure 6.7 on EC2:2004).
    :param compressionFlange: true if flange is compressed.
    :param nationalAnnex: identifier of the national annex.
    '''
    flangeStrutAngle= checkFlangeStrutAngleLimits(flangeStrutAngle, compressionFlange, nationalAnnex)
    cotgThetaF= 1/math.tan(flangeStrutAngle)
    fyd= shearReinfSteel.fyd()
    vRds= Asf/sf*fyd*cotgThetaF/hf # expression 6.21 of EC2:2004
    vRdmax= getMaximumShearFlangeStrutCrushingStress(concrete, flangeStrutAngle, compressionFlange, nationalAnnex) # expression 6.22 of EC2:2004
    return min(vRds, vRdmax)

# 7.3.2 Minimum reinforcement areas

def getAsMinCrackControl(concrete, reinfSteel, h, Act, sigmaC= 0.0, sigma_s= None, memberType= None):
    ''' Return the minimum area of reinforcing steel within the tensile zone
        according to expression 7.1 of clause 7.3.2 of EC2:2004.

    :param concrete: concrete material.
    :param reinfSteel: reinforcing steel material.
    :param h: section depth.
    :param Act: area of concrete within tensile zone. The tensile zone is 
                that part of the section which is calculated to be in tension 
                just before formation of the first crack.
    :param sigmaC: is the mean stress of the concrete acting on the part 
                   of the section under consideration: sigmaC= NEd/(bh).
    :param sigma_s: absolute value of the maximum stress permitted in the 
                    reinforcement immediately after formation of the crack. 
                    This may be taken as the yield strength of the 
                    reinforcement, f_yk, A lower value may, however, be needed 
                    to satisfy the crack width limits according to the maximum 
                    bar size or the maximum bar spacing (see 7.3.3 (2) 
                    of EN 1992-1-1).
    '''
    kc= 0.4 # coefficient which takes account of the stress distribution
            # within the section immediately prior to cracking and of the
            # change of the lever arm
    if(sigmaC<0.0):
        kc= 1.0
    elif(sigmaC==0.0):
        kc= 0.4
    else: # (sigmaC>0)
        h_aster= min(thickness, 1.0)
        k1= 1.5 if (sigmaC<0.0) else 2.0*h_aster/(3*thickness)
        if(memberType=='flange'):
            className= type(self).__name__
            methodName= sys._getframe(0).f_code.co_name
            errMsg= className+'.'+methodName+"; member type: " + str(memberType) + " not implemented.\n"
            lmsg.error(errMsg)
            kc= 0.5
        else:
            kc= min(1, 0.4*(sigmaC/(k1*thickness/h_aster*fct_eff)))
    k= 1.0 # coefficient which allows for the effect of non-uniform
           # self-equilibrating stresses, which lead to a reduction of
           # restraint forces.
    if(h>=0.8):
        k= 0.65
    elif(h>0.3):
        k= -0.7*h+1.21 # linear interpolation.
    fctef= concrete.fctm() # mean value of the tensile strength of the
                           # concrete effective at the time when the cracks
                           # may first be expected to occur.
    sigma_s= reinfSteel.fyk
    return kc*k*fctef/sigma_s*Act

def getAsMinBeams(concrete, reinfSteel, h, z, bt, d, nationalAnnex= None):
    ''' Return the minimum area of reinforcing steel within the tensile zone
        according to expression 9.1N of EC2:2004.

    :param concrete: concrete material.
    :param reinfSteel: reinforcing steel material.
    :param h: section depth.
    :param z: inner lever arm.
    :param bt: mean width of the tension zone.
    :param d: effective depth.
    :param nationalAnnex: identifier of the national annex.
    '''
    if(nationalAnnex=='Spain'):
        W= bt*h**2/6.0
        fctmfl= max(1.6-h,1.0)*concrete.fctm()
        return W/z*fctmfl/reinfSteel.fyd()
    else: # recommended values.
        return max(0.26*concrete.fctm()/reinfSteel.fyk,0.0013)*bt*d
    
def getAsMaxBeams(Ac, nationalAnnex= None):
    ''' Return the minimum area of reinforcing steel within the tensile zone
        according to expression 9.1N of EC2:2004.

    :param Ac: area of concrete cross-section. 
    :param nationalAnnex: identifier of the national annex.
    '''
    return .04*Ac

def getAsMinColumns(concrete, reinfSteel, NEd, Ac, nationalAnnex= None):
    ''' Return the minimum area of longitudinal reinforcement
        according to expression 9.12N of EC2:2004.

    :param concrete: concrete material.
    :param reinfSteel: reinforcing steel material.
    :param NEd: sdesign axial compression force.
    :param Ac: area of the concrete section.
    :param nationalAnnex: identifier of the national annex.
    '''
    if(nationalAnnex=='Spain'):
        fycd= min(self.steel.fyd(),400e6)
        retval= max(0.1*NEd/fycd, 0.004*Ac)
    else: # recommended values.
        retval= max(0.1*NEd/self.steel.fyd(), 0.002*Ac)
    return retval

def getAsMaxColumns(concrete, reinfSteel, Ac, nationalAnnex= None):
    ''' Return the minimum area of longitudinal reinforcement
        according to clause 9.5.2 (3) of EC2:2004.

    :param concrete: concrete material.
    :param reinfSteel: reinforcing steel material.
    :param NEd: sdesign axial compression force.
    :param Ac: area of the concrete section.
    :param nationalAnnex: identifier of the national annex.
    '''
    if(nationalAnnex=='Spain'): # Clause 9.5.2 (3)
        fycd= min(self.steel.fyd(),400e6)
        retval= 1.0*concrete.fcd()/fycd*Ac
    else: # Clause 9.5.2 (3) recommended values.
        retval= 0.04*Ac
    return retval

def getAsMinWalls(concrete, reinfSteel, Ac, thickness, vertical= True, compressedSide= False, verticalReinforcementArea= None, nationalAnnex= None):
    ''' Return the minimum area of longitudinal reinforcement in 
        the tensile zone (or in the compressed one if compressedSide==True)
        according to clause 9.6.2 and 9.6.3 of EC2:2004.

    :param concrete: concrete material.
    :param reinfSteel: reinforcing steel material.
    :param Ac: area of the concrete section.
    :param thickness: wall thickness.
    :param vertical: if true compute the minimum for the vertical reinforcement,
                     otherwise return the minimum for the horizontal one.
    :param compressedSide: if true compute the minimum for the vertical
                           reinforcement in the compressed side, otherwise
                           return the minimum reinforcement for the tensile
                           zone.
    :param verticalReinforcementArea: area of the vertical reinforcement used
                                      to calculate the horizontal reinforcement
                                      at the same side.
    :param nationalAnnex: identifier of the national annex.
    '''
    if(nationalAnnex=='Spain'):
        fyk= self.steel.fyk
        if(vertical): # 9.6.2 (1)
            if(fyk>=500e6):
                retval= 0.0009*Ac
            else:
                retval= 0.0012*Ac
            retval= max(retval, 0.04*Ac*concrete.fcd()/reinfSteel.fyd())
            if(compressedSide):
                retval*=0.3
        else: # 9.6.3 (1)
            Ac_eff= Ac
            if(thickness>0.5):
                Ac_eff= Ac/thickness*0.5
            if(fyk>=500e6):
                retval= 0.0032*Ac_eff
            else:
                retval= 0.004*Ac_eff
    else: # Recommended values.
        if(vertical): # 9.6.2
            retval= 0.001*Ac # Note 1
        else: # 9.6.3
            retval= 0.001*Ac
            if(verticalReinforcementArea):
                retval= max(retval, 0.25*verticalReinforcementArea)
            else:
                methodName= sys._getframe(0).f_code.co_name
                errMsg= methodName+"; the area of the vertical reinforcement is needed to compute the minimum area of the horizontal one.\n"
                lmsg.error(errMsg)
                retval= None
    return retval

# Check normal stresses limit state.
class BiaxialBendingNormalStressController(lscb.BiaxialBendingNormalStressControllerBase):
    '''Object that controls normal stresses limit state.'''

    def __init__(self,limitStateLabel):
        ''' Constructor.
        
        :param limitStateLabel: label that identifies the limit state.
        '''
        super(BiaxialBendingNormalStressController,self).__init__(limitStateLabel)

class UniaxialBendingNormalStressController(lscb.UniaxialBendingNormalStressControllerBase):
    '''Object that controls normal stresses limit state (uniaxial bending).'''

    def __init__(self,limitStateLabel):
        ''' Constructor.
        
        :param limitStateLabel: label that identifies the limit state.
        '''
        super(UniaxialBendingNormalStressController,self).__init__(limitStateLabel)



# Shear checking.

class ShearController(lscb.ShearControllerBase):
    '''Shear control according to EHE-08.'''

    ControlVars= cv.RCShearControlVars
    def __init__(self, limitStateLabel, solutionProcedureType= lscb.defaultStaticNonLinearSolutionProcedure, nationalAnnex= None):
        ''' Constructor.
        
        :param limitStateLabel: label that identifies the limit state.
        :param solutionProcedureType: type of the solution procedure to use
                                      when computing load combination results.
        :param nationalAnnex: identifier of the national annex.
        '''
        super(ShearController,self).__init__(limitStateLabel, fakeSection= False, solutionProcedureType= solutionProcedureType)
        self.concreteFibersSetName= "concrete" # Name of the concrete fibers set.
        self.rebarFibersSetName= "reinforcement" # Name of the rebar fibers set.
        self.nationalAnnex= nationalAnnex # national annex.
        
    def getShearStrengthCrackedNoShearReinf(self, scc, concrete, reinfSteel, Nd, Md, Vd, Td, rcSets, circular= False):
        '''Return the design value of the shear resistance VRdc for cracked 
           sections subjected to bending moment, according to expressions 6.2.a 
           and 6.2.b of EC2:2004.

         :param scc: fiber model of the section.
         :param concrete: concrete material.
         :param reinfSteel: shear reinforcement steel.
         :param Nd: Design value of axial force (here positive if in tension)
         :param Md: Absolute value of design value of bending moment.
         :param Vd: Absolute value of effective design shear (clause 42.2.2).
         :param Td: design value of torsional moment.
         :param rcSets: fiber sets in the reinforced concrete section.
         :param circular: if true we reduce the efectiveness of the shear 
                          reinforcement due to the transverse inclination of
                          its elements.
        '''
        Ac= rcSets.getConcreteArea(1) # Ac
        strutWidth= scc.getCompressedStrutWidth() # b0
        # area of the tensile reinforcement
        tensileReinforcementArea= 0.0
        if(rcSets.getNumTensionRebars()>0):
            tensileReinforcementArea= rcSets.tensionFibers.getArea(1)
        return getShearResistanceCrackedNoShearReinf(concrete= concrete, NEd= Nd, Ac= Ac, Asl= tensileReinforcementArea, bw= strutWidth, d= scc.getEffectiveDepth(), nationalAnnex= self.nationalAnnex)
    
    def getShearStrengthNonCrackedNoShearReinf(self, scc, concrete, reinfSteel, Nd, Md, Vd, Td, rcSets, circular= False):
        ''' Return the design value of the shear resistance VRdc for non-cracked 
            sections subjected to bending moment, according to expression 6.4 of
            EC2:2004.

         :param scc: fiber model of the section.
         :param concrete: concrete material.
         :param reinfSteel: shear reinforcement steel.
         :param Nd: Design value of axial force (here positive if in tension)
         :param Md: Absolute value of design value of bending moment.
         :param Vd: Absolute value of effective design shear (clause 42.2.2).
         :param Td: design value of torsional moment.
         :param rcSets: fiber sets in the reinforced concrete section.
         :param circular: if true we reduce the efectiveness of the shear 
                          reinforcement due to the transverse inclination of
                          its elements.
        '''
        Ac= rcSets.getConcreteArea(1) # Ac
        strutWidth= scc.getCompressedStrutWidth() # b0
        E0= rcSets.getConcreteInitialTangent()
        # second moment of area.
        I= scc.getHomogenizedI(E0)
        # first moment of area above and about the centroidal axis.
        S= scc.getSPosHomogenized(E0)
        alpha_l= 1.0 # see expression 6.4 in EC2:2004.
        return getShearResistanceNonCrackedNoShearReinf(concrete= concrete, I= I, S= S, NEd= Nd, Ac= Ac, bw= strutWidth, alpha_l= alpha_l)
    
    def getShearStrengthNoShearReinf(self, scc, concrete, reinfSteel, Nd, Md, Vd, Td, rcSets, circular= False):
        ''' Return the design value of the shear resistance VRdc for cracked
            or non-cracked sections subjected to bending moment, according 
            to expression 6.4 of EC2:2004.

         :param scc: fiber model of the section.
         :param concrete: concrete material.
         :param reinfSteel: shear reinforcement steel.
         :param Nd: Design value of axial force (here positive if in tension)
         :param Md: Absolute value of design value of bending moment.
         :param Vd: Absolute value of effective design shear (clause 42.2.2).
         :param Td: design value of torsional moment.
         :param rcSets: fiber sets in the reinforced concrete section.
         :param circular: if true we reduce the efectiveness of the shear 
                          reinforcement due to the transverse inclination of
                          its elements.
        '''
        retval= 0.0
        #tensionedReinforcement= rcSets.tensionFibers
        self.isBending= scc.isSubjectedToBending(0.1)
        #numberOfTensionedRebars= rcSets.getNumTensionRebars()
        if(self.isBending):
            self.eps1= rcSets.getMaxConcreteStrain()
            # design tensile strength of the concrete.
            fctdH= concrete.fctd()
            E0= rcSets.getConcreteInitialTangent()
            if((E0*self.eps1)<fctdH): # Non cracked section
                retval= self.getShearStrengthNonCrackedNoShearReinf(scc= scc, concrete= concrete, reinfSteel= reinfSteel, Nd= Nd, Md= Md, Vd= Vd, Td= Td, rcSets= rcSets, circular= circular)
            else: # cracked
                retval= self.getShearStrengthCrackedNoShearReinf(scc= scc, concrete= concrete, reinfSteel= reinfSteel, Nd= Nd, Md= Md, Vd= Vd, Td= Td, rcSets= rcSets, circular= circular)
        else: # not bending.
            # In real problems you don't need to check the shear strength right
            # over the pinned support (M= 0) so, normally, this code is not
            # reach. Anyway, we return a safe estimation based on the shear
            # strength of a T-beam flange in its plane.
            hf= scc.getCompressedStrutWidth()
            Ac= rcSets.getConcreteArea(1) # Ac
            DeltaX= Ac/hf
            retval= getConcreteFlangeShearStrength(concrete= concrete, hf= hf, DeltaX= DeltaX, nationalAnnex= None)
        return retval
        
    def getShearStrengthShearReinf(self, scc, concrete, reinfSteel, Nd, Md, Vd, Td, rcSets, circular= False):
        ''' Compute the shear strength at failure WITH shear reinforcement.
         XXX Presstressing contribution not implemented yet.

         :param scc: fiber model of the section.
         :param concrete: concrete material.
         :param reinfSteel: shear reinforcement steel.
         :param Nd: Design value of axial force (here positive if in tension)
         :param Md: Absolute value of design value of bending moment.
         :param Vd: Absolute value of effective design shear (clause 42.2.2).
         :param Td: design value of torsional moment.
         :param rcSets: fiber sets in the reinforced concrete section.
         :param circular: if true we reduce the efectiveness of the shear 
                          reinforcement due to the transverse inclination of
                          its elements.
        '''
        Ac= rcSets.getConcreteArea(1) # Ac
        strutWidth= scc.getCompressedStrutWidth() # b0
        isBending= scc.isSubjectedToBending(0.1)
        if(isBending):
            innerLeverArm= scc.getMechanicLeverArm() # z
        else: # not bending
            # In real problems you don't need to check the shear strength right
            # over the pinned support (M= 0) so, normally, this code is not
            # reach. Anyway, we return a safe estimation of the lever arm.
            sectionDepth= Ac/strutWidth
            innerLeverArm= 0.7*sectionDepth
        return getShearResistanceShearReinf(concrete= concrete, NEd= Nd, Ac= Ac, bw= strutWidth, Asw= self.Asw, s= self.stirrupSpacing, z= innerLeverArm, shearReinfSteel= reinfSteel, shearReinfAngle= self.alpha, webStrutAngle= math.pi/4.0, nationalAnnex= self.nationalAnnex)

    def getShearStrength(self, scc, concrete, reinfSteel, Nd, Md, Vd, Td, rcSets, circular= False):
        ''' Compute the shear strength at failure WITH or WITHIOUT shear 
            reinforcement.
         XXX Presstressing contribution not implemented yet.

         :param scc: fiber model of the section.
         :param concrete: concrete material.
         :param reinfSteel: shear reinforcement steel.
         :param Nd: Design value of axial force (here positive if in tension)
         :param Md: Absolute value of design value of bending moment.
         :param Vd: Absolute value of effective design shear (clause 42.2.2).
         :param Td: design value of torsional moment.
         :param rcSets: fiber sets in the reinforced concrete section.
         :param circular: if true we reduce the efectiveness of the shear 
                          reinforcement due to the transverse inclination of
                          its elements.
        '''
        if(self.Asw==0):
            retval= self.getShearStrengthNoShearReinf(scc= scc, concrete= concrete, reinfSteel= reinfSteel, Nd= Nd, Md= Md, Vd= Vd, Td= Td, rcSets= rcSets, circular= circular)
        else:
            retval= self.getShearStrengthShearReinf(scc= scc, concrete= concrete, reinfSteel= reinfSteel, Nd= Nd, Md= Md, Vd= Vd, Td= Td, rcSets= rcSets, circular= circular)
        return retval
        
    def checkInternalForces(self, sct, Nd, Md, Vd, Td):
        '''  Compute the shear strength at failure.
         XXX Presstressing contribution not implemented yet.

         :param sct: reinforced concrete section object to chech shear on.
         :param Nd: Design value of axial force (positive if in tension)
         :param Md: Absolute value of design value of bending moment.
         :param Vd: Absolute value of effective design shear (clause 42.2.2).
         :param Td: design value of torsional moment.
        '''
        section= sct.getProp('sectionData')
        concreteCode= section.getConcreteType()
        reinforcementCode= section.getReinfSteelType()
        shReinf= section.getShearReinfY()
        circular= section.isCircular()
        strutWidth= sct.getCompressedStrutWidth() # bw
        self.Asw= shReinf.getAs()
        self.stirrupSpacing= shReinf.shReinfSpacing
        self.alpha= shReinf.angAlphaShReinf
        #Searching for the best theta angle (concrete strut inclination).
        if(self.Asw>0.0):
            self.theta= getWebStrutAngleForSimultaneousCollapse(concrete= concreteCode, bw= strutWidth, s= self.stirrupSpacing, Asw= self.Asw, shearReinfSteel= reinforcementCode, shearReinfAngle= shReinf.angAlphaShReinf)
        else:
            self.theta= math.pi/4.0
        #We calculate Vu for several values of theta and chose the highest Vu with its associated theta
        rcSets= self.extractFiberData(sct, concreteCode, reinforcementCode)
        self.Vu= self.getShearStrength(sct, concreteCode,reinforcementCode,Nd,Md,Vd,Td, rcSets, circular)
        VuTmp= self.Vu
        if(VuTmp!=0.0):
            FCtmp= Vd/VuTmp
        else:
            FCtmp= 1e99
        return FCtmp, VuTmp
        
    def checkSection(self, sct, elementDimension):
        ''' Check shear on the section argument.

        :param sct: reinforced concrete section object to chech shear on.
        :param elementDimension: dimension of the element (1, 2 or 3).
        '''
        NTmp= sct.getStressResultantComponent("N")
        MyTmp= sct.getStressResultantComponent("My")
        MzTmp= sct.getStressResultantComponent("Mz")
        MTmp= math.sqrt((MyTmp)**2+(MzTmp)**2)
        VyTmp= sct.getStressResultantComponent("Vy")
        VzTmp= sct.getStressResultantComponent("Vz")
        VTmp= self.getShearForce(Vy= VyTmp, Vz= VzTmp, elementDimension= elementDimension)
        TTmp= sct.getStressResultantComponent("Mx")
        #Searching for the best theta angle (concrete strut inclination).
        #We calculate Vu for several values of theta and chose the highest Vu with its associated theta
        FCtmp, VuTmp= self.checkInternalForces(sct= sct, Nd= NTmp, Md= MTmp, Vd= VTmp, Td= TTmp)
        return FCtmp, VuTmp, NTmp, VyTmp, VzTmp, TTmp, MyTmp, MzTmp 

    def check(self, elements, combName):
        ''' For each element in the set 'elements' passed as first parameter
        and the resulting internal forces for the load combination 'combName'  
        passed as second parameter, this method calculates all the variables 
        involved in the shear-ULS checking and obtains the capacity factor.
        In the case that the calculated capacity factor is smaller than the 
        smallest obtained for the element in previous load combinations, this 
        value is saved in the element results record.

        Elements processed are those belonging to the phantom model, that is to 
        say, of type xc.ZeroLengthSection. As we have defined the variable 
        fakeSection as False, a reinfoced concrete fiber section is generated
        for each of these elements. 

        XXX Rebar orientation not taken into account yet.
        '''
        if(self.verbose):
            lmsg.log("Postprocessing combination: "+combName)
        for e in elements:
            # Call getResisting force to update the internal forces.
            R= e.getResistingForce() 
            if(__debug__):
                if(not R):
                    AssertionError('Can\'t get the resisting force.')        
            scc= e.getSection()
            idSection= e.getProp("idSection")
            masterElementDimension= e.getProp("masterElementDimension")
            FCtmp, VuTmp, NTmp, VyTmp, VzTmp, TTmp, MyTmp, MzTmp= self.checkSection(sct= scc, elementDimension= masterElementDimension)
            Mu= 0.0 #Apparently EC2 doesn't use Mu
            if(FCtmp>=e.getProp(self.limitStateLabel).CF):
                e.setProp(self.limitStateLabel, self.ControlVars(idSection= idSection, combName= combName, CF= FCtmp, N= NTmp, My= MyTmp, Mz= MzTmp, Mu= Mu, Vy= VyTmp, Vz= VzTmp, theta= self.theta, Vu=VuTmp)) # Worst case
                
def get_nu_mu(concrete, steel, b, h, d, dp, x, As1, As2):
    ''' Return internal forces at failure for a rectangular section according
        to clause 6.1 of EC2:2004 part 1.

    :param concrete: concrete of the section.
    :param steel: reinforcement steel material.
    :param b: section width.
    :param h: section depth.
    :param d: effective depth of the cross-section.
    :param dp: effective cover of the compression reinforcement.
    :param x: depth of the neutral axis.
    :param As1: area of the tension reinforcement.
    :param As2: area of the compression reinforcement.
    '''

    eps_cu= -concrete.getEpscu3()
    eps_ud= steel.epsilon_ud()
    limRange2= d*eps_cu/(eps_cu+eps_ud)
    xlim= eps_cu*d/(eps_cu+steel.fyd()/steel.Es)
    eta= concrete.getEtaC()
    fcd= -concrete.fcd()
    lmbd= concrete.getLambdaC()
    xb= x*b
    h_2= h/2.0
    h_2md= h_2-d
    if(x<0): # Range 1
        eps_s2= eps_ud*(dp-x)/(d-x)
        sigma_s2_eps_s2= steel.getStressD(eps_s2)
        Nu= - As2*sigma_s2_eps_s2 - As1*sigma_s1_eps_ud
        Mu= - As2*sigma_s2_eps_s2*(h_2-dp) - As1*sigma_s1_eps_ud*h_2md
    elif(x<=limRange2): # Range 2:
        eps_s2= eps_ud*(x-dp)/(d-x)
        sigma_s2_eps_s2= steel.getStressD(eps_s2)
        Nu= eta*fcd*lmbd*xb + As2*sigma_s2_eps_s2 - As1*sigma_s1_eps_ud
        Mu= eta*fcd*lmbd*xb*(h_2-lmbd*x/2) + As2*sigma_s2_eps_s2*(h_2-dp) - As1*sigma_s1_eps_ud*h_2md
    elif(x<=xlim): # Range 3
        eps_s2= eps_cu*(x-dp)/x
        sigma_s2_eps_s2= steel.getStressD(eps_s2)
        eps_s1= eps_cu*(d-x)/x
        sigma_s1_eps_s1= steel.getStressD(eps_s1)
        Nu= eta*fcd*lmbd*xb + As2*sigma_s2_eps_s2 - As1*sigma_s1_eps_s1
        Mu= eta*fcd*lmbd*xb*(h_2-lmbd*x/2) + As2*sigma_s2_eps_s2*(h_2-dp) - As1*sigma_s1_eps_s1*h_2md
    elif(x<=d): # Range 4
        eps_s2= eps_cu*(x-dp)/x
        sigma_s2_eps_s2= steel.getStressD(eps_s2)
        eps_s1= eps_cu*(d-x)/x
        sigma_s1_eps_s1= steel.getStressD(eps_s1)
        Nu= eta*fcd*lmbd*xb + As2*sigma_s2_eps_s2 - As1*sigma_s1_eps_s1
        Mu= eta*fcd*lmbd*xb*(h_2-lmbd*x/2) + As2*sigma_s2_eps_s2*(h_2-dp) - As1*sigma_s1_eps_s1*h_2md
    elif(x<=h): # Range 4a:
        eps_s2= eps_cu*(x-dp)/x
        sigma_s2_eps_s2= steel.getStressD(eps_s2)
        eps_s1= eps_cu*(x-d)/x
        sigma_s1_eps_s1= steel.getStressD(eps_s1)
        Nu= eta*fcd*lmbd*xb + As2*sigma_s2_eps_s2 + As1*sigma_s1_eps_s1
        Mu= eta*fcd*lmbd*xb*(h_2-lmbd*x/2) + As2*sigma_s2_eps_s2*(h_2-dp) + As1*sigma_s1_eps_s1*h_2md
    else: # Range 5:
        eps_s2= εc3*(x-dp)/(x-h_2)
        sigma_s2_eps_s2= steel.getStressD(eps_s2)
        eps_s1= εc3*(x-d)/(x-h_2)
        sigma_s1_eps_s1= steel.getStressD(eps_s1)
        h_x= h/x
        Nu= eta*fcd*(1- (1-lmbd)*h_x)*h*b + As2*sigma_s2_eps_s2 + As1*sigma_s1_eps_s1
        Mu= eta*fcd*(1- (1-lmbd)*h_x)*h*b*(h_2-(1-(1-lmbd)*h_x)*h_2) + As2*sigma_s2_eps_s2*(h_2-dp) + As1*sigma_s1_eps_s1*h_2md

    return Nu, Mu

#       ___ ___ ___    _     _                       ___ 
#      | __/ __|_  )__/ |   /_\  _ _  _ _  _____ __ | __|
#      | _| (__ / /___| |  / _ \| ' \| ' \/ -_) \ / | _| 
#      |___\___/___|  |_| /_/ \_\_||_|_||_\___/_\_\ |_|  
# EN 1992-1-1:2004. Annex F.
# Tension reinforcement expressions for in-plane stress conditions
class Ec2InPlaneStressLimitStateData(lsd.ULS_LimitStateData):
    ''' In-plane stress conditions limit state data.

    '''
    def __init__(self):
        '''Constructor.
        '''
        super(Ec2InPlaneStressLimitStateData,self).__init__(limitStateLabel= 'ULS_Ec2InPlaneStressResistance', outputDataBaseFileName= fn.ec2InPlaneStressVerificationResultsFile, designSituation= 'permanent')
        
    def getStressesDict(self, nmbComb, elems):
        '''Creates a dictionary with the element's stresses.

        :param nmbComb: combination name.
        :param elems: element set.
        '''
        return eif.get_stresses_dict(nmbComb, elems)
    
    def readInternalForces(self, setCalc):
        ''' Read the stresses for the elements in the set argument.

        :param setCalc: elements to read stresses for.
        '''
        return lsd.read_stresses_dict(self.getStressesFileName(), setCalc)
        
    def checkElements(self, elementsToCheck, outputCfg= lsd.VerifOutVars()):
        '''Checking of stresses in ultimate limit states
            (see self.dumpCombinations).

        :param elementsToCheck: elements to check.
        :param outputCfg: instance of class 'VerifOutVars' which defines the 
               variables that control the output of the checking (set of 
               elements to be analyzed, append or not the results to a file,
               generation or not of lists, ...)
        '''
        return elementsToCheck.check(limitStateData= self, outputCfg= outputCfg, threeDim= True)
    
    def runChecking(self ,outputCfg):
        '''This method reads, for the elements in setCalc, the stresses 
        previously calculated and saved in the corresponding file.
        Using the 'initControlVars' and 'updateEfficiencyForSet' methods of 
        the controller, the appropriate attributes are assigned to the 
        elements and the associated limit state verification is run.
        The results are written to a file in order to be displayed or listed.

        :param outputCfg: instance of class 'VerifOutVars' which defines the 
               variables that control the output of the checking (set of 
               elements to be analyzed, append or not the results to the 
               result file [defatults to 'N'], generation or not
               of list file [defatults to 'N', ...)
        '''
        retval= super(Ec2InPlaneStressLimitStateData,self).runChecking(outputCfg, sections= [''])
        return retval
    
    def check(self, setCalc, controller, appendToResFile='N', listFile='N', calcMeanCF='N'):
        ''' Perform limit state checking.

        :param setCalc: set of elements to be checked (defaults to 'None' which 
               means that all the elements in the file of internal forces
               results are analyzed) 
        :param controller: object that controls the limit state checking.
        :param appendToResFile:  'Yes','Y','y',.., if results are appended to 
               existing file of results (defaults to 'N')
        :param listFile: 'Yes','Y','y',.., if latex listing file of results 
               is desired to be generated (defaults to 'N')
        :param calcMeanCF: 'Yes','Y','y',.., if average capacity factor is
               meant to be calculated (defaults to 'N')
        '''
        outputCfg= lsd.VerifOutVars(setCalc= setCalc, controller= controller, appendToResFile= appendToResFile, listFile= listFile, calcMeanCF= calcMeanCF, outputDataBaseFileName= self.getOutputDataBaseFileName())
        return self.runChecking(outputCfg= outputCfg)

ec2InPlaneStressResistance= Ec2InPlaneStressLimitStateData()

class Ec2InPlaneStressController(lscb.LimitStateControllerBase):
    '''Object that controls reinforced concrete ULS using the
       expressions given in Annex F of EN 1992-1-1:2004.'''

    ControlVars= cv.Ec2InPlaneStressControlVars
    def __init__(self, limitStateLabel, solutionProcedureType= lscb.defaultStaticLinearSolutionProcedure):
        ''' Constructor.

        :param limitStateLabel: limit state identifier.
        :param solutionProcedureType: type of the solution procedure to use
                                      when computing load combination results.
        '''
        super(Ec2InPlaneStressController, self).__init__(limitStateLabel= limitStateLabel, solutionProcedureType= solutionProcedureType)

    def initControlVars(self,setCalc):
        '''Initialize control variables over elements.

        :param setCalc: set of elements to which define control variables
        '''
        for e in setCalc.elements:
            e.setProp(self.limitStateLabel, self.ControlVars())
            
    def readStresses(self, stressesCombFileName, setCalc= None):
        '''Launch checking.

        :param stressesCombFileName: Name of the file containing the stresses 
                                    on the element gauss points.
        '''
        # Read internal forces results.
        intForcItems= lsd.read_internal_forces_file(stressesCombFileName, setCalc)
        return intForcItems[2]
            
    def updateEfficiency(self, elem, elementStresses):
        ''' Compute the efficiency of the element material
            subjected to the stresses argument and update
            its value if its bigger than the previous one.

        :param elem: finite element whose section will be checked.
        :param elementStresses: stresses on the element.
        '''
        # Get available strengths.
        ftd1_avail= elem.getProp('ftd1_avail')
        ftd2_avail= elem.getProp('ftd2_avail')
        sigma_c_avail= elem.getProp('sigma_c_avail')
        if(ftd1_avail and ftd2_avail and sigma_c_avail):
            # Check each element section.
            for stresses in elementStresses:
                sigma_11= -stresses.sigma_11
                sigma_12= stresses.sigma_12
                sigma_22= -stresses.sigma_22
                if(sigma_11>0.0 and sigma_22>0.0 and (sigma_11*sigma_22>sigma_12**2)): # no reinforcement needed.
                   ftd1_req= 0.0
                   ftd2_req= 0.0
                   sigma_c_req= 0.0
                   CFtmp= 0.0
                else:
                    
                    abs_sigma_12= abs(sigma_12)
                    if(sigma_11<abs_sigma_12):
                        ftd1_req= abs_sigma_12-sigma_11
                        ftd2_req= max(abs_sigma_12-sigma_22, 0)
                        sigma_c_req= 2*abs_sigma_12
                    else:
                        ftd1_req= 0.0
                        ftd2_req= sigma_12**2/sigma_11-sigma_22
                        sigma_c_req= sigma_11*(1+(sigma_12/sigma_11)**2)
                if(ftd1_req<0 or ftd2_req<0 or sigma_c_req<0):
                    className= type(self).__name__
                    methodName= sys._getframe(0).f_code.co_name
                    errMsg= className+'.'+methodName+"; somethig when wrong.\n"
                    if(ftd1_req<0):
                        errMsg+= ' negative required strenght on axis 1: '+str(ftd1_req)+'\n'
                    if(ftd2_req<0):
                        errMsg+= ' negative required strenght on axis 2: '+str(ftd2_req)+'\n'
                    if(sigma_c_req<0):
                        errMsg+= ' negative required strenght for concrete: '+str(sigma_c_req)+'\n'
                    lmsg.error(errMsg)
                    exit(1)
                ftd1_cf= ftd1_req/ftd1_avail
                ftd2_cf= ftd2_req/ftd2_avail
                sigma_c_cf= sigma_c_req/sigma_c_avail
                CFtmp= max(ftd1_cf, ftd2_cf, sigma_c_cf)
                CF= elem.getProp(self.limitStateLabel).CF
                if(CFtmp>CF):
                    cv= self.ControlVars(combName= stresses.idComb, CF= CFtmp, sigma_11= sigma_11, sigma_12= sigma_12, sigma_22= sigma_22, ftd1_req= ftd1_req, ftd2_req= ftd2_req, sigma_c_req= sigma_c_req, ftd1_cf= ftd1_cf, ftd2_cf= ftd2_cf, sigma_c_cf= sigma_c_cf)
                    elem.setProp(self.limitStateLabel, cv)

