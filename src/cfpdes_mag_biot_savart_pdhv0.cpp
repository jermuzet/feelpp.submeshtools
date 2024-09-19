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

#include "biot_savart_impl_P0.hpp"

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
    J.on(_range=markedelements(mesh,"Conductor"), _expr=idv(current_density));


    //BIOT_SAVART.HPP L359
    BiotSavartImpl<3,2,1,1> impl_object;
    // MeshType box_mesh = createSubmesh(mesh, markedfaces(mesh, "Infty"));
    auto box_mesh = createSubmesh( _mesh=mesh, _range=markedfaces(mesh,"Infty"), _update=0 );
    auto A = impl_object.biot_savart_A_impl( conductor_mesh, box_mesh, J );

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
    auto rangeEltBC = markedelements(mesh,"Infty");
    // std::cout << "rangeEltBC " << rangeEltBC << std::endl;
    // auto Ah = Pchv<1>(mesh);//, rangeEltBC );
    // auto a = Ah->element();
    auto a = runBiotSavart<mesh_type>(mesh);
    cfpdes->modelMesh().template updateField<mesh_type>( "Amap", idv(a), rangeEltBC, "Pchv1" );

    cfpdes->printAndSaveInfo();
    cfpdes->solve();
    cfpdes->exportResults();

    return 0;
}
