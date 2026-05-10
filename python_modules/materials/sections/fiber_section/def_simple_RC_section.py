# -*- coding: utf-8 -*-

from __future__ import division
from __future__ import print_function

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AO_O)"
__copyright__= "Copyright 2015, LCPT and AO_O"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@ciccp.es" "ana.Ortega@ciccp.es"

import sys
import os
import geom
import xc
from materials.sections import section_properties
from materials.sections.fiber_section import shear_reinforcement
from materials.sections.fiber_section import torsion_reinforcement
from materials.sections.fiber_section import main_reinforcement
from materials.sections.fiber_section import column_main_reinforcement
from materials.sections.fiber_section import rc_fiber_section_parameters
from materials.sections.fiber_section import rc_section_base
from materials.sections.fiber_section import basic_rectangular_rc_section
from materials.sections.fiber_section import rc_rectangular_section
from materials.sections.fiber_section import plot_fiber_section as pfs
from misc_utils import log_messages as lmsg

# Classes defining reinforcement.
ColumnMainReinforcement= column_main_reinforcement.ColumnMainReinforcement
ShearReinforcement= shear_reinforcement.ShearReinforcement
TorsionReinforcement= torsion_reinforcement.TorsionReinforcement
ReinfRow= main_reinforcement.ReinfRow
LongReinfLayers= main_reinforcement.LongReinfLayers
RCFiberSectionParameters= rc_fiber_section_parameters.RCFiberSectionParameters
RCSectionBase= rc_section_base.RCSectionBase
BasicRectangularRCSection= basic_rectangular_rc_section.BasicRectangularRCSection
RCRectangularSection= rc_rectangular_section.RCRectangularSection

# Convenience functions.
RebarRow2ReinfRow= main_reinforcement.RebarRow2ReinfRow
rebLayer_mm= main_reinforcement.rebLayer_mm
rebLayerByNumFi_mm= main_reinforcement.rebLayerByNumFi_mm
rebLayer_m= main_reinforcement.rebLayer_m
rebLayerByNumFi_m= main_reinforcement.rebLayerByNumFi_m

def compute_element_rc_sections(elements, propName= None):
    ''' Return a list containing the reinforced concrete sections computed
        from the values of the properties defined in the elements argument. 
        Those properties are:

        - baseSection: RCSectionBase derived object containing the geometry
                       and the material properties of the reinforcec concrete
                       section.
        - reinforcementUpVector: reinforcement "up" direction which defines
                                 the position of the positive reinforcement
                                 (bottom) and the negative reinforcement
                                 (up).
        - reinforcementIVector: (for slabs) direction corresponding to 
                                the first RC section
        - bottomReinforcement: LongReinfLayers objects defining the 
                               reinforcement at the bottom of the section.
        - topReinforcement: LongReinfLayers objects defining the 
                            reinforcement at the top of the section.
        - shearReinforcement: ShearReinforcement objects defining the 
                              reinforcement at the bottom of the section.
     
     :param elements: elements for which the reinforce concrete sections 
                      will be computed.
     :param propName: name of the property that stores the section names.
    '''
    retval= list()
    for el in elements:
        elementSections= rc_rectangular_section.compute_element_reinforcement(el)
        # Assign elements to each section.
        for i, eSection in enumerate(elementSections):
            if(eSection not in retval):
                eSection.elements= [(el.tag, i)]
                retval.append(eSection)
            else:
                idx= retval.index(eSection)
                retval[idx].elements.append((el.tag, i))
    # Rename the new sections.        
    for i, s in enumerate(retval):
        s.name+= str(i)
        s.sectionDescr+= ' '+str(i)

    if(not propName is None and (len(elements)>0)):
        # Assign the sections names to the elements
        preprocessor= elements[0].getDomain.getPreprocessor
        elemHandler= preprocessor.getElementHandler
        for elm in elements:
            elm.setProp(propName, ['','']) # Initialize property
        for sct in retval:
            for tple in sct.elements:
                # Each tuple has (element tag, section number).
                eTag= tple[0] # Element identifier.
                sectionIdx= tple[1] # Index of the sct section in the element.
                element= elemHandler.getElement(eTag)
                sectionNames= element.getProp(propName)
                if(sectionNames[sectionIdx]!=''):
                    methodName= sys._getframe(0).f_code.co_name
                    lmsg.error(methodName+'; element '+str(eTag) + ' has alreade section: '+sectionNames[sectionIdx])
                sectionNames[sectionIdx]= sct.name
                element.setProp(propName, sectionNames)
    return retval
