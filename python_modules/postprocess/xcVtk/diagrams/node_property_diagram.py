 # -*- coding: utf-8 -*-
''' Diagram to display a property defined at nodes over linear elements. '''

from postprocess.xcVtk.diagrams import property_diagram as pd

class NodePropertyDiagram(pd.PropertyDiagram):
    '''Diagram to display a property defined at nodes over linear elements.
    '''
    def __init__(self, scaleFactor, lRefModSize, fUnitConv, sets, attributeName, rgMinMax= None):
        ''' Constructor.

        :param scaleFactor: scale factor for the diagram (can be negative too).
        :param lRefModSize: reference length of the model (how big the model is).
        :param fUnitConv: unit conversion factor (i.e N->kN => fUnitConv= 1e-3).
        :param sets:      list of element sets for which the diagram will be displayed.
        :param attributeName: displacement to display (uX or uY or uZ or rotX or rotY or rotZ).
        :param rgMinMax: range (vmin,vmax) with the maximum and minimum values  
                         of the scalar field (if any) to be represented. All 
                         the values less than vmin are displayed in blue and 
                         those greater than vmax in red (defaults to None)
        '''
        super(NodePropertyDiagram,self).__init__(scaleFactor= scaleFactor, lRefModSize= lRefModSize, fUnitConv= fUnitConv, sets= sets, attributeName= attributeName, rgMinMax= rgMinMax)

    def getValueForNode(self, node):
        ''' Get the value of the property in the given node.

        :param node: node to get the property from.
        '''
        if self.propertyName == 'uX':
          return node.getDispXYZ[0]
        elif self.propertyName == 'uY':
          return node.getDispXYZ[1]
        elif self.propertyName == 'uZ':
          return node.getDispXYZ[2]
        elif self.propertyName == 'rotX':
          return node.getRotXYZ[0]
        elif self.propertyName == 'rotY':
          return node.getRotXYZ[1]
        elif self.propertyName == 'rotZ':
          return node.getRotXYZ[2]
        else:
          return node.getProp(self.propertyName)

    def computeDiagramValues(self, eSet):
        ''' Return the values needed to create the diagram representation.'''
        self.valueCouples= list()
        self.elements= list()
        for e in eSet.elements:
            self.vDir= e.getJVector3d(True) #initialGeometry= True
            v0= self.getValueForNode(e.getNodes[0])
            v1= self.getValueForNode(e.getNodes[1])
            self.elements.append(e)
            self.valueCouples.append((v0, v1))
        if(self.rgMinMax): # filter values.
            self.valueCouples= self.filterValueCouples(self.valueCouples)
        self.autoScale() # Update scale.

