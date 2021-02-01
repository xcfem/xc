Make sure you have installed the python-sphinx package


First we search for the sphinxDoc directory:

`cd .../xc/doc/sphinxDoc`

Then we create the html files:

`make html`

We change to our local [XCmanual](http://github.com/xcfem/XCmanual/) repository:

`cd /usr/local/src/prg/XCmanual`

and update the newly created/updated files:

`sh update.sh`

finally we push this repository into GitHub as usual.

The result can be seen [here](http://xcfem.github.io/XCmanual/) 

