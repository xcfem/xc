# -*- coding: utf-8 -*-
''' Diagram display a attribute defined over linear elements. '''

__author__= "Luis C. Pérez Tato (LCPT) , Ana Ortega (AO_O) "
__copyright__= "Copyright 2016, LCPT, AO_O"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@ciccp.es, ana.ortega@ciccp.es "


# from misc_utils import log_messages as lmsg
from postprocess.xcVtk.diagrams import attribute_diagram as ad
from postprocess import extrapolate_elem_attr

class ElementAttributeDiagram(ad.AttributeDiagram):
    '''Diagram of element properties'''
    envelopes= set(['N+','N-','My+','My-','Mz+','Mz-','Vy+','Vy-','Vz+','Vz-','T+','T-'])

    def __init__(self,scaleFactor, lRefModSize, fUnitConv, sets, attributeName, rgMinMax= None):
        '''Diagram that represents a attribute value over several sets of elements.

        :param scaleFactor: scale factor (size of the diagram).
        :param lRefModSize: reference length of the model (how big the model is).
        :param fUnitConv: unit conversion factor (i.e N->kN => fUnitConv= 1e-3).
        :param attributeName: name of the element's attribute to represent.
        :param sets: represent the field over those element sets.
        :param rgMinMax: range (vmin,vmax) with the maximum and minimum values  
                           of the scalar field (if any) to be represented. All 
                           the values less than vmin are displayed in blue and 
                           those greater than vmax in red (defaults to None)
        '''
        super(ElementAttributeDiagram,self).__init__(scaleFactor= scaleFactor, lRefModSize= lRefModSize, fUnitConv= fUnitConv, sets= sets, attributeName= attributeName, rgMinMax= rgMinMax)

    def computeDiagramValues(self):
        ''' Compute the values needed to create the diagram representation.'''
        self.valueCouples= list()
        self.elements= list()
        self.directions= list()
        filtered_elements= self.filterElements()
        for e in  filtered_elements:
            self.directions.append(e.getJVector3d(True)) #initialGeometry= True
            value= getattr(e, self.featureName) 
            self.elements.append(e)
            self.valueCouples.append((value, value))
        if(self.rgMinMax): # filter values.
            self.valueCouples= self.filterValueCouples(self.valueCouples)
            lmsg.warning('Displayed values have been clipped whitin the range: ('+str(self.rgMinMax[0])+', '+str(self.rgMinMax[1])+"), so they don't correspond to the computed ones.")
        self.autoScale() # Update scale.
        # Return the number of computed values.
        return len(self.valueCouples)

