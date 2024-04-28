# -*- coding: utf-8 -*-
''' Display of diagrams (forces, moments, ...) on linear elements'''

__author__= "Ana Ortega (AO_O) and Luis C. Pérez Tato (LCPT)"
__copyright__= "Copyright 2015, AO_O and LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= " ana.Ortega@ciccp.es, l.pereztato@ciccp.es"

from postprocess.xcVtk.diagrams import colored_diagram as cd
from postprocess import control_vars as cv
# from misc_utils import log_messages as lmsg

class InternalForceDiagram(cd.ColoredDiagram):
    '''Diagram of internal forces (N,My,Mz,T,Vy,Vz)

    :ivar sets:      list of element sets for which the diagram will be displayed.
    :ivar component: property to be displayed 
                     (possible arguments: 'N', 'My', 'Mz'Vz,...)
    :ivar defaultDirection: default direction of the diagram (J: element local
                            j vector or K: element local K vector).
    '''
    def __init__(self,scale,fUnitConv,sets,component, defaultDirection= 'J'):
        ''' Diagram constructor

        :param scale:     scale factor for the diagram (can be negative too).
        :param fUnitConv: unit conversion factor (i.e N->kN => fUnitConv= 1e-3).
        :param sets:      list of element sets for which the diagram will be displayed.
        :param component: property to be displayed 
                         (possible arguments: 'N', 'My', 'Mz'Vz,...)
        :param defaultDirection: default direction of the diagram (J: element
                                 local j vector or K: element local K vector).
        '''

        super(InternalForceDiagram,self).__init__(scale,fUnitConv)
        self.conjuntos= sets
        self.component= component
        self.defaultDirection= defaultDirection

    def getElementComponentData(self,elem, silent= False):
        '''Returns the data to use to represent the diagram over the element

        :param elem: element to deal with.
        :param component: component to represent:
        :param silent: if true don't complain about non-existent properties.
        '''
        # this code seems not used anymore 17/05/2020 LCPT
        return cv.get_element_internal_force_component_data(elem, self.component, self.defaultDirection)

    def addDiagram(self):
        self.creaEstrucDatosDiagrama()
        self.creaLookUpTable()
        self.creaActorDiagrama()

        indxDiagrama= 0
        for s in self.conjuntos:
            for e in s.elements:
                e.getResistingForce()
                componentData= self.getElementComponentData(e)
                indxDiagrama= self.appendDataFromElementEnds(componentData[0],e,indxDiagrama, componentData[1], componentData[2])

        self.updateLookUpTable()
        self.updateActorDiagrama()



