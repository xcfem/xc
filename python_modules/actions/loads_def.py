# -*- coding: utf-8 -*-


__author__= "Ana Ortega (AO_O)"
__copyright__= "Copyright 2017, AO_O"
__license__= "GPL"
__version__= "3.0"
__email__= "ana.Ortega@ciccp.es"

import xc_base
import geom
import xc

class InertialLoad(object):
    '''Inertial load (density*acceleration) applied to the elements in the list
    of mesh-sets

    :ivar name:     name identifying the load
    :ivar lstMeshSets: list of mesh-sets (of type LinSetToMesh or SurfSetToMesh)
    :ivar vAccel:   acceleration vector xc.Vector([ax,ay,az])
    '''
    def __init__(self,name, lstMeshSets, vAccel):
        self.name=name
        self.lstMeshSets=lstMeshSets
        self.vAccel= vAccel

    def appendLoadToCurrentLoadPattern(self):
        for ms in self.lstMeshSets:
            if 'shell' in ms.elemType:
                loadVector=ms.matSect.getAreaDensity()*self.vAccel
                el_group=ms.surfSet.getElements
            elif 'beam' in ms.elemType:
                loadVector=ms.matSect.getLongitudinalDensity()*self.vAccel
                el_group=ms.linSet.getElements
            for e in el_group:
                e.vector3dUniformLoadGlobal(loadVector)

            


