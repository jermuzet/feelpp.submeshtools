/* -*- mode: c++; coding: utf-8; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4; show-trailing-whitespace: t -*- vim:fenc=utf-8:ft=tcl:et:sw=4:ts=4:sts=4

   This file is part of the Feel library

   Author(s): Christophe Prud'homme <christophe.prudhomme@feelpp.org>
   Author(s): Cecile Daversin <daversin@math.unistra.fr>
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
   \file biot_savart_impl.hpp
   \author Christophe Prud'homme <christophe.prudhomme@feelpp.org>
   \author Cecile Daversin <daversin@math.unistra.fr>
   \date 2016-14-02
*/


#ifndef __BS_IMPL_HPP
#define __BS_IMPL_HPP 1

#include<vector>

#include <feel/feeldiscr/pch.hpp>
#include <feel/feeldiscr/pchv.hpp>
#include <feel/feeldiscr/pdh.hpp>
#include <feel/feeldiscr/pdhv.hpp>

namespace Feel
{
    template<int DimC, int DimB, int Order, int G_order>
    class BiotSavartImpl
    {
    public :

        //! geometry entities type composing the mesh, here Simplex in Dimension Dim (Order G_order)
        typedef Simplex<DimC,G_order> convex_type;
        typedef Simplex<DimB,G_order,DimC> convex_box_type;

        //! mesh type
        typedef Mesh<convex_type> mesh_type;
        typedef Mesh<convex_box_type> mesh_box_type;
        //! mesh shared_ptr<> type
        typedef std::shared_ptr<mesh_type> mesh_ptrtype;
        typedef std::shared_ptr<mesh_box_type> mesh_box_ptrtype;

        // using space_type = Pch_type<mesh_type,Order>;
        // using  space_ptrtype = std::shared_ptr<space_type>;
        // typedef typename space_type::element_type element_type;

        using box_space_type = Pch_type<mesh_box_type,Order>;
        using box_space_ptrtype = std::shared_ptr<box_space_type>;
        typedef typename box_space_type::element_type box_element_type;

        //! Approximation space for vector inside (B)
        // using vec_space_type = Pdhv_type<mesh_type,Order-1>;
        using vec_space_type = FunctionSpace<mesh_type, bases<Lagrange<Order-1, Vectorial,Discontinuous,PointSetFekete>> >;
        using vec_space_ptrtype =  std::shared_ptr<vec_space_type>;
        typedef typename vec_space_type::element_type vec_element_type;

        using vec_box_space_type = Pchv_type<mesh_box_type,Order>;
        using vec_box_space_ptrtype = std::shared_ptr<vec_box_space_type>;
        typedef typename vec_box_space_type::element_type vec_box_element_type;
        typedef boost::tuple< vec_box_element_type, vec_box_element_type > return_type;

        //! the exporter factory type
        using export_type = Exporter<mesh_box_type,mesh_box_type::nOrder>;
        using export_ptrtype = std::shared_ptr<export_type>;

        typedef boost::tuple<node_type, size_type, uint16_type > dof_point_type;
        typedef typename std::vector<dof_point_type> dof_points_type;

        //boost::tuple<vec_element_type,vec_element_type>
        //void
        FEELPP_DONT_INLINE
        boost::tuple<boost::any, boost::any>
        biot_savart_impl( mesh_ptrtype& conductor_mesh,
                          mesh_box_ptrtype& box_mesh,
                          vec_element_type current_density,
                          std::string appName);

        FEELPP_DONT_INLINE
        vec_box_element_type
        biot_savart_A_impl( mesh_ptrtype& conductor_mesh,
                            mesh_box_ptrtype& box_mesh,
                            vec_element_type current_density);

        FEELPP_DONT_INLINE
        vec_box_element_type
        biot_savart_B_impl( mesh_ptrtype& conductor_mesh,
                            mesh_box_ptrtype& box_mesh,
                            vec_element_type current_density);

        void
        init( mesh_ptrtype& conductor_mesh,
              mesh_box_ptrtype& box_mesh);

