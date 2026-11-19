# -*- coding: utf-8 -*-
'''Control vars to store variables that control the limit state checking
   using Annex LL (informative) of EN 1992-2:2005.
'''

__author__= "Luis C. Pérez Tato (LCPT), Ana Ortega(AO_O)"
__copyright__= "Copyright 2026,LCPT, AO_O"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@ciccp.es, ana.Ortega@ciccp.es"

from postprocess.control_variables import basic_control_variables as bcv

#       ___ ___ ___   ___     _                       _    _    
#      | __/ __|_  )_|_  )   /_\  _ _  _ _  _____ __ | |  | |   
#      | _| (__ / /___/ /   / _ \| ' \| ' \/ -_) \ / | |__| |__ 
#      |___\___/___| /___| /_/ \_\_||_|_||_\___/_\_\ |____|____|
#                                                              
# EN 1992-2:2002. Annex LL.
# EC2 Sandwich model control variables.
class Ec2AnnexLLControlVars(bcv.ShellControlVars):
    '''Control vars to check shell reinforced concrete elements 
       according to Annex LL of Eurocode 2 part 2.

    :ivar ts: outer layer thickness.
    :ivar concrete_cf: concrete efficiency.
    :ivar crush_ok: outer layer thickness smaller than the maximum.
    :ivar shear_cf: out of plane shear efficiency,
    :ivar as1_top_req: required top reinforcement area along the axis 1.
    :ivar as1_top_comb: worst combination for required top reinforcement
                         area along the axis 1.
    :ivar as1_bottom_req: required bottom reinforcement area along the 
                           axis 1.
    :ivar as1_bottom_comb: worst combination for required bottom 
                            reinforcement area along the axis 1.
    :ivar as2_top_req: required bottom reinforcement area along the axis 2.
    :ivar as2_top_comb: worst combination for required top reinforcement
                         area along the axis 2.
    :ivar as2_bottom_req: required bottom reinforcement area along the 
                           axis 2.
    :ivar as1_bottom_comb: worst combination for required bottom 
                            reinforcement area along the axis 2.
    :ivar case_bot: label of the design case used for the bottom layer with the
                    following meaning:

                    "1: x+y tension"
                        nx >= -|nxy| and ny >= -|nxy|. Both reinforcement 
                        directions are in tension and the concrete strut is 
                        at 45 degrees: 
                        f_tx = nx + |nxy|, f_ty = ny + |nxy|, f_c = 2|nxy|.
                    "2: x unreinforced"
                        nx < -|nxy|. The concrete carries the x compression 
                        alone, so f_tx = 0 and f_ty = ny + nxy**2 / |nx|. The 
                        strut is no longer at 45 degrees.
                    "3: y unreinforced"
                        Same as case 2 with x and y swapped (ny < -|nxy|).
                    "4: biaxial compr."
                        The layer is in biaxial compression (the reinforcement 
                        of case 2 or 3 would be negative). No reinforcement is
                        needed and f_c is the principal compressive force.

    :ivar case_top: label of the design case used for the top layer (same 
                    meaning than in case_top).
    '''
    def __init__(self, combName= 'nil', CF= -1.0, n1= 0.0, n2= 0.0, n12= 0.0, m1= 0.0, m2= 0.0, m12= 0.0, q13= 0.0, q23= 0.0, ts= 0.0, concrete_cf= -1.0, concrete_cf_comb= None, crush_ok= False, shear_cf= 0.0, as1_top_req= 0.0, as1_top_comb= None, as1_top_cf= 0.0, as1_bottom_req= 0.0, as1_bottom_comb= None, as1_bottom_cf= 0.0, as2_top_req= 0.0, as2_top_comb= None, as2_top_cf= 0.0, as2_bottom_req= 0.0, as2_bottom_comb= None, as2_bottom_cf= 0.0, case_top= None, case_bottom= None):
        ''' Constructor.

        :param combName: name of the load combinations to deal with
        :param CF: capacity factor (efficiency) (defaults to -1.0; CF<1.0 -> Ok; CF>1.0 -> KO)
        :param n1: axial force parallel to axis 1.
        :param n2: axial force parallel to axis 2.
        :param n12: in plane shear force.
        :param m1: bending moment parallel to axis 1.
        :param m2: bending moment parallel to axis 2.
        :param m12: twisting stress resultant.
        :param q13: out of plane shear force normal to axis 1 and parallel to axis 3.
        :param q23: out of plane shear force normal to axis 1 and parallel to axis 3.
        :param concrete_cf: concrete efficiency.
        :param concrete_cf_comb: worst combination for concrete efficiency.
        :param crush_ok: outer layer thickness smaller than the maximum.
        :param shear_cf: out of plane shear efficiency,
        :param as1_top_req: required top reinforcement area along the axis 1.
        :param as1_top_comb: worst combination for required top reinforcement
                             area along the axis 1.
        :param as1_bottom_req: required bottom reinforcement area along the 
                               axis 1.
        :param as1_bottom_comb: worst combination for required bottom 
                                reinforcement area along the axis 1.
        :param as2_top_req: required bottom reinforcement area along the axis 2.
        :param as2_top_comb: worst combination for required top reinforcement
                             area along the axis 2.
        :param as2_bottom_req: required bottom reinforcement area along the 
                               axis 2.
        :param as1_bottom_comb: worst combination for required bottom 
                                reinforcement area along the axis 2.
        :param case_bot: label of the design case used for the bottom layer 
                         with the following meaning:

                         "1: x+y tension"
                             nx >= -|nxy| and ny >= -|nxy|. Both reinforcement 
                             directions are in tension and the concrete strut 
                             is at 45 degrees: 
                             f_tx = nx + |nxy|, f_ty = ny + |nxy|, f_c = 2|nxy|.
                         "2: x unreinforced"
                             nx < -|nxy|. The concrete carries the x 
                             compression alone, so f_tx = 0 and 
                             f_ty = ny + nxy**2 / |nx|. The strut is no longer 
                             at 45 degrees.
                         "3: y unreinforced"
                             Same as case 2 with x and y swapped (ny < -|nxy|).
                         "4: biaxial compr."
                             The layer is in biaxial compression (the 
                             reinforcement of case 2 or 3 would be negative). 
                             No reinforcement is needed and f_c is the 
                             principal compressive force.

        :param case_top: label of the design case used for the top layer (same 
                         meaning than in case_top).
        '''
        super().__init__(combName, CF= CF, n1= n1, n2= n2, n12= n12, m1= m1, m2= m2, m12= m12, q13= q13, q23= q23)
        self.concrete_cf= concrete_cf # concrete efficiency.
        self.concrete_cf_comb= concrete_cf_comb # worst combination for concrete efficiency.
        self.crush_ok= crush_ok # outer layer thickness smaller than the maximum.
        self.shear_cf= shear_cf # concrete_efficiency in shear.
        
        self.as1_top_req= as1_top_req # required top reinforcement area along the axis 1.
        self.as1_top_comb= as1_top_comb # worst load combination.
        self.as1_top_cf= as1_top_cf # worst capacity factor (efficiency).
        
        self.as1_bottom_req= as1_bottom_req # required bottom reinforcement area along the  axis 1.
        self.as1_bottom_comb= as1_bottom_comb # worst load combination.
        self.as1_bottom_cf= as1_bottom_cf # worst capacity factor (efficiency).
        
        self.as2_top_req= as2_top_req # required bottom reinforcement area along the axis 2.
        self.as2_top_comb= as2_top_comb # worst load combination.
        self.as2_top_cf= as2_top_cf # worst capacity factor (efficiency).
        
        self.as2_bottom_req= as2_bottom_req # required bottom reinforcement area along the axis 2.
        self.as2_bottom_comb= as2_bottom_comb # worst load combination.
        self.as2_bottom_cf= as2_bottom_cf # worst capacity factor (efficiency).
        
        self.case_bottom= case_bottom # label of the design case used for the bottom layer 
        self.case_top= case_top # label of the design case used for the top layer.

    def getDict(self):
        ''' Return a dictionary containing the object data.'''
        retval= super().getDict()
        retval.update({'concrete_cf': self.concrete_cf, 'concrete_cf_comb': self.concrete_cf_comb, 'crush_ok': self.crush_ok, 'shear_cf': self.shear_cf, 'as1_top_req':self.as1_top_req, 'as1_top_comb': self.as1_top_comb, 'as1_top_cf': self.as1_top_cf, 'as1_bottom_req':self.as1_bottom_req, 'as1_bottom_comb': self.as1_bottom_comb, 'as1_bottom_cf': self.as1_bottom_cf, 'as2_top_req':self.as2_top_req, 'as2_top_comb': self.as2_top_comb, 'as2_top_cf': self.as2_top_cf, 'as2_bottom_req':self.as2_bottom_req, 'as2_bottom_comb': self.as2_bottom_comb, 'as2_bottom_cf': self.as2_bottom_cf, 'case_bottom': self.case_bottom, 'case_top': self.case_top})
        return retval
       
    def setFromDict(self,dct):
        ''' Set the data values from the dictionary argument.

        :param dct: dictionary containing the values of the object members.
        '''
        super(VonMisesControlVars, self).setFromDict(dct)
        self.concrete_cf= dct['concrete_cf']
        self.concrete_cf_comb= dct['concrete_cf_comb']
        self.crush_ok= dct['crush_ok']
        self.shear_cf= dct['shear_cf']
        
        self.as1_top_req= dct['as1_top_req']
        self.as1_top_comb= dct['as1_top_comb']
        self.as1_top_cf= dct['as1_top_cf']
        
        self.as1_bottom_req= dct['as1_bottom_req']
        self.as1_bottom_comb= dct['as1_bottom_comb']
        self.as1_bottom_cf= dct['as1_bottom_cf']
        
        self.as2_top_req= dct['as2_top_req']
        self.as2_top_comb= dct['as2_top_comb']
        self.as2_top_cf= dct['as2_top_cf']
        
        self.as2_bottom_req= dct['as2_bottom_req']
        self.as2_bottom_comb= dct['as2_bottom_comb']
        self.as2_bottom_cf= dct['as2_bottom_cf']
        
        self.case_bottom= dct['case_bottom']
        self.case_top= dct['case_top']

    def getCF(self):
        ''' Return the capacity factor.'''
        return self.CF
