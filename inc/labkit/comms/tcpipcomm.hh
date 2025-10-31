#ifndef LK_TCPIP_COMM_HH
#define LK_TCPIP_COMM_HH

#include <labkit/comms/basiccomm.hh>

#include <sys/socket.h>
#include <arpa/inet.h>

namespace labkit 
{

/** \brief Communication interface based on UNIX TCP sockets
 *
 *  This class is a C++ wrapper for the C UNIX socket api (sys/socket.h).
 */
class TcpipComm : public BasicComm {
public:
    /// Default constructor
    TcpipComm() : BasicComm() {};
    
    /** \brief Open socket at specified ip address and port.
     *  
     *  \param t_ip_addr IPv4 address (e.g. "192.168.2.200").
     *  \param t_port Port of socket to connect to.
     */
    TcpipComm(std::string t_ip_addr, unsigned t_port);

    /// Destructor
    virtual ~TcpipComm();

    /// Open TCP/IP communication with stored settings
    void open() override { this->open(m_ip_addr, m_port); }
    /// Open TCP/IP communication with provided settings
    void open(std::string t_ip_addr, unsigned t_port);
    /// Close TCP/IP communication
    void close() override;

    int writeRaw(const uint8_t* t_data, size_t t_len) override;
    int readRaw(uint8_t* t_data, size_t t_max_len, 
        unsigned t_timeout_ms = DFLT_TIMEOUT_MS) override;

    /// Set ip address
    void setIp(std::string t_ip_addr) noexcept { m_ip_addr = t_ip_addr; }
    /// Returns ip address
    std::string getIp() const { return m_ip_addr; }

    /// Set port number
    void setPort(unsigned t_port) noexcept { m_port =  t_port; }
    /// Returns port number
    unsigned getPort() const { return m_port; }

    /// Set read/write buffer size
    void setBufferSize(size_t t_buf_size);

    /// Set read/write timeout in milliseconds
    void setTimeout(unsigned t_timeout_ms);

    // Returns interface type
    CommType type() const noexcept override { return TCPIP; }

    // Returns human readable info string
    std::string getInfo() const noexcept override;

private:
    int m_socket_fd {-1};
    struct sockaddr_in m_instr_addr;
    struct timeval m_timeout;
    std::string m_ip_addr {"127.0.0.1"};
    unsigned m_port {0};

    void checkAndThrow(int stat, const std::string& msg) const;
};

}

#endif