        // should create optionnal params for A and/or B
        void
        export_results( const std::string& appName,
                        mesh_box_ptrtype& box_mesh,
                        vec_box_element_type A,
                        vec_box_element_type B);

    private:
        // commC1M = communicator with 1 Omega_M part (first procs of the group) + all Omega C
        // commsC1M = set of commC1M communicators, for each part of Omega M
        std::vector< mpi::communicator > commsC1M;

        // local isIn vector : isIn[0] = proc is in Omega_C, isIn[1] = proc is in Omega_M
        std::vector<int> isIn;

        vec_box_space_ptrtype Xh_box_global;
        vec_space_ptrtype Xh_cond_global;
        box_space_ptrtype Xh_box_global_scalar;
    };

}


template<int DimC, int DimB, int Order, int G_order>
boost::tuple<boost::any, boost::any>
Feel::BiotSavartImpl<DimC, DimB, Order, G_order>::biot_savart_impl( Feel::BiotSavartImpl<DimC, DimB, Order, G_order>::mesh_ptrtype& conductor_mesh,
                                                                    Feel::BiotSavartImpl<DimC, DimB, Order, G_order>::mesh_box_ptrtype& box_mesh,
                                                                    vec_element_type current_density,
                                                                    std::string appName)
{
    auto coeff = 1/(4*M_PI);
    auto mu0 = 4*M_PI*1e-7; //SI unit : H.m-1 = m.kg.s-2.A-2
    if( soption(_name="units") == "mm" )
        mu0 *= 1000;

    //LOG(INFO) << "biot_savart_impl:" << std::endl;
    tic();
    namespace mpi=boost::mpi;

    // calc init instead
    init(conductor_mesh, box_mesh);

    vec_box_element_type A = Xh_box_global->element(); //magnetic potential
    vec_box_element_type B = Xh_box_global->element(); //magnetic field
    A.zero();
    B.zero();

    // add 2 more quantities to get approx:
    // vec_box_element_type A = Xh_box_global->element();
    // vec_box_element_type B = Xh_box_global->element();

    // Omega_M : magnetic field region (box_mesh)
    // Omega_C : conductor (conductor_mesh)

    // 3 : Communications
    // For each commC1M, Omega_M part send coordinates of its dofs to all parts of Omega_C
    // All parts of Omega_C compute the integrals and re-send it to Omega_M

    dof_points_type dofM; //dofs
    std::vector<double> intM;
    //std::vector<double> intMsum;
    int dofM_size;
    std::vector<mpi::request> requests(1);

    Environment::worldComm().barrier();
    for(int i=0; i<commsC1M.size(); i++)
        {
            LOG(INFO)  << "biot_savart_impl: Proc " << Environment::worldComm().globalRank() << "] " << i << "th group loop" << std::endl;
            if( commsC1M[i] ) // Current communicator
                {
                    if(commsC1M[i].rank() == 0 ) // Proc of rank 0 is the Omega_M part
                        {
                            dofM.clear();
                            for ( size_type dof_id = 0; dof_id < Xh_box_global->nLocalDofWithGhost() ; ++dof_id )
                                dofM.push_back( Xh_box_global->dof()->dofPoint(dof_id) );
                            dofM_size = dofM.size();

                            LOG(INFO) << "Proc[" << Environment::worldComm().globalRank()
                                      << "]:" << dofM_size << " dofs in Omega_M" << std::endl;
                        }

                    //Proc 0 broadcasts its dofs
                    tic();
                    mpi::broadcast( commsC1M[i], dofM_size, 0);
                    dofM.resize( dofM_size );
                    mpi::broadcast( commsC1M[i], dofM.data(), dofM.size(), 0);
                    toc("broadcast", FLAGS_v > 0);

                    intM.resize( 2*dofM_size );    // 2 is actually the number of fields to compute
                    //intMsum.resize( 2*dofM_size );

                    //commsC1M[i].barrier(); //wait for broadcast

                    tic();
                    if( isIn[0] ) // Parts of Omega_C
                        {
                            std::vector<Eigen::Matrix<double,3,1>> coords( dofM_size );
                            // fill vector of coordinates
                            for(int d=0; d<dofM_size; d++)
                                {
                                    auto dof_coord = dofM[d].template get<0>();
                                    Eigen::Matrix<double,3,1> coord;
                                    coord << dof_coord[0],dof_coord[1],dof_coord[2];
                                    coords[d] = coord;
                                }

                            // Integrals computation
                            auto dist = inner( _e1v-P(),_e1v-P(), mpl::int_<InnerProperties::IS_SAME|InnerProperties::SQRT>() );
                            auto mgn_pot = integrate(_range = elements( conductor_mesh ),
                                                     _expr=mu0*coeff*idv(current_density)/dist,
                                                     _quad=_Q<1>()
                                                     ).template evaluate(coords);

                            auto mgn_field = integrate(_range = elements( conductor_mesh ),
                                                       _expr=mu0*coeff*cross(idv(current_density), _e1v-P())/(dist*dist*dist),
                                                       _quad=_Q<1>()
                                                       ).template evaluate(coords);
                            // auto pot = integrate(_range = elements( conductor_mesh ), _expr=mu0*coeff/dist ,  _quad=_Q<1>()).evaluate(coords);
                            // auto field = integrate(_range = elements( conductor_mesh ), _expr=mu0*coeff*(_e1v-P())/dist ,  _quad=_Q<1>()).evaluate(coords);

                            // Fill results tab
                            for(int d=0; d<dofM_size; d++)
                                {
                                    auto dof_comp = dofM[d].template get<2>();
                                    intM[2*d] = mgn_pot[d](dof_comp,0);         // 2 is actually the number of fields to compute
                                    intM[2*d + 1] = mgn_field[d](dof_comp,0);
                                    // intM[2*d + 2] = pot[d](dof_comp,0);
                                    // intM[2*d + 3] = field[d](dof_comp,0);
                                }
                        }
                    toc("integral_computation",FLAGS_v>0);

                    //commsC1M[i].barrier();

                    // // Add all contributions to int M in proc master of subcomm
                    // tic();
                    // mpi::reduce(commsC1M[i], intM.data(), intM.size(), intMsum.data(), std::plus<double>(), 0);
                    // toc("reduce_contributions_of_integrals", FLAGS_v > 0);

                    if( commsC1M[i].rank() == 0 ) // Proc in \Omega_M has now all the contribs of \Omega_C
                        {
                            // Complete A and B element_type
                            for(int d=0; d<dofM_size; d++)
                                {
                                    A.set(dofM[d].template get<1>(), intM[2*d] );       // 2 is actually the number of fields to compute
                                    B.set(dofM[d].template get<1>(), intM[2*d+1]);
                                    // A.set(dofM[d].template get<1>(), intM[2*d+2] );
                                    // B.set(dofM[d].template get<1>(), intM[2*d+3]);
                                }
                        }
                }
            Environment::worldComm().barrier();
        } // Each proc having dofs in Omega_M have integrals values on its dof

    A.close();
    B.close();
    LOG(INFO) << "Proc[" << Environment::worldComm().globalRank() << "] finished" << std::endl;
    toc("[biot-savart] computing A and B", FLAGS_v>0);

    return boost::make_tuple( (boost::any)A, (boost::any)B);
}

