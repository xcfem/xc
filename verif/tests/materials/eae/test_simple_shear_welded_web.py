# -*- coding: utf-8 -*-
'''Beam simple joint by direct web welding: computation of the required length of a weld bead, according to the clause 61.4.1 of EAE.'''
# Verification by means of the example in the page XXV-31 of the book
# "Estructuras Metálicas" de Vicente Cudós Samblancat
# (url={https://books.google.ch/books?id=7UrJnQEACAAJ}).

from materials.eae import EAE_limit_state_checking
import math

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AOO)"
__copyright__= "Copyright 2015, LCPT and AOO"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

# Loads
Vd= 8585*9.81 # Design value of the load.

# Geometry
h= 270e-3 # Beam cross-section depth expressed in meters.
tw= 6.6e-3 # Web thickness expressed in meters.
a= 4e-3 # Throat depth expressed in meters.
l= 5.15925e-2 # Weld beads length.

# Material properties
fy= 275e6 # S-275-JR steel
fyd= fy/1.1 # Strength reduction factor.
fu= 430e6 # Steel strength of the pieces to be welded S-275-JR (table 59.8.2 page 304 EAE).
Es= 2e11 # Elastic modulus of the steel.

# Safety data
betaW= 0.8 # Correlation factor for S-275 steel (table 59.8.2 page 304 EAE).
gammaMw= 1.0/math.sqrt(3) # Steel partial safety factor for joints (clause 59.8.2 page 304 EAE).



# Partial results.
VRd= EAE_limit_state_checking.VdSoldAlma(l,a,betaW,gammaMw,2600*9.81/1e-4)
lMax= EAE_limit_state_checking.LongMaxSoldAlma(tw)

ratio1= abs(((VRd-Vd)/VRd))

'''
print "Weld bead length l= ",l*1000,"mm \n"
print "Maximum lenght of the weld beads lMax= ",lMax*1000," mm\n"
print "Fillet weld throat a= ",a*1000,"mm \n"
print "Shear force la Vd= ",Vd/1000," kN\n"
print "Shear resistance of the joint VRd= ",VRd/1000," kN\n"
print "VRd/Vd= ",VRd/Vd
print "ratio1= ",ratio1
'''

import os
from miscUtils import log_messages as lmsg
fname= os.path.basename(__file__)
if (abs(ratio1)<0.1):
  print "test ",fname,": ok."
else:
  lmsg.error(fname+' ERROR.')

