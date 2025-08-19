# -*- coding: utf-8 -*-
''' TDConcrete according to Model Code 2010.'''


__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AOO)"
__copyright__= "Copyright 2025, LCPT and AOO"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com" "anaOrtegaOrt@gmail.com"

import math
from materials import typical_materials

class ShrinkageAndCreepParameters(object):
    ''' Shrinkage and creep parameters according Model Code 2010.

    :ivar concrete: concrete material.
    :ivar cement: 32.5N, 32.5R, 42.5N, 42.5R, 52.5N, 52.5R)
    :ivar h0: notional size of the section (2*A/u)
    :ivar T: average temperature over entire analysis period.
    :ivar RH: average relative humidity over entire analysis period.
    :ivar ts: time at start of drying (days).
    :ivar t0: loading age (days).
    :ivar rca: percentage of coarse recycled aggregate concrete in the 
               mixture.

    :ivar xi_cbs_2: this is a free fitting shrinkage parameter; its default
                    value is 1.0.
    :ivar xi_cds_2: this is a free fitting shrinkage parameter; its default
                    value is 1.0.
    :ivar xi_cb_2: this is a free fitting creep parameter; its default
                    value is 1.0.
    :ivar xi_cd_2: this is a free fitting creep parameter; its default
                    value is 1.0.
    '''
    def __init__(self, concrete, cement, h0, T, RH, ts, t0, rca= 0.0, xi_cbs_2= 1.0, xi_cds_2= 1.0, xi_cb_2= 1.0, xi_cd_2= 1.0):
        ''' Constructor.

        :param concrete: concrete material.
        :param cement: 32.5N, 32.5R, 42.5N, 42.5R, 52.5N, 52.5R)
        :param h0: notional size of the section (2*A/u)
        :param T: average temperature over entire analysis period.
        :param RH: average relative humidity over entire analysis period.
        :param ts: time at start of drying (days).
        :param t0: loading age (days).
        :param rca: percentage of coarse recycled aggregate concrete in the 
                    mixture.
        :param xi_cbs_2: this is a free fitting shrinkage parameter; its 
                         default value is 1.0.
        :param xi_cds_2: this is a free fitting shrinkage parameter; its 
                         default value is 1.0.
        :param xi_cb_2: this is a free fitting creep parameter; its 
                        default value is 1.0.
        :param xi_cd_2: this is a free fitting creep parameter; its 
                        default value is 1.0.
        '''
        self.concrete= concrete # concrete material.
        self.cement= cement # 32.5N, 32.5R, 42.5N, 42.5R, 52.5N, 52.5R)
        self.h0= h0 # notional size of the section (2*A/u)
        self.T= T # average temperature over entire analysis period.
        self.RH= RH # average relative humidity over entire analysis period.
        self.ts= ts # time at start of drying (days).
        self.t0= t0 # loading age (days).
        self.rca= rca # percentage of coarse recycled aggregate concrete in the 
                      #   mixture.
        self.xi_cbs_2= xi_cbs_2 # this is a free fitting shrinkage parameter;
                                # its default value is 1.0.
        self.xi_cds_2= xi_cds_2 # this is a free fitting shrinkage parameter;
                                # its default value is 1.0.
        self.xi_cb_2= xi_cb_2 # this is a free fitting creep parameter;
                              # its default value is 1.0.
        self.xi_cd_2= xi_cd_2 # this is a free fitting creep parameter;
                              # its default value is 1.0.

    def get_xi_cs_RAC(self):
        ''' Correction coefficient for shrinkage with Recycled Aggregate 
            Concrete.
        '''
        fcmMPa= -self.concrete.getFcm()/1e6
        return max(1,(self.rca/fcmMPa)**0.3)

    def get_xi_cc_RAC(self):
        ''' Correction coefficient for creep with Recycled Aggregate Concrete.
        '''
        fcmMPa= -self.concrete.getFcm()/1e6
        return max(1,1.12*(self.rca/fcmMPa)**0.15)
    
    def get_alpha_bs(self):
        ''' Return the alpha_bs parameter that depends on the cement type.
        '''
        retval= 600
        if(self.cement=="32.5N"):
            retval= 800
        elif(self.cement=="32.5R"):
            retval= 700
        elif(self.cement=="42.5N"):
            retval= 700
        elif(self.cement=="42.5R"):
            retval= 600
        elif(self.cement=="52.5N"):
            retval= 600
        elif(self.cement=="52.5R"):
            retval= 600
        return retval

    def get_epsilon_cbs0(self):
        alpha_bs= self.get_alpha_bs()
        fcmP= -self.concrete.getFcm()/1e6/10.0
        return -alpha_bs*math.pow(fcmP/(6+fcmP),2.5)*1e-6

    def get_alpha_ds1(self):
        ''' Return the value of alpha_ds1 that depends on the cement type.'''
        retval= 6
        if(self.cement=="32.5N"):
            retval= 3
        elif(self.cement=="32.5R"):
            retval= 4
        elif(self.cement=="42.5N"):
            retval= 4
        elif(self.cement=="42.5R"):
            retval= 6
        elif(self.cement=="52.5N"):
            retval= 6
        elif(self.cement=="52.5R"):
            retval= 6
        return retval
    
    def get_alpha_ds2(self):
        ''' Return the value of alpha_ds1 that depends on the cement type.'''
        retval= .012
        if(self.cement=="32.5N"):
            retval= 0.013
        elif(self.cement=="32.5R"):
            retval= 0.012
        elif(self.cement=="42.5N"):
            retval= 0.012
        elif(self.cement=="42.5R"):
            retval= 0.012
        elif(self.cement=="52.5N"):
            retval= 0.012
        elif(self.cement=="52.5R"):
            retval= 0.012
        return retval
    
    def get_epsilon_cds0(self):
        alpha_ds1= self.get_alpha_ds1()
        alpha_ds2= self.get_alpha_ds2()
        fcm= -self.concrete.getFcm()/1e6
        return (220+110*alpha_ds1)*math.exp(-alpha_ds2*fcm)*1e-6

    def get_beta_s1(self):
        ratio= -35e6/self.concrete.getFcm()
        return min(math.pow(ratio, 0.1), 1.0)

    def get_beta_as(self, t):
        ''' Return the beta_as parameter.

        :param t: time at analysis.
        '''
        return 1-math.exp(-0.2*self.xi_cbs_2*math.sqrt(t-self.ts))
    
    def get_beta_rh_shrinkage(self):
        retval= 0.25
        beta_s1= self.get_beta_s1()
        if(self.RH<99*beta_s1):
            retval= -1.55*(1-(self.RH/100)**3)
        return retval

    def get_beta_ds(self, t):
        ''' Return the beta_ds parameter.

        :param t: time at analysis.
        '''
        B18= t-self.ts # time at which shrinkage is calculated.
        B12= self.xi_cds_2
        B10= self.h0*1e3
        return math.sqrt(B18/(0.035*B10**2*B12+B18))

    def get_epsilon_cbs(self, t):
        ''' Return the epsilon_cbs strain.

        :param t: time at analysis.
        '''
        epsilon_cbs0= self.get_epsilon_cbs0()
        beta_as= self.get_beta_as(t)
        return epsilon_cbs0*beta_as*self.get_xi_cs_RAC()
    
    def get_epsilon_cds(self, t):
        ''' Return the epsilon_cds strain.

        :param t: time at analysis.
        '''
        epsilon_cds0= self.get_epsilon_cds0()
        beta_rh= self.get_beta_rh_shrinkage()
        beta_ds= self.get_beta_ds(t)
        return epsilon_cds0*beta_rh*beta_ds*self.get_xi_cs_RAC()
    
    def get_epsilon_cs(self, t):
        ''' Return the epsilon_cs strain.

        :param t: time at analysis.
        '''
        return self.get_epsilon_cbs(t)+self.get_epsilon_cds(t)

    def get_opensees_epsba(self):
        return self.get_xi_cs_RAC()*self.get_epsilon_cbs0()
    
    def get_opensees_epsbb(self):
        return self.xi_cds_2
    
    def get_opensees_epsda(self):
        return self.get_xi_cs_RAC()*self.get_epsilon_cds0()*self.get_beta_rh_shrinkage()
    
    def get_opensees_epsdb(self):
        B10= self.h0*1e3
        return 0.035*B10**2*self.xi_cds_2

    # def get_alpha_na(self):
    #     ''' Return the alpha_na parameter which depends on the aggregate type.
    #     '''
    #     retval= 1.0
    #     if self.concrete.typeAggregate == 'L': # limestone.
    #         retval= 0.9
    #     elif self.concrete.typeAggregate == 'S': # sandstone.
    #         retval= 0.7
    #     elif self.concrete.typeAggregate == 'B': # basalt.
    #         retval= 1.2
    #     elif self.concrete.typeAggregate == 'Q': # quartzite.
    #         retval= 1.0
    #     return retval

    def get_fc_t0(self):
        ''' Return the compressive strength at loading time.'''
        return self.concrete.getFcmT(t= self.t0)
        
    def get_s(self):
        ''' Return the s parameter that depends on the cement type.
        '''
        retval= 0.38
        if(self.cement=="32.5N"):
            retval= 0.38
        elif(self.cement=="32.5R"):
            retval= 0.25
        elif(self.cement=="42.5N"):
            retval= 0.25
        elif(self.cement=="42.5R"):
            retval= 0.2
        elif(self.cement=="52.5N"):
            retval= 0.2
        elif(self.cement=="52.5R"):
            retval= 0.2
        return retval
        
    def get_Ec_t0(self):
        ''' Return the tensile strength at loading time.'''
        return self.concrete.getEcmT(t= self.t0)
    
    def get_fctm_t0(self):
        ''' Return the compressive strength at loading time.'''
        return self.concrete.getFctmT(t= self.t0)

    def get_alpha(self):
        ''' Return the alpha parameter that depends on the cement type.
        '''
        retval= 1
        if(self.cement=="32.5N"):
            retval= -1
        elif(self.cement=="32.5R"):
            retval= 0
        elif(self.cement=="42.5N"):
            retval= 0
        elif(self.cement=="42.5R"):
            retval= 1
        elif(self.cement=="52.5N"):
            retval= 1
        elif(self.cement=="52.5R"):
            retval= 1
        return retval
    
    def get_t0_T_adj(self, t):
        ''' Loading age adjusted for cement type and temperature.
        '''
        return max(0.5,self.t0*math.exp(13.65-4000/(273+self.T))*(9/(2+(self.t0*math.exp(13.65-4000/(273+self.T)))**1.2)+1)**self.get_alpha())

    def get_alpha_fcm(self):
        return math.sqrt(-35e6/self.concrete.getFcm())

    def get_beta_h(self):
        B10= self.h0*1e3        
        return min(1.5*B10+250*self.get_alpha_fcm(),1500*self.get_alpha_fcm())
    
    def get_beta_bc_fcm(self):
        fcmMPa= abs(self.concrete.getFcm()/1e6)
        return 1.8/(fcmMPa**0.7)
    
    def get_beta_bc(self, t):
        ''' Return the beta_bc parameter.

        :param t: time at analysis.
        '''
        t_adj= self.get_t0_T_adj(t)
        return math.log((30/t_adj+0.035)**2*(t-self.t0)/self.xi_cb_2+1)
    
    def get_beta_dc_fcm(self):
        fcmMPa= abs(self.concrete.getFcm()/1e6)
        return 412/(fcmMPa**1.4)
    
    def get_beta_rh_creep(self):
        B10= self.h0*1e3        
        return (1-self.RH/100)/((0.1*B10/100)**(1/3))
    
    def get_beta_dc_t0(self):
        t0_adj= self.get_t0_T_adj(self.t0)
        return 1/(0.1+t0_adj**0.2)
        
    def get_gamma_t0(self):
        t0_adj= self.get_t0_T_adj(self.t0)
        return 1/(2.3+3.5/math.sqrt(t0_adj))
    
    def get_beta_dc(self, t):
        ''' Return the beta_dc parameter.

        :param t: time at analysis.
        '''
        beta_h= self.get_beta_h()
        return ((t-self.t0)/(beta_h*self.xi_cd_2+(t-self.t0)))**self.get_gamma_t0()
    def get_phi_bc(self, t):
        ''' Return the phi_bc parameter.

        :param t: time at analysis.
        '''
        return self.get_beta_bc(t)*self.get_beta_bc_fcm()*self.get_xi_cc_RAC()
    
    def get_phi_dc(self, t):
        ''' Return the phi_dc parameter.

        :param t: time at analysis.
        '''
        J22= self.get_beta_dc(t)
        J20= self.get_beta_dc_t0()
        J19= self.get_beta_rh_creep()
        J18= self.get_beta_dc_fcm()
        return J22*J20*J19*J18*self.get_xi_cc_RAC()
    
    def get_phi(self, t):
        ''' Return the phi parameter.

        :param t: time at analysis.
        '''
        return self.get_phi_bc(t)+self.get_phi_dc(t)

    def get_opensees_phiba(self):
        '''Parameter for the effect of compressive strength on basic creep 
           βbc(fcm), as per Model Code 2010.
        '''
        return self.get_beta_bc_fcm()*self.get_xi_cc_RAC()
    
    def get_opensees_phibb(self):
        '''Fitting parameter within the basic creep time evolution function 
           as per Model Code 2010 and prEN1992-1-1:2017.
        '''
        return self.xi_cb_2
    
    def get_opensees_phida(self):
        '''Product of βdc(fcm) and β(RH), as per Model Code 2010.
        '''
        return self.get_beta_dc_fcm()*self.get_beta_rh_creep() 
    
    def get_opensees_phidb(self):
        '''Fitting constant within the drying creep time evolution function 
            as per Model Code 2010.
        '''
        return self.get_beta_h()*self.xi_cd_2
    
    def get_opensees_cem(self):
        return self.get_alpha()
    
