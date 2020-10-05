# -*- coding: utf-8 -*-

''' Display of diagrams (forces, moments, ...) on linear elements'''

__author__= "Ana Ortega (AO_O) and Luis C. Pérez Tato (LCPT)"
__copyright__= "Copyright 2015, AO_O and LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= " ana.Ortega@ciccp.es, l.pereztato@ciccp.es"

import geom
import vtk
from postprocess.xcVtk.diagrams import colored_diagram as cd
from postprocess import control_vars as cv
from misc_utils import log_messages as lmsg

class InternalForceDiagram(cd.ColoredDiagram):
    '''Diagram of internal forces (N,My,Mz,T,Vy,Vz)'''
    def __init__(self,scale,fUnitConv,sets,component):
        ''' Diagram constructor

            :ivar scale:     scale factor for the diagram (can be negative too).
            :ivar fUnitConv: unit conversion factor (i.e N->kN => fUnitConv= 1e-3).
            :ivar sets:      list of element sets for which the diagram will be displayed.
            :ivar component: property to be displayed 
                             (possible arguments: 'N', 'My', 'Mz'Vz,...)
        '''

        super(InternalForceDiagram,self).__init__(scale,fUnitConv)
        self.conjuntos= sets
        self.component= component

    def getElementComponentData(self,elem, silent= False):
        '''Returns the data to use to represent the diagram over the element

           :param elem: element to deal with.
           :param component: component to represent:
           :param silent: if true don't complain about non-existent properties.
        '''
        # this code seems not used anymore 17/05/2020 LCPT
        elemVDir= elem.getJVector3d(True) #initialGeometry= True
        values= elem.getValuesAtNodes(self.component, silent)
        if((self.component == 'Qy') or (self.component == 'Vy')):
            elemVDir= elem.getJVector3d(True) # initialGeometry= True 
        elif((self.component == 'Qz') or (self.component == 'Vz')):
            elemVDir= elem.getKVector3d(True) # initialGeometry= True 
        elif(self.component == 'My'):
            elemVDir= elem.getKVector3d(True) # initialGeometry= True 
        elif(self.component == 'Mz'):
            elemVDir= elem.getJVector3d(True) # initialGeometry= True 
        else:
          lmsg.warning("'component :'"+ self.component+ "' unknown.")
        return [elemVDir,values[0],values[1]]
    

    def addDiagram(self):
        self.creaEstrucDatosDiagrama()
        self.creaLookUpTable()
        self.creaActorDiagrama()

        indxDiagrama= 0
        indiceSet= 0
        numSetsDiagrama= len(self.conjuntos)
        for s in self.conjuntos:
          for e in s.elements:
            e.getResistingForce()
            componentData= self.getElementComponentData(e)
            indxDiagrama= self.appendDataFromElementEnds(componentData[0],e,indxDiagrama,componentData[1],componentData[2])

        self.updateLookUpTable()
        self.updateActorDiagrama()



