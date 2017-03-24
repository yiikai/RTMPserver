 g++ -g  -o URS *.cpp -lavformat -lavdevice -lavcodec -lavutil  -lswresample  -lswscale -lm  -pthread -ldl -lx264 -I ./ -I /home/yiikai/openssl-1.0.1f/include  -L ../openssl/openssl-1.0.1f/ -lcrypto -lssl  -ldl -O0  

