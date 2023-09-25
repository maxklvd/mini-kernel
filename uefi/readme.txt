This is a short instruction on how to build OVMF.fd.
For more information, see https://github.com/tianocore/tianocore.github.io/wiki/How-to-build-OVMF

$ git clone git://github.com/tianocore/edk2.git
$ cd edk2
$ git submodule update --init
$ source ./edksetup.sh
$ make -C BaseTools/
$ build -a X64 -t GCC5 -b RELEASE -p OvmfPkg/OvmfPkgX64.dsc
$ find Build -name OVMF.fd
