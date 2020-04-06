#ifndef MULTYTHREADING_TCP_SERVER_H
#define MULTYTHREADING_TCP_SERVER_H


#include <boost/bind.hpp>
#include <boost/asio.hpp>

using boost::asio::ip::tcp;


// Here we create asynchrounous session
class session
{
public:
    session(boost::asio::io_service& io_service, int * value)
            : socket_(io_service)
    {
        // value is server's static variable to change state
        this->value = value;
    }

    tcp::socket& socket()
    {
        return socket_;
    }

    void start()
    {
        socket_.async_read_some(boost::asio::buffer(data_, max_length),
                                boost::bind(&session::handle_read, this,
                                            boost::asio::placeholders::error,
                                            boost::asio::placeholders::bytes_transferred));
    }

private:
    bool check_request(std::string strconst){
        for (int i = 0; i < strconst.size(); i++){
            if (this->data_[i] == strconst[i]){

            } else{
                return false;
            }
        }
        return true;
    }
    void handle_read(const boost::system::error_code& error,
                     size_t bytes_transferred)
    {
        if (!error)
        {
            if (check_request(this->_get_const)){
                if (*this->value)
                    boost::asio::async_write(socket_,
                                             boost::asio::buffer(finish, sizeof(finish)),
                                             boost::bind(&session::handle_write, this,
                                                         boost::asio::placeholders::error));
                else{
                    boost::asio::async_write(socket_,
                                             boost::asio::buffer(cont, sizeof(cont)),
                                             boost::bind(&session::handle_write, this,
                                                         boost::asio::placeholders::error));
                }
            }
            if (check_request(this->_set_const)){
                *this->value = 1;
                boost::asio::async_write(socket_,
                                         boost::asio::buffer(ok, sizeof(ok)),
                                         boost::bind(&session::handle_write, this,
                                                     boost::asio::placeholders::error));
            }
            boost::asio::async_write(socket_,
                                     boost::asio::buffer(none, sizeof(none)),
                                     boost::bind(&session::handle_write, this,
                                                 boost::asio::placeholders::error));
        }
        else
        {
            delete this;
        }
    }

    void handle_write(const boost::system::error_code& error)
    {
        if (!error)
        {
            socket_.async_read_some(boost::asio::buffer(data_, max_length),
                                    boost::bind(&session::handle_read, this,
                                                boost::asio::placeholders::error,
                                                boost::asio::placeholders::bytes_transferred));
        }
        else
        {
            delete this;
        }
    }

    tcp::socket socket_;
    enum { max_length = 1024 };
    char data_[max_length];
    int * value;
    std::string _get_const = "get";
    std::string _set_const = "set";
    std::string finish = "finished";  // added paddings to match length
    std::string cont = "continue";
    std::string ok = "ok      ";
    std::string none = "none    ";
};


// Asynchronous socker server
// compatible both with linux and windows
class server
{
public:
    server(boost::asio::io_service& io_service, short port)
            : io_service_(io_service),
              acceptor_(io_service, tcp::endpoint(tcp::v4(), port))
    {
        this->status = 0;
        start_accept();
    }

private:
    void start_accept()
    {
        session* new_session = new session(io_service_, &this->status);
        acceptor_.async_accept(new_session->socket(),
                               boost::bind(&server::handle_accept, this, new_session,
                                           boost::asio::placeholders::error));
    }

    void handle_accept(session* new_session,
                       const boost::system::error_code& error)
    {
        if (!error)
        {
            new_session->start();
        }
        else
        {
            std::cerr<<error.value()<<std::endl;
            delete new_session;
        }

        start_accept();
    }

    boost::asio::io_service& io_service_;
    tcp::acceptor acceptor_;
    int status;
};


#endif //MULTYTHREADING_TCP_SERVER_H
