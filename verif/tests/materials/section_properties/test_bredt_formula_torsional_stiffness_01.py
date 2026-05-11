''' Compute torsional constant of a rectangular box 100x50 mm, 2mm 
thickness using Bredt's formula.
'''

from materials.sections import torsional_stiffness

# External area.
A_e = 100e-3 * 50e-3 
# Four segments: two of 100mm, two of 50mm
lengths = 2*[100e-3, 50e-3]
thicknesses = 4*[2e-3]

J = torsional_stiffness.calculate_bredt_torsion(A_e, lengths, thicknesses)
Jref= 2e-6/3
ratio1= abs(J-Jref)/Jref

'''
print(f"Torsional Constant J_t: {J} mm^4")
print("ratio1= ", ratio1)
'''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if (abs(ratio1)<1e-12):
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')
