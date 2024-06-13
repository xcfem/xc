# -*- coding: utf-8 -*-
'''Check that redefined equal operator work OK when comparing with None.'''

from __future__ import print_function
from __future__ import division

__author__= "Ana Ortega (AOO) and Luis C. Pérez Tato (LCPT)"
__copyright__= "Copyright 2015, AOO and LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

from materials.ehe import EHE_materials


#Data
#Type of concrete used in the deck slab
concrete= EHE_materials.HA25
steel= EHE_materials.B500S

testOK= (concrete!=None)
testOK&= (steel!=None)


import os
from misc_utils import log_messages as lmsg
fName= os.path.basename(__file__)
if(testOK):
    print("test "+fName+": ok.")
else:
    lmsg.error('test: '+fName+' ERROR.')
