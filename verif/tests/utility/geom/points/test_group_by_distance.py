# -*- coding: utf-8 -*-
''' Group points by distance.'''

import geom
from geom_utils import group_by_distance as gbd

__author__= "Luis C. Pérez Tato (LCPT)"
__cppyright__= "Copyright 2025 LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@ciccp.es"

points3d= [geom.Pos3d(4366.47, 5276.77, 10.7786), geom.Pos3d(4367.18, 5275.78, 10.3901), geom.Pos3d(4366.47, 5276.77, 10.3786), geom.Pos3d(4367.18, 5275.78, 9.29012), geom.Pos3d(4367.18, 5275.78, 8.74012), geom.Pos3d(4365.84, 5274.04, 10.6963), geom.Pos3d(4363.13, 5270.52, 10.4088), geom.Pos3d(4365.84, 5274.04, 10.3963), geom.Pos3d(4363.13, 5270.52, 9.30876), geom.Pos3d(4365.84, 5274.04, 9.29627), geom.Pos3d(4363.13, 5270.52, 8.75876), geom.Pos3d(4362.76, 5270.04, 10.7105), geom.Pos3d(4362.06, 5271.04, 10.399), geom.Pos3d(4362.76, 5270.04, 10.4105), geom.Pos3d(4362.06, 5271.04, 9.29897), geom.Pos3d(4362.76, 5270.04, 9.31047), geom.Pos3d(4362.18, 5269.29, 10.4131), geom.Pos3d(4362.18, 5269.29, 9.3131), geom.Pos3d(4362.18, 5269.29, 8.7631), geom.Pos3d(4366.61, 5275.04, 10.6927), geom.Pos3d(4366.61, 5275.04, 10.3927), geom.Pos3d(4366.61, 5275.04, 9.29272)]

points2d= list()
for p in points3d:
    points2d.append(geom.Pos2d(p.x, p.y))

groups3d= gbd.group_points3d_by_distance(points3d, .1)
groups2d= gbd.group_points2d_by_distance(points2d, .1)

groups3dRef= {0: [0], 1: [1], 2: [2], 3: [3], 4: [4], 5: [5], 6: [6], 7: [7], 8: [8], 9: [9], 10: [10], 11: [11], 12: [12], 13: [13], 14: [14], 15: [15], 16: [16], 17: [17], 18: [18], 19: [19], 20: [20], 21: [21]}
groups2dRef= {0: [0, 2], 1: [1, 3, 4], 5: [5, 7, 9], 6: [6, 8, 10], 11: [11, 13, 15], 12: [12, 14], 16: [16, 17, 18], 19: [19, 20, 21]}

testOK= ((groups3d==groups3dRef) and (groups2d==groups2dRef))

'''
print(groups3d, len(groups3d))
print(groups2d, len(groups2d))
'''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if testOK:
    print('test: '+fname+': ok.')
else:
    lmsg.error(fname+": ERROR.")
