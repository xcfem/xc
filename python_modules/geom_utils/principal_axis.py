# -*- coding: utf-8 -*-
"""
Computes principal axes of a point cloud. Original authors:
"Pierre Poulain" , "Justine Guegan", "Edithe Selwa", "Steven C. Howell"
"""
from __future__ import division
from __future__ import print_function

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AO_O)"
__copyright__= "Copyright 2015, LCPT and AO_O"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com" "ana.ortega.ort@gmail.com"


import numpy
import geom

def get_principal_axis_3D(points):
    ''' Compute principal axes

    :param points: 3D point cloud.
    '''
    #create coordinates array
    coord= numpy.array(points, float)

    # compute geometric center
    center= numpy.mean(coord, 0)

    # center with geometric center
    coord= coord - center

    # compute principal axis matrix
    inertia = numpy.dot(coord.transpose(), coord)
    e_values, e_vectors = numpy.linalg.eig(inertia)
    # warning eigen values are not necessary ordered!
    # http://docs.scipy.org/doc/numpy/reference/generated/numpy.linalg.eig.html

    #--------------------------------------------------------------------------
    # order eigen values (and eigen vectors)
    #
    # axis1 is the principal axis with the biggest eigen value (eval1)
    # axis2 is the principal axis with the second biggest eigen value (eval2)
    # axis3 is the principal axis with the smallest eigen value (eval3)
    #--------------------------------------------------------------------------
    order = numpy.argsort(e_values)
    eval3, eval2, eval1 = e_values[order]
    axis3, axis2, axis1 = e_vectors[:, order].transpose()

    # scale factor to enhance the length of axis in Pymol
    scale_factor = 20
    #--------------------------------------------------------------------------
    # center axes to the geometric center of the molecule
    # and rescale them by order of eigen values
    #--------------------------------------------------------------------------
    # the large vector is the first principal axis
    v1= 3 * scale_factor * axis1
    # the medium vector is the second principal axis
    v2= 2 * scale_factor * axis2
    # the small vector is the third principal axis
    v3= 1 * scale_factor * axis3
    return geom.Ref3d3d(geom.Pos3d(center[0],center[1],center[2]),geom.Vector3d(v1[0],v1[1],v1[2]),geom.Vector3d(v2[0],v2[1],v2[2]),geom.Vector3d(v3[0],v3[1],v3[2]))
    
