# -*- coding: utf-8 -*-
''' Display of diagrams (forces, moments, ...) on linear elements'''

from __future__ import division
from __future__ import print_function

__author__= "Luis C. Pérez Tato (LCPT) , Ana Ortega (AO_O) "
__copyright__= "Copyright 2016, LCPT, AO_O"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@ciccp.es, ana.ortega@ciccp.es "

from postprocess.xcVtk.diagrams import control_var_diagram as cvd
from misc_utils import log_messages as lmsg

class InternalForceDiagram(cvd.ControlVarDiagram):
    '''Diagram of results in beam elements from a limit state verification file.


    '''
    def __init__(self, scaleFactor, lRefModSize, fUnitConv, sets, attributeName, component, defaultDirection= 'J', rgMinMax= None):
        ''' Diagram constructor

        :param scaleFactor: scale factor for the diagram (can be negative too).
        :param lRefModSize: reference length of the model (how big the model is).
        :param fUnitConv: unit conversion factor (i.e N->kN => fUnitConv= 1e-3).
        :param sets:      list of element sets for which the diagram will be displayed.
        :param attributeName: name of the property that containst the control
                              variable.
        :param component: component of the control variable to be displayed 
                         (possible arguments: 'N', 'My', 'Mz'Vz,...).
        :param defaultDirection: default direction of the diagram (J: element local
                                 j vector or K: element local K vector).
        :param rgMinMax: range (vmin,vmax) with the maximum and minimum values  
                         of the scalar field (if any) to be represented. All 
                         the values less than vmin are displayed in blue and 
                         those greater than vmax in red (defaults to None)
        '''
        super(InternalForceDiagram,self).__init__(scaleFactor= scaleFactor, fUnitConv= fUnitConv, sets= sets, attributeName= attributeName, component= component, defaultDirection= defaultDirection, rgMinMax= rgMinMax, lRefModSize= lRefModSize)
