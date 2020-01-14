echo "compiling..."
g++ mycp.cpp -o mycp

echo "running \"./mycp test/ test_new/ -show -count -time\": \n"
./mycp -show test/ test_new/

echo "\nls test -l: "
ls -l test
echo "\n"
echo "ls test_new -l:"
ls -l test_new
