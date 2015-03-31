/* Generated from orogen/lib/orogen/templates/tasks/Task.cpp */

#include "Standalone.hpp"
#include <magnetometer_lsm303/lsm303.hpp>

using namespace magnetometer_lsm303;

Standalone::Standalone(std::string const& name)
    : StandaloneBase(name)
{
}

Standalone::Standalone(std::string const& name, RTT::ExecutionEngine* engine)
    : StandaloneBase(name, engine)
{
}

Standalone::~Standalone()
{
}



/// The following lines are template definitions for the various state machine
// hooks defined by Orocos::RTT. See Standalone.hpp for more detailed
// documentation about them.

bool Standalone::configureHook()
{
    if (! StandaloneBase::configureHook())
        return false;
    return true;
}
bool Standalone::startHook()
{
    if (! StandaloneBase::startHook())
        return false;
    return true;
}
void Standalone::updateHook()
{
    base::samples::IMUSensors sample;
    _imu.read(sample);

    double heading = magnetometer_lsm303::computeHeading(sample.acc,sample.mag);
    _heading.write(heading);

    base::samples::RigidBodyState rbs(true);
    rbs.time = base::Time::now();
    //rbs.sourceFrame = _source_frame;
    //rbs.targetFrame = _target_frame;
    Eigen::AngleAxisd aa(heading,Eigen::Vector3d::UnitZ());
    base::Quaterniond h(aa);

    base::Quaterniond acc2world(Eigen::Quaternion<double>::FromTwoVectors(sample.acc,Eigen::Vector3d::UnitZ()));
    
    rbs.orientation = acc2world * h;
    _orientation.write(rbs);
    
    StandaloneBase::updateHook();
}
void Standalone::errorHook()
{
    StandaloneBase::errorHook();
}
void Standalone::stopHook()
{
    StandaloneBase::stopHook();
}
void Standalone::cleanupHook()
{
    StandaloneBase::cleanupHook();
}
