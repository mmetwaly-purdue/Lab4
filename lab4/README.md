Environment Setup
echo "export PATH="/home/min/a/ece695x/labs/common/dlxos/bin:$PATH"" >> ~/.bashrc
echo "export LD_LIBRARY_PATH=/home/min/a/ece695x/labs/common/gcc/lib" >> ~/.bashrc
source ~/.bashrc

Flat

Compile OS
mainframer.sh 'cd flat/os && make'

Compile a particular user program
mainframer.sh '(cd flat/apps/fdisk && make)'
mainframer.sh '(cd flat/apps/ostests && make)'
mainframer.sh '(cd flat/apps/FileTest && make)'

Clean OS
mainframer.sh 'cd flat/os && make clean'

Clean user program
mainframer.sh '(cd flat/apps/fdisk && make clean)'
mainframer.sh '(cd flat/apps/ostests && make clean)'
mainframer.sh '(cd flat/apps/FileTest && make clean)'

How to Run User Program
cd flat/apps/fdisk
make run
cd flat/apps/ostests
make run
cd flat/apps/FileTest
make run