# -*- coding: utf-8 -*-
'''Creep verification test.

 Data taken from "Eurocode 2. Worked examples." Rev A 31-03-2017
  page 80. Result according Annex B of Eurocode 2 compared with those obtained
  with the tool: https://eurocodeapplied.com/design/en1992/creep-shrinkage

https://www.theconcreteinitiative.eu/images/ECP_Documents/Eurocode2_WorkedExamples.pdf
'''

from __future__ import print_function
from __future__ import division

__author__= "Ana Ortega (AOO) and Luis C. Pérez Tato (LCPT)"
__copyright__= "Copyright 2024, AOO and LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

from materials.ec2 import EC2_materials

# Data

# Type of concrete used in the deck slab
concrDeck= EC2_materials.EC2Concrete("C35/45",-35e6,1.5) # page 73
 
Ac= 17.43 # area of the concrete slab (m2); page 80.
u= 28.64 # perimeter exposed to drying (m); page 80.
h0= 2*Ac/u # notional size of the member h0.
RH= 70 # ambient relative humidity(%); page 79.
t0= 28 # age of concrete at prestressing time; page 79.
t_inf= 25550 # corresponding to a life-time of 70 years; page 79.

fiRH= concrDeck.getCreepFiRH(RH,h0) # factor to allow for the effect of relative
                                   # humidity on the notional creep coefficient
                                   
betafcm= concrDeck.getCreepBetafcm() # factor to allow for the effect of
                                     # concrete strength on the notional creep
                                     # coefficient

betat0= concrDeck.getCreepBetat0(t0)  # factor to allow for the effect of
                                      # concrete age at loading
                                      # on the notional creep coefficient
                                      
betactt0= concrDeck.getCreepBetactt0(t_inf,t0,RH,h0) # coefficient to describe
                                                     # the development of creep
                                                     # with time after loading.

betaH= concrDeck.getCreepBetaH(RH,h0) # coefficient depending on the relative
                                      # humidity (RH) and the notional member
                                      # size (h0).
                                      
fi0=concrDeck.getCreepFi0(t0,RH,h0) # notional creep coefficient for the
                                    # calculation of the creep coefficient.
                            
#Creep coefficients:
fit_inf_t0= concrDeck.getCreepFitt0(t_inf,t0,RH,h0)

ratio0= round(fi0,3)-1.493

'''
print('h0= '+'{:.3f}'.format(h0))
print('fiRH= '+'{:.3f}'.format(fiRH))
print('betafcm= '+'{:.3f}'.format(betafcm))
print('betat0= '+'{:.3f}'.format(betat0))
print('betaH= '+'{:.3f}'.format(betaH))
print('betactt0= '+'{:.3f}'.format(betactt0))
print('fi0= '+'{:.3f}'.format(fi0))
print('fi= '+'{:.3f}'.format(fit_inf_t0))
'''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if ratio0<1e-6:
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')

