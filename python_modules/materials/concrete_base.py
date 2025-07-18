# -*- coding: utf-8 -*-
''' Base classes for reinforced concrete materials.'''

from __future__ import division
from __future__ import print_function

__author__= "Ana Ortega (AO_O) and Luis C. Pérez Tato (LCPT)"
__copyright__= "Copyright 2015, AO_O and LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= " ana.Ortega.Ort@gmail.com, l.pereztato@gmail.com"


import math
import sys
import scipy.interpolate
from scipy import stats
from materials import typical_materials
from postprocess.reports import graph_material
from materials.sections import material_with_DK_diagrams as matWDKD
import matplotlib.pyplot as plt
import numpy as np
import geom
import xc
from postprocess.reports import graph_material as mg
from misc_utils import log_messages as lmsg

class ReinforcedConcreteLimitStrains(object):
    ''' Limit strains for reinforced concrete material.

    :ivar EpsC0: Average of the initial minimum compressive strain in the concrete (see figure 42.1.3 in EHE-08).
    :ivar EpsCU: Ultimate bending strain in the concrete (see figure 42.1.3 in EHE-08).
    :ivar SMaxStrain: maximum allowable strain for passive reinforcement.
    '''
    def __init__(self,EpsCU= -3.5e-3,EpsC0= -2.0e-3,SMaxStrain= 10e-3):
        ''' Constructor.

        :param EpsC0: Average of the initial minimum compressive strain in the concrete (see figure 42.1.3 in EHE-08).
        :param EpsCU: Ultimate bending strain in the concrete (see figure 42.1.3 in EHE-08).
        :param SMaxStrain: maximum allowable strain for passive reinforcement.
        '''  
        self.EpsCU= EpsCU
        self.EpsC0= EpsC0
        self.SMaxStrain= SMaxStrain
        
    def bendingOK(self, epsCMin, epsSMax):
        '''Check for flexural.

        :param epsCMin: minimum strain for concrete.
        :param epsSMax: maximum strain for reinforcing steel.
        '''
        retval= False
        if(epsCMin>=self.EpsCU): # Minimum concrete strain OK.
            if(epsSMax<=self.SMaxStrain): # Maximum reinforcing steel strain OK.
                retval= True
        return retval

    def getBendingEfficiency(self, epsCMin,epsSMax):
        '''Return efficiency in flexural bending.

        :param epsCMin: minimum strain for concrete.
        :param epsSMax: maximum strain for reinforcing steel.
        '''
        return (max(epsCMin/self.EpsCU,epsSMax/self.SMaxStrain))

    def compressiveBendingOK(self, epsCMin,epsCMax):
        '''Check for compressive or flexural compressive strength.

        :param epsCMin: minimum strain for concrete.
        :param epsCMax: maximum strain for concrete.
        '''
        retval= False
        if(epsCMin>=(self.EpsCU - 3/4.0*epsCMax)): # Concrete minimum strain OK.
            if(epsCMax>=self.EpsC0): # Concrete maximum strain OK.
                retval= True
        return retval

    def getCompressiveBendingEfficiency(self, epsCMin, epsCMax):
        '''Return efficiency for compressive or flexural compressive strength..

        :param epsCMin: minimum strain for concrete.
        :param epsCMax: maximum strain for concrete.
        '''
        return (max(epsCMin/(self.EpsCU - 3/4.0*epsCMax),epsCMax/self.EpsC0))

    def tensileBendingOK(self, epsSMax):
        '''Check for tensile or flexural tensile stress.

        :param epsSMax: maximum strain for reinforcing steel.
        '''
        retval= False
        if(epsSMax<=self.SMaxStrain): # Maximum reinforcing steel strain OK.
            retval= True
        return retval

    def getTensileBendingEfficiency(self, epsSMax):
        '''Return efficiency for tensile or flexural tensile stress.

        :param epsSMax: maximum strain for reinforcing steel.
        '''
        return (epsSMax/self.SMaxStrain)

    def getNormalStressesEfficiency(self, tipoSol, epsCMin, epsCMax, epsSMax):
        ''' Return efficiency under normal stresses.'''
        if(tipoSol==1): # Tensile or flexural tensile.
            retval= self.getTensileBendingEfficiency(epsSMax)
        else:
            if(tipoSol==2): # Bending.
                retval= self.getBendingEfficiency(epsCMin,epsSMax)
            else:
                if(tipoSol==3): # Compressive or flexural compressive.
                    retval= self.getCompressiveBendingEfficiency(epsCMin,epsCMax)
                else:
                    retval= -100.0
        return retval


class Concrete(matWDKD.MaterialWithDKDiagrams):
    ''' Concrete model base class.

    :ivar matTagD:  tag of the uni-axial material in the design diagram
    :ivar fck:       characteristic (5%) cylinder strength of the concrete [Pa]
    :ivar gmmC:      partial safety factor for concrete
    :ivar nuc:       Poisson coefficient
    :ivar cemType:   type of cement

      - ``R``  for cement of strength Classes CEM 42,5 R, CEM 52,5 Nand CEM 52,5 R (Class R) [cementos de alta resistencia en EHE]
      - ``N``  for cement of strength Classes CEM 32,5 R, CEM 42,5 N (Class N) [cementos normales en EHE]
      - ``S``  for cement of strength Classes CEM 32,5 N (Class S) [cementos de endurecimiento lento en EHE]

    :ivar alfacc:    factor which takes account of the fatigue in the concrete when it is subjected to high levels of compression stress due to long duration loads. Normally alfacc=1 (default value)

    :ivar tensionStiffparam: variable to determine the behaviour of concrete 
    under tension. If tensionStiffparam is an instance of the class 
    paramTensStiffness, tension stiffness of concrete is considered in the 
    constitutive model to take into account the tensile capacity of the intact
    concrete between cracks. The stress strain relationship corresponds to a 
    concrete02 material (linear tension softening),  based on the 
    tension-stiffening constitutive model proposed by Stramandinoli & 
    La Rovere (ref. article: Engineering Structures 30 (2008) 2069-2080).

    :ivar initTensStiff: variable that also determines the behaviour of 
     concrete under tension. If initTensStiff==True a concrete02 material model 
     is initialized with a tension capacity almost equal to 0 (equivalent to 
     the concrete01 diagram). Defaults to False

    If tensionStiffparam is None and initTensStiff==False (default values) no 
    tensile strength is considered; the stress strain relationship corresponds 
    to a concrete01 material (zero tensile strength).

    '''
    nuc= 0.2 #** Poisson coefficient
    cemType='N'     #type of cement:
             #    = 0,20 for cement of strength Classes CEM 42,5 R, CEM 52,5 Nand CEM 52,5 R (Class R) 
             #           [cementos de alta resistencia en EHE]
             #    = 0,25 for cement of strength Classes CEM 32,5 R, CEM 42,5 N (Class N) 
             #           [cementos normales en EHE]
             #    = 0,38 for cement of strength Classes CEM 32,5 N (Class S) 
             #           [cementos de endurecimiento lento en EHE]
    tensionStiffparam=None
    
    
    # Definition of «derived» properties of the material.
    def __init__(self, nmbConcrete, fck, gammaC, alphacc= 1.0):
        ''' Constructor.

        :param nmbConcrete: material name.
        :param fck: characteristic (5%) cylinder strength of the concrete.
        :param gammaC: partial safety factor for concrete.
        :param alphacc: factor which takes account of the fatigue in the concrete when it is subjected to high levels of compression stress due to long duration loads. Normally alfacc=1 (default value)
        '''
        super(Concrete,self).__init__(nmbConcrete)
        self.fck= fck #** characteristic (5%) cylinder strength of the concrete [Pa]
        self.gmmC= gammaC #** Partial safety factor for concrete
        self.initTensStiff=False
        self.alfacc= alphacc

    def getDict(self):
        ''' Return a dictionary with the values of the object members.'''
        retval= super().getDict()
        retval.update({'fck':self.fck, 'gamma_c':self.gmmC, 'init_tens_stiff':self.initTensStiff, 'alpha_cc':self.alfacc})
        return retval

    def setFromDict(self, dct):
        ''' Set the member values from those in the given dictionary.'''
        self.fck= dct['fck']
        self.gmmC= dct['gamma_c']
        self.initTensStiff= dct['init_tens_stiff']
        self.alfacc= dct['alpha_cc']
        super().setFromDict(dct)

    def __eq__(self, other):
        '''Overrides the default implementation'''
        if(other is not None):
            if(self is not other):
                retval= super().__eq__(other= other)
                if(retval):
                    retval= (self.fck == other.fck)
                if(retval):
                    retval= (self.gmmC == other.gmmC)
                if(retval):
                    retval= (self.initTensStiff == other.initTensStiff)
                if(retval):
                    retval= (self.alfacc == other.alfacc)
            else:
                retval= True
        else:
            retval= False
        return retval
        
    def setupName(self, matName):
        ''' Material setup.

        :param matName: material name.
        '''
        super().setupName(matName)
        if(matName):
            # Creep model for concrete.
            self.nmbDiagTD= "dgTD"+self.materialName # Name identifying the linear elastic stress-strain diagram.
            self.matTagTD= -1 # Tag of the uniaxial material with the design stress-strain diagram .
            self.materialDiagramTD= None # Design stress-strain diagram.
    
    def density(self,reinforced= True):
        '''Return concrete density in kg/m3.'''
        if(reinforced):
            return 2500
        else:
            return 2400
        
    def fcd(self):
        '''concrete design strength [Pa][-]
        '''
        return self.alfacc*self.fck/self.gmmC 

    def getCDepth(self):
        ''' Return the depth of the concrete rectangular compression block
            (see figure 12 SIA 262:2013 or figure 39.5b of clause 39.5 b 
            of EHE).
        '''
        retval= 0.8
        if(self.fck>50e6):
            retval-= (self.fck-50e6)/400e6
        return retval

    def fmaxK(self):
        '''maximum characteristic strength of the concrete [Pa][-]
        '''
        #in the previous version fmaxK was calculated as the product of 0.85 by fck
