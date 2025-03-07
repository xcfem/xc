# -*- coding: utf-8 -*-
''' Python script to generate the documentation of the verification examples.
(work in progress)
'''

import os
import ast
import logging

mypath= './'
files= [os.path.join(dirpath,f) for (dirpath, dirnames, filenames) in os.walk(mypath) for f in filenames]

python_scripts= list()
for f in files:
    if(f.endswith('.py')):
        python_scripts.append(f)
docstrings= dict()
for fName in python_scripts:
    with open(fName, 'r') as f:
        tree = ast.parse(f.read())
        docstring = ast.get_docstring(tree)
        if(docstring):
            docstrings[fName]= docstring
            print(fName)
            print('===================================================')
            print(docstring)
            print('===================================================\n')
        else:
            logging.warning("Docstring missing for file: '"+ fName+"'")
