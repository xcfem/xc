# -*- coding: utf-8 -*-

#Based on sXML-master projet on gitHub

__author__= "Luis C. Pérez Tato (LCPT)"
__copyright__= "Copyright 2015 LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

from import_export.sciaXML.xml_basics import scxml_properties_table as tp

class SCXMLPropertiesSubTable(tp.SCXMLPropertiesTable):
  '''SCIA XML properties sub table.'''

  def getXMLElementLabel(self):
    '''Returns an XML element label.'''
    return "def_subtable"