#        return self.alfacc*self.fck
        return self.fck

    def fmaxD(self):
        return self.fcd()

    def fckMPa(self):
        '''Characteristic (5%) cylinder strength of the concrete in 
           MPa (absolute value)''' 
        return abs(self.fck)*1e-6

    def fctm(self):
        '''mean tensile strength [Pa][+] (table 3.1 EC2)'''
        return self.getFctm() 

    def fctk(self):
        '''characteristic tensile strength [Pa] (5% fractile)'''
        return self.getFctk005()

    def fctd(self):
        '''Design mean tensile strength [Pa][+].'''
        return abs(self.fctk())/self.gmmC 

    def epsilon0(self):
        ''' strain at peak stress at parabola-rectangle diagram'''
        return self.getEpsc2()

    def epsilonU(self):
        ''' nominal ultimate strain at parabola-rectangle diagram'''
        return self.getEpscu2()

    def expnPR(self):
        ''' exponent n for the parabola-rectangle diagram '''
        return self.getExpN()

    def Ecm(self):
        '''longitudinal secant modulus of deformation, at 28 days
           (slope of the secant of the actual stress-strain curve)) [Pa] [+] 
        '''
        return self.getEcm()

    def Ect(self):
        '''Elastic modulus in the tensile linear-elastic range of concrete
        [Pa] [+] 
        '''
        return self.E0()

    def Gcm(self):
        ''' Shear elactic modulus [Pa][+] '''
        return self.getEcm()/(2*(1+self.nuc))  #**

    def getFcm(self):
        '''Fcm: mean compressive strength [Pa][-] at 28 days 
        (table 3.1 EC2, art. 39.6 EHE-08)
        '''
        return (self.fckMPa()+8)*(-1e6)
 
    def getFctk005(self):
        '''Fctk005: tensile strength [Pa][+] 5% fractile (table 3.1 EC2).'''
        return 0.7*self.getFctm()
 
    def getFctk095(self):
        '''Fctk095: tensile strength [Pa][+] 95% fractile (table 3.1 EC2).'''
        return 1.3*self.getFctm()

    def defTDConcreteMC10(self, preprocessor):
        ''' Defines a TDConcreteMC10 uniaxial material.

        :param preprocessor: pre-processor for the finite element problem.
        '''
        retval= None
        if(hasattr(self,'creepParameters')):
            name= self.nmbDiagTD
           
            cp= self.creepParameters
            Ec= self.getEcmT(t= cp.age) # concrete modulus of elasticity at loading age.
            fc= self.getFcmT(t= cp.age) # Compressive strength at loading age.
            fct= self.getFctmT(t= cp.age) # Tensile strength at loading age.
            Ecm= self.Ecm() # 28-day modulus of elasticity.
            retval= typical_materials.defTDConcreteMC10(preprocessor= preprocessor,name= name, fc= fc, ft= fct, Ec= Ec, Ecm= Ecm, beta= cp.beta, age= cp.age, epsba= cp.epsba, epsbb= cp.epsbb, epsda= cp.epsda, epsdb= cp.epsdb, phiba= cp.phiba, phibb= cp.phibb, phida= cp.phida, phidb= cp.phidb, tcast= cp.tcast, cem= cp.cem)
        else:
            className= type(self).__name__
            methodName= sys._getframe(0).f_code.co_name
            lmsg.warning(className+'.'+methodName+'; creep parameters are not defined. Command ignored.')
        return retval
    
    def defDiagE(self, preprocessor, overrideRho= None):
        ''' Returns and XC linear elastic uniaxial material.

        :param preprocessor: pre-processor for the finite element problem.
        '''
        rho= self.density()
        if(overrideRho):
            rho= overrideRho
        self.materialDiagramE= typical_materials.defElasticMaterial(preprocessor= preprocessor, name= self.nmbDiagE, E= self.getEcm(), rho= rho)
        self.matTagE= self.materialDiagramE.tag
        return self.materialDiagramE

    
    def clearDiagE(self):
        ''' Clear previously defined diagram.'''
        if(self.materialDiagramE):
            self.matTagE= -1
            self.materialDiagramE= None
            
    def defDiagK(self,preprocessor):
        ''' Defines a uniaxial material to represent the characteristic stress-strain diagram

        - If tensionStiffparam is an instance of the class paramTensStiffness,          tension stiffness of concrete is considered in the constitutive model  
        to take into account tensile capacity of the intact concrete between 
        cracks. The stress strain relationship corresponds to a concrete02 
        material (linear tension softening), based on the tension-stiffening  
        constitutive model proposed by Stramandinoli & La Rovere (ref. article: 
        Engineering Structures 30 (2008) 2069-2080)

        -If initTensStiff==True a concrete02 material model 
        is initialized with a tension capacity almost equal to 0 (equivalent to 
        the concrete01 diagram). Defaults to False

        -If tensionStiffparam is None and initTensStiff==False (default value)
         no tensile strength is considered; the stress strain relationship  
         corresponds to a concrete01 material (zero tensile strength).
        '''
        if self.tensionStiffparam:
            self.tensionStiffparam.diagType='K'
            '''
            Approximation of the exponential decay curve in the post-cracking 
            range by means of its regression line
            '''
            ftdiag=self.tensionStiffparam.pointOnsetCracking()['ft']       #stress at the adopted point for concrete onset cracking
            self.ft=ftdiag
            ectdiag=self.tensionStiffparam.pointOnsetCracking()['eps_ct']  #strain at the adopted point for concrete onset cracking
            self.epsct0=ectdiag
            #eydiag=self.tensionStiffparam.eps_y()                          #reinforcing steel strain at yield point
            Etsdiag=abs(self.tensionStiffparam.regresLine()['slope'])
            self.Ets=Etsdiag
            self.epsctu=ectdiag+ftdiag/Etsdiag
            self.materialDiagramK= typical_materials.defConcrete02(preprocessor=preprocessor,name= self.nmbDiagK,epsc0=self.epsilon0(),fpc=self.fmaxK(),fpcu=0.85*self.fmaxK(),epscu=self.epsilonU(),ratioSlope=0.1,ft=ftdiag,Ets=Etsdiag)
            self.materialDiagramK.epsct0=ectdiag
            self.materialDiagramK.epsctu=ectdiag+ftdiag/Etsdiag
            '''
            Approximation of the exponential decay curve in the post-cracking 
            range by means of a regression line that passes through the point 
            (eps_ct,f_ct) where cracking starts.
            This approximation produces a less conservative result 
            '''
            # ftdiag=self.tensionStiffparam.f_ct
            # Etsdiag=-self.tensionStiffparam.slopeRegresLineFixedPoint()
            # self.materialDiagramK= typical_materials.defConcrete02(preprocessor=preprocessor,name= self.nmbDiagK,epsc0=self.epsilon0(),fpc=self.fmaxK(),fpcu=0.85*self.fmaxK(),epscu=self.epsilonU(),ratioSlope=0.1,ft=ftdiag,Ets=Etsdiag)
        elif(self.initTensStiff):
            ftdiag=self.fctk()/10.
#            self.ft=ftdiag
            ectdiag=ftdiag/self.E0()
#            self.epsct0=ectdiag
            #eydiag=self.tensionStiffparam.eps_y()                          #reinforcing steel strain at yield point
            Etsdiag=ftdiag/(5*ectdiag)
#            self.Ets=Etsdiag
#            self.epsctu=ectdiag+ftdiag/Etsdiag
            self.materialDiagramK= typical_materials.defConcrete02(preprocessor=preprocessor,name= self.nmbDiagK,epsc0=self.epsilon0(),fpc=self.fmaxK(),fpcu=0.85*self.fmaxK(),epscu=self.epsilonU(),ratioSlope=0.1,ft=ftdiag,Ets=Etsdiag)
