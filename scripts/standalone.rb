require 'orocos'
require 'vizkit'

include Orocos

Orocos.initialize

Orocos.run "magnetometer_lsm303::Standalone" => "mag", "imu_xsens::Task" => "imu_xsens" do
  mag = Orocos.name_service.get("mag")
  imu = Orocos.name_service.get("imu_xsens")

  imu.port = "/dev/ttyUSB0" 
  imu.timeout = 1.0

  mag.imu.connect_to imu.calibrated_sensors

  imu.configure
  mag.configure

  imu.start
  mag.start
  gui = Vizkit.default_loader.TaskInspector()
  gui.add_name_service(Orocos::Async.name_service)
  gui.show()
  Vizkit.exec()
end 
