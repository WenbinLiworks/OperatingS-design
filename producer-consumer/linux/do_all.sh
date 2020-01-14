
echo "compiling..."
g++ sem_main.cpp -o sem_main -lpthread
g++ producer_main.cpp -o producer_main -lpthread
g++ consumer_main.cpp -o consumer_main -lpthread
./sem_main
