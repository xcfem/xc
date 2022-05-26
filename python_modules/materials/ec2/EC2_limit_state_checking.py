# -*- coding: utf-8 -*-
from __future__ import print_function

''' Classes and functions for limit state checking according to Eurocode 2. '''

__author__= "Ana Ortega (AO_O) "
__copyright__= "Copyright 2016, AO_O"
__license__= "GPL"
__version__= "3.0"
__email__= "ana.ortega@ciccp.es "

import sys
import math
import geom
from materials import limit_state_checking_base as lscb
from materials.ec2 import EC2_materials
from materials.sections.fiber_section import fiber_sets
from postprocess import control_vars as cv
from misc_utils import log_messages as lmsg
from solution import predefined_solutions
from materials import concrete_base

class CrackStraightController(lscb.LimitStateControllerBase):
    '''Definition of variables involved in the verification of the cracking
    serviceability limit state according to EC2 when considering a concrete
    stress-strain diagram that takes account of the effects of tension 
    stiffening.

    :ivar k1: coefficient that takes account of the bound properties of the 
          bonded reinforcement. k1=0.8 for high bond bars, k1=1.0 for bars with
          effectively plain surface (e.g. prestressing tendons). Defaults to 0.8
    :ivar k3: defaults to the value recommended by EC2 and in EHE k3=3.4
    :ivar k4: defaults to the value recommended by EC2 and EHE k4=0.425
    '''
    ControlVars= cv.RCCrackStraightControlVars
    def __init__(self, limitStateLabel):
        ''' Constructor.
        
        :param limitStateLabel: label that identifies the limit state.
        '''
        super(CrackStraightController,self).__init__(limitStateLabel,fakeSection= False)
        self.k1=0.8
        self.k3=3.4
        self.k4=0.425

    def EC2_k2(self,eps1,eps2):
        '''Return the coefficient k2 involved in the calculation of the mean 
        crack distance according to EC2. This coefficient represents the effect of 
        the tension diagram in the section.

        :param eps1: maximum deformation calculated in the section at the limits 
           of the tension zone
        :param eps2: minimum deformation calculated in the section at the limits 
           of the tension zone
        '''
        k2= (eps1+eps2)/(2.0*eps1)
        return k2

    def EC2_hceff(self,h,d,x):
        '''Return the maximum height to be considered in the calculation of the 
        concrete effective area in tension.
    
        :param width: section width
        :param h: lever arm
        :param d: effective depth of the cross-section 
        :param x: depth of the neutral fiber.

        '''
        hceff=min(2.5*(h-d),abs(h+x)/3.,h/2.)
        return hceff

    def check(self,elements,nmbComb):
        ''' For each element in the set 'elememts' passed as first parameter and 
        the resulting internal forces for the load combination 'nmbComb'  
        passed as second parameter, this method calculates all the variables 
        involved in the crack-SLS checking and obtains the crack width.
        In the case that the calculated crack width is greater than the 
        biggest obtained for the element in previous load combinations, this value
        is saved in the element results record. 

        Elements processed are those belonging to the phantom model, that is to 
        say, of type xc.ZeroLengthSection. As we have defined the variable 
        fakeSection as False, a reinfoced concrete fiber section is generated
        for each of these elements. 
        '''
        if(self.verbose):
          lmsg.log("Postprocessing combination: "+nmbComb)
        for e in elements:
            Aceff=0  #init. value
            R=e.getResistingForce()
            sct=e.getSection()
            sctCrkProp=lscb.fibSectLSProperties(sct)
            sctCrkProp.setupStrghCrackDist()
            hceff=self.EC2_hceff(sctCrkProp.h,sctCrkProp.d,sctCrkProp.x)
            Acgross=sct.getGrossEffectiveConcreteArea(hceff)
            Aceff=sct.getNetEffectiveConcreteArea(hceff,"tensSetFb",15.0)
            concrete=EC2_materials.concrOfName[sctCrkProp.concrName]
            rfSteel=EC2_materials.steelOfName[sctCrkProp.rsteelName]
            k2=self.EC2_k2(sctCrkProp.eps1,sctCrkProp.eps2)
#            print('elem= ',e.tag, ' Aceff= ',Aceff)
            if Aceff<=0:
                
                s_rmax=0
            else:
                ro_s_eff=sctCrkProp.As/Aceff      #effective ratio of reinforcement
                s_rmax=self.k3*sctCrkProp.cover+self.k1*k2*self.k4*sctCrkProp.fiEqu/ro_s_eff
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
        self.solutionProcedure.solveComb(nmbComb)
        for e in elements:
            sct=e.getSection()
            rfset=sct.getFiberSets()["reinfSetFb"]
            eps_sm=rfset.getStrainMax()
            srmax=e.getProp("s_rmax")
#            eps_cm=concrete.fctm()/2.0/concrete.E0()
#            wk=srmax*(eps_sm-eps_cm)
            wk=srmax*eps_sm