#            self.materialDiagramK.epsct0=ectdiag
#            self.materialDiagramK.epsctu=ectdiag+ftdiag/Etsdiag
            
        else:
            self.materialDiagramK= typical_materials.defConcrete01(preprocessor=preprocessor,name= self.nmbDiagK,epsc0=self.epsilon0(),fpc=self.fmaxK(),fpcu=self.fmaxK(),epscu=self.epsilonU())
        self.matTagK= self.materialDiagramK.tag
        return self.materialDiagramK #30160925 was 'return self.matTagK'
    
    def clearDiagK(self):
        ''' Clear previously defined diagram.'''
        if(self.materialDiagramK):
            self.matTagK= -1
            self.materialDiagramK= None

    def defDiagD(self,preprocessor):
        ''' Defines a uniaxial material to represent the design stress-strain 
        diagram

        - If tensionStiffparam is an instance of the class paramTensStiffness,          tension stiffness of concrete is considered in the constitutive model  
        to take into account tensile capacity of the intact concrete between 
        cracks. The stress strain relationship corresponds to a concrete02 
        material (linear tension softening), based on the tension-stiffening  
        constitutive model proposed by Stramandinoli & La Rovere (ref. article: 
        Engineering Structures 30 (2008) 2069-2080)

        -If initTensStiff==True a concrete02 material model 
        is initialized with a tension capacity almost equal to 0 (equivalent to 
        the concrete01 diagram). Defaults to False

        -If tensionStiffparam is None and initTensStiff==False (default values)
         no tensile strength is considered; the stress strain relationship  
         corresponds to a concrete01 material (zero tensile strength).
        '''
        if self.tensionStiffparam:
            self.tensionStiffparam.diagType='D'
            ftdiag=self.tensionStiffparam.pointOnsetCracking()['ft']
            self.ft=ftdiag
            ectdiag=self.tensionStiffparam.pointOnsetCracking()['eps_ct']
            self.epsct0=ectdiag
            # eydiag= self.tensionStiffparam.eps_y()
            Etsdiag=abs(self.tensionStiffparam.regresLine()['slope'])
            self.Ets=Etsdiag
            self.epsctu=ectdiag+ftdiag/Etsdiag
            self.materialDiagramD= typical_materials.defConcrete02(preprocessor=preprocessor,name= self.nmbDiagD,epsc0=self.epsilon0(),fpc=self.fmaxD(),fpcu=0.85*self.fmaxD(),epscu=self.epsilonU(),ratioSlope=0.1,ft=ftdiag,Ets=Etsdiag)
            self.materialDiagramD.epsct0=ectdiag
            self.materialDiagramD.epsctu=ectdiag+ftdiag/Etsdiag
        elif(self.initTensStiff):
            ftdiag=self.fctk()/10.
            ectdiag=ftdiag/self.E0()
            Etsdiag=ftdiag/(5*ectdiag)
            self.materialDiagramD= typical_materials.defConcrete02(preprocessor=preprocessor,name= self.nmbDiagD,epsc0=self.epsilon0(),fpc=self.fmaxD(),fpcu=0.85*self.fmaxD(),epscu=self.epsilonU(),ratioSlope=0.1,ft=ftdiag,Ets=Etsdiag)
        else:
            self.materialDiagramD= typical_materials.defConcrete01(preprocessor=preprocessor,name= self.nmbDiagD,epsc0=self.epsilon0(),fpc=self.fmaxD(),fpcu=self.fmaxD(),epscu=self.epsilonU())
        self.matTagD= self.materialDiagramD.tag
        return self.materialDiagramD #30160925 was 'return self.matTagD'
    
    def clearDiagD(self):
        ''' Clear previously defined diagram.'''
        if(self.materialDiagramD):
            self.matTagD= -1
            self.materialDiagramD= None
            
    def defDiagTD(self, preprocessor):
        ''' Returns and XC TDConcreteMC10 uniaxial material.

        :param preprocessor: pre-processor for the finite element problem.
        '''
        self.materialDiagramTD= self.defTDConcreteMC10(preprocessor= preprocessor)
        self.matTagTD= self.materialDiagramTD.tag
        return self.materialDiagramTD
    
    def clearDiagTD(self):
        ''' Clear previously defined diagram.

        :param preprocessor: pre-processor for the finite element problem.
        '''
        if(self.materialDiagramTD):
            self.matTagTD= -1
            self.materialDiagramTD= None
    
    def defDiag(self, preprocessor, matDiagType):
        ''' Returns an XC uniaxial material corresponding to the stress-strain
            diagram of the concrete.

        :param preprocessor: pre-processor for the finite element problem.
        :param matDiagType: diagram type; if "k" return the diagram 
                            corresponding to characteristic values of the 
                            material, if "d" return the design values one.
        '''
        retval= None
        if matDiagType.lower() =='d':
            if(self.materialDiagramD): # if already defined.
                retval= self.materialDiagramD
            else: # if not defined yet, do it now.
                retval= self.defDiagD(preprocessor)
        elif matDiagType.lower() =='k':
            if(self.materialDiagramK): # if already defined.
                retval= self.materialDiagramK
            else: # if not defined yet, do it now.
                retval= self.defDiagK(preprocessor)
        elif(matDiagType=='elastic'):
            if(self.materialDiagramE): # if already defined.
                retval= self.materialDiagramE
            else: # if not defined yet, do it now.
                retval= self.defDiagE(preprocessor)
        elif matDiagType.lower() =='td':
            if(self.materialDiagramTD): # if already defined.
                retval= self.materialDiagramTD
            else: # if not defined yet, do it now.
                retval= self.defDiagTD(preprocessor)
        else:
            className= type(self).__name__
            methodName= sys._getframe(0).f_code.co_name
            lmsg.error(className+'.'+methodName+'; diagram type : '+str(self.matDiagTyp)+' is not known.')
        return retval

    def clearDiagrams(self):
        ''' Clear previously defined material diagrams.'''
        self.clearDiagD()
        self.clearDiagK()
        self.clearDiagE()
        self.clearDiagTD()

    def sigmaPR(self,eps):
        ''' stress as function of strain according to parabola-rectangle diagram'''
        return self.sigmac(eps)

#Time-dependent properties of concrete according to Eurocode 2
    def getBetaCC(self, t= 28):
        '''beta_cc: coefficient (EC2:2004: sect. 3.1.2 paragraph (6) expression  (3.2), EHE-08: art. 31.3).

        :param t:      age of the concrete in days
        '''
        if self.cemType=='R': # high-strength cement
            s=0.20
        elif self.cemType=='S': # slow cement
            s=0.38
        else: # normal cement
            s=0.25
        return math.exp(s*(1-math.sqrt(28/t)))

    def getFcmT(self, t=28):
        '''FcmT: mean concrete compressive strength [Pa][-] at an age of t days 
        (sect. 3.1.2 EC2, art. 31.3 EHE-08)
 
        :param t: age of the concrete in days.
        '''
        return self.getFcm()*self.getBetaCC(t)

    def getFckt(self,t=28):
        '''Fckt: concrete compressive strength [Pa][-] at time t (for stages) 
           (sect. 3.1.2 EC2)

        :param t:      age of the concrete in days
        '''
        if t >= 28:
            return self.fckMPa()*(-1e6)
        elif t > 3:
            return (abs(self.getFcmT(t))*(1e-6)-8)*(-1e6)
        else:
            return "t must be > 3 days"

#Shrinkage
#   Basic drying shrinkage
    def getShrEpscd0(self, RH):
        '''Basic drying shrinkage strain [-]
        for the calculation of the drying shrinkage strain
        according to equattion B.11 of clause B.2 
        (Annex B Eurocode 2:2004 part 1-1)
 
        :param RH: ambient relative humidity(%)
        '''
        fcmMPa=abs(self.getFcm())/1e6
        fcm0MPa=10       
        epscd0= -0.85*((220+110*self.getShrAlfads1())*math.exp(-self.getShrAlfads2()*fcmMPa/fcm0MPa))*1e-6*self.getShrBetaRH(RH) # equation B-11
        return epscd0


    def getShrBetaRH(self,RH):
        '''Coefficient for the calculation of the basic drying shrinkage 
           strain according to equation B.12 of clause 
           B.2 (Annex B Eurocode 2:2004 part 1-1). DEPRECATED by Eurocode2:2021

        :param RH:  ambient relative humidity(%)
        '''
        if RH<99:
            betaRH=1.55*(1-(RH/100)**3)
        else:
            betaRH=0.25
        return betaRH

    def getShrAlfads1(self):
        '''Coefficient for the calculation of the
        basic drying shrinkage strain 
        (Annex B Eurocode 2 part 1-1)
        '''
        if self.cemType=='R':    #high-strength cement
            alfads1=6
        elif self.cemType=='S':  #slow cement
            alfads1=3
        else:                   #normal cement
            alfads1=4
        return alfads1

    def getShrAlfads2(self):
        '''Coefficient for the calculation of the basic drying shrinkage strain (Annex B Eurocode 2 part 1-1)
        '''
        if self.cemType=='R':    #high-strength cement
            alfads2=0.11
        elif self.cemType=='S':  #slow cement
            alfads2=0.13
        else:                    #normal cement
            alfads2=0.12
        return alfads2

    def getShrEpscd(self,t,ts,RH,h0):
        '''Drying shrinkage strain[-] (art. 3.1.4 Eurocode 2 part 1-1)

        :param t:     age of concrete in days at the moment considered
        :param ts:    age of concrete in days at the beginning of drying shrinkage (or swelling).  Normally this is at the end of curing
        :param RH:    ambient relative humidity(%)
        :param h0:  notional size of the member.

               - h0=``2*Ac/u``, where:
               - Ac= cross sectional area
               - u = perimeter of the member in contact with the atmosphere

        '''
        #epscd: drying shrinkage strain
        if t<ts:
            shrEpscd=0.0
        else:
            shrEpscd=self.getShrBetadstts(t,ts,h0)*self.getShrKh(h0)*self.getShrEpscd0(RH)
        return shrEpscd 
    

    def getShrKh(self,h0):
        '''coefficient depending on the notional size h0
        for the calculation of the drying shrinkage strain
        (table 3.3 Eurocode 2 part 1-1)

        :param h0:  notional size of the member.
                    - h0=``2*Ac/u``, where:
                         - Ac= cross sectional area
                         - u = perimeter of the member in contact with 
                               the atmosphere.
        '''
        x=(0.0, 0.1, 0.2, 0.3, 0.5, 1e10)
        y=(1.0,1.0,0.85,0.75,0.70,0.70)
        f= scipy.interpolate.interp1d(x, y)
        kh= f(h0)
        return kh

    def getShrBetadstts(self,t,ts,h0):
        '''coefficient for the calculation of the drying shrinkage strain
        according to expression 3.10 of clause 3.1.4 of Eurocode 2:2004 
        part 1-1. DEPRECATED in Eurocode 2:2021.
 
        :param t:     age of concrete in days at the moment considered
        :param ts:    age of concrete in days at the beginning of drying shrinkage (or swelling)
                   Normally this is at the end of curing
        :param h0:  notional size of the member.

                   - h0= 2*Ac/u, where:
                   - Ac= cross sectional area
                   - u= perimeter of the member in contact with the atmosphere
        '''
        if t<ts:
            shrBetadstts=0
        else:
            shrBetadstts=(t-ts)/(t-ts+0.04*math.pow(h0*1e3,3.0/2.0))
        return shrBetadstts 

