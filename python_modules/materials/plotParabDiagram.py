# -*- coding: utf-8 -*-

#*    Nov. 2015   Ana Ortega    *

from __future__ import division
from materials.ehe import EHE_concrete
from materials.ec2 import EC2_concrete


#conc= EC2_concrete.EC2Concrete("C25",-25e6,1.5)
#conc= EHE_concrete.EHEConcrete("HA25",-25e6,1.5)
#conc= EC2_concrete.EC2Concrete("C70",-70e6,1.5)
conc= EHE_concrete.EHEConcrete("HA70",-70e6,1.5)
conc.alfacc=0.85

X=[]
Y=[]
eps=0
while eps>=-0.0035:
    sigm=conc.sigmaPR(eps)
    print 'eps=',eps,'sigm=',sigm
    X.append(eps)
    Y.append(-sigm)
    eps+=-0.0001

import pylab as pl
pl.plot(X,'go')
pl.plot(Y,'bo')
pl.show()



