# -*- coding: utf-8 -*-

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AO_O)"
__copyright__= "Copyright 2015, LCPT and AO_O"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@ciccp.es" "ana.Ortega@ciccp.es"

import sympy

def toLaTeX(inputStr):
    ''' Add escape characters as needed to display the input string in LaTeX
        correctly.

    :param inputStr: input string.
    '''
    retval= inputStr.replace('\\(','(')
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

def mathExprToLaTeX(mathExprStr):
    ''' Convert the math expression in the input string to LaTeX.

    :param mathExprStr: math expression input string.
    '''
    return sympy.latex(sympy.sympify(mathExprStr))
  
