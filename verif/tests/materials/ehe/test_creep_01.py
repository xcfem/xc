# -*- coding: utf-8 -*-
''' Computing of the creep factor according to EHE-08. '''

from materials.ehe import EHE_materials
import math

__author__= "Luis C. Pérez Tato (LCPT)"
__copyright__= "Copyright 2014, LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

def sqr(a):
  return a*a

# 30 MPa, 1 día.
fi1= EHE_materials.HA30.getCreepFitt0(10e3,1,50,50)
err2= sqr(fi1-5.6)
fi2= EHE_materials.HA30.getCreepFitt0(10e3,1,50,600)
err2= err2+sqr(fi2-3.8)
fi3= EHE_materials.HA30.getCreepFitt0(10e3,1,70,50)
err2= err2+sqr(fi3-4.3)
fi4= EHE_materials.HA30.getCreepFitt0(10e3,1,70,600)
err2= err2+sqr(fi4-3.3)
fi5= EHE_materials.HA30.getCreepFitt0(10e3,1,90,50)
err2= err2+sqr(fi5-3.1)
fi6= EHE_materials.HA30.getCreepFitt0(10e3,1,90,600)
err2= err2+sqr(fi6-2.7)

# 30 MPa, 28 días.
fiB1= EHE_materials.HA30.getCreepFitt0(10e3,28,50,50)
err2= err2+sqr(fiB1-3)
fiB2= EHE_materials.HA30.getCreepFitt0(10e3,28,50,600)
err2= err2+sqr(fiB2-2)
fiB3= EHE_materials.HA30.getCreepFitt0(10e3,28,70,50)
err2= err2+sqr(fiB3-2.3)
fiB4= EHE_materials.HA30.getCreepFitt0(10e3,28,70,600)
err2= err2+sqr(fiB4-1.7)
fiB5= EHE_materials.HA30.getCreepFitt0(10e3,28,90,50)
err2= err2+sqr(fiB5-1.6)
fiB6= EHE_materials.HA30.getCreepFitt0(10e3,28,90,600)
err2= err2+sqr(fiB6-1.5)

# 30 MPa, 1800 días.
fiC1= EHE_materials.HA30.getCreepFitt0(10e3,1800,50,50)
err2= err2+sqr(fiC1-1.3)
fiC2= EHE_materials.HA30.getCreepFitt0(10e3,1800,50,600)
err2= err2+sqr(fiC2-0.9)
fiC3= EHE_materials.HA30.getCreepFitt0(10e3,1800,70,50)
err2= err2+sqr(fiC3-1.0)
fiC4= EHE_materials.HA30.getCreepFitt0(10e3,1800,70,600)
err2= err2+sqr(fiC4-0.8)
fiC5= EHE_materials.HA30.getCreepFitt0(10e3,1800,90,50)
err2= err2+sqr(fiC5-0.7)
fiC6= EHE_materials.HA30.getCreepFitt0(10e3,1800,90,600)
err2= err2+sqr(fiC6-0.7)


# 70 MPa, 1 día.
fi71= EHE_materials.HA70.getCreepFitt0(10e3,1,50,50)
err2= err2+sqr(fi71-2.6)
fi72= EHE_materials.HA70.getCreepFitt0(10e3,1,50,600)
err2= err2+sqr(fi72-2.0)
fi73= EHE_materials.HA70.getCreepFitt0(10e3,1,70,50)
err2= err2+sqr(fi73-2.2)
fi74= EHE_materials.HA70.getCreepFitt0(10e3,1,70,600)
err2= err2+sqr(fi74-1.8)
fi75= EHE_materials.HA70.getCreepFitt0(10e3,1,90,50)
err2= err2+sqr(fi75-1.7)
fi76= EHE_materials.HA70.getCreepFitt0(10e3,1,90,600)
err2= err2+sqr(fi76-1.6)


# 70 MPa, 28 días.
fi7B1= EHE_materials.HA70.getCreepFitt0(10e3,28,50,50)
err2= err2+sqr(fi7B1-1.4)
fi7B2= EHE_materials.HA70.getCreepFitt0(10e3,28,50,600)
err2= err2+sqr(fi7B2-1.1)
fi7B3= EHE_materials.HA70.getCreepFitt0(10e3,28,70,50)
err2= err2+sqr(fi7B3-1.2)
fi7B4= EHE_materials.HA70.getCreepFitt0(10e3,28,70,600)
err2= err2+sqr(fi7B4-1.0)
fi7B5= EHE_materials.HA70.getCreepFitt0(10e3,28,90,50)
err2= err2+sqr(fi7B5-0.9)
fi7B6= EHE_materials.HA70.getCreepFitt0(10e3,28,90,600)
err2= err2+sqr(fi7B6-0.8)

# 70 MPa, 1800 días.
fi7C1= EHE_materials.HA70.getCreepFitt0(10e3,1800,50,50)
err2= err2+sqr(fi7C1-0.6)
fi7C2= EHE_materials.HA70.getCreepFitt0(10e3,1800,50,600)
err2= err2+sqr(fi7C2-0.5)
fi7C3= EHE_materials.HA70.getCreepFitt0(10e3,1800,70,50)
err2= err2+sqr(fi7C3-0.5)
fi7C4= EHE_materials.HA70.getCreepFitt0(10e3,1800,70,600)
err2= err2+sqr(fi7C4-0.4)
fi7C5= EHE_materials.HA70.getCreepFitt0(10e3,1800,90,50)
err2= err2+sqr(fi7C5-0.4)
fi7C6= EHE_materials.HA70.getCreepFitt0(10e3,1800,90,600)
err2= err2+sqr(fi7C6-0.4)

ratio1= math.sqrt(err2)

'''
print "fi1= ",fi1
print "fi2= ",fi2
print "fi3= ",fi3
print "fi4= ",fi4
print "fi5= ",fi5
print "fi6= ",fi6,"\n\n"

print "fiB1= ",fiB1
print "fiB2= ",fiB2
print "fiB3= ",fiB3
print "fiB4= ",fiB4
print "fiB5= ",fiB5
print "fiB6= ",fiB6,"\n\n"

print "fiC1= ",fiC1
print "fiC2= ",fiC2
print "fiC3= ",fiC3
print "fiC4= ",fiC4
print "fiC5= ",fiC5
print "fiC6= ",fiC6,"\n\n"

print "fi71= ",fi71
print "fi72= ",fi72
print "fi73= ",fi73
print "fi74= ",fi74
print "fi75= ",fi75
print "fi76= ",fi76,"\n\n"

print "fi7B1= ",fi7B1
print "fi7B2= ",fi7B2
print "fi7B3= ",fi7B3
print "fi7B4= ",fi7B4
print "fi7B5= ",fi7B5
print "fi7B6= ",fi7B6,"\n\n"

print "fi7C1= ",fi7C1
print "fi7C2= ",fi7C2
print "fi7C3= ",fi7C3
print "fi7C4= ",fi7C4
print "fi7C5= ",fi7C5
print "fi7C6= ",fi7C6,"\n\n"

print "err2= ",err2
print "ratio1= ",(ratio1)
   '''

import os
from miscUtils import LogMessages as lmsg
fname= os.path.basename(__file__)
if (ratio1<0.4):
  print "test ",fname,": ok."
else:
  lmsg.error(fname+' ERROR.')
