# -*- coding: utf-8 -*-
''' Verification test of the cálculo del coeficiente de fluencia
   según EHE-08. '''
from __future__ import division

__author__= "Luis C. Pérez Tato (LCPT)"
__copyright__= "Copyright 2014, LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"
import xc_base
import geom
import xc

from materials.ehe import relajacion_acero

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AOO)"
__copyright__= "Copyright 2015, LCPT and AOO"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"


alpha= 0.7
sigmaInic= alpha*1860e6
incSigma1= -relajacion_acero.getPerdidaTensionRelajacionFinalEHE("alambre","superestabilizado",alpha,sigmaInic)
sigmaRemanente1= sigmaInic+incSigma1
incSigma2= -relajacion_acero.getPerdidaTensionRelajacionTEHE("alambre","superestabilizado",alpha,1e6/24,sigmaInic)
sigmaRemanente2= sigmaInic+incSigma2
incSigma3= -relajacion_acero.getPerdidaTensionRelajacionTCEB("alambre","superestabilizado",alpha,1e6/24,sigmaInic)
sigmaRemanente3= sigmaInic+incSigma3
ratio1= abs(incSigma1/sigmaInic+0.058)/0.058
ratio2= abs(incSigma2/sigmaInic+0.058)/0.058
ratio3= abs(incSigma3/sigmaInic+0.058)/0.058

''' 
print "cociente1= ",incSigma1/sigmaInic
print "incSigma1= ",incSigma1/1e6," MPa\n"
print "sigmaRemanente1= ",sigmaRemanente1/1e6," MPa\n"
print "cociente2= ",incSigma2/sigmaInic
print "incSigma2= ",incSigma2/1e6," MPa\n"
print "sigmaRemanente2= ",sigmaRemanente2/1e6," MPa\n"
print "cociente3= ",incSigma3/sigmaInic
print "incSigma3= ",incSigma3/1e6," MPa\n"
print "sigmaRemanente3= ",sigmaRemanente3/1e6," MPa\n"
print "ratio1= ",(ratio1)
print "ratio2= ",(ratio2)
print "ratio3= ",(ratio3)
'''

import os
from miscUtils import LogMessages as lmsg
fname= os.path.basename(__file__)
if (ratio1<1e-5) & (ratio2<6e-3) & (ratio3<6e-3):
  print "test ",fname,": ok."
else:
  lmsg.error(fname+' ERROR.')
