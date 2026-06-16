# -*- coding: utf-8 -*-
''' Home made test. Test Arcelor structural steel shapes properties.'''

from __future__ import print_function

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AOO)"
__copyright__= "Copyright 2021, LCPT and AOO"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@ciccp.es"

from materials.sections.structural_shapes import arcelor_metric_shapes

shapes= list()
test= list()

shapes.append(arcelor_metric_shapes.find_nearest_steel_shape_by_depth('IPE_.*', 200e-3))
test.append((shapes[-1]['nmb']=='IPE_200'))
shapes.append(arcelor_metric_shapes.find_nearest_steel_shape_by_depth('IPE_A.*', 320e-3))
test.append((shapes[-1]['nmb']=='IPE_A_330'))
shapes.append(arcelor_metric_shapes.find_nearest_steel_shape_by_depth('IPN.*', 125e-3))
test.append((shapes[-1]['nmb']=='IPN_120'))
shapes.append(arcelor_metric_shapes.find_nearest_steel_shape_by_depth('HE.*AAdot', 225e-3))
test.append((shapes[-1]['nmb']=='HE_240_AAdot'))
shapes.append(arcelor_metric_shapes.find_nearest_steel_shape_by_depth('HL.*Rdot', 1125e-3))
test.append((shapes[-1]['nmb']=='HL_1100_Rdot'))
shapes.append(arcelor_metric_shapes.find_nearest_steel_shape_by_depth('UPN_.*', 70e-3))
test.append((shapes[-1]['nmb']=='UPN_80'))
shapes.append(arcelor_metric_shapes.find_nearest_steel_shape_by_depth('L.*', 41e-3))
test.append((shapes[-1]['nmb']=='L40x40x4'))
shapes.append(arcelor_metric_shapes.find_nearest_steel_shape_by_depth('AU.*', 442e-3))
test.append((shapes[-1]['nmb']=='AU_18'))

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
