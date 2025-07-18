# -*- coding: utf-8 -*-
''' Group points by distance.'''


from scipy.spatial.distance import cdist
from scipy.spatial.distance import pdist

__author__= "Luis C. Pérez Tato (LCPT)"
__cppyright__= "Copyright 2025 LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@ciccp.es"


def group_points3d_by_distance(points, threshold):
    ''' Return group of points that are near than the given threshold. The 
        points are identified by its index in the given point list.

    :param points: list of points.
    :param threshold: upper limit of the distance to consider the points in the
                      same group.
    '''
    sz= len(points)
    coords= list()
    for p in points:
        coords.append([p.y, p.y, p.z])
    distances= cdist(coords, coords)
    groups= dict()
    already_grouped= set()
    for i in range(0,sz):
        if(i not in already_grouped):
            if(i not in groups):
                groups[i]= list()
                for j in range(i,sz):
                    if(j not in already_grouped):
                        d= distances[i][j]
                        if(d<threshold):
                            groups[i].append(j)
                            already_grouped.add(j)
    return groups

def group_points2d_by_distance(points, threshold):
    ''' Return group of points that are near than the given threshold. The 
        points are identified by its index in the given point list.

    :param points: list of points.
    :param threshold: upper limit of the distance to consider the points in the
                      same group.
    '''
    sz= len(points)
    coords= list()
    for p in points:
        coords.append([p.y, p.y])
    distances= cdist(coords, coords)
    groups= dict()
    already_grouped= set()
    for i in range(0,sz):
        if(i not in already_grouped):
            if(i not in groups):
                groups[i]= list()
                for j in range(i,sz):
                    if(j not in already_grouped):
                        d= distances[i][j]
                        if(d<threshold):
                            groups[i].append(j)
                            already_grouped.add(j)
    return groups
