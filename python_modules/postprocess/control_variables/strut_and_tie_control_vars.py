# -*- coding: utf-8 -*-
'''Control vars to store variables that control the reinforced concrete
   limit state checking using strut-and-tie models.
'''

__author__= "Luis C. Pérez Tato (LCPT), Ana Ortega(AO_O)"
__copyright__= "Copyright 2026,LCPT, AO_O"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@ciccp.es, ana.Ortega@ciccp.es"

from postprocess.control_variables import basic_control_variables as bcv

class StrutAndTieControlVars(bcv.CFN):
    '''Control variables for elements of strut-and-tie models.

    :ivar typo: element type ('strut' or 'tie').
    '''
    def __init__(self, idSection= 'nil', CF= -1.0, combName= None, N= 0.0, stress= 0.0, typo= None, inverted= False):
        '''
        Constructor.

        :param idSection: section identifier
        :param combName: name of the load combinations to deal with.
        :param N: axial force (defaults to 0.0).
        :param typo: element type 'strut' or 'tie'.
        :param inverted: if true, the element is working in the opposite way
                         to what was intended: strut in tension or tie in
                         compression.
        '''
        super(StrutAndTieControlVars,self).__init__(CF= CF, combName= combName, N= N)
        self.idSection= idSection # Section identifier.
        self.stress= stress
        self.typo= typo
        self.inverted= inverted
        
    def getDict(self):
        ''' Return a dictionary containing the object data.'''
        retval= super(StrutAndTieControlVars,self).getDict()
        if(self.stress!=0.0):
            retval.update({'stress':self.stress})
        retval.update({'idSection':self.idSection, 'typo':self.typo, 'inverted':self.inverted})
        return retval
       
    def setFromDict(self,dct):
        ''' Set the data values from the dictionary argument.

        :param dct: dictionary containing the values of the object members.
        '''
        super(StrutAndTieControlVars,self).setFromDict(dct)
        self.idSection= dct['idSection']
        if('stress' in dct):
            self.stress= retval['stress']
        self.typo= dct['typo']
        self.inverted= dct['inverted']
