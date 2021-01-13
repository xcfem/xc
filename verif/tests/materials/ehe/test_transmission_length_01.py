# -*- coding: utf-8 -*-
''' Test calculation of transfer and anchorage lengths according to EHE.
    Home made test.'''

from __future__ import division
from __future__ import print_function

from materials.ehe import EHE_materials
from materials.ehe import EHE_limit_state_checking
import os
from misc_utils import log_messages as lmsg

__author__= "Luis Claudio Pérez Tato (LCPT"
__copyright__= "Copyright 2020, LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

concrete= EHE_materials.HP30
strand= EHE_materials.Y1860S7Strand_9_3

steelStressAtTransfer= 1080e6
steelDesignUltimateStress= strand.getUltimateStress()
tendonStressUnderDesignLoad= strand.getDesignUltimateStress()
tendonStressAfterAllLosses= 872e6

transmissionLength= strand.getTransmissionLength(concrete= concrete, pos= 'I', sg_pi= steelStressAtTransfer, suddenRelease= False, ELU= True, t= 28)
anchorageLength= strand.getAnchorageLength(concrete= concrete, pos= 'I', sg_pi= steelStressAtTransfer, sg_pd= tendonStressUnderDesignLoad, sg_pcs= tendonStressAfterAllLosses, suddenRelease= True, ELU= True, t= 28)

ratio1= abs(transmissionLength-0.7846875)/0.7846875
ratio2= abs(anchorageLength-1.8473767663)/1.8473767663

'''
print('transference length: ', transmissionLength, 'm')
print('development length: ', anchorageLength, 'm')
'''

fname= os.path.basename(__file__)
if((ratio1<1e-6) and (ratio2<1e-6)):
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')