#   Autogenous shrinkage strain
    def getShrEpsca(self, t, t0= 0.0):
        '''Autogenous shrinkage strain according to expression 3.11 of 
           clause 3.1.4 Eurocode 2:2004 part 1-1. DEPRECATED in Eurocode 2:2021

        :param t: age of concrete in days at the moment considered
        :param t0: minimum age of concrete for loading (see SOFiSTiK Benchmark
                   No. 21. Real Creep and Shrinkage Calculation of a T-Beam
                   Prestressed CS page 7). 
        '''
        epsca=self.getShrEpscainf(t)*self.getShrBetaast(t= t, t0= t0)
        return epsca

    def getShrEpscainf(self,t):
        '''coefficient for calculating the autogenous shrinkage strain [-]
         according to expression 3.12 of clause 3.1.4 Eurocode 2:2004 part 1-1.
         DEPRECATED in Eurocode 2:2021

        :param t: age of concrete in days at the moment considered
        '''
        epscainf=2.5*(self.fckMPa()-10)*1e-6
        return epscainf*(-1)

    def getShrBetaast(self, t, t0= 0.0):
        '''coefficient for calculating the autogenous shrinkage strain
        according to expression 3.13 of clause 3.1.4 Eurocode 2:2004 part 1-1
        and to expression B.27 of clause B.6 Eurocode 2:2021 part 1-1

        :param t: age of concrete in days at the moment considered
        :param t0: minimum age of concrete for loading (see SOFiSTiK Benchmark
                   No. 21. Real Creep and Shrinkage Calculation of a T-Beam
                   Prestressed CS page 7). 
        '''
        t= max(0,t)
        betaast=1-math.exp(-0.2*t**0.5)
        if(t0!=0.0):
            betaast-= 1-math.exp(-0.2*t0**0.5)
        return betaast

#Total shrinkage
    def getShrEpscs(self, t:float, ts:float ,RH:float, h0:float, t0= 0):
        '''Total shrinkage strain = 
        autogenous + drying shrinkages

        :param t: age of concrete in days at the moment considered
        :param ts: age of concrete in days at the beginning of drying shrinkage (or swelling) Normally this is at the end of curing
        :param RH: ambient relative humidity(%)
        :param h0: notional size of the member.

                  - h0= 2*Ac/u, where:
                  - Ac= cross sectional area
                  - u = perimeter of the member in contact with the atmosphere
        :param t0: minimum age of concrete for loading (see SOFiSTiK Benchmark
                   No. 21. Real Creep and Shrinkage Calculation of a T-Beam
                   Prestressed CS page 7). 
        '''
        epscs= self.getShrEpscd(t,ts,RH,h0)+self.getShrEpsca(t= t, t0= t0)
        return epscs

#Creep
    def getCreepFitt0(self,t,t0,RH,h0, gamma_t0= 0.3):
        '''Creep coefficient  
        (Annex B Eurocode 2 part 1-1 : 2004 - Eq. B.1)

        :param t: age of concrete in days at the moment considered
        :param t0: age of concrete in days at loading
        :param RH: ambient relative humidity(%)
        :param h0: notional size of the member in mm

                  - h0= 2*Ac/u, where:
                  - Ac= cross sectional area
                  - u = perimeter of the member in contact with the atmosphere
        :param gamma_t0: exponent of the expression (B.7) of EN 1992-1-1:2004.
                         This equation is identical to the equation 5.1-71a of
                         the ﬁb Model Code 2010 except for this exponent that
                         is computed there with equation 5.1-71b. The exponent
                         is introduced here as a parameter to allowing use the
                         expression of the Model Code 2010.
        '''
        fitt0= self.getCreepFi0(t0,RH,h0)*self.getCreepBetactt0(t,t0,RH,h0, gamma_t0= gamma_t0)
        return fitt0

    def getCreepAlfa1(self):
        '''Coefficient for the calculation of the creep coefficient
        (Annex B Eurocode 2 part 1-1 : 2004 - Eq. B.8c)
        '''
        fcmMPa=abs(self.getFcm())/1e6
        alfa1=(35/fcmMPa)**0.7
        return alfa1

    def getCreepAlfa2(self):
        '''Coefficient for the calculation of the creep coefficient
        (Annex B Eurocode 2 part 1-1 : 2004 - Eq. B.8c)
        '''
        fcmMPa=abs(self.getFcm())/1e6
        alfa2=(35/fcmMPa)**0.2
        return alfa2

    def getCreepAlfa3(self):
        '''Coefficient for the calculation of the creep coefficient
        (Annex B Eurocode 2 part 1-1 : 2004 - Eq. B.8c)
        '''
        fcmMPa=abs(self.getFcm())/1e6
        alfa3=(35/fcmMPa)**0.5
        return alfa3

    def getCreepFiRH(self,RH,h0):
        '''factor to allow for the effect of relative humidity 
        on the notional creep coefficient
        (Annex B Eurocode 2 part 1-1 : 2004 - Eq. B.2)

        :param RH:    ambient relative humidity(%)
        :param h0:  notional size of the member.
 
                  - h0= 2*Ac/u, where:
                  - Ac= cross sectional area
                  - u = perimeter of the member in contact with the atmosphere
        '''
        fcmMPa=abs(self.getFcm())/1e6
        h0mm= h0*1e3
        if fcmMPa <= 35:
            fiRH=1+(1-RH/100)/0.1/h0mm**(1/3.0)
        else:
            fiRH=(1+(1-RH/100)/0.1/h0mm**(1/3.0)*self.getCreepAlfa1())*self.getCreepAlfa2()
        return fiRH

    def getCreepBetafcm(self):
        '''factor to allow for the effect of concrete strength on the notational creep coefficient
        (Annex B Eurocode 2 part 1-1 : 2004 - Eq. B.4)
        '''
        fcmMPa=abs(self.getFcm())/1e6
        betafcm=16.8/fcmMPa**(1/2.0)
        return betafcm

    def getCreepBetat0(self,t0):
        '''factor to allow for the effect of concrete age at loading on the notational creep coefficient 
        (Annex B Eurocode 2 part 1-1 : 2004 - Eq. B.5)

        :param t0:    age of concrete in days at loading
        '''
        betat0=1/(0.1+t0**0.20)
        return betat0

    def getCreepFi0(self,t0,RH,h0):
        '''notational creep coefficient for the calculation of the creep coefficient
        (Annex B Eurocode 2 part 1-1 : 2004)

        :param t0:    age of concrete in days at loading
        :param RH:    ambient relative humidity(%)
        :param h0:  notional size of the member.

                   - h0= 2*Ac/u, where:
                   - Ac= cross sectional area
                   - u= perimeter of the member in contact with the atmosphere
        '''
        fi0=self.getCreepFiRH(RH,h0)*self.getCreepBetafcm()*self.getCreepBetat0(t0)
        return fi0

    def getCreepBetaH(self, RH, h0):
        '''coefficient coefficient depending on the relative humidity (RH) 
           and the notional member size
        (Annex B Eurocode 2 part 1-1 : 2004 - Eq. B.8a and B8b)
 
        :param RH: ambient relative humidity(%)
        :param h0: notional size of the member.
                   - h0=2*Ac/u, where:
                   - Ac= cross sectional area
                   - u = perimeter of the member in contact with the atmosphere
        '''
        fcmMPa=abs(self.getFcm())/1e6
        if fcmMPa <= 35:
            retval= min(1.5*(1+(0.012*RH)**18)*h0*1e3+250,1500) # B.8a
        else:
            retval= min(1.5*(1+(0.012*RH)**18)*h0*1e3+250*self.getCreepAlfa3(),1500*self.getCreepAlfa3()) # B.8b
        return retval

    def getCreepBetactt0(self,t,t0,RH,h0, gamma_t0= 0.3):
        '''coefficient to describe the development of creep with time after loading, used to calculate the creep coefficient
        (Annex B Eurocode 2 part 1-1 : 2004 - Eq. B.7)
 
        :param t: age of concrete in days at the moment considered.
        :param t0: age of concrete in days at loading.
        :param RH: ambient relative humidity(%)
        :param h0: notional size of the member.

                   - h0=2*Ac/u, where:
                   - Ac= cross sectional area
                   - u = perimeter of the member in contact with the atmosphere
        :param gamma_t0: exponent of the expression (B.7) of EN 1992-1-1:2004.
                         This equation is identical to the equation 5.1-71a of
                         the ﬁb Model Code 2010 except for this exponent that
                         is computed there with equation 5.1-71b. The exponent
                         is introduced here as a parameter to allowing use the
                         expression of the Model Code 2010.
        '''
        fcmMPa=abs(self.getFcm())/1e6
        betaH= self.getCreepBetaH(RH= RH, h0= h0)
        betactt0=((t-t0)/(betaH+t-t0))**gamma_t0
        return betactt0

