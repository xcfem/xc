# -*- coding: utf-8 -*-
''' Computation of the creep factor according to EHE-08. '''
from __future__ import print_function
from __future__ import division

__author__= "Luis C. Pérez Tato (LCPT)"
__copyright__= "Copyright 2014, LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

from materials.ehe import EHE_materials
from materials.ceb import CEB_materials

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AOO)"
__copyright__= "Copyright 2015, LCPT and AOO"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

prestressingSteelEHE= EHE_materials.EHEPrestressingSteel('prestressingSteelEHE',fpk= 1171e6,fmax= 1860e6, alpha= 0.7,steelRelaxationClass=2, tendonClass= 'wire')

prestressingSteelCEB= CEB_materials.CEBPrestressingSteel('prestressingSteelCEB',fpk= 1171e6,fmax= 1860e6, alpha= 0.7,steelRelaxationClass=2, tendonClass= 'wire')

alpha= 0.7
sigmaInic= alpha*1860e6
incSigma1= -prestressingSteelEHE.getRelaxationStressLossFinal(sigmaInic)
sigmaRemanente1= sigmaInic+incSigma1
incSigma2= -prestressingSteelEHE.getRelaxationStressLossT(1e6/24,sigmaInic)
sigmaRemanente2= sigmaInic+incSigma2
incSigma3= -prestressingSteelCEB.getRelaxationStressLossT(1e6/24,sigmaInic)
sigmaRemanente3= sigmaInic+incSigma3
ratio1= abs(incSigma1/sigmaInic+0.058)/0.058
ratio2= abs(incSigma2/sigmaInic+0.058)/0.058
ratio3= abs(incSigma3/sigmaInic+0.058)/0.058

''' 
print("cociente1= ",incSigma1/sigmaInic)
print("incSigma1= ",incSigma1/1e6," MPa\n")
print("sigmaRemanente1= ",sigmaRemanente1/1e6," MPa\n")
print("cociente2= ",incSigma2/sigmaInic)
print("incSigma2= ",incSigma2/1e6," MPa\n")
print("sigmaRemanente2= ",sigmaRemanente2/1e6," MPa\n")
print("cociente3= ",incSigma3/sigmaInic)
print("incSigma3= ",incSigma3/1e6," MPa\n")
print("sigmaRemanente3= ",sigmaRemanente3/1e6," MPa\n")
print("ratio1= ",(ratio1))
print("ratio2= ",(ratio2))
print("ratio3= ",(ratio3))
'''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if (ratio1<1e-5) & (ratio2<6e-3) & (ratio3<6e-3):
  print("test ",fname,": ok.")
else:
  lmsg.error(fname+' ERROR.')
