# -*- coding: utf-8 -*-
''' Home made test. Test Arcelor structural steel shapes properties.'''

from __future__ import print_function

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AOO)"
__copyright__= "Copyright 2021, LCPT and AOO"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@ciccp.es"

from materials.sections.structural_shapes import en_10210_shapes

shapes= list()
test= list()

# shapes.append(en_10210_shapes.find_nearest_steel_shape_by_depth('SHS.*', 80e-3))
# test.append((shapes[-1]['nmb']=='SHS80x80x2'))
shapes.append(en_10210_shapes.find_nearest_steel_shape_by_depth('RHS.*', 80e-3))
test.append((shapes[-1]['nmb']=='RHS80x40x2'))
#shapes.append(en_10210_shapes.find_nearest_steel_shape_by_depth('CHS.*', 80e-3))
#test.append((shapes[-1]['nmb']=='CHS80x40x2'))

cumple= True
for sh, t in zip(shapes,test):
    cumple= cumple and t
    # print(sh['nmb'], t, cumple)

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if cumple:
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')
