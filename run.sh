rm -r build
sh make.sh
echo '*****************************run result****************************************'
cd ./build/App
./main /home/cold/intel/MPAC-MLDP2/tpl/param/a.param
