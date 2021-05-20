# -*- coding: utf-8 -*-

def toLaTex(str):
  retval= str.replace('\\(','(')
  retval= retval.replace('\\)',')')
  retval= retval.replace('\\[','[')
  retval= retval.replace('\\]',']')
  retval= retval.replace('\\]',']')
  retval= retval.replace('_','\\_')
  retval= retval.replace('%','\\%')
  retval= retval.replace('$','\\$')
  retval= retval.replace('&','\\&')
  #retval= retval.replace('>','$>$')
  return retval