template<int DimC, int DimB, int Order, int G_order>
void
Feel::BiotSavartImpl<DimC, DimB, Order, G_order>::export_results( const std::string&  appName,
                                                                  Feel::BiotSavartImpl<DimC, DimB, Order, G_order>::mesh_box_ptrtype& box_mesh,
                                                                  vec_box_element_type A,
                                                                  vec_box_element_type B)
{
    //LOG(INFO) << "biot_savart_impl: export_results" << std::endl;

    auto mu0 = 4*M_PI*1e-7; //SI unit : H.m-1 = m.kg.s-2.A-2
    if( soption(_name="units") == "mm" )
        mu0 *= 1000;

    tic();
    // export_ptrtype exporter( export_type::New( (boost::format( "%1%-biotsavart" ) % appName).str() ) );
    export_ptrtype M_export = exporter( _mesh=box_mesh, _name=(boost::format( "%1%-biotsavart" ) % appName).str() );

    // should also export A and B
    // M_export->step(0)->setMesh(box_mesh);
    if ( A.size() != 0 )
        M_export->add( "A_bs", A );
    M_export->add( "B_bs", B );

    // Computation + export for levitation
    if( boption("biot_savart.levitation") )
        {
            Xh_box_global_scalar = box_space_type::New( box_mesh ); //To export levitation quantities

            double chi = doption("biot_savart.chi");
            double rho = doption("biot_savart.rho");
            double g = doption("biot_savart.g");

            auto B2_bs = vf::project( _space=Xh_box_global_scalar, _range=elements(box_mesh), _expr=trans(idv(B))*idv(B) );
            auto gradB2_bs = vf::project( _space=Xh_box_global, _range=elements(box_mesh), _expr=trans(gradv(B2_bs)) );

            auto mgn_gravit = vf::project( _space=Xh_box_global_scalar, _range=elements(box_mesh), _expr=chi*idv(B2_bs)/(2*mu0) - rho*g*Pz() );
            auto grad_mgn_gravit =  vf::project( _space=Xh_box_global, _range=elements(box_mesh), _expr=trans(gradv(mgn_gravit)) );

            M_export->add( "B2_bs", B2_bs );
            M_export->add( "Grad_B2_bs", gradB2_bs );
            M_export->add( "Magneto_gravit_bs", mgn_gravit );
            M_export->add( "Grad_magneto_gravit_bs", grad_mgn_gravit );
        }
    M_export->save();
    toc("[biot-savart] export results", FLAGS_v>0);

    tic();
    auto B_m = minmax( _range=elements(B.mesh()), _pset=_Q<4>(), _expr=inner(idv(B),idv(B)) );
    auto B_min = B_m.min();
    auto B_max = B_m.max();
    auto B_p_min = B_m.argmin();
    auto B_p_max = B_m.argmax();

    if( Environment::worldComm().globalRank() == 0 )
        {
            std::cout << "\n BiotSavart MAGNETIC FIELD VALUES :" << std::endl;
            Eigen::IOFormat CleanFmt(Eigen::StreamPrecision, Eigen::DontAlignCols, ", ", ", ", "", "", "{", "}");
            std::cout << "B min : " << math::sqrt(B_min) << " T at " << B_p_min.format(CleanFmt) << "\n";
            std::cout << "B max : " << math::sqrt(B_max) << " T at " << B_p_max.format(CleanFmt) << "\n";
            std::cout << "******************************** \n" << std::endl;
        }
    toc("[biot-savart] basic stats", FLAGS_v>0);

}

