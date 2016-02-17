! { dg-do run }
! { dg-require-effective-target offload_device }

program e_57_3
  use omp_lib, only: omp_is_initial_device, omp_get_num_devices,&
  omp_get_default_device, omp_set_default_device
  logical :: res
  integer :: default_device

  default_device = omp_get_default_device ()
  !$omp target
    res = omp_is_initial_device ()
  !$omp end target
  if (res) call abort

  call omp_set_default_device (omp_get_num_devices ())
  !$omp target
    res = omp_is_initial_device ()
  !$omp end target
  if (.not. res) call abort
end program
