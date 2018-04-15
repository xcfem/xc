# -*- coding: utf-8 -*-
''' Classes and functions for limit state checking according to Eurocode 2. '''

__author__= "Ana Ortega (AO_O) "
__copyright__= "Copyright 2016, AO_O"
__license__= "GPL"
__version__= "3.0"
__email__= "ana.ortega@ciccp.es "

import math
import xc_base
import geom
from materials import limit_state_checking_base as lscb
from materials.ec2 import EC2_materials
from materials.sections.fiber_section import fiber_sets
from postprocess import control_vars as cv
from miscUtils import LogMessages as lmsg
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
    def __init__(self,limitStateLabel):
        super(CrackStraightController,self).__init__(limitStateLabel,fakeSection= False)
        self.k1=0.8
        self.k3=3.4
        self.k4=0.425

    def initControlVars(self,elements):
        ''' Initialize control variables over elements.

        :param elements: elements to define control variables in
        '''
        for e in elements:
            e.setProp(self.limitStateLabel,cv.RCCrackStraightControlVars())

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
#            print 'elem= ',e.tag, ' Aceff= ',Aceff
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
        predefined_solutions.resuelveComb(self.preprocessor,nmbComb,self.analysis,1)
        for e in elements:
            sct=e.getSection()
            rfset=sct.getFiberSets()["reinfSetFb"]
            eps_sm=rfset.getStrainMax()
            srmax=e.getProp("s_rmax")
#            eps_cm=concrete.fctm()/2.0/concrete.E0()
#            wk=srmax*(eps_sm-eps_cm)
            wk=srmax*eps_sm
            print ' eps_sm= ',eps_sm, ' srmax= ', srmax, ' wk= ',wk
#            print 'e.getProp(self.limitStateLabel).wk', e.getProp(self.limitStateLabel).wk
            if (wk>e.getProp(self.limitStateLabel).wk):
                R=e.getProp('ResF')
                e.setProp(self.limitStateLabel,cv.RCCrackStraightControlVars(idSection=e.getProp("idSection"),combName=nmbComb,N=-R[0],My=-R[4],Mz=-R[5],s_rmax=srmax,eps_sm=eps_sm,wk=wk))



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