template<int DimC, int DimB, int Order, int G_order>
void
Feel::BiotSavartImpl<DimC, DimB, Order, G_order>::init( Feel::BiotSavartImpl<DimC, DimB, Order, G_order>::mesh_ptrtype& conductor_mesh,
                                                        Feel::BiotSavartImpl<DimC, DimB, Order, G_order>::mesh_box_ptrtype& box_mesh)
{
    //LOG(INFO) << "biot_savart_impl: init" << std::endl;
    namespace mpi=boost::mpi;

    Xh_box_global = vec_box_space_type::New( box_mesh );
    Xh_cond_global = vec_space_type::New( conductor_mesh );

    LOG(INFO) << "[Biot_Savart_imp] Number of dofs in box mesh = " <<  Xh_box_global->nDof() << std::endl;
    LOG(INFO) << "[Biot_Savart_imp] Number of dofs in cond mesh = " << Xh_cond_global->nDof() << std::endl;
    // Omega_M : magnetic field region (box_mesh)
    // Omega_C : conductor (conductor_mesh)

    // 1 : Identify which procs are in Omega_C, Omega_M
    // local isIn vector : isIn[0] = proc is in Omega_C, isIn[1] = proc is in Omega_M
    isIn = {0, 0}; //( 2, 0 );

#if 0
    if( nelements( markedelements(mesh, conductor) ) > 0 ) // current proc has dofs in Omega_C
        isIn[0] = 1;
    if( nelements( markedelements(mesh, box) ) > 0 ) // current proc has dofs in Omega_M
        isIn[1] = 1;
#endif
    if( Xh_cond_global->nLocalDof()  > 0 ) // current proc has dofs in Omega_C
        isIn[0] = 1;
    if( Xh_box_global->nLocalDof() > 0 ) // current proc has dofs in Omega_M
        isIn[1] = 1;

    // Propagate localization infos (isIn) to all procs
    tic();
    std::vector<int> isInGlob( 2*Environment::worldComm().size() );
    mpi::all_gather( Environment::worldComm(), isIn.data(), 2, isInGlob );
    toc("localization_infos_propagation", FLAGS_v > 0);

    // 2 : Create sub communicators
    // pC : procs having dofs in Omega_C
    // pM : procs having dofs in Omega_M
    tic();
    std::vector<int> pC,pM,pC1M;
    for(int i=0; i<Environment::worldComm().size(); i++)
        {
            if( isInGlob[2*i] )
                pC.push_back( i );
            if( isInGlob[2*i + 1] )
                pM.push_back( i ); //Check if pM has at least one element for each procs
        }

    // commC = communicator with all procs which in pC
    auto groupC = Environment::worldComm().group().include(pC.begin(), pC.end());
    mpi::communicator commC(Environment::worldComm(), groupC);

    // commM = communicator with all procs which in pM
    auto groupM = Environment::worldComm().group().include(pM.begin(), pM.end());
    mpi::communicator commM(Environment::worldComm(), groupM);

    // commC1M = communicator with 1 Omega_M part (first procs of the group) + all Omega C
    // commsC1M = set of commC1M communicators, for each part of Omega M
    //std::vector< mpi::communicator > commsC1M;

    // First group
    pC1M.resize( pC.size() );
    pC1M = pC;
    auto find = std::find( pC1M.begin(), pC1M.end(), pM[0] );
    if( find != pC1M.end() )
        pC1M.erase(find); //No double definition
    pC1M.insert(pC1M.begin(), pM[0] );

    auto groupC1M = Environment::worldComm().group().include(pC1M.begin(), pC1M.end());
    mpi::communicator commC1M(Environment::worldComm(), groupC1M);
    commsC1M.push_back( commC1M );

    // Other groups
    for( int i=1; i<pM.size(); i++)
        {
            pC1M.clear();
            pC1M.resize( pC.size() );
            pC1M = pC;
            auto find = std::find( pC1M.begin(), pC1M.end(), pM[i] );
            if( find != pC1M.end() )
                pC1M.erase(find);  //No double definition
            pC1M.insert(pC1M.begin(), pM[i] );

            auto groupC1M = Environment::worldComm().group().include(pC1M.begin(), pC1M.end());
            mpi::communicator commC1M(Environment::worldComm(), groupC1M);
            commsC1M.push_back( commC1M );
        }
    toc("subcomms_creation", FLAGS_v > 0);
    //LOG(INFO) << "biot_savart_impl: init done" << std::endl;
}

