FROM ubuntu
ENV TZ=Europe/Moscow
RUN apt-get clean && apt-get update -y
RUN DEBIAN_FRONTEND=noninteractive TZ=Etc/UTC apt-get -y install tzdata git sudo build-essential autoconf libtool libssl-dev zlib1g-dev librdkafka-dev pkg-config cmake gcc git g++ mysql-client libmysqlclient-dev libboost-all-dev libssl-dev && apt-get clean
RUN git clone -b master https://github.com/pocoproject/poco.git && cd poco && mkdir cmake-build && cd cmake-build && cmake .. && make && sudo make install && cd .. && cd ..
RUN git clone https://github.com/tdv/redis-cpp.git && cd redis-cpp && mkdir build && cd build && cmake .. && make && sudo make install && cd .. && cd ..
RUN git clone https://github.com/anbgd/MAI-HL-class && sudo ldconfig && cd MAI-HL-class && mkdir build && cd build && cmake .. && make && cd .. && sudo chmod +x start.sh && ./start.sh && sudo chmod +x load_data.sh && ./load_data.sh
ENTRYPOINT [ "MAI-HL-class/build/service" ]
EXPOSE 8082