# -*- coding: utf-8 -*-

'''EC2_reinforcing_steel.py: reinforcing steel as defined in EC2 
(from Annex C)EHE (spanish RC standard).
 Eurocode 2 permits to use a various steel yielding grades ranging
from 400 MPa to 600 MPa.
Ductility grades:
   -A: cold worked  eps_uk>=2.5% k>=1.05
   -B: hot rolled   eps_uk>=5.0% k>=1.08
   -C: seismic      eps_uk>=7.5% k>=1.15 k<1.35
'''

__author__= "Luis C. Pérez Tato (LCPT) Ana Ortega (A_OO)"
__copyright__= "Copyright 2015, LCPT, A_OO"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com ana.Ortega.Ort@gmail.com"

import math
from materials import reinforcingSteel

#
#   nmbAcero: Name identifying the material.
#   fyk:      characteristic value of the yield strength.
#   emax:     maximum strain in tension
#   gammaS:   partial factor for material.
#   k:        fmaxk/fyk ratio
S400A= reinforcingSteel.ReinforcingSteel(nmbAcero="S400A", fyk=400e6, emax=0.025,gammaS=1.15,k=1.05)
S400B= reinforcingSteel.ReinforcingSteel(nmbAcero="S400B", fyk=400e6, emax=0.05,gammaS=1.15,k=1.08)
S400C= reinforcingSteel.ReinforcingSteel(nmbAcero="S400C", fyk=400e6, emax=0.075,gammaS=1.15,k=1.15)

S450A= reinforcingSteel.ReinforcingSteel(nmbAcero="S450A", fyk=450e6, emax=0.025,gammaS=1.15,k=1.05)
S450B= reinforcingSteel.ReinforcingSteel(nmbAcero="S450B", fyk=450e6, emax=0.05,gammaS=1.15,k=1.08)
S450C= reinforcingSteel.ReinforcingSteel(nmbAcero="S450C", fyk=450e6, emax=0.075,gammaS=1.15,k=1.15)

S500A= reinforcingSteel.ReinforcingSteel(nmbAcero="S500A", fyk=500e6, emax=0.025,gammaS=1.15,k=1.05)
S500B= reinforcingSteel.ReinforcingSteel(nmbAcero="S500B", fyk=500e6, emax=0.05,gammaS=1.15,k=1.08)
S500C= reinforcingSteel.ReinforcingSteel(nmbAcero="S500C", fyk=500e6, emax=0.075,gammaS=1.15,k=1.15)

S550A= reinforcingSteel.ReinforcingSteel(nmbAcero="S550A", fyk=550e6, emax=0.025,gammaS=1.15,k=1.05)
S550B= reinforcingSteel.ReinforcingSteel(nmbAcero="S550B", fyk=550e6, emax=0.05,gammaS=1.15,k=1.08)
S550C= reinforcingSteel.ReinforcingSteel(nmbAcero="S550C", fyk=550e6, emax=0.075,gammaS=1.15,k=1.15)

S600A= reinforcingSteel.ReinforcingSteel(nmbAcero="S600A", fyk=600e6, emax=0.025,gammaS=1.15,k=1.05)
S600B= reinforcingSteel.ReinforcingSteel(nmbAcero="S600B", fyk=600e6, emax=0.05,gammaS=1.15,k=1.08)
S600C= reinforcingSteel.ReinforcingSteel(nmbAcero="S600C", fyk=600e6, emax=0.075,gammaS=1.15,k=1.15)