template<int DimC, int DimB, int Order, int G_order>
typename Feel::BiotSavartImpl<DimC, DimB, Order, G_order>::vec_box_element_type
Feel::BiotSavartImpl<DimC, DimB, Order, G_order>::biot_savart_A_impl( Feel::BiotSavartImpl<DimC, DimB, Order, G_order>::mesh_ptrtype& conductor_mesh,
                                                                      Feel::BiotSavartImpl<DimC, DimB, Order, G_order>::mesh_box_ptrtype& box_mesh,
                                                                      vec_element_type current_density)
{
    //LOG(INFO) << "biot_savart_impl: biot_savart_A_impl" << std::endl;
    std::cout << "biot_savart_impl: biot_savart_A_impl" << std::endl;
    auto coeff = 1/(4*M_PI);
    auto mu0 = 4*M_PI*1e-7; //SI unit : H.m-1 = m.kg.s-2.A-2
    // if( soption(_name="units") == "mm" )
    //     mu0 *= 1000;

    tic();
    namespace mpi=boost::mpi;
    // call init instead
    init(conductor_mesh, box_mesh);
    // std::cout << "init(conductor_mesh, box_mesh);" << std::endl;
// #if 0
    std::cout << "Proc " << Environment::worldComm().globalRank()
              << " Number of dofs in box mesh = " <<  Xh_box_global->nLocalDof() << std::endl;
    std::cout << "Proc " << Environment::worldComm().globalRank()
              << " Number of dofs in cond mesh = " << Xh_cond_global->nLocalDof() << std::endl;
// #endif

    vec_box_element_type A = Xh_box_global->element(); //magnetic potential
    A.zero();

    // 3 : Communications
    // For each commC1M, Omega_M part send coordinates of its dofs to all parts of Omega_C
    // All parts of Omega_C compute the integrals and re-send it to Omega_M

    dof_points_type dofM; //dofs
    std::vector<double> intM;
    //std::vector<double> intMsum;
    int dofM_size;
    std::vector<mpi::request> requests(1);

    
    Environment::worldComm().barrier();
    for(int i=0; i<commsC1M.size(); i++)
        {
            LOG(INFO) << "biot_savart_A_impl: Proc[" << Environment::worldComm().globalRank() << "] " << i << "th group loop" << std::endl;
            std::cout << "biot_savart_A_impl: Proc[" << Environment::worldComm().globalRank() << "] " << i << "th group loop" << std::endl;
            // std::cout << "commsC1M0: "<< commsC1M <<"Proc[" << Environment::worldComm().globalRank() << "] "<< std::endl;
            if( commsC1M[i] ) // Current communicator
                {
                    std::cout << "commsC1M["<<i<<"] : [" << commsC1M[i].rank() << "/"<< commsC1M.size() <<"] " <<Environment::worldComm().globalRank()<< std::endl;
                    if(commsC1M[i].rank() == 0 ) // Proc of rank 0 is the Omega_M part
                        {
                            dofM.clear();
                            for ( size_type dof_id = 0; dof_id < Xh_box_global->nLocalDofWithGhost() ; ++dof_id )
                                dofM.push_back( Xh_box_global->dof()->dofPoint(dof_id) );
                            dofM_size = dofM.size();

                            LOG(INFO) << "Proc[" << Environment::worldComm().globalRank()
                                      << "]: " << dofM_size << " dofs in Omega_M" << std::endl;
                            std::cout << "Proc[" << Environment::worldComm().globalRank()
                                      << "]: " << dofM_size << " dofs in Omega_M" << std::endl;
                        }

                    //Proc 0 broadcasts its dofs
                    tic();
                    mpi::broadcast( commsC1M[i], dofM_size, 0);
                    dofM.resize( dofM_size );
                    mpi::broadcast( commsC1M[i], dofM.data(), dofM.size(), 0);
                    toc("broadcast", FLAGS_v > 0);
                    std::cout << "Proc[" << Environment::worldComm().globalRank()
                                      << "]: " <<  " broadcast" << std::endl;
                    intM.resize( dofM_size );
                    //intMsum.resize( dofM_size );

                    //commsC1M[i].barrier(); //wait for broadcast

                    tic();
                    if( isIn[0] ) // Parts of Omega_C
                        {
                            std::vector<Eigen::Matrix<double,3,1>> coords( dofM_size );
                            // fill vector of coordinates
                            std::cout << "Proc[" << Environment::worldComm().globalRank()
                                      << "]: " <<  " fill vector of coordinates" << std::endl;
                            for(int d=0; d<dofM_size; d++)
                                {
                                    auto dof_coord = dofM[d].template get<0>();
                                    Eigen::Matrix<double,3,1> coord;
                                    coord << dof_coord[0],dof_coord[1],dof_coord[2];
                                    coords[d] = coord;
                                }
                            std::cout << "Proc[" << Environment::worldComm().globalRank()
                                      << "]: " <<  " before dist" << std::endl;
                            // Integrals computation
                            auto dist = inner( _e1v-P(),_e1v-P(), mpl::int_<InnerProperties::IS_SAME|InnerProperties::SQRT>() );
                            std::cout << "Proc[" << Environment::worldComm().globalRank()
                                      << "]: " <<  " before mgn_pot: mu0=" <<mu0<<", coeff="<< coeff<< std::endl;
                            auto mgn_pot = integrate(_range = elements( conductor_mesh ),
                                                     _expr=mu0*coeff*idv(current_density)/dist,
                                                     _quad=_Q<1>()
                                                     ).template evaluate(coords);

                            std::cout << "Proc[" << Environment::worldComm().globalRank()
                                      << "]: " <<  " Fill results tab" << std::endl;
                            // Fill results tab
                            for(int d=0; d<dofM_size; d++)
                                {
                                    auto dof_comp = dofM[d].template get<2>();
                                    intM[d] = mgn_pot[d](dof_comp,0);
                                }
                            std::cout << "Proc[" << Environment::worldComm().globalRank()
                                      << "]: " <<  " end if in omega_c" << std::endl;
                        }
                    toc("integral_computation",FLAGS_v>0);

                    //commsC1M[i].barrier();

                    // // Add all contributions to int M in proc master of subcomm
                    // tic();
                    // mpi::reduce(commsC1M[i], intM.data(), intM.size(), intMsum.data(), std::plus<double>(), 0);
                    // toc("reduce_contributions_of_integrals", FLAGS_v > 0);
                    std::cout << "Proc[" << Environment::worldComm().globalRank()
                                      << "]: " <<  " Proc in Omega_M has now all the contribs of Omega_C" << std::endl;
                    if( commsC1M[i].rank() == 0 ) // Proc in \Omega_M has now all the contribs of \Omega_C
                        {
                            std::cout << "Proc[" << Environment::worldComm().globalRank()
                                      << "]: " <<  " commsC1M[i].rank[" <<commsC1M[i].rank()<<"]==0"<< std::endl;
                            // Complete A element_type
                            for(int d=0; d<dofM_size; d++)
                                {
                                    // A.set(dofM[d].template get<1>(), intMsum[d] );
                                    A.set(dofM[d].template get<1>(), intM[d] );
                                }
                        }
                }
            Environment::worldComm().barrier();
            std::cout << "Proc[" << Environment::worldComm().globalRank()
                                      << "]: " <<  " end 1 for loop" << std::endl;
        } // Each proc having dofs in Omega_M have integrals values on its dof

    A.close();
    LOG(INFO) << "Proc[" << Environment::worldComm().globalRank() << "] finished" << std::endl;
    std::cout << "Proc[" << Environment::worldComm().globalRank() << "] finished" << std::endl;
    toc("[biot-savart] compute A", FLAGS_v>0);
    return A;
}