def get_TDConcrete_mc10(preprocessor, name, concrete, cement, h0, T, RH, ts, age, beta, rca= 0.0, xi_cbs_2= 1.0, xi_cds_2= 1.0, xi_cb_2= 1.0, xi_cd_2= 1.0):
    '''
    Defines a TDConcreteMC10 uniaxial material.

    :param preprocessor: preprocessor of the finite element problem.
    :param name: name identifying the new TDConcreteMC10 material. 
    :param concrete: basic concrete material.
    :param cement: cement type (32.5N, 32.5R, 42.5N, 42.5R, 52.5N, 52.5R)
    :param h0: notional size of the section (2*A/u)
    :param T: average temperature over entire analysis period.
    :param RH: average relative humidity over entire analysis period.
    :param ts: time at start of drying (days).
    :param age: concrete age at first loading.
    :param beta: tension softening parameter.
    :param rca: percentage of coarse recycled aggregate concrete in the 
                mixture.
    :param xi_cbs_2: this is a free fitting shrinkage parameter; its 
                     default value is 1.0.
    :param xi_cds_2: this is a free fitting shrinkage parameter; its 
                     default value is 1.0.
    :param xi_cb_2: this is a free fitting creep parameter; its 
                    default value is 1.0.
    :param xi_cd_2: this is a free fitting creep parameter; its 
                    default value is 1.0.
    '''
    # concrete parameters.
    Ec = concrete.getEcmT(t= age) # concrete modulus of elasticity at loading age.
    Ecm= concrete.getEcm() # 28-day modulus of elasticity
    fcm= concrete.getFcm() # mean 28-day cylinder compressive strength (this is a dummy parameter since compression behavior is linear).
    ft= concrete.getFctm() # the tensile strength (splitting or axial tensile strength should be input, rather than the flexural).

    # shrinkage and creep parameters.
    ## Shrinkage
    shrinkageAndCreepParameters= ShrinkageAndCreepParameters(concrete= concrete, cement= cement, h0= h0, T= T, RH= RH, ts= ts, t0= age)
    epsba= shrinkageAndCreepParameters.get_opensees_epsba() # ultimate basic shrinkage strain, εcbs,0, as per Model Code 2010. Row 21 column F in the TDCI_input spreadsheet.
    epsbb= shrinkageAndCreepParameters.get_opensees_epsbb() # fitting parameter within the basic shrinkage time evolution function as per Model Code 2010 and prEN1992-1-1:2017. Row 22 column F in the TDCI_input spreadsheet.
    epsda= shrinkageAndCreepParameters.get_opensees_epsda() # product of εcds,0 and βRH, as per Model Code 2010. Row 23 column F in the TDCI_input spreadsheet.
    epsdb= shrinkageAndCreepParameters.get_opensees_epsdb() # fitting parameter within the drying shrinkage time evolution function as per Model Code 2010 and prEN1992-1-1:2017. Row 24 column F in the TDCI_input spreadsheet.

    ## Creep
    phiba= shrinkageAndCreepParameters.get_opensees_phiba() # parameter for the effect of compressive strength on basic creep βbc(fcm), as per Model Code 2010. Row 20 column M in the TDCI_input spreadsheet.
    phibb= shrinkageAndCreepParameters.get_opensees_phibb() # fitting parameter within the basic creep time evolution function as per Model Code 2010 and prEN1992-1-1:2017. Row 21 column M in the TDCI_input spreadsheet.
    phida= shrinkageAndCreepParameters.get_opensees_phida() # product of βdc(fcm) and β(RH), as per Model Code 2010. Row 22 column M in the TDCI_input spreadsheet.
    phidb= shrinkageAndCreepParameters.get_opensees_phidb() # fitting constant within the drying creep time evolution function as per Model Code 2010. Row 23 column M in the TDCI_input spreadsheet.

    ## Cement type
    cem= shrinkageAndCreepParameters.get_opensees_cem() # coefficient dependent on the type of cement: –1 for 32.5N, 0 for 32.5R and 42.5N and 1 for 42.5R, 52.5N and 52.5R.
    return typical_materials.defTDConcreteMC10(preprocessor= preprocessor, name= 'tdConcrete', fcm= fcm, ft= ft, Ec= Ec, Ecm= Ecm, beta= beta, age= age, epsba= epsba, epsbb= epsbb, epsda= epsda, epsdb= epsdb, phiba= phiba, phibb= phibb, phida= phida, phidb= phidb, tcast= 0.0, cem= cem)
