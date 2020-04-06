#pragma clang diagnostic push
#pragma ide diagnostic ignored "MemberFunctionCanBeStatic"
#pragma ide diagnostic ignored "performance-unnecessary-value-param"
#pragma clang diagnostic ignored "-Wmissing-noreturn"

#ifndef MULTYTHREADING_SOCKETSERVERSYSTEMINTERRUPTER_H
#define MULTYTHREADING_SOCKETSERVERSYSTEMINTERRUPTER_H

#include "PlatformDefiner.h"
#include <iostream>
#include <functional>


#if defined(PLATFORM_CODE)
    #if PLATFORM_CODE == linux
        #include <unistd.h>
        #include "tcp_server.h"


        // : public AbstractSystemInterrupter
        class SocketServerSystemInterrupter: public AbstractSystemInterrupter{
            using kill_signature = std::function<void ()>;
            const static int port = 10100;
            pid_t pid;
            boost::asio::io_service io_service;
            tcp::socket socket;
        public:
            explicit SocketServerSystemInterrupter(kill_signature to_kill):socket(io_service){
                static bool created;
                this->pid = 0;
                if (!created){
                    created = true;
                    this->run();
                }
                while(true){
                    try {
                        socket.connect(tcp::endpoint(boost::asio::ip::address::from_string("127.0.0.1"), port));
                        break;
                    }catch(std::exception & ){
                    }
                }
            }
            static void _exit(int signal){
                exit(0);
            }
            void run(){
                this->pid = fork();
                if (pid < 0){
                    perror("Error while socket creation");
                    exit(1);
                } else if (pid == 0){
                    setsid();
                    SocketServerSystemInterrupter::runsocket();

                } else{
                }
            }

            void runsocket(){
                boost::asio::io_service io_service;
                using namespace std; // For atoi.
                server s(io_service, port);

                io_service.run();
            }

            void handle_error() override{};
            void handle_success() override{
                const std::string msg = "set";
                boost::system::error_code error;
                boost::asio::write( socket, boost::asio::buffer(msg), error );
                if( !error ) {
                }
                else {
                    std::cerr << "send failed: " << error.message() << std::endl;
                }
                boost::asio::streambuf receive_buffer;
                boost::asio::read(socket, receive_buffer, boost::asio::transfer_at_least(8), error);
                std::string line(boost::asio::buffer_cast<const char*>(receive_buffer.data()), receive_buffer.size());
            }
            bool check_next_sync_call() override{
                const std::string msg = "get";
                boost::system::error_code error;
                boost::asio::write( socket, boost::asio::buffer(msg), error );
                boost::asio::streambuf receive_buffer;
                boost::asio::read(socket, receive_buffer, boost::asio::transfer_at_least(8), error);
                std::string line(boost::asio::buffer_cast<const char*>(receive_buffer.data()), receive_buffer.size());
                return line != "finished";
            }
            ~SocketServerSystemInterrupter(){
                socket.close();
                // sleep to let all threads receive data from handled process
                sleep(1
                );
                signal(SIGSEGV, SIG_IGN);
                kill(pid, SIGSEGV);
            }
        };

        #endif
    #endif
#endif //MULTYTHREADING_SOCKETSERVERSYSTEMINTERRUPTER_H

#pragma clang diagnostic pop