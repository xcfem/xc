# -*- coding: utf-8 -*-
''' LaTeX related utils.'''

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AO_O)"
__copyright__= "Copyright 2015, LCPT and AO_O"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@ciccp.es" "ana.Ortega@ciccp.es"

import sympy
import tempfile
import os
import flatlatex

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
  
def latex_to_pdf(latexCode:str, pdfFileName, superTabular= False):
    ''' Creates a PDF file from the given LaTeX code.

    :param latexCode: text in LaTeX format.
    :param pdfFileName: output file name.
    :param superTabular: if true, use supertabular instead longtable.
    '''
    texDocument= u'''\\documentclass{article}%\n
    \\usepackage[utf8]{inputenc}%\n'''
    if(superTabular):
        texDocument+= '\\usepackage{supertabular}%\n'
    else:
        texDocument+= '\\usepackage{longtable}%\n'
    texDocument+= '''\\begin{document}%\n
    **LaTeXCode**
    \\end{document}%\n
    '''
    texDocument= texDocument.replace('**LaTeXCode**', latexCode)
    with tempfile.NamedTemporaryFile(mode= 'w', delete= False) as tmp:
        tmp.write(texDocument)
        outputFileName= tmp.name
    os.rename(outputFileName, outputFileName+'.tex')
    outputFileName+='.tex'
    dirName= os.path.dirname(outputFileName)
    pdfLaTeXCommand= 'pdflatex --interaction=batchmode'
    pdfLaTeXCommand+= ' -output-directory '+ dirName 
    pdfLaTeXCommand+= ' ' + outputFileName
    print(pdfLaTeXCommand)
    os.system(pdfLaTeXCommand)    
    os.system(pdfLaTeXCommand)    
    os.system(pdfLaTeXCommand)
    pdfOutput= outputFileName.replace('.tex', '.pdf')
    os.replace(pdfOutput, pdfFileName)
    ### Remove unneeded files
    for extension in ['.aux', '.log']:
        fName= outputFileName.replace('.tex', extension)
        os.unlink(fName)
    os.unlink(outputFileName)

def get_latex_symbol_from_name(compName: str):
    '''Return the LaTeX symbol corresponding to the given field component 
       name.

    :param compName: field component name.
    '''
    retval= compName
    if(compName.startswith('epsilon_')):
        tokens= compName.split('_')
        retval= '\\'+tokens[0]+'_{'+tokens[1]+'}'
    elif(compName=='epsilon'):
        retval= '\\epsilon'
    elif(compName.startswith('sigma_')):
        tokens= compName.split('_')
        retval= '\\'+tokens[0]+'_{'+tokens[1]+'}'
    elif(compName[0]== 'u'):
        retval= '\\'+compName[0]+'_{'+compName[1]+'}'
    elif(compName.startswith('rot')):
        retval= '\\theta'+'_{'+compName[3:]+'}'
    elif(compName.startswith('kappa_')):
        tokens= compName.split('_')
        retval= '\\'+tokens[0]+'_{'+tokens[1]+'}'
    elif(compName=='kappa'):
        retval= '\\kappa'
    elif(compName.startswith('gamma_')):
        tokens= compName.split('_')
    elif(compName=='gamma'):
        retval= '\\kappa'
    elif(compName.startswith('theta_')):
        tokens= compName.split('_')
    elif(compName in ['My', 'Mz', 'Qy', 'Qz', 'Vy', 'Vz']):
        retval= '\\'+compName[0]+'_{'+compName[1]+'}'
    elif(compName in ['N1', 'N2', 'N12', 'M1', 'M2', 'M12', 'Q1', 'Q2', 'Q12', 'V1', 'V2', 'V12']):
        retval= '\\'+compName[0]+'_{'+compName[1:]+'}'
    elif(compName in ['n1', 'n2', 'n12', 'm1', 'm2', 'm12', 'q1', 'q2', 'q12', 'v1', 'v2', 'v12']):
        retval= '\\'+compName[0]+'_{'+compName[1:]+'}'
    return retval

def get_unicode_symbol_from_name(compName: str):
    '''Return the unicode symbol corresponding to the given field component 
       name.

    :param compName: field component name.
    '''
    c= flatlatex.converter()
    latexExpr= get_latex_symbol_from_name(compName)
    return c.convert(latexExpr)

def get_table_row(value_list):
    '''Return a tex line to insert on any kind of LaTeX table.

    :param value_list: list of values of the columns for the table 
                       (they will be converted to strings).
    '''
    retval=''
    for v in value_list:
        retval+=str(v)+' & '
    retval= retval[:-2]
    retval+=' \\\\ \n'
    return retval