template<int DimC, int DimB, int Order, int G_order>
typename Feel::BiotSavartImpl<DimC, DimB, Order, G_order>::vec_box_element_type
Feel::BiotSavartImpl<DimC, DimB, Order, G_order>::biot_savart_B_impl( Feel::BiotSavartImpl<DimC, DimB, Order, G_order>::mesh_ptrtype& conductor_mesh,
                                                                      Feel::BiotSavartImpl<DimC, DimB, Order, G_order>::mesh_box_ptrtype& box_mesh,
                                                                      vec_element_type current_density)
{
    LOG(INFO) << "biot_savart_impl: biot_savart_B_impl" << std::endl;
    auto coeff = 1/(4*M_PI);
    auto mu0 = 4*M_PI*1e-7; //SI unit : H.m-1 = m.kg.s-2.A-2
    if( soption(_name="units") == "mm" )
        mu0 *= 1000;

    tic();
    namespace mpi=boost::mpi;

    // call init instead
    init(conductor_mesh, box_mesh);

#if 0
    std::cout << "Proc " << Environment::worldComm().globalRank()
              << " Number of dofs in box mesh = " <<  Xh_box_global->nLocalDof() << std::endl;
    std::cout << "Proc " << Environment::worldComm().globalRank()
              << " Number of dofs in cond mesh = " << Xh_cond_global->nLocalDof() << std::endl;
#endif

    vec_box_element_type B = Xh_box_global->element(); //magnetic field
    B.zero();

    // 3 : Communications
    // For each commC1M, Omega_M part send coordinates of its dofs to all parts of Omega_C
    // All parts of Omega_C compute the integrals and re-send it to Omega_M

    dof_points_type dofM; //dofs
    std::vector<double> intM;
    //std::vector<double> intMsum;
    int dofM_size;
    std::vector<mpi::request> requests(1);

    LOG(INFO) << "commsC1M.size()=" << commsC1M.size() << std::endl;
    Environment::worldComm().barrier();
    for(int i=0; i<commsC1M.size(); i++)
        {
            LOG(INFO) << "biot_savart_B_impl: Proc[" << Environment::worldComm().globalRank() << "] " << i << "th group loop" << std::endl;
            if( commsC1M[i] ) // Current communicator
                {
                    if(commsC1M[i].rank() == 0 ) // Proc of rank 0 is the Omega_M part
                        {
                            dofM.clear();
                            for ( size_type dof_id = 0; dof_id < Xh_box_global->nLocalDofWithGhost() ; ++dof_id )
                                dofM.push_back( Xh_box_global->dof()->dofPoint(dof_id) );
                            dofM_size = dofM.size();

                            LOG(INFO) << "Proc[" << Environment::worldComm().globalRank()
                                      << "]: " << dofM_size << " dofs in Omega_M" << std::endl;
                        }

                    //Proc 0 broadcasts its dofs
                    tic();
                    mpi::broadcast( commsC1M[i], dofM_size, 0);
                    dofM.resize( dofM_size );
                    mpi::broadcast( commsC1M[i], dofM.data(), dofM.size(), 0);
                    toc("broadcast", FLAGS_v > 0);

                    intM.resize( dofM_size );
                    //intMsum.resize( dofM_size );

                    //commsC1M[i].barrier(); //wait for broadcast

                    tic();
                    if( isIn[0] ) // Parts of Omega_C
                        {
                            std::vector<Eigen::Matrix<double,3,1>> coords( dofM_size );
                            // fill vector of coordinates
                            for(int d=0; d<dofM_size; d++)
                                {
                                    auto dof_coord = dofM[d].template get<0>();
                                    Eigen::Matrix<double,3,1> coord;
                                    coord << dof_coord[0],dof_coord[1],dof_coord[2];
                                    coords[d] = coord;
                                }

                            // Integrals computation
                            auto dist = inner( _e1v-P(),_e1v-P(), mpl::int_<InnerProperties::IS_SAME|InnerProperties::SQRT>() );

                            auto mgn_field = integrate(_range = elements( conductor_mesh ),
                                                       _expr=mu0*coeff*cross(idv(current_density), _e1v-P())/(dist*dist*dist),
                                                       _quad=_Q<1>()
                                                       ).template evaluate(coords);
                            // Fill results tab
                            for(int d=0; d<dofM_size; d++)
                                {
                                    auto dof_comp = dofM[d].template get<2>();
                                    intM[d] = mgn_field[d](dof_comp,0);
                                }
                        }
                    toc("integral_computation",FLAGS_v>0);

                    //commsC1M[i].barrier();

                    // // Add all contributions to int M in proc master of subcomm
                    // tic();
                    // mpi::reduce(commsC1M[i], intM.data(), intM.size(), intMsum.data(), std::plus<double>(), 0);
                    // toc("reduce_contributions_of_integrals", FLAGS_v > 0);

                    if( commsC1M[i].rank() == 0 ) // Proc in \Omega_M has now all the contribs of \Omega_C
                        {
                            // Complete A and B element_type
                            for(int d=0; d<dofM_size; d++)
                                {
                                    // B.set(dofM[d].template get<1>(), intMsum[d]);
                                    B.set(dofM[d].template get<1>(), intM[d]);
                                }
                        }
                }
            Environment::worldComm().barrier();
        } // Each proc having dofs in Omega_M have integrals values on its dof

    B.close();
    LOG(INFO) << "Proc[" << Environment::worldComm().globalRank() << "] finished" << std::endl;
    toc("[biot-savart] compute B", FLAGS_v>0);
    return B;
}

#endif //__BS_IMPL_HPP
