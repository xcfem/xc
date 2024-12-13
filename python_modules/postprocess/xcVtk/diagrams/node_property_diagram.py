 # -*- coding: utf-8 -*-
''' Diagram to display a property defined at nodes over linear elements. '''

import geom
from postprocess.xcVtk.diagrams import property_diagram as pd

class NodePropertyDiagram(pd.PropertyDiagram):
    '''Diagram to display a property defined at nodes over linear elements.
    '''
    def __init__(self, scaleFactor, lRefModSize, fUnitConv, sets, attributeName, defaultDirection= 'J', defaultValue= 0.0, rgMinMax= None):
        ''' Constructor.

        :param scaleFactor: scale factor for the diagram (can be negative too).
        :param lRefModSize: reference length of the model (how big the model is).
        :param fUnitConv: unit conversion factor (i.e N->kN => fUnitConv= 1e-3).
        :param sets:      list of element sets for which the diagram will be displayed.
        :param attributeName: displacement to display (uX or uY or uZ or rotX or rotY or rotZ).
        :param defaultDirection: default direction of the diagram (J: element 
                                 local j vector or K: element local K vector).
        :param rgMinMax: range (vmin,vmax) with the maximum and minimum values  
                         of the scalar field (if any) to be represented. All 
                         the values less than vmin are displayed in blue and 
                         those greater than vmax in red (defaults to None)
        '''
        super(NodePropertyDiagram,self).__init__(scaleFactor= scaleFactor, lRefModSize= lRefModSize, fUnitConv= fUnitConv, sets= sets, attributeName= attributeName, rgMinMax= rgMinMax)
        self.defaultDirection= defaultDirection
        self.defaultValue= defaultValue

    def getValueForNode(self, node):
        ''' Get the value of the property in the given node.

        :param node: node to get the property from.
        '''
        retval= None
        if self.propertyName == 'uX':
            retval= node.getDispXYZ[0]
        elif self.propertyName == 'uY':
            retval= node.getDispXYZ[1]
        elif self.propertyName == 'uZ':
            retval= node.getDispXYZ[2]
        elif self.propertyName == 'rotX':
            retval= node.getRotXYZ[0]
        elif self.propertyName == 'rotY':
            retval= node.getRotXYZ[1]
        elif self.propertyName == 'rotZ':
            retval= node.getRotXYZ[2]
        else:
            if(node.hasProp(self.propertyName)): # don't bother with warnings.
               retval= node.getProp(self.propertyName)
        if(retval is None):
            retval= self.defaultValue
        return retval

    def computeDiagramValues(self):
        ''' Return the values needed to create the diagram representation.'''
        self.valueCouples= list()
        self.elements= list()
        self.directions= list()
        for s in self.elemSets:
            for e in s.elements:
                if(self.defaultDirection):
                    if(self.defaultDirection=='J'): # local J vector.
                        vDir= e.getJVector3d(True) #initialGeometry= True
                    elif(self.defaultDirection=='K'): # local K vector.
                        vDir= e.getKVector3d(True) #initialGeometry= True
                    elif(self.defaultDirection=='X'): # global x vector.
                        vDir= geom.Vector3d(1,0,0)
                    elif(self.defaultDirection=='Y'): # global y vector.
                        vDir= geom.Vector3d(0,1,0)
                    elif(self.defaultDirection=='Z'): # global z vector.
                        vDir= geom.Vector3d(0,0,1)
                    else: # local K vector.
                        vDir= e.getKVector3d(True) #initialGeometry= True
                    self.directions.append(vDir)
                v0= self.getValueForNode(e.getNodes[0])
                v1= self.getValueForNode(e.getNodes[1])
                self.elements.append(e)
                self.valueCouples.append((v0, v1))
        if(self.rgMinMax): # filter values.
            self.valueCouples= self.filterValueCouples(self.valueCouples)
            lmsg.warning('Displayed values have been clipped whitin the range: ('+str(self.rgMinMax[0])+', '+str(self.rgMinMax[1])+"), so they don't correspond to the computed ones.")
        self.autoScale() # Update scale.

