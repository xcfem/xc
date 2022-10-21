# -*- coding: utf-8 -*-

#Based on sXML-master projet on gitHub

__author__= "Luis C. Pérez Tato (LCPT)"
__copyright__= "Copyright 2015 LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

import xml.etree.cElementTree as ET

class SCXMLEnum(object):
    '''Enumeration for SCIA XML.'''
    def __init__(self,items):
        '''Constructor.

        :param items: enumeration values (like mon,tue,wed,thu,fri,sat,sun).
        ''' 
        if(items):
            self.items= items
        else:
            self.items= list()
    def getXMLElement(self,parent):
        '''Returns the corresponding XML element for the enum object.

        :param parent: owner of this object in the XML structure.'''
        tb= ET.SubElement(parent,"def_enum")
        tb.set("size",str(len(self.items)))
        for i in self.items:
            it= i.getXMLElement(tb)
            if __debug__:
                if(not it):
                    AssertionError('Couldn\'t get XML element.')
        return tb

    
    
    

