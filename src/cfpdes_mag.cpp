#include <feel/feelcore/environment.hpp>
#include <toolboxes/feel/feelmodels/coefficientformpdes/coefficientformpdes.hpp>
#include <feel/feelmodels/coefficientformpdes/coefficientformpdes_registered_type.hpp>
// #include <feel/feelfilters/loadmesh.hpp>

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
    // a.load(_path="$cfgdir/Magnetic_potential.h5",_name="Magnetic_potential");
    // cfpdes->modelMesh().template updateField<mesh_type>( "Amap", idv(a), "Pchv1" );//,rangeEltBC, "Pchv1" );
    
    cfpdes->printAndSaveInfo();
    cfpdes->solve();
    cfpdes->exportResults();

    return 0;
}
