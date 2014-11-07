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

bool Task::configureHook()
{
  imu_cal.assign(_number_devices,base::samples::IMUSensors());
  imu_raw.assign(_number_devices,base::samples::IMUSensors());
  mDriver = new magnetometer_lsm303::Driver();
  mDriver->setReadTimeout(base::Time::fromMilliseconds(_timeout));
  if(!_device.get().empty()) mDriver->open(_device.get());

  //TODO only set Calibration matrices, if set
  int i;
  for (i = 0; i < _acc_correction_matrices.get().size(); ++i)
  {
      mDriver->setAccCalibrationMatrix(i, _acc_correction_matrices.get().at(i));
  }

  for (i = 0; i < _mag_correction_matrices.get().size(); ++i)
  {
      mDriver->setMagCalibrationMatrix(i, _mag_correction_matrices.get().at(i));
  }

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
        imu_cal.at(no).time = base::Time::now();
        imu_cal.at(no).acc = mDriver->getAcc();
        imu_cal.at(no).mag = mDriver->getMag();
        imu_cal.at(no).gyro = base::Vector3d::Ones() * base::unknown<double>();
        _calibrated_values.write(imu_cal);

        imu_raw.at(no).time = base::Time::now();
        imu_raw.at(no).acc[0] = mDriver->getRawAccX();
        imu_raw.at(no).acc[1] = mDriver->getRawAccY();
        imu_raw.at(no).acc[2] = mDriver->getRawAccZ();
        imu_raw.at(no).mag[0] = mDriver->getRawMagX();
        imu_raw.at(no).mag[1] = mDriver->getRawMagY();
        imu_raw.at(no).mag[2] = mDriver->getRawMagZ();
        imu_raw.at(no).gyro   = base::Vector3d::Ones() * base::unknown<double>(); 
        _raw_values.write(imu_raw);
    }
    catch(iodrivers_base::TimeoutError){
        std::cout << "Timeout error while reading" << std::endl;
    }
    catch(std::out_of_range &oor){
        std::cout << "Out of range error. Please check for right amount of sensors given." << std::endl;
        stop();
        cleanup();
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
