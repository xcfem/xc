# -*- coding: utf-8 -*-
''' Structural steel as specified in API standard.
'''

__author__= "Luis C. Pérez Tato (LCPT) , Ana Ortega (AO_O) "
__copyright__= "Copyright 2016, LCPT, AO_O"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@ciccp.es, ana.ortega@ciccp.es "

from materials.ec3 import EC3_materials

class APISteel(EC3_materials.EC3Steel):
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

        super(APISteel,self).__init__(fy, fy16, fy40, fy63, fy80, fy100, fy125, fu, gammaM, gammaM1, gammaM2)

    
# API Specification 5CT, 10th Edition
M80= APISteel(fy= 552e6, fy16= 552e6, fy40= 552e6, fy63= 552e6, fy80= 552e6, fy100= 552e6, fy125= 552e6, fu= 689e6, gammaM= 1.1)

