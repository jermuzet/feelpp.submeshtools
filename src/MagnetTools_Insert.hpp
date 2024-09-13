/* -*- mode: c++; coding: utf-8; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4; show-trailing-whitespace: t -*- vim:fenc=utf-8:ft=tcl:et:sw=4:ts=4:sts=4

   This file is part of the Feel library

   Author(s): Christophe Prud'homme <christophe.prudhomme@feelpp.org>
   Author(s): Cecile Daversin <cecile.daversin@lncmi.cnrs.fr>
   Date: 2011-16-12

   Copyright (C) 2008-2010 Universite Joseph Fourier (Grenoble I)
   Copyright (C) CNRS

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
/**
   \file bmap.hpp
   \author Christophe Prud'homme <christophe.prudhomme@feelpp.org>
   \author Cecile Daversin <daversin@math.unistra.fr>
   \date 2015-18-02
*/

#ifndef __MAGNETTOOLS_INSERT_HPP
#define __MAGNETTOOLS_INSERT_HPP

#include <fstream>
#include <iostream>
#include <string>

#include <feel/feelcore/environment.hpp>

// **** Add specific headers and defs for MagnetTools/B_Map *** //

extern bool omp_on;
extern bool omp_already_on;

extern int gsl_error_mode;
extern int plot_mode;
extern int check_mode;
extern int sym_mode;
extern int read_mode;
extern int verbose_mode;
extern int geom_mode;
extern int ensight_on;

extern std::string ensight_mode;

extern double T_ref;
extern double factor_J;
extern double factor_P;
extern double factor_Sigma;
extern double factor_Constraint;
extern double units_by_defaults;

#include "Constraints.h"
extern double ep;
extern Constraints Actual_Constraints;

#include "Aubert_files.h"
#include "parallel.h"
#include "NumIntegration.h"
#include "eps_params.h"
extern int num_integ;
extern int num_eval;

// To avoid loading/writing an eps_params.dat
// this should be changed to allow use of bmap_options (see eps_params.h)
extern epsparams Precision;
extern gsl_integration_data ** Integ_ptr;

// Class bmap
namespace Feel
{
    struct InsertData
    {
        std::string datafileName; //file .d
        std::string units; //m or mm
        double units_by_defaults = 1;

        vector<Tube>         Tubes;
        vector<BitterMagnet> PolyHelix;
        vector<BitterMagnet> OtherHelices;
        vector<BitterMagnet> BitterMagnets;
        vector<UnifMagnet>   UnifMagnets;
        vector<ThinSolenoid> PassiveShims;
        vector<vector<UnifMagnet> >   Screens;

        InsertData(const std::string & datafile, const string & unit, const double & I_H=0, const double & I_B=0, const double & I_S=0)
        {
            int proc_rank = Environment::worldComm().globalRank();

            datafileName = datafile; //file .d
            units = unit; //m or mm
            if ( units == "mm")
                {
                    Feel::cout << "readMagneticField: units are in mm" << std::endl;
                    units_by_defaults = 1.e-3;
                }

            // should only be read once and the broadcast data
            if(proc_rank == 0)
                {
                    tic();
                    Feel::cout << "loading InsertData from " << datafileName << " ..." << std::endl << std::flush;
                    int status = read_aubert_files(datafileName, Tubes, PolyHelix, OtherHelices, BitterMagnets, UnifMagnets, PassiveShims, sym_mode, read_mode);
                    if ( status != 0 )
                        throw std::logic_error( "InsertData: error when loading insert data from " + datafileName );
                    toc("Init MagnetTools", FLAGS_v > 0);
                }

            tic();
            Integ_ptr = new gsl_integration_data * [my_get_max_threads()];
            for (int i=0; i<my_get_max_threads(); i++){
                Integ_ptr[i] = new gsl_integration_data(num_integ);
            }

            gsl_set_error_handler_off();
            toc("Init GSL", FLAGS_v > 0);
        }

        // ~InsertData()
        // {
        //     // should properly delete Integ_ptr
        //     //Environment::worldComm().barrier(); //waiting for all procs to finish
        // }

        void set_currents(const double & I_H=0, const double & I_B=0, const double & I_S=0)
        {
            int proc_rank = Environment::worldComm().globalRank();

            // should only be read once and the broadcast data
            if(proc_rank == 0)
                {
                    tic();
                    int n_circuits = 0;
                    if (PolyHelix.size() > 0) n_circuits++;
                    if (BitterMagnets.size() > 0) n_circuits++;
                    if (UnifMagnets.size() > 0) n_circuits++;

                    Vect Current(n_circuits);
                    int idx=0;
                    if (PolyHelix.size() > 0)
                        {
                            Current[idx] = I_H;
                            idx++;
                        }
                    if (BitterMagnets.size() > 0)
                        {
                            Current[idx] = I_B;
                            idx++;
                        }
                    if (UnifMagnets.size() > 0)
                        {
                            Current[idx] = I_S;
                            idx++;
                        }

                    update_currents(Tubes, PolyHelix, BitterMagnets, UnifMagnets, Screens,  OtherHelices, Current);
                    toc("Update MagnetTools Current", FLAGS_v > 0);
                }

            tic();
            mpi::broadcast( Environment::worldComm(), Tubes, 0);
            mpi::broadcast( Environment::worldComm(), PolyHelix, 0);
            mpi::broadcast( Environment::worldComm(), BitterMagnets, 0);
            mpi::broadcast( Environment::worldComm(), UnifMagnets, 0);
            mpi::broadcast( Environment::worldComm(), PassiveShims, 0);
            mpi::broadcast( Environment::worldComm(), Screens, 0);
            mpi::broadcast( Environment::worldComm(), OtherHelices, 0);
            toc("broadcast Updated MagnetTools", FLAGS_v > 0);
            Feel::cout << "set_currents done" << std::endl;
       }

    };
}
#endif /* __MAGNETTOOLS_INSERT_HPP 1 */
