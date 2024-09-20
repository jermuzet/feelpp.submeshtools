#include <filesystem>
#include <iostream>

#include <feel/feelcore/environment.hpp>
#include <toolboxes/feel/feelmodels/coefficientformpdes/coefficientformpdes.hpp>
#include <feel/feelmodels/coefficientformpdes/coefficientformpdes_registered_type.hpp>
// #include <feel/feelfilters/loadmesh.hpp>


// #include <feel/feelcore/environment.hpp>
#include <feel/feelcore/json.hpp>
#include <feel/feelcore/ptreetools.hpp>
#include <feel/feelcore/utility.hpp>
#include <feel/feeldiscr/pchv.hpp>
#include <feel/feeldiscr/pdhv.hpp>
#include <feel/feeldiscr/minmax.hpp>
#include <feel/feelfilters/exporter.hpp>
#include <feel/feelfilters/loadmesh.hpp>
#include <feel/feelvf/form.hpp>
#include <feel/feelvf/vf.hpp>
#include <feel/feelts/bdf.hpp>

#include "MagnetTools.hpp"
#include "MagnetTools_Insert.hpp"

namespace Feel
{
template <typename MeshType>
auto runBmap( std::shared_ptr<MeshType> const& mesh )
{
    std::string datafileName = Environment::expand("$cfgdir/torus.d"); //file .d
    // eventually replace $cfgdir by its value
    Feel::cout << "insertdatafile: " << datafileName << std::endl;
    std::string units = "m"; //m or mm

    Feel::InsertData insert(datafileName, units,  0.0, 31000.0, 0.0);
    insert.set_currents(0.0, 31000.0, 0.0);
    MyDouble units_by_defaults = insert.units_by_defaults;
    
    // using mesh_t = Mesh<Simplex<Dim>>;
    // auto mesh = loadMesh( _mesh = new mesh_t, _filename = specs["/Meshes/bmap/Import/filename"_json_pointer].get<std::string>() );
    
    using A_space_ptrtype = Pchv_ptrtype<MeshType, 1>; // valid ??
    A_space_ptrtype Xh = Pchv<1>(mesh);

    auto A = Xh->element();

    std::vector<bool> done( Xh->dof()->nDof(), false );
    std::vector< std::vector<size_type> > dof_id_list;
    std::vector<size_type> dof_comp_list( Xh->dof()->nDofComponents(), 0);

    // Build geometric mapping
    using A_space_type = Pchv_type<MeshType, 1>;
    typename A_space_type::basis_type::points_type p(MeshType::nRealDim,1);
    auto gmpc = mesh->gm()->preCompute( mesh->gm(), p );

    ublas::vector<MyDouble> field_dof( 3 );
    
    for ( auto const& eltWrap : elements(mesh) )
        {
            auto const& elt = unwrap_ref( eltWrap );
            // Build geometric context associated with element
            auto gmc = mesh->gm()->template context<vm::JACOBIAN|vm::KB>( mesh->element( elt.id() ), gmpc );

            for( auto const& ldof : Xh->dof()->localDof( elt.id() ) )
                {
                    auto idx_in_elt = ldof.first.localDofPerComponent();
                    auto index_global = Xh->dof()->localToGlobal( elt.id(), idx_in_elt, 0 ).index();

                    if( done[index_global] == false )
                        {
                            for( auto c = 0; c < Xh->dof()->nDofComponents(); ++c )
                                {
                                    size_type dof_id = Xh->dof()->localToGlobal( elt.id(), idx_in_elt, c ).index();
                                    dof_comp_list[c] = dof_id;

                                    if( c == 0 )
                                        {
                                            // Add coord to B_Map input file
                                            auto coords = boost::get<0>(Xh->dof()->dofPoint(dof_id));

                                            MyDouble x = coords[0] * units_by_defaults;
                                            MyDouble y = coords[1] * units_by_defaults;
                                            MyDouble z = coords[2] * units_by_defaults;
                                            MyDouble r = std::sqrt(coords[0]*coords[0]+coords[1]*coords[1])*units_by_defaults;
                                            
                                            MyDouble atheta = 0; //GSL_NAN;
                                           
                                            atheta=compute_a_field(r,z, insert.PolyHelix, insert.BitterMagnets, insert.UnifMagnets, insert.OtherHelices);
                                            

                                            /***/
                                            Feel::cout << "Computing A on (" << x  <<  "," << y <<"," << z << ") : ";
                                            Feel::cout << atheta << " ";
                                            Feel::cout << std::endl;
                                            /****/

                                            double ax = atheta*std::cos(std::atan2(y,x)); //GSL_NAN;
                                            double ay = atheta*std::sin(std::atan2(y,x)); //GSL_NAN;
                                            field_dof[0] = ax;
                                            field_dof[1] = ay;
                                            field_dof[2] = 0;
                                        }

                                    A.set( dof_comp_list[c], field_dof[c] );
                                }


                            done[index_global] = true; // All components of dof idx has been seen
                        }
                }
        }

    return A;
}}



int main( int argc, char* argv[] )
{

    std::filesystem::path pwd = std::filesystem::current_path() ;
    std::cout << "pwd " << pwd << std::endl;

    using namespace Feel;

    po::options_description cfpdesoptions( "coefficient-form-pdes options" );
    cfpdesoptions.add( toolboxes_options( "coefficient-form-pdes", "cfpdes" ) );

	Environment env( _argc=argc, _argv=argv,
                     _desc=cfpdesoptions,
                     _about=about(_name="toolboxes_cfpdes",
                                  _author="Feel++ Consortium",
                                  _email="feelpp-devel@feelpp.org"));
    std::cout << "proc " << Environment::rank()+1
              <<" of "<< Environment::numberOfProcessors()
              << std::endl;


    std::filesystem::path cwd = std::filesystem::current_path() ;
    std::cout << "cwd " << cwd << std::endl;

    using model_type = FeelModels::coefficient_form_PDEs_t< Simplex<3,1> >;
    using mesh_type = typename model_type::mesh_type;

    // init cfpdes & mesh
    auto cfpdes = std::make_shared<model_type>( "cfpdes" );
    cfpdes->init();
    auto mesh = cfpdes->mesh();
    auto rangeElt = markedelements(mesh,"Conductor");

    // load V on Conductor submesh
    auto Vh = Pch<1>( mesh, rangeElt );
    auto v = Vh->element();
    v.load(_path="$cfgdir/electric.save/electric-potential.h5",_name="electric-potential");
    cfpdes->modelMesh().template updateField<mesh_type>( "V", idv(v), rangeElt, "Pch1" );

    // load T on Conductor submesh
    auto Th = Pch<1>( mesh, rangeElt );
    auto t = Th->element();
    t.load(_path="$cfgdir/heat.save/temperature.h5",_name="temperature");
    cfpdes->modelMesh().template updateField<mesh_type>( "T", idv(t), rangeElt, "Pch1" );


    // auto rangeEltBC = markedelements(mesh,"Infty");
    // std::cout << "rangeEltBC " << rangeEltBC << std::endl;
    // auto Ah = Pchv<1>(mesh);//, rangeEltBC );
    // auto a = Ah->element();

    // compute & load A from Magnettools
    auto a = runBmap<mesh_type>(mesh);
    cfpdes->modelMesh().template updateField<mesh_type>( "Amap", idv(a), "Pchv1" );//,rangeEltBC, "Pchv1" );
    
    cfpdes->printAndSaveInfo();
    cfpdes->solve();
    cfpdes->exportResults();

    return 0;
}
