# -*- coding: utf-8 -*-
''' Calculation of mass properties for a set of geometric objects.'''

__author__= "Luis C. Pérez Tato (LCPT) , Ana Ortega (A_OO) "
__copyright__= "Copyright 2026, LCPT, A_OO"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com, ana.Ortega.Ort@gmail.com "

import geom

def get_center_of_mass_2d(factoredGeomObjList):
    ''' Compute the center of mass of the given list of 2D geometric object.
        Each list item must be a tuple of the form (factor, geomObj) where
        factor multiplies the area of the given object in the computation of the
        center of mass of the set.

    :param factoredGeomList: list of tuples of the form [(f0, geomObj0),
                             (f1, geomObj1), ..., (fN, geomObjN)].
    '''
    x0= 0.0
    y0= 0.0
    totalArea= 0.0
    for (f, geomObj) in factoredGeomObjList:
        area= geomObj.getArea()
        fArea= f*area
        centroid= geomObj.getCenterOfMass()
        totalArea+= fArea
        x0+= centroid.x*fArea
        y0+= centroid.y*fArea
    x0/=totalArea
    y0/=totalArea
    return geom.Pos2d(x0, y0)

def get_inertia_2d(factoredGeomObjList):
    ''' Compute the Ix, Iy, Pxy moments of inertia of the given list of 
        2D geometric objects. Each list item must be a tuple of the form
        (factor, geomObj) where factor multiplies the area and inertia of the
        given object in the computation.

    :param factoredGeomList: list of tuples of the form [(f0, geomObj0),
                             (f1, geomObj1), ..., (fN, geomObjN)].
    :returns: (Ix, Iy, Pxy)
    '''
    centroid= get_center_of_mass_2d(factoredGeomObjList)
    x0= centroid.x
    y0= centroid.y
    totalArea= 0.0
    IX= 0.0
    IY= 0.0
    PXY= 0.0
    for (f, geomObj) in factoredGeomObjList:
        fIx= f*geomObj.getIx()
        fIy= f*geomObj.getIy()
        fPxy= f*geomObj.getPxy()
        fArea= f*geomObj.getArea()
        centroid= geomObj.getCenterOfMass()
        IX+= fIx+fArea*(centroid.x-x0)**2
        IY+= fIy+fArea*(centroid.y-y0)**2
        PXY+= fPxy+fArea*(centroid.x-x0)*(centroid.y-y0)
    return (IX, IY, PXY)
        
