# -*- coding: utf-8 -*-

#Based on sXML-master projet on gitHub

__author__= "Luis C. Pérez Tato (LCPT)"
__copyright__= "Copyright 2015 LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

import PropertiesTable as tp

class PropertiesSubTable(tp.PropertiesTable):
  '''SCIA XML properties sub table.'''

  def getXMLElementLabel(self):
    '''Returns an XML element label.'''
    return "def_subtable"

