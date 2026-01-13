 # -*- coding: utf-8 -*-
''' Diagram to display a property defined at nodes over linear elements. '''

__author__= "Luis C. Pérez Tato (LCPT) , Ana Ortega (AO_O) "
__copyright__= "Copyright 2016, LCPT, AO_O"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@ciccp.es, ana.Ortega@ciccp.es "

import geom
from postprocess.xcVtk.diagrams import property_diagram as pd
from misc_utils import log_messages as lmsg

class NodePropertyDiagram(pd.PropertyDiagram):
    '''Diagram to display a property defined at nodes over linear elements.
    '''
    def __init__(self, scaleFactor, lRefModSize, fUnitConv, sets, propertyName, defaultDirection= 'J', defaultValue= 0.0, rgMinMax= None):
        ''' Constructor.

        :param scaleFactor: scale factor for the diagram (can be negative too).
        :param lRefModSize: reference length of the model (how big the model is).
        :param fUnitConv: unit conversion factor (i.e N->kN => fUnitConv= 1e-3).
        :param sets:      list of element sets for which the diagram will be displayed.
        :param propertyName: property to display.
        :param defaultDirection: default direction of the diagram (J: element 
                                 local j vector or K: element local K vector).
        :param rgMinMax: range (vmin,vmax) with the maximum and minimum values  
                         of the scalar field (if any) to be represented. All 
                         the values less than vmin are displayed in blue and 
                         those greater than vmax in red (defaults to None)
        '''
        super(NodePropertyDiagram,self).__init__(scaleFactor= scaleFactor, lRefModSize= lRefModSize, fUnitConv= fUnitConv, sets= sets, propertyName= propertyName, rgMinMax= rgMinMax)
        self.defaultDirection= defaultDirection
        self.defaultValue= defaultValue

    def getValueForNode(self, node):
        ''' Get the value of the property in the given node.

        :param node: node to get the property from.
        '''
        retval= None
        if self.featureName == 'uX':
            retval= node.getDispXYZ[0]
        elif self.featureName == 'uY':
            retval= node.getDispXYZ[1]
        elif self.featureName == 'uZ':
            retval= node.getDispXYZ[2]
        elif self.featureName == 'rotX':
            retval= node.getRotXYZ[0]
        elif self.featureName == 'rotY':
            retval= node.getRotXYZ[1]
        elif self.featureName == 'rotZ':
            retval= node.getRotXYZ[2]
        else:
            if(node.hasProp(self.featureName)): # don't bother with warnings.
               retval= node.getProp(self.featureName)
        if(retval is None):
            retval= self.defaultValue
        return retval

    def computeDiagramValues(self):
        ''' Compute the values needed to create the diagram representation.'''
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
        # Return the number of computed values.
        return len(self.valueCouples)