#            print(' eps_sm= ',eps_sm, ' srmax= ', srmax, ' wk= ',wk)
#            print('e.getProp(self.limitStateLabel).wk', e.getProp(self.limitStateLabel).wk)
            if (wk>e.getProp(self.limitStateLabel).wk):
                R=e.getProp('ResF')
                e.setProp(self.limitStateLabel, self.ControlVars(idSection=e.getProp("idSection"),combName=nmbComb,N=-R[0],My=-R[4],Mz=-R[5],s_rmax=srmax,eps_sm=eps_sm,wk=wk))



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

def ro_eff(A_s,width,h_c_eff):
    '''
    Returns the effective reinforcement ratio [A_s/A_ceff]depth of the effective area of concrete in tension surrounding
    the reinforcement or prestressing tendons, according to EC2

    :param A_s: area of reinforcment steel
    :param width: width of the RC cross-section
    :param ,h_c_eff: depth of the effective area of concrete in 
    tension surrounding the reinforcement or prestressing tendons
    '''
    retval=A_s/width/h_c_eff
    return retval

def s_r_max(k1,k2,k3,k4,cover,fiReinf,ro_eff):
    '''
    Returns the maximum crack spacing, according to EC2
 
    :param k1: coefficient which takes account of the bond properties
               of the bonded reinforcement:

               - =0.8 for high bond bars
               - =1.6 for bars with an effectively plain surface (e.g. prestressing tendons)

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
    retval=k3*cover+k1*k2*k4*fiReinf/ro_eff
    return retval

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

def getStrutAngleForSimultaneousCollapse(concrete, bw, s, Asw, shearReinfSteel, shearReinfAngle= math.pi/2.0, nationalAnnex= None):
    ''' Return the strut angle that makes concrete collapse at the same time
        that the shear reinforcement (V_{Rd,s}= V_{Rd,max})

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
    cotgTheta= math.sqrt((bw*s*nu*fcd)/(Asw*fywd*math.sin(shearReinfAngle))-1)
    return math.atan(1.0/cotgTheta)    

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
    alpha_cw= concrete.getAlphaCw(NEd, Ac, nationalAnnex)
    # nu1: strength reduction factor for concrete cracked in shear
    nu1= concrete.getShearStrengthReductionFactor(nationalAnnex)
    fcd= -concrete.fcd() # design value of concrete compressive strength (MPa).
    fywd= shearReinfSteel.fyd()
    return 0.5*alpha_cw*nu1*fcd/fywd*bw*s

def getStrutAngleLimits(nationalAnnex= None):
    ''' Return true if the strut angle is inside the limits specified
        in the expression 6.7N of EC2:2004.

    :param nationalAnnex: identifier of the national annex.
    '''
    if(nationalAnnex=='Spain'): #EN 1992-1-1:2004 Apartado 6.2.3 (2)
        limSup= math.atan(1/0.5)
        limInf= math.atan(1/2.0)
    else:
        limSup= math.atan(1.0)
        limInf= math.atan(1/2.5)
    return limInf, limSup    

def getShearResistanceShearReinf(concrete, NEd, Ac, bw, Asw, s, z, shearReinfSteel, shearReinfAngle= math.pi/2.0, strutAngle= math.pi/4.0, nationalAnnex= None):
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
    :param strutAngle: (theta) angle between the concrete compression strut and the beam axis perpendicular to the shear force.
    :param nationalAnnex: identifier of the national annex.
    '''
    limInf, limSup= getStrutAngleLimits(nationalAnnex)
    if((strutAngle<limInf) or (strutAngle>limSup)): # eq 6.7N
        methodName= sys._getframe(0).f_code.co_name
        errString= methodName+'; strut angle: '+str(math.degrees(strutAngle))+' out of range: ['+str(math.degrees(limInf))+','+str(math.degrees(limSup))+']'
        if(strutAngle<limInf):
            strutAngle= limInf
        elif(strutAngle>limSup):
            strutAngle= limSup
        errString+= ' using '+str(math.degrees(strutAngle))
        lmsg.warning(errString)
    cotgTheta= 1/math.tan(strutAngle)
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

def getAdditionalTensileForceMainReinf(VEd, shearReinfAngle= math.pi/2.0, strutAngle= math.pi/4.0):
    ''' Return the additional tensile force, in the longitudinal reinforcement
        due to shear VEd according to expression 6.18 of EC2:2004.

    :param VEd: design value of the applied shear force.
    :param shearReinfAngle: (alpha) angle between shear reinforcement and the beam axis perpendicular to the shear force.
    :param strutAngle: (theta) angle between the concrete compression strut and the beam axis perpendicular to the shear force.
    '''
    return 0.5*VEd*(1.0/math.tan(strutAngle)-1.0/math.tan(shearReinfAngle))
