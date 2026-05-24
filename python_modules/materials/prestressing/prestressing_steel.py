# -*- coding: utf-8 -*-
''' Base classes for prestressing steel materials.'''

from __future__ import division
from __future__ import print_function

__author__= "Ana Ortega (AO_O) and Luis C. Pérez Tato (LCPT)"
__copyright__= "Copyright 2026, AO_O and LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= " ana.Ortega.Ort@gmail.com, l.pereztato@gmail.com"


import sys
import scipy.interpolate
from scipy import stats
from materials import typical_materials
from materials.sections import material_with_DK_diagrams as matWDKD
from misc_utils import log_messages as lmsg

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
    
    def _define_characteristic_diagram(self, preprocessor, initialStress):
        '''Characteristic stress-strain diagram.

        :param preprocessor: pre-processor for the finite element problem.
        :param initialStress: value of the initial stress.
        '''
        return typical_materials.defSteel02(preprocessor,self.nmbDiagK,self.Es,self.fpk,self.bsh,initialStress)
    
    def defDiagK(self, preprocessor, initialStress):
        '''Characteristic stress-strain diagram.

        :param preprocessor: pre-processor for the finite element problem.
        :param initialStress: value of the initial stress.
        '''
        retval= None
        materialExists= self.diagKExists(preprocessor)
        if(materialExists):
            retval= self.getDiagK(preprocessor)
        else:
            retval= self._define_characteristic_diagram(preprocessor, initialStress)
        self._set_preprocessor(preprocessor)
        return retval
  
    def _define_design_diagram(self, preprocessor, initialStress):
        '''Design stress-strain diagram.

        :param preprocessor: pre-processor for the finite element problem.
        :param initialStress: value of the initial stress.
        '''
        return typical_materials.defSteel02(preprocessor,self.nmbDiagD,self.Es,self.fpd(),self.bsh,initialStress)
    
    def defDiagD(self,preprocessor, initialStress):
        '''Design stress-strain diagram.

        :param preprocessor: pre-processor for the finite element problem.
        :param initialStress: value of the initial stress.
        '''
        retval= None
        materialExists= self.diagDExists(preprocessor)
        if(materialExists):
            retval= self.getDiagD(preprocessor)
        else:
            retval= self._define_design_diagram(preprocessor, initialStress)
        self._set_preprocessor(preprocessor)
        return retval

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
