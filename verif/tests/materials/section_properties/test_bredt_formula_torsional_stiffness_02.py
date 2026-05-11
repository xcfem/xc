''' Compute torsional constant of a rectangular box 100x50 mm, 2mm 
thickness using Bredt's formula.
'''

import geom
from materials.sections import torsional_stiffness

# External area.
extPlg= geom.Polygon2d([geom.Pos2d(-50e-3, -25e-3), geom.Pos2d(50e-3, -25e-3), geom.Pos2d(50e-3, 25e-3), geom.Pos2d(-50e-3, 25e-3)])
intPlg= geom.Polygon2d([geom.Pos2d(-48e-3, -23e-3), geom.Pos2d(48e-3, -23e-3), geom.Pos2d(48e-3, 23e-3), geom.Pos2d(-48e-3, 23e-3)])

J= torsional_stiffness.estimate_bred_torsion_for_polygonal_section(extPlg, intPlg)
Jref= 2e-6/3
ratio1= abs(J-Jref)/Jref

'''
print(f"Torsional Constant J: {J} m^4")
print('Reference value J_ref: ', Jref, 'm^4')
print("ratio1= ", ratio1)
'''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if (abs(ratio1)<.1):
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')
