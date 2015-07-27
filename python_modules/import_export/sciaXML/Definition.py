# -*- coding: utf-8 -*-

#Based on sXML-master projet on gitHub



class Definition(object):
  uri= ''

  def __init__(self, uri):
    self.uri= uri

  def getUri(self):
    return self.uri

  def setUri(self, uri):
    self.uri = uri

