# -*- coding: utf-8 -*-

__author__= "Luis C. Pérez Tato (LCPT) Ana Ortega (AOO)"
__copyright__= "Copyright 2015, LCPT AOO"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com  ana.Ortega.Ort@gmail.com"

import sys

from misc_utils import log_messages as lmsg
from vtk.vtkCommonCore import vtkStringArray

def VtkCreaStrArraySetData(setToDraw, entTypeName, attr):
    '''Creates an array of strings with information associated 
    to the points and cells.

    :param setToDraw:   set of entities to be displayed
    :param entTypeName: type of entities to be displayed 
                        ("pnts", "lines", "nodes", "elementos")
    :param attr: attribute to be stored in the array
    '''
    # Define matrix
    arr= vtkStringArray()
    arr.SetName(attr)
    # Select container.
    container= None
    if(entTypeName=="pnts"):
        container= setToDraw.getPoints
    elif(entTypeName=="lines"):
        container= setToDraw.getLines
    elif(entTypeName=="nodes"):
        container= setToDraw.nodes
    elif(entTypeName=="elementos"):
        container= setToDraw.elements
    else:
        methodName= sys._getframe(0).f_code.co_name
        lmsg.error(methodName+"; error: "+entTypeName+" not implemented.")
    # Populate array.
    for e in container:
        tmp= str(getattr(e,attr))
        arr.InsertValue(e.getIdx,tmp)
    return arr
