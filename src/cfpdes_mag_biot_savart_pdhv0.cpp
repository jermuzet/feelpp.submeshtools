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

#include "biot_savart_impl.hpp"

namespace Feel
{
template <typename MeshType>
auto runBiotSavart( std::shared_ptr<MeshType> const& mesh )
{

    // MeshType conductor_mesh = createSubmesh(mesh, markedelements(mesh, "Conductor"));
    auto conductor_mesh = createSubmesh( _mesh=mesh, _range=markedelements(mesh,"Conductor") , _update=0);
    // std::cout << "conductor_mesh created" << std::endl;

    auto Jh = Pchv<1>( mesh, markedelements(mesh,"Conductor") );
    auto current_density = Jh->element();
    // std::cout << "current_density created" << std::endl;
    current_density.load(_path="$cfgdir/currentDensity.save/J.h5",_name="J");
    // std::cout << "current_density loaded" << std::endl;


    auto Jh0 = Pdhv<0>( mesh, markedelements(mesh,"Conductor") );
    auto J = Jh0->element();
    J.on(_range=elements(mesh), _expr=idv(current_density));
    
    

    std::vector<bool> done( Jh->dof()->nDof(), false );
    std::vector< std::vector<size_type> > dof_id_list;
    std::vector<size_type> dof_comp_list( Jh->dof()->nDofComponents(), 0);
    ublas::vector<double> field_dof( 3 );

    // Build geometric mapping
    using J_space_type = Pchv_type<MeshType, 1>;
    typename J_space_type::basis_type::points_type p(MeshType::nRealDim,1);
    auto gmpc = conductor_mesh->gm()->preCompute( conductor_mesh->gm(), p );
    for ( auto const& eltWrap : elements(conductor_mesh) )
        {
            auto const& elt = unwrap_ref( eltWrap );
            // Build geometric context associated with element
            auto gmc = conductor_mesh->gm()->template context<vm::JACOBIAN|vm::KB>( conductor_mesh->element( elt.id() ), gmpc );

            for( auto const& ldof : Jh->dof()->localDof( elt.id() ) )
                {
                    auto idx_in_elt = ldof.first.localDofPerComponent();
                    auto index_global = Jh->dof()->localToGlobal( elt.id(), idx_in_elt, 0 ).index();

                    if( done[index_global] == false )
                        {
                            for( auto c = 0; c < Jh->dof()->nDofComponents(); ++c )
                                {
                                    size_type dof_id = Jh->dof()->localToGlobal( elt.id(), idx_in_elt, c ).index();
                                    dof_comp_list[c] = dof_id;

                                    if( c == 0 )
                                        {
                                            // Add coord to B_Map input file
                                            auto coords = boost::get<0>(Jh->dof()->dofPoint(dof_id));

                                            // how to make this to work for Axi, 2D and 3D??
                                            // coords have to be in m at this point
                                            double x = coords[0] ;
                                            double y = coords[1] ;
                                            double z = coords[2] ;
                                            
                       
                                            /***/
                                            Feel::cout << "Computing (" << x  <<  "," << y <<"," << z << ") : ";
                                            Feel::cout << std::endl;
                                            /****/
                                            
                                            // field_dof[0] = ax;
                                            // field_dof[1] = ay;
                                            // field_dof[2] = 0;
                                        }

                                    // auto ji = current_density.get()

                                    // J.set( dof_comp_list[c],  );
                                }


                            done[index_global] = true; // All components of dof idx has been seen
                        }
                }
        }


    //BIOT_SAVART.HPP L359
    BiotSavartImpl<3,2,1,1> impl_object;
    // MeshType box_mesh = createSubmesh(mesh, markedfaces(mesh, "Infty"));
    auto box_mesh = createSubmesh( _mesh=mesh, _range=markedfaces(mesh,"Infty"), _update=0 );
    auto A = impl_object.biot_savart_A_impl( conductor_mesh, box_mesh, current_density );

    // auto Ah = Pchv<1>(mesh);//, rangeEltBC );
    // auto A = Ah->element();
    return A;
    
}
}


int main( int argc, char* argv[] )
{
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

    using model_type = FeelModels::coefficient_form_PDEs_t< Simplex<3,1> >;
    using mesh_type = typename model_type::mesh_type;

    auto cfpdes = std::make_shared<model_type>( "cfpdes" );
    cfpdes->init();
    auto mesh = cfpdes->mesh();
    auto rangeElt = markedelements(mesh,"Conductor");

    auto Vh = Pch<1>( mesh, rangeElt );
    auto v = Vh->element();
    v.load(_path="$cfgdir/electric.save/electric-potential.h5",_name="electric-potential");
    cfpdes->modelMesh().template updateField<mesh_type>( "V", idv(v), rangeElt, "Pch1" );

    auto Th = Pch<1>( mesh, rangeElt );
    auto t = Th->element();
    t.load(_path="$cfgdir/heat.save/temperature.h5",_name="temperature");
    cfpdes->modelMesh().template updateField<mesh_type>( "T", idv(t), rangeElt, "Pch1" );

    // std::cout << "begin runBiotSavart" << std::endl;
    // auto rangeEltBC = markedelements(mesh,"Infty");
    // std::cout << "rangeEltBC " << rangeEltBC << std::endl;
    // auto Ah = Pchv<1>(mesh);//, rangeEltBC );
    // auto a = Ah->element();
    auto a = runBiotSavart<mesh_type>(mesh);
    cfpdes->modelMesh().template updateField<mesh_type>( "Amap", idv(a), "Pchv1" );//,rangeEltBC, "Pchv1" );
    
    cfpdes->printAndSaveInfo();
    cfpdes->solve();
    cfpdes->exportResults();

    return 0;
}
