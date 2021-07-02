# -*- coding: utf-8 -*-
''' Naive bolted plate model.'''

from __future__ import division
from __future__ import print_function

__author__= "Luis C. Pérez Tato (LCPT) , Ana Ortega (AO_O) "
__copyright__= "Copyright 2020, LCPT, AO_O"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@ciccp.es, ana.ortega@ciccp.es "

import math
import json
import importlib
import xc_base
import geom
from import_export import block_topology_entities as bte
from misc_utils import log_messages as lmsg
from connections.steel_connections import plates
  
class Stiffener(plates.Plate):
    ''' Stiffener plates. This class must be code agnostic
        i.e. no AISC, EC3, EAE clauses here.
    '''
    def __init__(self, width, length, thickness= 10e-3, steelType= None):
        ''' Constructor.

        :param width: plate width.
        :param length: plate length.
        :param thickness: plate thickness.
        :param steelType: steel type.
        '''
        super(Stiffener,self).__init__(width= width, length= length, thickness= thickness, steelType= steelType)

    def getContour(self):
        ''' Return the contour points of the plate.
        '''
        weldPline= geom.get_3d_polylines(list(self.weldLines.values()),1e-3)[0]
        print('number of vertices: ', weldPline.getNumVertices())
        weldPline.simplify(0.01)
        print('number of vertices: ', weldPline.getNumVertices())
        return weldPline.getVertexList()
    
    def getObjectTypeAttr(self):
        ''' Return the object type attribute (used in getBlocks).'''
        return 'stiffener'

    
