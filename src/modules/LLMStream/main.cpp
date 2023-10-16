#include "Module.h"

int main(int argc, char** argv)
{
    yarp::os::Network::init();

    yarp::os::ResourceFinder rf;
    rf.configure(argc,argv);

    Module module;
    
    if(!module.runModule(rf))
    {
        yError() << "Cannot run the module";
        return EXIT_FAILURE;
    }
    
}