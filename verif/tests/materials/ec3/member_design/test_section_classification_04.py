# -*- coding: utf-8 -*-
'''Classification of steel plate (flat element) (clause 5.5 of 
EN 1993-1-1:2005).
'''

__author__= "Luis C. Pérez Tato (LCPT)"
__copyright__= "Copyright 2026, LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

from materials.ec3 import EC3_materials

S235JR= EC3_materials.S235JR
S235JR.gammaM= 1.00

thinPlateShape= EC3_materials.PlateShape(steel=S235JR, name='#200x1', thickness= 1e-3, width= 0.2)
thin_plate_c_compression= thinPlateShape.getClassInCompression()
thin_plate_c_bending= thinPlateShape.getClassInBending()

mediumPlateShape= EC3_materials.PlateShape(steel=S235JR, name='#200x2.5', thickness= 2.5e-3, width= 0.2)
medium_plate_c_compression= mediumPlateShape.getClassInCompression()
medium_plate_c_bending= mediumPlateShape.getClassInBending()

thickPlateShape= EC3_materials.PlateShape(steel=S235JR, name='#200x20', thickness= 20e-3, width= 0.2)
thick_plate_c_compression= thickPlateShape.getClassInCompression()
thick_plate_c_bending= thickPlateShape.getClassInBending()

testOK= (thin_plate_c_compression==4) and (thin_plate_c_bending== 4) and (medium_plate_c_compression==4) and (medium_plate_c_bending== 2) and (thick_plate_c_compression==3) and (thick_plate_c_bending== 1)

'''
print(thin_plate_c_compression)
print(thin_plate_c_bending)
print(medium_plate_c_compression)
print(medium_plate_c_bending)
print(thick_plate_c_compression)
print(thick_plate_c_bending)
print(testOK)
'''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if testOK:
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')