#Parabola-rectangle diagrams for concrete under compression 
    def sigmac(self,eps):
        '''Stress [Pa][-] from parabola-rectangle diagram

        :param eps: strain [-]
        '''
        if(eps<0):
            if(eps>self.getEpsc2()):
                return self.fcd()*(1-(1-eps/self.getEpsc2())**self.getExpN())
            else:
                return self.fcd()
        else:
            return 0.0 

    def tangc(self,eps):
        ''' Tangent of the parabola-rectangle diagram

        :param eps: strain [-]
        '''
        if(eps<0):
          if(eps>self.getEpsc2()):
            return self.fcd()*self.getExpN()*(1-eps/self.getEpsc2())**(self.getExpN()-1)*(1/self.getEpsc2())
          else:
            return 0.0
        else:
          return 0.0

    def E0(self):
        ''' Tangent in the origin point of concrete01 and concrete02 diagrams
        '''
        return 2*self.fmaxK()/self.epsilon0()
        
    def plotDesignStressStrainDiagram(self, preprocessor, path=''):
        '''Draws the concrete design diagram.

        :param preprocessor: pre-processor of the FE problem.
        :param path: path to store the graphic output file.
        '''
        if self.materialDiagramD is None:
            self.defDiagD(preprocessor) # Define stress-strain design diagram.
        title= self.getDesignStressStrainDiagramTitle()
        if self.tensionStiffparam is None:
            retval= graph_material.UniaxialMaterialDiagramGraphic(epsMin= self.epsilonU(), epsMax=0, title= title)
        else:
            retval= graph_material.UniaxialMaterialDiagramGraphic(epsMin= self.epsilonU(), epsMax=20*self.fctd()/self.E0(), title= title)
        retval.setupGraphic(plt, self.materialDiagramD)
        outputFileName= self.getDesignStressStrainDiagramFileName(path= path, withExtension= True)
        retval.savefig(plt, outputFileName)
        return retval
    
    def getElasticMaterialData(self, overrideRho= None):
        '''Return the elastic material constitutive model.

        :param overrideRho: if defined (not None), override the value of 
                            the material density.
        '''
        materialModelName= self.materialName + 'ElasticMaterialData'
        rho= self.density()
        if(overrideRho):
            rho= overrideRho
        return typical_materials.MaterialData(name= materialModelName, E= self.getEcm(), nu= self.nuc, rho= rho)

    def defElasticMaterial(self, preprocessor, name:str= None, overrideRho= None):
        '''Constructs an elastic uniaxial material appropriate 
         for analysis of trusses.

        :param preprocessor: preprocessor of the finite element problem.
        :param name: name for the new material.
        :param overrideRho: if defined (not None), override the value of 
                            the material density.
        '''
        if(name is None):
            name= self.materialName+'_uniaxial'
        rho= self.density()
        if(overrideRho):
            rho= overrideRho
        return typical_materials.defElasticMaterial(preprocessor, name, E= self.getEcm(), rho= rho)

    def defElasticNoTensMaterial(self, preprocessor, name:str= None, a= 0.0, b= 1.0, overrideRho= None):
        '''Constructs a no tension elastic uniaxial material appropriate 
         for analysis of trusses.

        :param preprocessor: preprocessor of the finite element problem.
        :param name: name for the new material.
        :param a: parameter to define the tension behaviour.
        :param b: parameter to define the tension behaviour.
        :param overrideRho: if defined (not None), override the value of 
                            the material density.
        '''
        if(name is None):
            name= self.materialName+'_uniaxial'
        rho= self.density()
        if(overrideRho):
            rho= overrideRho
        return typical_materials.defElastNoTensMaterial(preprocessor= preprocessor, name= name, E= self.getEcm(), a= a, b= b, overrideRho= rho)
            
    def defElasticSection2d(self, preprocessor, sectionProperties, overrideRho= None):
        '''Constructs an elastic section material appropriate 
         for elastic analysis of 2D beam elements.

         :param preprocessor: preprocessor of the finite element problem.
         :param sectionProperties: mass properties of the section.
         :param overrideRho: if defined (not None), override the value of 
                            the material density.
         '''
        matData= self.getElasticMaterialData(overrideRho= overrideRho)
        return sectionProperties.defElasticSection2d(preprocessor, matData, overrideRho= overrideRho)
        
    def defElasticShearSection2d(self, preprocessor, sectionProperties, overrideRho= None):
        '''Constructs an elastic section material appropriate 
         for elastic analysis of 2D beam elements including shear
         deformations.

         :param preprocessor: preprocessor of the finite element problem.
         :param sectionProperties: mass properties of the section.
         :param overrideRho: if defined (not None), override the value of 
                            the material density.
         '''
        matData= self.getElasticMaterialData(overrideRho= overrideRho)
        return sectionProperties.defElasticShearSection2d(preprocessor, matData,overrideRho= overrideRho)

    def defElasticSection3d(self, preprocessor, sectionProperties, overrideRho= None):
        '''Constructs an elastic section material appropriate 
         for elastic analysis of 3D beam elements.

         :param preprocessor: preprocessor of the finite element problem.
         :param sectionProperties: mass properties of the section.
         :param overrideRho: if defined (not None), override the value of 
                            the material density.
         '''
        matData= self.getElasticMaterialData(overrideRho= overrideRho)
        return sectionProperties.defElasticSection3d(preprocessor, matData, overrideRho= overrideRho)

    def defElasticShearSection3d(self, preprocessor, sectionProperties, overrideRho= None):
        '''Constructs an elastic section material appropriate 
         for elastic analysis of 3D beam elements including shear
         deformations.

         :param preprocessor: preprocessor of the finite element problem.
         :param sectionProperties: mass properties of the section.
         :param overrideRho: if defined (not None), override the value of 
                            the material density.
         '''
        matData= self.getElasticMaterialData(overrideRho= overrideRho)
        return sectionProperties.defElasticShearSection3d(preprocessor,matData, overrideRho= overrideRho) 
    
    def defElasticPlateSection(self, preprocessor, name, thickness, overrideRho= None):
        '''Constructs an elastic isotropic section material appropriate 
           for elastic analysis of plate elements.

        :param preprocessor: preprocessor of the finite element problem.
        :param name: name identifying the new section.
        :param thickness: section thickness.
        :param overrideRho: if defined (not None), override the value of 
                            the material density.
        '''
        retval= None
        materialHandler= preprocessor.getMaterialHandler
        if(materialHandler.materialExists(name)):
            className= type(self).__name__
            methodName= sys._getframe(0).f_code.co_name
            lmsg.warning(className+'.'+methodName+'; section: '+name+' already defined.')
            retval= materialHandler.getMaterial(name)
        else:
            rho= self.density()
            if(overrideRho):
                rho= overrideRho
            retval= typical_materials.defElasticPlateSection(preprocessor, name,E= self.getEcm(), nu=self.nuc , rho= rho, h= thickness)
        return retval

    def defElasticMembranePlateSection(self, preprocessor, name, thickness, overrideRho= None):
        '''Constructs an elastic isotropic section material appropriate 
           for elastic analysis of plate and shell elements

        :param preprocessor: preprocessor of the finite element problem.
        :param name: name identifying the section
        :param thickness: section thickness.
        :param overrideRho: if defined (not None), override the value of 
                            the material density.
        '''
        retval= None
        materialHandler= preprocessor.getMaterialHandler
        if(materialHandler.materialExists(name)):
            className= type(self).__name__
            methodName= sys._getframe(0).f_code.co_name
            lmsg.warning(className+'.'+methodName+'; section: '+name+' already defined.')
            retval= materialHandler.getMaterial(name)
        else:
            rho= self.density()
            if(overrideRho):
                rho= overrideRho
            retval= typical_materials.defElasticMembranePlateSection(preprocessor,name,E= self.getEcm(), nu=self.nuc ,rho= rho, h= thickness)
        return retval

    def defElasticIsotropicPlaneStrain(self, preprocessor, name:str= None, overrideRho= None):
        '''Constructs an elastic uniaxial material appropriate 
         for analysis of trusses.

        :param preprocessor: preprocessor of the finite element problem.
        :param name: name for the new material.
        :param overrideRho: if defined (not None), override the value of 
                            the material density.
        '''
        if(name is None):
            name= self.materialName+'_elastic_isotropic_plane_strain'
        rho= self.density()
        if(overrideRho):
            rho= overrideRho
        return typical_materials.defElasticIsotropicPlaneStrain(preprocessor, name= name, E= self.getEcm(), nu= self.nuc, rho= rho)
    
    def defElasticIsotropicPlaneStress(self, preprocessor, name:str= None, overrideRho= None):
        ''' Defines an elastic isotropic plane stress material.

        :param preprocessor: pre-processor for the finite element problem.
        :param name: name for the new material.
        :param overrideRho: if defined (not None), override the value of 
                            the material density.
        '''
        if(name is None):
            name= self.materialName+'_elastic_isotropic_plane_stress'
        rho= self.density()
        if(overrideRho):
            rho= overrideRho
        return typical_materials.defElasticIsotropicPlaneStress(preprocessor, name= name, E= self.getEcm(), nu= self.nuc, rho= rho)

