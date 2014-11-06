/* Generated from orogen/lib/orogen/templates/tasks/Task.cpp */

#include "Task.hpp"
#include <rtt/extras/FileDescriptorActivity.hpp>

using namespace magnetometer_lsm303;

Task::Task(std::string const& name)
    : TaskBase(name), mDriver(0)
{
}

Task::Task(std::string const& name, RTT::ExecutionEngine* engine)
    : TaskBase(name, engine), mDriver(0)
{
}

Task::~Task()
{
}



/// The following lines are template definitions for the various state machine
// hooks defined by Orocos::RTT. See Task.hpp for more detailed
// documentation about them.

bool Task::configureHook()
{
  //TODO reserve NUM_SENSORS 
  sensors.assign(_number_devices,base::samples::IMUSensors());
  mDriver = new magnetometer_lsm303::Driver();
  mDriver->setReadTimeout(base::Time::fromMilliseconds(_timeout));
  if(!_device.get().empty()) mDriver->open(_device.get());

  //TODO only set Calibration matrices, if set
  mDriver->setAccCalibrationMatrix(0,_acc_correction_matrix.get());
  mDriver->setMagCalibrationMatrix(0,_mag_correction_matrix.get());
//  if(!_acc_correction_matrix.get().empty()) mDriver->setAccCalibrationMatrix(_acc_correction_matrix.get());

    if (! TaskBase::configureHook())
        return false;
    return true;
}
bool Task::startHook()
{
  RTT::extras::FileDescriptorActivity* fd_activity = getActivity<RTT::extras::FileDescriptorActivity>();
  int fd = mDriver->getFileDescriptor();
  fd_activity->watch(fd);
  fd_activity->setTimeout(_timeout);
  
    if (! TaskBase::startHook())
        return false;
    return true;
}

void Task::updateHook()
{
  try{
    mDriver->read();
    int no = mDriver->getDevNo();
    sensors.at(no).time = base::Time::now();
    sensors.at(no).acc = mDriver->getAcc();
    sensors.at(no).mag = mDriver->getMag();
    _test.write(sensors);
    if(mDriver->getDevNo() == 0) { //TODO support chained devices, right now just using first device
      base::samples::IMUSensors imu_raw, imu_cal;
      imu_raw.time = imu_cal.time = base::Time::now();
      imu_raw.acc[0] = mDriver->getRawAccX();
      imu_raw.acc[1] = mDriver->getRawAccY();
      imu_raw.acc[2] = mDriver->getRawAccZ();
      imu_raw.mag[0] = mDriver->getRawMagX();
      imu_raw.mag[1] = mDriver->getRawMagY();
      imu_raw.mag[2] = mDriver->getRawMagZ();
      //TODO invalidate imu_raw.gyro.invalidate();
      _raw_values.write(imu_raw);
      imu_cal.acc  = mDriver->getAcc();
      imu_cal.mag  = mDriver->getMag();
      //TODO invalidate imu_cal.gyro.invalidate();
      _calibrated_values.write(imu_cal);

      //TODO check for calibration values and if found write to _calibrated_values
      //TODO construct RigidBodyState and put out to _heading
    }
  }
  catch(iodrivers_base::TimeoutError){
    std::cout << "Timeout error while reading" << std::endl;
  }

  TaskBase::updateHook();
}

void Task::errorHook()
{
    TaskBase::errorHook();
}

void Task::stopHook()
{
  RTT::extras::FileDescriptorActivity* activity =
    getActivity<RTT::extras::FileDescriptorActivity>();

  if(activity) activity->clearAllWatches();

  TaskBase::stopHook();
}
void Task::cleanupHook()
{
  mDriver->close();
    TaskBase::cleanupHook();
}
