# -*- coding: utf-8 -*-
'''Control vars to store variables that control the limit state checking
   using Annex F of EN 1992-1-1:2004.
'''

__author__= "Luis C. Pérez Tato (LCPT), Ana Ortega(AO_O)"
__copyright__= "Copyright 2026,LCPT, AO_O"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@ciccp.es, ana.Ortega@ciccp.es"

from postprocess.control_variables import basic_control_variables as bcv

#       ___ ___ ___    _     _                       ___ 
#      | __/ __|_  )__/ |   /_\  _ _  _ _  _____ __ | __|
#      | _| (__ / /___| |  / _ \| ' \| ' \/ -_) \ / | _| 
#      |___\___/___|  |_| /_/ \_\_||_|_||_\___/_\_\ |_|  
# EN 1992-1-1:2004. Annex F.
# Tension reinforcement expressions for in-plane stress conditions
class Ec2InPlaneStressControlVars(bcv.PlaneStressControlVars):
    '''Control vars to check tension reinforcement expressions for 
       in-plane stress conditions according to Annex F of Eurocode 2
       part 1.

    :ivar ftd1_req: required reinforcement strength along the axis 1.
    :ivar ftd2_req: required reinforcement strength along the axis 2.
    :ivar sigma_c_req: required concrete strength.
    '''
    def __init__(self, combName= 'nil', CF= -1.0, sigma_11= 0.0, sigma_12= 0.0, sigma_22= 0.0, ftd1_req= 0.0, ftd2_req= 0.0, sigma_c_req= 0.0, ftd1_cf= -1.0, ftd2_cf= -1.0, sigma_c_cf= -1.0):
        ''' Constructor.

        :param combName: name of the load combinations to deal with
        :param CF: capacity factor (efficiency) (defaults to -1.0; CF<1.0 -> Ok; CF>1.0 -> KO)
        :param sigma_11:  (1,1) component of the stress tensor.
        :param sigma_12:  (1,2) component of the stress tensor.
        :param sigma_22:  (2,2) component of the stress tensor.
        :param ftd1_req: required reinforcement strength along the axis 1.
        :param ftd2_req: required reinforcement strength along the axis 2.
        :param sigma_c_req: required concrete strength.
        :param ftd1_cf: capacity factor of the required reinforcement strength along the axis 1.
        :param ftd2_cf: capacity factor of the required reinforcement strength along the axis 2.
        :param sigma_c_cf: capacity factor of the required concrete strength.
        '''
        super(Ec2InPlaneStressControlVars,self).__init__(combName, CF= CF, sigma_11= sigma_11, sigma_12= sigma_12, sigma_22= sigma_22)
        self.ftd1_req= ftd1_req # required reinforcement strength along the axis 1.
        self.ftd2_req= ftd2_req # required reinforcement strength along the axis 2.
        self.sigma_c_req= sigma_c_req # required concrete strength.
        self.ftd1_cf= ftd1_cf # capacity factor of the required reinforcement strength along the axis 1.
        self.ftd2_cf= ftd2_cf # capacity factor of the required reinforcement strength along the axis 2.
        self.sigma_c_cf= sigma_c_cf # capacity factor of the required concrete strength.

    def getDict(self):
        ''' Return a dictionary containing the object data.'''
        retval= super(Ec2InPlaneStressControlVars,self).getDict()
        retval.update({'ftd1_req':self.ftd1_req, 'ftd2_req':self.ftd2_req, 'sigma_c_req':self.sigma_c_req, 'ftd1_cf':self.ftd1_cf, 'ftd2_cf':self.ftd2_cf, 'sigma_c_cf':self.sigma_c_cf})
        return retval
       
    def setFromDict(self,dct):
        ''' Set the data values from the dictionary argument.

        :param dct: dictionary containing the values of the object members.
        '''
        super(VonMisesControlVars, self).setFromDict(dct)
        self.ftd1_req= dct['ftd1_req']
        self.ftd2_req= dct['ftd2_req']
        self.sigma_c_req= dct['sigma_c_req']
        self.ftd1_cf= dct['ftd1_cf']
        self.ftd2_cf= dct['ftd2_cf']
        self.sigma_c_cf= dct['sigma_c_cf']

    def getCF(self):
        ''' Return the capacity factor.'''
        return self.CF