class paramTensStiffness(object):
    '''Parameters to generate a concrete02 material based on the tension-stiffening constitutive 
       model of concrete proposed by Stramandinoli & La Rovere 
       (ref. article: Engineering Structures 30 (2008) 2069-2080).

    :ivar concrMat: concrete material of the RC section
    :ivar reinfMat: reinforcing steel material of the RC section 
    :ivar reinfRatio: section reinforcement ratio, obtained as:
        -ro=As/Ac for members subjected to direct tension.
        -roef=As/Acef for members under bending, where Acef is the effective
        area, that corresponds to the tensile zone in the member section.
        Aef can be obtained as (see CEB-FIP MC-90) Aef = 2.5b(h-d) < b(h-x)/3, where x is 
        the neutral axis depth. Aef can be estimated as Aef~=bh/4 
    :ivar nPoints: number of (strain,stress) pairs of values to approximate the exponential
      decay curve adopted for the post-cracking range (defaults to 50)
    :ivar diagType: type of diagram: 'K' or 'k' for characteristic, 'D' or 'd' for design
      (defaults to characteristic)
    :ivar E_c: concrete elastic modulus [Pa] [+] (defaults to concrMat.Ecm())
    :ivar f_ct: concrete tensile strength [Pa][+] (defaults to concrMat.fctd() or concrMat.fctk())
    :ivar E_ct: concrete elastic modulus [Pa] [+] in the tensile linear-elastic 
      range (defaults to concrMat.Ect())
    :ivar E_s: steel elastic modulus [Pa] [+] (defaults to reinfMat.Es)
    :ivar eps_y: reinforcing steel strain at yield point [Pa][+]
      (defaults to reinfMat.eyd() or reinfMat.eyk())
    '''

    def __init__(self,concrMat,reinfMat,reinfRatio,diagType):
        self.concrMat=concrMat
        self.reinfMat=reinfMat
        self.ro=reinfRatio
        self.diagType=diagType
        self.nPoints=50
        self.E_c=concrMat.Ecm()
        if self.diagType in ['d','D']:
            self.f_ct=self.concrMat.fctd()
        else:
            self.f_ct=self.concrMat.fctk()
        self.E_ct=self.concrMat.Ect()       
        self.E_s=self.reinfMat.Es
        '''Reinforcing steel strain at yield point  [Pa][+]'''
        if self.diagType in ['d','D']:
            self.eps_y=self.reinfMat.eyd()
        else:
            self.eps_y=self.reinfMat.eyk()

    def nu(self):
        '''return steel-to-concrete elastic modulus ratio nu = Es/Ec
        '''
        retval=self.E_s/self.E_c
        return retval

    def alfa(self):
        '''returns the parameter for the exponential decay curve adopted for the post-cracking range, 
        until yielding of reinforcement takes place
        '''
        retval=0.017+0.255*self.nu()*self.ro-0.106*(self.nu()*self.ro)**2+0.016*(self.nu()*self.ro)**3
        return retval

    def ptosExpCurvPostCracking(self):
        '''return a list of strains and the list of their corresponding stresses in exponential 
        decay curve adopted for the post-cracking range, until yielding of reinforcement takes place

        '''
        eps_cr=self.f_ct/self.E_ct           #strain corresponding to concrete tensile strength
        incr=(self.eps_y-eps_cr)/(self.nPoints)
        strainPts=np.arange(eps_cr,self.eps_y+incr/2,incr).tolist()
        stressPts=list()
        for eps in strainPts:
            stress=self.f_ct*math.exp(-self.alfa()*eps/eps_cr)
            stressPts.append(stress)
        return {'strainPts':strainPts , 'stressPts':stressPts }

    def regresLine(self):
        '''return the slope [a] and interceptY [b]  (the value of y when x=0) of the regression line y=ax+b 
        that approaches the exponential decay curve adopted for the post-cracking range. It also returns interceptX
        (the value of x when y=0)
 
        '''
        strainPts=self.ptosExpCurvPostCracking()['strainPts']
        stressPts=self.ptosExpCurvPostCracking()['stressPts']
        rline=stats.linregress(strainPts,stressPts)
        return {'slope': rline.slope, 'interceptY':rline.intercept, 'interceptX': -rline.intercept/rline.slope}

    def pointOnsetCracking(self):
        '''return the calculated (strain,stress) point of the concrete diagram where cracking starts. It's obtained as 
        the intersection of the straight line thas reproduces the linear-elastic range in tension (before cracking) with
        the regression line  that approaches the exponential decay curve adopted for the post-cracking range
        '''
        rgLin=self.regresLine()
        r1=geom.Line2d(geom.Pos2d(rgLin['interceptX'],0.),geom.Pos2d(0,rgLin['interceptY']))
        r2=geom.Line2d(geom.Pos2d(0.,0.),geom.Pos2d(self.f_ct/self.E_ct,self.f_ct))
        pInt=r1.getIntersection(r2)[0]
        return {'eps_ct':pInt.x, 'ft':pInt.y}

    def slopeRegresLineFixedPoint(self):
        '''return the slope of the regression line that approaches the exponential decay curve adopted for the 
        post-cracking range, passings through the point (eps_ct,f_ct) [point of concrete onset cracking]
 
        '''
        strainPts=np.array(self.ptosExpCurvPostCracking()['strainPts'])
        stressPts=np.array(self.ptosExpCurvPostCracking()['stressPts'])
        x=strainPts-strainPts[0]
        y=stressPts-stressPts[0]
        x = x[:,np.newaxis]
        slope, _, _, _ = np.linalg.lstsq(x, y)
        return slope[0]

class CreepParameters(object):
    ''' Parameters that define concrete creep for TD concrete materials.

    :ivar beta: tension softening parameter.
    :ivar age: analysis time at initiation of drying (in days).
    :ivar epsba: ultimate basic shrinkage strain, εcbs,0, as per Model Code 2010.
    :ivar epsbb: fitting parameter within the basic shrinkage time evolution function as per Model Code 2010 and prEN1992-1-1:2017.
    :ivar epsda: product of εcds,0 and βRH, as per Model Code 2010.
    :ivar epsdb: fitting parameter within the drying shrinkage time evolution function as per Model Code 2010 and prEN1992-1-1:2017.
    :ivar phiba: parameter for the effect of compressive strength on basic creep βbc(fcm), as per Model Code 2010.
    :ivar phibb: fitting parameter within the basic creep time evolution function as per Model Code 2010 and prEN1992-1-1:2017.
    :ivar phida: product of βdc(fcm) and β(RH), as per Model Code 2010.
    :ivar phidb: fitting constant within the drying creep time evolution function as per Model Code 2010.
    :ivar tcast: analysis time corresponding to concrete casting in days (note: concrete will not be able to take on loads until the age of 2 days).
    :ivar cem: coefficient dependent on the type of cement: –1 for 32.5N, 0 for 32.5R and 42.5N and 1 for 42.5R, 52.5N and 52.5R.
    '''
    def __init__(self,  beta, age, epsba, epsbb, epsda, epsdb, phiba, phibb, phida, phidb, tcast, cem):
        '''
        Constructor.

        :param beta: tension softening parameter.
        :param age: analysis time at initiation of drying (in days).
        :param epsba: ultimate basic shrinkage strain, εcbs,0, as per Model Code 2010.
        :param epsbb: fitting parameter within the basic shrinkage time evolution function as per Model Code 2010 and prEN1992-1-1:2017.
        :param epsda: product of εcds,0 and βRH, as per Model Code 2010.
        :param epsdb: fitting parameter within the drying shrinkage time evolution function as per Model Code 2010 and prEN1992-1-1:2017.
        :param phiba: parameter for the effect of compressive strength on basic creep βbc(fcm), as per Model Code 2010.
        :param phibb: fitting parameter within the basic creep time evolution function as per Model Code 2010 and prEN1992-1-1:2017.
        :param phida: product of βdc(fcm) and β(RH), as per Model Code 2010.
        :param phidb: fitting constant within the drying creep time evolution function as per Model Code 2010.
        :param tcast: analysis time corresponding to concrete casting in days (note: concrete will not be able to take on loads until the age of 2 days).
        :param cem: coefficient dependent on the type of cement: –1 for 32.5N, 0 for 32.5R and 42.5N and 1 for 42.5R, 52.5N and 52.5R.
        '''
        self.beta= beta # tension softening parameter.
        self.age= age # analysis time at initiation of drying (in days).
        
        self.epsba= epsba # ultimate basic shrinkage strain, εcbs,0, as per Model Code 2010.
        self.epsbb= epsbb # fitting parameter within the basic shrinkage time evolution function as per Model Code 2010 and prEN1992-1-1:2017.
        self.epsda= epsda # product of εcds,0 and βRH, as per Model Code 2010.
        self.epsdb= epsdb # fitting parameter within the drying shrinkage time evolution function as per Model Code 2010 and prEN1992-1-1:2017.
        
        self.phiba= phiba # parameter for the effect of compressive strength on basic creep βbc(fcm), as per Model Code 2010.
        self.phibb= phibb # fitting parameter within the basic creep time evolution function as per Model Code 2010 and prEN1992-1-1:2017.
        self.phida= phida # product of βdc(fcm) and β(RH), as per Model Code 2010.
        self.phidb= phidb # fitting constant within the drying creep time evolution function as per Model Code 2010.
        
        self.tcast= tcast # analysis time corresponding to concrete casting in days (note: concrete will not be able to take on loads until the age of 2 days).
        self.cem= cem # coefficient dependent on the type of cement: –1 for 32.5N, 0 for 32.5R and 42.5N and 1 for 42.5R, 52.5N and 52.5R.
        
    
def defDiagKConcrete(preprocessor, concreteRecord):
    ''' Define concrete stress-strain characteristic diagram.

    :param preprocessor: pre-processor of the finite element model.
    :param concreteRecord: concrete material data.
    '''
    methodName= sys._getframe(0).f_code.co_name
    lmsg.warning(methodName+'; deprecated: use concreteRecord.defDiagK(preproccesor)')
    return concreteRecord.defDiagK(preprocessor)

def defDiagDConcrete(preprocessor, concreteRecord):
    ''' Define concrete stress-strain design diagram.

    :param preprocessor: pre-processor of the finite element model.
    :param concreteRecord: concrete material data.
    '''
    methodName= sys._getframe(0).f_code.co_name
    lmsg.warning(methodName+'; deprecated: use concreteRecord.defDiagK(preproccesor)')
    return concreteRecord.defDiagD(preprocessor)

def concreteDesignDiagramTest(preprocessor, concreteRecord):
    '''Calculates the differece between the stresses obtained from the
       Concrete01 uniaxial material (getStress() ) and the 
       theoretical law defined in Python (see sigmac()).

    :param preprocessor: pre-processor of the finite element model.
    :param concreteRecord: concrete data.
    '''
    diagTag= concreteRecord.defDiagD(preprocessor) # Define concrete stress-strain design diagram.
    if(__debug__):
        if(not diagTag):
            AssertionError('Can\'t create the diagram.')        

    diagConcrete= preprocessor.getMaterialHandler.getMaterial(concreteRecord.nmbDiagD)
    incr= concreteRecord.epsilonU()/20
    errMax= 0.0
    e=  -0.1e-8
    while(e>=concreteRecord.epsilonU()):
        diagConcrete.setTrialStrain(e,0.0)
        diagConcrete.commitState()
  #      sg= sigmaDConcrete(e,concreteRecord)
        sg=concreteRecord.sigmac(e)
        stress= diagConcrete.getStress()
        err= abs((sg-stress)/sg)
        errMax= max(err,errMax)
        e=(e+incr)
    return errMax

