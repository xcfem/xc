# -*- coding: utf-8 -*-

'''
   Computation of average properties for stratified soils as described in
   4.5.5.7 "Guía de cimentaciones en obras de carreteras"
   (https://books.google.ch/books?id=a0eoygAACAAJ)
   2009
'''
from __future__ import print_function

__author__= "Luis C. Pérez Tato (LCPT)"
__copyright__= "Copyright 2016, LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

from geotechnics import frictional_cohesional_soil as fcs
import math

hi= [1.0,1.0,100.0]
rhoi= [2000,2100,2200]
phii= [math.radians(25),math.radians(27.5),math.radians(30)]
ci= [1e3,2e3,3e3]
stratifiedSoil= fcs.StratifiedSoil(hi,rhoi,phii,ci)
H= stratifiedSoil.computeAffectedDepth(1.5)

equivalentRho= stratifiedSoil.getEquivalentRho(H)
equivalentRhoTeor= (rhoi[0]*hi[0]+rhoi[1]*(hi[1]/2.0))/H
equivalentC= stratifiedSoil.getEquivalentC(H)
equivalentCTeor= (ci[0]*hi[0]+ci[1]*(hi[1]/2.0))/H
equivalentPhi= stratifiedSoil.getEquivalentPhi(H)
equivalentPhiTeor= math.atan(math.exp((math.log(math.tan(phii[0]))*hi[0]+math.log(math.tan(phii[1]))*(hi[1]/2.0))/H))

ratio1= (equivalentRho-equivalentRhoTeor)/equivalentRhoTeor
ratio2= (equivalentC-equivalentCTeor)/equivalentCTeor
ratio3= (equivalentPhi-equivalentPhiTeor)/equivalentPhiTeor

# print('equivalentRho= ', equivalentRho)
# print('equivalentRhoTeor= ', equivalentRhoTeor)
# print('equivalentC= ', equivalentC)
# print('equivalentCTeor= ', equivalentCTeor)
# print('equivalentPhi= ', math.degrees(equivalentPhi))
# print('equivalentPhiTeor= ', math.degrees(equivalentPhiTeor))

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if (ratio1<1e-3) & (ratio2<1e-3) & (ratio3<1e-3):# & (ratio4<3e-2):
    print('test: '+fname+': ok.')
else:
    lmsg.error('test: '+fname+' ERROR.')
