# Feel++ Submesh Tools

This repository provides scripts to load fields on a submesh in a model containing this submesh

## How to build the project

Open folder in container.

Build the project:

$ cmake --preset default
$ cmake --build --preset default


## Example with Tore physic by physic

command lines:

Move to build folder:
$ cd build/default/src

Thermoelectric computation (use standard toolboxe):
$ feelpp_toolbox_thermoelectric --config-file /workspaces/feelpp.submeshtools/src/cases/Tore_physicsbyphysics/np_1/Tore_thelec.cfg

Retrieve saved temperature & electric potential, save current density:
$ scp -r /home/feelpp/feelppdb/Tore_physicsbyphysics/thelec/np_1/*.save  /workspaces/feelpp.submeshtools/src/cases/Tore_physicsbyphysics/np_1/
$ ./feelpp_fp_cfpdes_mag --config-file /workspaces/feelpp.submeshtools/src/cases/Tore_physicsbyphysics/np_1/Tore_current.cfg
$ scp -r /home/feelpp/feelppdb/Tore_physicsbyphysics/current/np_1/*.save  /workspaces/feelpp.submeshtools/src/cases/Tore_physicsbyphysics/np_1/

Magnetic computation :
$ ./feelpp_fp_cfpdes_mag --config-file /workspaces/feelpp.submeshtools/src/cases/Tore_physicsbyphysics/np_1/Tore_magnetic.cfg

Retrieve saved magnetic potential, save magnetic field:
$ scp -r /home/feelpp/feelppdb/Tore_physicsbyphysics/magnetic/np_1/*.save  /workspaces/feelpp.submeshtools/src/cases/Tore_physicsbyphysics/np_1/
$ ./feelpp_fp_cfpdes_mag --config-file /workspaces/feelpp.submeshtools/src/cases/Tore_physicsbyphysics/np_1/Tore_magneticField.cfg
$ scp -r /home/feelpp/feelppdb/Tore_physicsbyphysics/magneticField/np_1/*.save  /workspaces/feelpp.submeshtools/src/cases/Tore_physicsbyphysics/np_1/

Elastic computation :
$ ./feelpp_fp_cfpdes_elas --config-file /workspaces/feelpp.submeshtools/src/cases/Tore_physicsbyphysics/np_1/Tore_elastic.cfg


## Magnetic Computation with bmap or biot-savart:

Magnetic computation with Bmap :
$ ./feelpp_fp_cfpdes_mag_amap --config-file /workspaces/feelpp.submeshtools/src/cases/Tore_physicsbyphysics/np_1/Tore_magnetic.cfg

Magnetic computation with Biot-Savart :
$ ./feelpp_fp_cfpdes_mag_biotsavart --config-file /workspaces/feelpp.submeshtools/src/cases/Tore_physicsbyphysics/np_1/Tore_magnetic.cfg