def concreteDesignTangentTest(preprocessor, concreteRecord):
    ''' Calculates the differece between the values of the tangent obtained
     from de Concrete01 uniaxial material (getTangent() ) and the 
     theoretical law defined in Python (see tangc()).

    :param preprocessor: pre-processor of the finite element model.
    :param concreteRecord: concrete data.
    '''
    # Define concrete stress-strain design diagram.
    diagTag= concreteRecord.defDiagD(preprocessor)
    if(__debug__):
        if(not diagTag):
            AssertionError('Can\'t create the diagram.')        
    diagConcrete= preprocessor.getMaterialHandler.getMaterial(concreteRecord.nmbDiagD)
    incr= concreteRecord.epsilonU()/20
    errMax= 0.0
    e=-0.1e-8
    while(e>=concreteRecord.epsilonU()):
        diagConcrete.setTrialStrain(e,0.0)
        diagConcrete.commitState()
  #      tg= concreteRecord.tangDConcrete(e)
        tg=concreteRecord.tangc(e)
        tgDiag= diagConcrete.getTangent()
        if(tg!=0.0):
          err= abs((tg-tgDiag)/tg)
        else:
          err= abs(tg-tgDiag)
        errMax= max(err,errMax)
        e=(e+incr)
    return errMax

# Reinforcing steel.

class ReinforcingSteel(matWDKD.MaterialWithDKDiagrams):
    '''Reinforcing steel parameters 

      :ivar fyk:      Characteristic value of the yield strength.
      :ivar emax:     maximum strain in tension
      :ivar gammaS:   Partial factor for material.
      :ivar k:        fmaxk/fyk ratio (Annex C of EC2: class A k>=1,05,
                      class B k>=1,08)
    '''
    Es= 2e11 # Elastic modulus of the material.
    k=1.05   # fmaxk/fyk ratio (Annex C of EC2: class A k>=1,05 B , class B k>=1,08)
    rho= 7850 # kg/m3
    def __init__(self, steelName= None, fyk= 0.0, emax= 0.0, gammaS= 1.15, k=1.05):
        ''' Constructor.

        :param steelName: identifier for the steel material.
        :param fyk: characteristic value of the steel yield stress.
        :param emax:     maximum strain in tension
        :param gammaS:   Partial factor for material.
        :param k:        fmaxk/fyk ratio (Annex C of EC2: class A k>=1,05,
                         class B k>=1,08)
        '''
        super(ReinforcingSteel,self).__init__(steelName)
        self.fyk= fyk # Characteristic value of the yield strength
        self.gammaS= gammaS
        self.emax= emax # Ultimate strain (rupture strain)
        self.k=k        # fmaxk/fyk ratio

    def getDict(self):
        ''' Return a dictionary with the object values.'''
        retval= super().getDict()
        retval.update({'fyk':self.fyk, 'gamma_s':self.gammaS, 'e_max':self.emax, 'k': self.k})
        return retval

    def setFromDict(self, dct):
        ''' Set the member values from those in the given dictionary.'''
        super().setFromDict(dct)
        self.fyk= dct['fyk']
        self.gammaS= dct['gamma_s']
        self.emax= dct['e_max']
        self.k= dct['k']

    def __eq__(self, other):
        '''Overrides the default implementation'''
        if(other is not None):
            if(self is not other):
                retval= super().__eq__(other= other)
                if(retval):
                    retval= (self.fyk==other.fyk)
                if(retval):
                    retval= (self.gammaS==other.gammaS)
                if(retval):
                    retval= (self.emax==other.emax)
                if(retval):
                    retval= (self.k==other.k)
            else:
                retval= True
        else:
            retval= False
        return retval
    
    def fmaxk(self):
        ''' Characteristic ultimate strength. '''
        return self.k*self.fyk
    
    def fyd(self):
        ''' Design yield stress. '''
        return self.fyk/self.gammaS
    
    def eyk(self):
        ''' Caracteristic strain at yield point. '''
        return self.fyk/self.Es
    
    def eyd(self):
        ''' Design strain at yield point. '''
        return self.fyd()/self.Es

    def epsilon_ud(self):
        ''' Return the steel strain limit.'''
        retval= 0.9*self.emax
        return retval
    
    def Esh(self):
        ''' Slope of the curve in the yielding region. '''
        return (self.fmaxk()-self.fyk)/(self.emax-self.eyk())
    
    def bsh(self):
        ''' Ratio between post-yield tangent and initial elastic tangent. '''
        return self.Esh()/self.Es

    def getStressK(self, eps):
        ''' Return the stress corresponding to the characterístic diagram.

        :param eps: deformation.
        '''
        return sigmas(eps, fy= self.fyk, ey= self.eyk(), Es= self.Es, Esh= self.Esh())

    def getStressD(self, eps):
        ''' Return the stress corresponding to the characterístic diagram.

        :param eps: deformation.
        '''
        return sigmas(eps, fy= self.fyd(), ey= self.eyd(), Es= self.Es, Esh= self.Esh())
    
    def defDiagK(self, preprocessor):
        ''' Returns an XC uniaxial material  corresponding to the characteristic
            values of the stress-strain diagram of the reinforcing steel.

        :param preprocessor: pre-processor for the finite element problem.
        '''
        self.materialDiagramK= typical_materials.defSteel01(preprocessor, self.nmbDiagK,self.Es,self.fyk,self.bsh())
        self.matTagK= self.materialDiagramK.tag
        return self.materialDiagramK #30160925 was 'return self.matTagK'
    
    def clearDiagK(self):
        ''' Clear previously defined diagram.'''
        if(self.materialDiagramK):
            self.matTagK= -1
            self.materialDiagramK= None

    def defDiagD(self, preprocessor):
        ''' Returns and XC uniaxial material corresponding to the design values
            of the stress-strain diagram of the reinforcing steel.

        :param preprocessor: pre-processor for the finite element problem.
        '''
        self.materialDiagramD= typical_materials.defSteel01(preprocessor, self.nmbDiagD,self.Es,self.fyd(),self.bsh())
        self.matTagD= self.materialDiagramD.tag
        return self.materialDiagramD #30160925 was 'return self.matTagD'
    
    def clearDiagD(self):
        ''' Clear previously defined diagram.'''
        if(self.materialDiagramD):
            self.matTagD= -1
            self.materialDiagramD= None

    def defDiagE(self, preprocessor):
        ''' Returns and XC linear elastic uniaxial material.

        :param preprocessor: pre-processor for the finite element problem.
        '''
        self.materialDiagramE= typical_materials.defElasticMaterial(preprocessor= preprocessor, name= self.nmbDiagE, E= self.Es, rho= self.rho)
        self.matTagE= self.materialDiagramE.tag
        return self.materialDiagramE
    
    def clearDiagE(self):
        ''' Clear previously defined diagram.'''
        if(self.materialDiagramE):
            self.matTagE= -1
            self.materialDiagramE= None
        
    def defDiag(self, preprocessor, matDiagType):
        ''' Returns an XC uniaxial material corresponding to the stress-strain
            diagram of the reinforcing steel.

        :param preprocessor: pre-processor for the finite element problem.
        :param matDiagType: diagram type; if "k" return the diagram 
                            corresponding to characteristic values of the 
                            material, if "d" return the design values one.
        '''
        retval= None
        if matDiagType.lower() =='d':
            if(self.materialDiagramD): # if already defined.
                retval= self.materialDiagramD
            else: # if not defined yet, do it now.
                retval= self.defDiagD(preprocessor)
        elif matDiagType.lower() =='k':
            if(self.materialDiagramK): # if already defined.
                retval= self.materialDiagramK
            else: # if not defined yet, do it now.
                retval= self.defDiagK(preprocessor)
        elif(matDiagType=='elastic'):
            if(self.materialDiagramE): # if already defined.
                retval= self.materialDiagramE
            else: # if not defined yet, do it now.
                retval= self.defDiagE(preprocessor)
        else:
            className= type(self).__name__
            methodName= sys._getframe(0).f_code.co_name
            lmsg.error(className+'.'+methodName+'; diagram types : '+str(matDiagType)+' is not known.')
            exit(1)
        return retval

    def clearDiagrams(self):
        ''' Clear the previously defined diagrams.'''
        self.clearDiagD()
        self.clearDiagK()
        self.clearDiagE()

    def plotDesignStressStrainDiagram(self, preprocessor, path=''):
        '''Draws the steel design diagram.

        :param preprocessor: pre-processor of the FE problem.
        :param path: path to store the graphic output file.
        '''
        if self.materialDiagramD is None:
            self.defDiagD(preprocessor)
        title= self.getDesignStressStrainDiagramTitle()
        retval= mg.UniaxialMaterialDiagramGraphic(-0.016,0.016, title)
        retval.setupGraphic(plt,self.materialDiagramD)
        outputFileName= self.getDesignStressStrainDiagramFileName(path= path, withExtension= False)
        retval.savefig(plt, outputFileName+'.png')
        retval.savefig(plt, outputFileName+'.eps')
        return retval
    
    def defElasticMaterial(self, preprocessor, name= None, overrideRho= None, initStrain= 0.0):
        ''' Return an elastic uniaxial material appropriate for example for
            truss elements

        :param preprocessor: preprocessor of the finite element problem.
        :param name: name for the new material (if None compute a suitable name).
        :param overrideRho: if defined (not None), override the value of 
                            the material density.
        :param initStrain: initial strain.
        '''        
        materialHandler= preprocessor.getMaterialHandler
        matName= name
        if(not matName):
            matName= self.materialName+'_xc_material'
        retval= materialHandler.newMaterial("elastic_material", matName)
        retval.E= self.Es
        matRho= self.rho
        if(overrideRho):
            matRho= overrideRho
        retval.rho= matRho
        if(initStrain!=0.0):
            retval.initialStrain= initStrain
        return retval
    
    def defElasticNoCompressionMaterial(self, preprocessor, name:str= None, a= 0.0, b= 1.0):
        '''Constructs a no tension elastic uniaxial material appropriate 
         for analysis of trusses.

        :param preprocessor: preprocessor of the finite element problem.
        :param a: parameter to define the compresion behaviour.
        :param b: parameter to define the compresion behaviour.
        :param name: name for the new material.
        '''
        if(name is None):
            name= self.materialName+'_uniaxial'
        return typical_materials.defElastNoCompressionMaterial(preprocessor= preprocessor, name= name, E= self.Es, a= a, b= b)

