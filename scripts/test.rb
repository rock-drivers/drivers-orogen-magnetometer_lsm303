require 'orocos'

include Orocos

Orocos.run "magnetometer_lsm303::Task" => "mag" do
  mag = Orocos.name_service.get("mag")

  mag.device = "serial:///dev/ttyUSB0:57600"
  mag.timeout = 2000

  array = [6.024509004513876409e-04, 9.222518010499308248e-06, 1.451897892865949735e-05,
           -9.251710063901275722e-06, 5.992525159615659947e-04, -2.658006955663259549e-07,
           -6.294512803626715024e-07, -5.806993195120457862e-07, 6.169533584373849706e-04,
           -1.716468480168042987e-01, -6.088222315248522504e-02, -7.332075855584828661e-01]

  mag.acc_correction_matrix = Eigen::MatrixX.from_a(array,4,3,false)

  mag.configure
  mag.start

  Orocos.watch(mag)
end
