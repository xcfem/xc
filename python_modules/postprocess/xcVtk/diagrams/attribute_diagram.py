# -*- coding: utf-8 -*-
''' Diagram to display an attribute defined at nodes over linear elements. '''

__author__= "Luis C. Pérez Tato (LCPT) , Ana Ortega (AO_O) "
__copyright__= "Copyright 2016, LCPT, AO_O"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@ciccp.es, ana.Ortega@ciccp.es "

import sys
from postprocess.xcVtk.diagrams import feature_diagram as fd
from misc_utils import log_messages as lmsg

class AttributeDiagram(fd.FeatureDiagram):
    '''Diagram to display a attribute defined at nodes over linear elements.
    '''
    def __init__(self, scaleFactor, lRefModSize, fUnitConv, sets, attributeName, rgMinMax= None):
        ''' Constructor.

        :param scaleFactor: scale factor for the diagram (can be negative too).
        :param lRefModSize: reference length of the model (how big the model is).
        :param fUnitConv: unit conversion factor (i.e N->kN => fUnitConv= 1e-3).
        :param sets:      list of element sets for which the diagram will be displayed.
        :param attributeName: attribute to display.
        :param rgMinMax: range (vmin,vmax) with the maximum and minimum values  
                         of the scalar field (if any) to be represented. All 
                         the values less than vmin are displayed in blue and 
                         those greater than vmax in red (defaults to None)
        '''
        super(AttributeDiagram, self).__init__(scaleFactor= scaleFactor, lRefModSize= lRefModSize, fUnitConv= fUnitConv, sets= sets, featureName= attributeName, rgMinMax= rgMinMax)
        
    def filterElements(self):
        ''' Return the elements that actually have the required attribute.
        '''
        retval= list()
        refused_elements= list()
        for s in self.elemSets:
            for e in s.elements:
                if(hasattr(e, self.featureName)):
                    retval.append(e)
                else:
                    refused_elements.append(e)
        if(len(refused_elements)>0):
            tag_lst= list()
            for e in refused_elements:
                tag_lst.append(e.tag)
            className= type(self).__name__
            methodName= sys._getframe(0).f_code.co_name
            warningMsg= '; elements: '+str(tag_lst) + " do not have the required attribute: '"+str(self.featureName)+'"'
            lmsg.warning(className+'.'+methodName+warningMsg)
        return retval
       


