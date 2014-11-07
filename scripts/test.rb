require 'orocos'

include Orocos

Orocos.run "magnetometer_lsm303::Task" => "mag" do
  mag = Orocos.name_service.get("mag")

  mag.device = "serial:///dev/ttyUSB0:57600"
  mag.timeout = 2000

  mag.number_devices = 2

  acc_corr = [6.024509004513876409e-04, 9.222518010499308248e-06, 1.451897892865949735e-05,
           -9.251710063901275722e-06, 5.992525159615659947e-04, -2.658006955663259549e-07,
           -6.294512803626715024e-07, -5.806993195120457862e-07, 6.169533584373849706e-04,
           -1.716468480168042987e-01, -6.088222315248522504e-02, -7.332075855584828661e-01]

  mag_corr_als = [2.082700000000000100e-04, 1.172800000000000020e-06, -6.503199999999999629e-07,
              0.000000000000000000e+00, 2.144699999999999981e-04, 5.749300000000000164e-06,
              0.000000000000000000e+00, 0.000000000000000000e+00, 2.087799999999999899e-04,
              -3.509766040000000115e-02, 2.122459460999999992e-01, -2.837946539999999795e-01]

  mag_corr_st = [2.102506634685734170e-04, 0.000000000000000000e+00, 0.000000000000000000e+00,
                 0.000000000000000000e+00, 2.128220991349812194e-04, 0.000000000000000000e+00,
                 0.000000000000000000e+00, 0.000000000000000000e+00, 2.081006681865948569e-04,
                 -4.160518231281334217e-02, 2.149566658130758756e-01, -2.847187529487317237e-01]

  acc_cor_m = Eigen::MatrixX.from_a(acc_corr,4,3,false)
  mag_cor_m = Eigen::MatrixX.from_a(mag_corr_als,4,3,false)
  mag.acc_correction_matrices = [acc_cor_m, acc_cor_m]
  mag.mag_correction_matrices = [mag_cor_m, mag_cor_m]

  mag.configure
  mag.start

  Orocos.watch(mag)
end
