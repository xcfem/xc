# -*- coding: utf-8 -*-
''' Reinforced concrete materials according to Model Code CEB-FIP 1990.'''
from __future__ import division

__author__= "Luis C. Pérez Tato (LCPT) , Ana Ortega (AO_O) "
__copyright__= "Copyright 2016, LCPT, AO_O"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@ciccp.es, ana.ortega@ciccp.es "

import math
import scipy.interpolate
from materials import concrete_base

# ************* Prestressing steel. ********************

class CEBPrestressingSteel(concrete_base.PrestressingSteel):
    ''' Prestressing steel according to Model Code CEB-FIP 1990.

    :param fpk: Elastic limit.
    :param fmax: Steel strength.
    :param alpha: stress-to-strength ratio.
    :param steelRelaxationClass: Relaxation class 1: normal, 2: improved, 
                                    and 3: relaxation for bars.
    :param tendonClass: Tendon class wire, strand or bar.

    Points defined in figure 2.3.3. of Model Code CEB-FIP 1990
    that can be used to determine the relaxation at 1000 hours:

    - ptsRO1000Class1: for normal relaxation strands and wires (class 1).
    - ptsRO1000Class2: for low relaxation strands and wires.
    - ptsRO1000Class3: for bars
    '''
    ptsRO1000Class1= scipy.interpolate.interp1d([0, 0.5, 0.6, 0.7, 0.8],[0, 0, 4, 12+1.0/9.0, 12+1.0/9.0])
    ptsRO1000Class2= scipy.interpolate.interp1d([0, 0.5, 0.6, 0.7, 0.8],[0, 0, 4/9.0, 2, 4+3.5/9.0])
    ptsRO1000Class3= scipy.interpolate.interp1d([0, 0.5, 0.6, 0.7, 0.8],[0, 0, 2, 4, 6+4/9.0])

    def __init__(self,steelName,fpk,fmax= 1860e6, alpha= 0.75, steelRelaxationClass=1, tendonClass= 'strand'):
        super(CEBPrestressingSteel,self).__init__(steelName,fpk,fmax,alpha,steelRelaxationClass, tendonClass)


    def getRO1000(self):
      '''
      Return the relaxation at 1000 hours after stressing (See figure 2.3.3
      at Model Code)
      '''
      if((self.tendonClass=="wire") | (self.tendonClass=="strand")):
          if(self.steelRelaxationClass==2):
              return self.ptsRO1000Class2(self.alpha)
          elif(steelRelaxationClass==1):
              return self.ptsRO1000Class1(self.alpha) 
      elif(self.tendonClass=="bar"):
          return self.ptsRO1000Class3(self.alpha)


    def getRelaxationT(self, tDays):
        ''' Return the relaxation at time tDays in days after stressing.

        :param tDays: Time in days after prestressing
                  (to make easier to deal with shrinkage and creep at the same time).
        '''
        tHours= tDays*24
        RO1000= self.getRO1000()
        if(tHours<1000):
            return RO1000*self.ptsShortTermRelaxation(tHours)
        else:
            return RO1000*pow(tHours/1000.0,self.getKRelaxation())

    def getRelaxationStressLossT(self, tDays, initialStress):
        '''
        Return change in tendon stress due to relaxation at time t.

        :param tDays: Time in days after prestressing
                  (to make easier to deal with shrinkage and creep at the same time).
        :param initialStress: Initial stress in tendon.

        '''
        LGRO120= math.log10(self.getRelaxationT(120/24.0))
        LGRO1000= math.log10(self.getRelaxationT(1000/24.0))
        k2= (LGRO1000-LGRO120)/(3-math.log10(120))
        k1= LGRO1000-3*k2
        tHours= tDays*24
        ROT= pow(10,k1+k2*math.log10(tHours))/100.0
        return initialStress*ROT
