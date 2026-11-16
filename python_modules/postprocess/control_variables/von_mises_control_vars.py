# -*- coding: utf-8 -*-
'''Control vars to store variables that control the limit state checking
   using Von-Mises criteria.
'''

__author__= "Luis C. Pérez Tato (LCPT), Ana Ortega(AO_O)"
__copyright__= "Copyright 2026,LCPT, AO_O"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@ciccp.es, ana.Ortega@ciccp.es"

from postprocess.control_variables import basic_control_variables as bcv

#      __   __          __  __ _            
#      \ \ / /__ _ _   |  \/  (_)___ ___ ___
#       \ V / _ \ ' \  | |\/| | (_-</ -_|_-<
#        \_/\___/_||_| |_|  |_|_/__/\___/__/
# Von mises control vars.                                   
class VonMisesControlVars(bcv.ControlVarsBase):
    '''Von Mises stresses control vars.

    :ivar von_mises_stress:  Von Mises stress (defaults to 0.0)
    :ivar CF: capacity factor (efficiency) (defaults to -1.0; CF<1.0 -> Ok; CF>1.0 -> KO)
    '''
    def __init__(self,combName= 'nil', CF= -1.0, vm_stress= 0.0):
        ''' Constructor.

        :param combName: name of the load combinations to deal with
        :param vm_stress: von mises stress (defaults to 0.0)
        :param CF: capacity factor (efficiency) (defaults to -1.0; CF<1.0 -> Ok; CF>1.0 -> KO)
        '''
        super(VonMisesControlVars,self).__init__(combName)
        self.vm_stress= vm_stress # von Mises stress.
        self.CF= CF # Capacity factor or efficiency

    def getDict(self):
        ''' Return a dictionary containing the object data.'''
        retval= super(VonMisesControlVars,self).getDict()
        retval.update({'vm_stress':self.vm_stress})
        return retval
       
    def setFromDict(self,dct):
        ''' Set the data values from the dictionary argument.

        :param dct: dictionary containing the values of the object members.
        '''
        super(VonMisesControlVars,self).setFromDict(dct)
        self.vm_stress= dct['vm_stress']

    def getCF(self):
        ''' Return the capacity factor.'''
        return self.CF
