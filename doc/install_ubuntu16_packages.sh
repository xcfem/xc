# Must run as root so that we can shutdown backuppc and mount drives 
if [ $(whoami) != "root" ]; then
	echo "Debe ejecutar este guión como «root»."
	echo "Use 'sudo sh instala_paquetes_debian.sh' e introduzca la contraseña cuando se le pida."
	exit 1
fi

echo "Algunos paquetes se encuentran en las áreas «contrib» y «non-free» de la distribución Debian por lo que deben incluirse estas áreas en el archivo «sources.list antes de ejecutar este guión."

# Verificar que el usuario desea continuar
read -p "Continuar (s/n)?" REPLY
if [ $REPLY != "s" ]; then
	echo "Exiting..."
	exit 1
fi

#matplotlib added thanks to a comment from Guillem González 
apt-get install cmake cimg-dev g++ gfortran gnuplot libarpack2-dev libarpack++2-dev libcgal-dev libdb-dev libf2c2-dev libglib2.0-dev libgmp3-dev libgtk2.0-dev libgtkgl2.0-dev libgtkglextmm-x11-1.2-dev libgtkmm-2.4-dev libgts-bin libgts-dev libhdf5-mpi-dev liblapack-dev libmpfr-dev libmysql++-dev libmetis-dev libparmetis-dev libplot-dev libsqlite3-dev libsuperlu-dev libsuitesparse-dev libvtk5-dev petsc-dev tcl-dev python-vtk python-scipy python-sympy python-matplotlib python-pandas python-sklearn

#mayavi installation. Ubuntu 'mayavi' package seems to require VTK 6 so we use pip

sudo -H pip install mayavi
