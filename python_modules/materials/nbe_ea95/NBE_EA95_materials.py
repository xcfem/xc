# -*- coding: utf-8 -*-
''' Structural steel as specified in NBE EA-95 standard.
    NBE EA-95 «Estructuras de acero en la edificación». Real Decreto 1829/1995, de 10 de noviembre.
'''

__author__= "Luis C. Pérez Tato (LCPT) , Ana Ortega (AO_O) "
__copyright__= "Copyright 2022, LCPT, AO_O"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@ciccp.es, ana.ortega@ciccp.es "

import scipy.interpolate
from materials import steel_base
from misc_utils import log_messages as lmsg
from materials.ec3 import EC3_materials

class EA95Steel(EC3_materials.EC3Steel):
    def __init__(self, fy, fy16, fy40, fy63, fy80, fy100, fy125, fu, gammaM, gammaM1= 1.0, gammaM2= 1.1):
        ''' Constructor.
          :param fy16: 0<t<16mm
          :param fy40: 16<t<40mm
          :param fy63: 40<t<63mm
          :param fy80: 63<t<80mm
          :param fy100: 80<t<100mm
          :param fy125: 100<t<125mm
          :param gammaM: partial safety factor for steel strength.
          :param gammaM1: partial factor for buckling resistance.
          :param gammaM2: partial factor for cross-sections in tension to fracture.
        '''

        super(EA95Steel,self).__init__(fy, fy16, fy40, fy63, fy80, fy100, fy125, fu, gammaM, gammaM1, gammaM2)

    
# EA-95 Tabla 2.1.2
convFactor= 9.81/1e-6

A37b= EA95Steel(fy= 24*convFactor, fy16= 24*convFactor, fy40= 23*convFactor, fy63= 22*convFactor, fy80= 20*convFactor, fy100= 19*convFactor, fy125= 18*convFactor, fu= 37*convFactor, gammaM= 1.0)
A37c= A37b
A37d= A37b

A42b= EA95Steel(fy= 26*convFactor, fy16= 26*convFactor, fy40= 25*convFactor, fy63= 24*convFactor, fy80= 23*convFactor, fy100= 22*convFactor, fy125= 21*convFactor, fu= 42*convFactor, gammaM= 1.0)
A42c= A42b
A42d= A42b

A52b= EA95Steel(fy= 36*convFactor, fy16= 36*convFactor, fy40= 35*convFactor, fy63= 34*convFactor, fy80= 33*convFactor, fy100= 32*convFactor, fy125= 31*convFactor, fu= 52*convFactor, gammaM= 1.0)
A52c= A52b
A52d= A52b


