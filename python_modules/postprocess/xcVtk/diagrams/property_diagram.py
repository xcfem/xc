 # -*- coding: utf-8 -*-
''' Diagram to display a property defined at nodes over linear elements. '''

from postprocess.xcVtk.diagrams import colored_diagram as cd

class PropertyDiagram(cd.ColoredDiagram):
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
        super(PropertyDiagram,self).__init__(scaleFactor= scaleFactor, fUnitConv= fUnitConv, rgMinMax= rgMinMax)
        self.lstSets= sets
        self.propertyName= attributeName
        self.lRefModSize= lRefModSize

    def getMaxAbsComp(self):
        '''Return the maximum absolute value of the component.
        It is used only for calculating auto-scale parameter, so 
        we compare only values of the component in first node of the elements
        '''
        maxV= 0.0
        for (v0, v1) in self.valueCouples:
            maxV= max(abs(v0), maxV)
            maxV= max(abs(v1), maxV)
        return maxV

    def autoScale(self):
        ''' Autoscale the diagram. '''
        maxAbsComp= self.getMaxAbsComp()
        if(maxAbsComp>0):
            self.scaleFactor*= 0.15*self.lRefModSize/(maxAbsComp*self.fUnitConv)
            
    def appendDataSetToDiagram(self, eSet, diagramIndex, defFScale=0.0):
        ''' Append property values to diagram.

        :param eSet: Element set.
        :param diagramIndex: index-counter for the values to insert.
        :param defFScale: factor to apply to current displacement of nodes 
                  so that the display position of each node equals to
                  the initial position plus its displacement multiplied
                  by this factor. (Defaults to 0.0, i.e. display of 
                  initial/undeformed shape)
        ''' 
        self.computeDiagramValues(eSet= eSet);
        return self.appendDataToDiagram(elements= self.elements, diagramIndex= diagramIndex, valueCouples= self.valueCouples, defFScale= defFScale)

    def clear(self):
        ''' Clear the diagram data.'''
        self.elements.clear()
        self.valueCouples.clear()

    def addDiagram(self, defFScale= 0.0):
        self.createDiagramDataStructure()
        self.createLookUpTable()
        self.createDiagramActor()

        diagramIndex= 0
        for s in self.lstSets:
            diagramIndex= self.appendDataSetToDiagram(eSet= s, diagramIndex= diagramIndex, defFScale= defFScale)

        self.clear()
        self.updateLookUpTable()
        self.updateDiagramActor()