def defReinfSteelCharacteristicDiagram(preprocessor, steelRecord):
    '''Characteristic stress-strain diagram.'''
    return steelRecord.defDiagK(preprocessor)

def defReinfSteelDesignDiagram(preprocessor, steelRecord):
    ''' Design stress-strain diagram. '''
    return steelRecord.defDiagD(preprocessor)

# Functions for reinforcing steel testing.

def sigmas(eps, fy, ey, Es, Esh):
    '''Stress-strain diagram of reinforcing steel, according to EC2 
       (the same one is adopted by EHE and SIA).

    :param eps: steel strain.
    :param fy: steel yield tensile strength.
    :param ey: steel strain at yield (ey= fyd/Es).
    :param Es: modulus of elasticity of the reinforcing steel.
    :param Esh: slope of the inclined top branch of the diagram.
    '''
    if(eps>0):
        if(eps<ey):
            return (Es*eps)
        else:
            return (fy+(eps-ey)*Esh)
    else:
        if(eps>-(ey)):
            return (Es*eps)
        else:
            return (-fy+(eps-ey)*Esh) 


def sigmaKReinfSteel(eps,matRecord):
    ''' Characteristic stress-strain diagram for reinforcing steel, 
        according to EC2.

    :param eps: steel strain.
    :param matRecord: material data.
    '''
    return sigmas(eps,matRecord.fyk,matRecord.eyk(),matRecord.Es,matRecord.Esh())

def sigmaDReinfSteel(eps,matRecord):
    '''Design stress-strain diagram for reinforcing steel, 
       according to EC2.

    :param eps: steel strain.
    :param matRecord: material data.
    '''
    return sigmas(eps,matRecord.fyd(),matRecord.eyd(),matRecord.Es,matRecord.Esh())

def testReinfSteelCharacteristicDiagram(preprocessor, matRecord):
    ''' Checking of characteristic stress-strain diagram.

    :param preprocessor: pre-processor of the finite element model.
    :param matRecord: material data.
    '''
    steelDiagram= defReinfSteelCharacteristicDiagram(preprocessor, matRecord)
    ##30160925 was:
    #  tag= defReinfSteelCharacteristicDiagram(preprocessor, matRecord)
    #  steelDiagram= preprocessor.getMaterialHandler.getMaterial(matRecord.nmbDiagK)
    incr= matRecord.emax/20
    errMax= 0.0
    e= 0.1e-8
    while(e < matRecord.emax+1):
        steelDiagram.setTrialStrain(e,0.0)
        steelDiagram.commitState()
        sg= sigmaKReinfSteel(e,matRecord)
        stress= steelDiagram.getStress()
        err= abs((sg-stress)/sg)
        errMax= max(err,errMax)
        e= e+incr
    return errMax

def testReinfSteelDesignDiagram(preprocessor, matRecord):
    '''Checking of design stress-strain diagram.

    :param preprocessor: pre-processor of the finite element model.
    :param matRecord: material data.
    '''
    steelDiagram= defReinfSteelDesignDiagram(preprocessor, matRecord)
    ##30160925 was:
    #  tag= defReinfSteelDesignDiagram(preprocessor, matRecord)
    #  steelDiagram= preprocessor.getMaterialHandler.getMaterial(matRecord.nmbDiagD)
    incr= matRecord.emax/20
    errMax= 0.0
    e= 0.1e-8
    while(e < matRecord.emax+1):
        steelDiagram.setTrialStrain(e,0.0)
        steelDiagram.commitState()
        sg= sigmaDReinfSteel(e,matRecord)
        err= abs((sg-steelDiagram.getStress())/sg)
        errMax= max(err,errMax)
        e= e+incr
    return errMax

#       ___            _                 _           
#      | _ \_ _ ___ __| |_ _ _ ___ _____(_)_ _  __ _ 
#      |  _/ '_/ -_|_-<  _| '_/ -_|_-<_-< | ' \/ _` |
#      |_| |_| \___/__/\__|_| \___/__/__/_|_||_\__, |
#       __| |_ ___ ___| |                      |___/ 
#      (_-<  _/ -_) -_) |                            
#      /__/\__\___\___|_|                            
# ************* Prestressing steel. ********************
class PrestressingSteel(matWDKD.MaterialWithDKDiagrams):
    '''Prestressing steel parameters 

    :ivar fpk: Elastic limit.
    :ivar fmax: Tenslile strength.
    :ivar steelRelaxationClass: Relaxation class 1: normal, 2: improved, 
                                 and 3: relaxation for bars.
    :ivar tendonClass: Tendon class wire, strand or bar.
    '''
    def __init__(self, steelName, fpk,fmax= 1860e6, alpha= 0.75, steelRelaxationClass=1, tendonClass= 'strand', Es= 190e9):
        ''' Prestressing steel base class.

           :param fpk: Elastic limit.
           :param fmax: Tensile strength.
           :param alpha: initial stress-to-strength ratio.
           :param steelRelaxationClass: Relaxation class 1: normal, 2: improved, 
                                        and 3: relaxation for bars.
           :param tendonClass: Tendon class wire, strand or bar.
           :param Es: elastic modulus.
        '''
        super(PrestressingSteel,self).__init__(steelName)
        self.gammaS= 1.15 # partial safety factor for steel.
        self.fpk= fpk # elastic limit.
        self.fmax= fmax # Tensile strength.
        self.Es= Es # elastic modulus.
        self.bsh= 0.001 # slope ratio (yield branch/elastic branch)
        self.steelRelaxationClass= steelRelaxationClass
        self.tendonClass= tendonClass
    
    def fpd(self):
        ''' Return the design value of the yield stress.'''
        return self.fpk/self.gammaS

    def getUltimateStress(self):
        ''' Return steel ultimate stress.'''
        return self.fmax
  
    def getDesignUltimateStress(self):
        ''' Return the desing value of the steel ultimate stress.'''
        return self.fmax/self.gammaS
    
    def defDiagK(self,preprocessor,initialStress):
        '''Characteristic stress-strain diagram.'''
        self.materialDiagramK= typical_materials.defSteel02(preprocessor,self.nmbDiagK,self.Es,self.fpk,self.bsh,initialStress)
        self.matTagK= self.materialDiagramK.tag
        return self.materialDiagramK
  
    def defDiagD(self,preprocessor,initialStress):
        '''Design stress-strain diagram.'''
        self.materialDiagramD= typical_materials.defSteel02(preprocessor,self.nmbDiagD,self.Es,self.fpd(),self.bsh,initialStress)
        self.matTagD= self.materialDiagramD.tag
        return self.materialDiagramD


class CEB_EHE_PrestressingSteel(PrestressingSteel):
    ''' Prestressing steel base class for EHE and CEB standards.

    :ivar alpha: stress-to-strength ratio.
    '''
    # Points from the table 38.9.b of EHE-08 to determine
    # relaxation at times shorter than 1000 hours.
    ptsShortTermRelaxation= scipy.interpolate.interp1d([0, 1, 5, 20, 100, 200, 500, 1000],[0, 0.25, 0.45, 0.55, 0.7, 0.8, 0.9, 1])
    
    def __init__(self, steelName, fpk, fmax= 1860e6, alpha= 0.75, steelRelaxationClass= 1, tendonClass= 'strand', Es= 190e9):
        ''' Prestressing steel base class.

           :param steelName: steel name.
           :param fpk: Elastic limit.
           :param fmax: Steel strength.
           :param alpha: stress-to-strength ratio.
           :param steelRelaxationClass: Relaxation class 1: normal, 
                                        2: improved, 
                                        and 3: relaxation for bars.
           :param tendonClass: tendon class: wire, strand or bar.
           :param Es: elastic modulus.
        '''
    
        super(CEB_EHE_PrestressingSteel,self).__init__(steelName= steelName, fpk= fpk, fmax= fmax, alpha= alpha, steelRelaxationClass= steelRelaxationClass, tendonClass= tendonClass, Es= Es)
        self.alpha= alpha # initial stress-to-strength ratio.
    
    def getKRelaxation(self):
        ''' Return the value of k factor for the relaxation expression
           from the relaxation class. See Model Code 1990 paragraph 2.3.4.5.
        '''
        if(self.steelRelaxationClass==1):
            return 0.12 
        elif(self.steelRelaxationClass==2):
            return 0.19 
        else:
            className= type(self).__name__
            methodName= sys._getframe(0).f_code.co_name
            lmsg.warning(className+'.'+methodName+'; relaxation class : '+str(self.steelRelaxationClass)+' not implemented.')
            return 0
        
    def tInic(self):
        return self.alpha**2*self.fmax # Final presstressing (initial at 75 percent  and 25 percent of total losses).

def createInteractionDiagram(materialHandler, concreteDiagram, steelDiagram, concreteSection):
    ''' Return the three-dimensional interaction Diagram for the section.

    :param materiaHandler: material handler.
    :param concreteDiagram: diagram for concrete material.
    :parma steelDiagram: diagram for steel material.
    :param concreteSection: concrete section to compute the interaction
                            diagram for.
    '''
    param= xc.InteractionDiagramParameters()
    param.concreteTag= concreteDiagram.tag # Set concrete type.
    param.reinforcementTag= steelDiagram.tag # Set steel type.
    return materialHandler.calcInteractionDiagram(concreteSection.name,param)

    
    
