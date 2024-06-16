 # -*- coding: utf-8 -*-

''' Diagram display a property defined over linear elements. '''

__author__= "Luis C. Pérez Tato (LCPT) , Ana Ortega (AO_O) "
__copyright__= "Copyright 2016, LCPT, AO_O"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@ciccp.es, ana.ortega@ciccp.es "


# from misc_utils import log_messages as lmsg
from postprocess.xcVtk.diagrams import property_diagram as pd
from postprocess import extrapolate_elem_attr

class ElementPropertyDiagram(pd.PropertyDiagram):
    '''Diagram of element properties'''
    envelopes= set(['N+','N-','My+','My-','Mz+','Mz-','Vy+','Vy-','Vz+','Vz-','T+','T-'])

    def __init__(self,scaleFactor, lRefModSize, fUnitConv, sets, propertyName, rgMinMax= None):
        '''Diagram that represents a property value over several sets of elements.

        :param scaleFactor: scale factor (size of the diagram).
        :param lRefModSize: reference length of the model (how big the model is).
        :param fUnitConv: unit conversion factor (i.e N->kN => fUnitConv= 1e-3).
        :param propertyName: name of the element's property to represent.
        :param sets: represent the field over those element sets.
        :param rgMinMax: range (vmin,vmax) with the maximum and minimum values  
                           of the scalar field (if any) to be represented. All 
                           the values less than vmin are displayed in blue and 
                           those greater than vmax in red (defaults to None)
        '''
        super(ElementPropertyDiagram,self).__init__(scaleFactor= scaleFactor, lRefModSize= lRefModSize, fUnitConv= fUnitConv, sets= sets, attributeName= attributeName, rgMinMax= rgMinMax)

    def computeDiagramValues(self, eSet):
        ''' Return the values needed to create the diagram representation.'''
        self.valueCouples= list()
        self.elements= list()
        if(self.propertyName=='FCTNCP'):
            for e in elems:
                self.vDir= e.getJVector3d(True) #initialGeometry= True
                values= e.getProp(self.propertyName) # [back node value, front node value]
                self.elements.append(e)
                self.valueCouples.append((values[0],values[1]))
        elif(self.propertyName=='FCVCP'):
            for e in elems:
                self.vDir= e.getJVector3d(True) #initialGeometry= True
                values= e.getProp(self.propertyName) # [back node value, front node value]
                self.elements.append(e)
                self.valueCouples.append((values[0],values[1]))      
        elif(self.propertyName=='N+'):
            for e in elems:
                self.vDir= e.getJVector3d(True) #initialGeometry= True
                values= e.getProp(self.propertyName) # [back node value, front node value]
                self.elements.append(e)
                self.valueCouples.append((values[0],values[1]))      
        elif(self.propertyName=='N-'):
            for e in elems:
                self.vDir= e.getJVector3d(True) #initialGeometry= True
                values= e.getProp(self.propertyName) # [back node value, front node value]
                self.elements.append(e)
                self.valueCouples.append((values[0],values[1]))      
        elif(self.propertyName=='My+'):
            for e in elems:
                self.vDir= e.elem.getKVector3d(True) # initialGeometry= True  
                values= e.getProp(self.propertyName) # [back node value, front node value]
                self.elements.append(e)
                self.valueCouples.append((values[0],values[1]))      
        elif(self.propertyName=='My-'):
            for e in elems:
                self.vDir= e.elem.getKVector3d(True) # initialGeometry= True  
                values= e.getProp(self.propertyName) # [back node value, front node value]
                self.elements.append(e)
                self.valueCouples.append((values[0],values[1]))      
        elif(self.propertyName=='Mz+'):
            for e in elems:
                self.vDir= e.elem.getJVector3d(True) # initialGeometry= True  
                values= e.getProp(self.propertyName) # [back node value, front node value]
                valueCouples.append((values[0],values[1]))      
        elif(self.propertyName=='Mz-'):
            for e in elems:
                self.vDir= e.elem.getJVector3d(True) # initialGeometry= True  
                values= e.getProp(self.propertyName) # [back node value, front node value]
                self.elements.append(e)
                self.valueCouples.append((values[0],values[1]))      
        elif(self.propertyName=='Vy+'):
            for e in elems:
                self.vDir= e.elem.getJVector3d(True) # initialGeometry= True  
                values= e.getProp(self.propertyName) # [back node value, front node value]
                self.elements.append(e)
                self.valueCouples.append((values[0],values[1]))      
        elif(self.propertyName=='Vy-'):
            for e in elems:
                self.vDir= e.elem.getJVector3d(True) # initialGeometry= True  
                values= e.getProp(self.propertyName) # [back node value, front node value]
                self.elements.append(e)
                self.valueCouples.append((values[0],values[1]))      
        elif(self.propertyName=='Vz+'):
            for e in elems:
                self.vDir= e.elem.getKVector3d(True) # initialGeometry= True  
                values= e.getProp(self.propertyName) # [back node value, front node value]
                self.elements.append(e)
                self.valueCouples.append((values[0],values[1]))      
        elif(self.propertyName=='Vz-'):
            for e in elems:
                self.vDir= e.elem.getKVector3d(True) # initialGeometry= True  
                values= e.getProp(self.propertyName) # [back node value, front node value]
                self.elements.append(e)
                self.valueCouples.append((values[0],values[1]))      
        else:
            elems= eSet.elements
            extrapolate_elem_attr.extrapolate_elem_function_attr(elems, self.propertyName, "getProp", self.propertyName)
            for e in elems:
                self.vDir= e.getJVector3d(True) #initialGeometry= True
                v0= e.getNodes[0].getProp(self.propertyName)
                v1= e.getNodes[1].getProp(self.propertyName)
                self.elements.append(e)
                self.valueCouples.append((v0,v1))
        if(self.rgMinMax): # filter values.
            self.valueCouples= self.filterValueCouples(self.valueCouples)
        self.autoScale() # Update scale.

