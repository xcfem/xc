# -*- coding: utf-8 -*-

#Based on sXML-master projet on gitHub

__author__= "Luis C. Pérez Tato (LCPT)"
__copyright__= "Copyright 2015 LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"


class SCXMLDefinition(object):
  uri= ''

  def __init__(self, uri):
    ''' Constructor.
        Parameters:
        uri: uniform resource identifier?
    '''
    self.uri= uri

  def getUri(self):
    ''' returns idenfifier.'''
    return self.uri

  def setUri(self, uri):
    ''' sets idenfifier.'''
    self.uri = uri

