#ifndef LK_ETH_SERIAL_COMM_HH
#define LK_ETH_SERIAL_COMM_HH

#include <labkit/comms/serialcomm.hh>
#include <labkit/comms/tcpipcomm.hh>

namespace labkit
{

/** \brief Communication interface for an ethernet to serial converter. 
 *
 *  The Waveshare RS232-485-422 TO POE ETH (B) ethernet to serial converter
 *  provides a serial communication interface via TCP/IP.
 */
class TcpipSerialComm : public SerialComm {
public:
    TcpipSerialComm() {};
    /** \brief Open TCP/IP socket with specified baud rate and frame format.
     *  
     *  \param ip_addr IPv4 address of the converter (e.g. "192.168.2.100")
     *  \param port Port of the TCP/IP socket.
     *  \param baud Baud rate in bits per second.
     *  \param nbits Number of data bits (8/7/6/5) per the frame.
     *  \param par_ena En-/disable parity for the frame.
     *  \param par_even Use even/off parity for the frame.
     *  \param stop_bits Number of stop bits (1/2) per frame.
     */
    TcpipSerialComm(std::string ip_addr, unsigned port, BaudRate t_baud = BAUD_9600, 
        CharSize t_csize = CHAR_8, Parity t_par = PAR_NONE, StopBits t_sbits = STOP_1);
    ~TcpipSerialComm();

    static constexpr unsigned PORT = 4196;

    void open() override;
    /// \copydoc eth_to_ser::eth_to_ser(std::string, unsigned, unsigned, unsigned, bool, bool, unsigned)
    void open(std::string ip_addr, unsigned port, BaudRate t_baud = BAUD_9600, 
        CharSize t_csize = CHAR_8, Parity t_par = PAR_NONE, StopBits t_sbits = STOP_1);
    void close() override;

    int writeRaw(const uint8_t* data, size_t len) override;
    
    int readRaw(uint8_t* data, size_t max_len,
        unsigned timeout_ms = DFLT_TIMEOUT_MS) override;

    // Returns human readable info string
    std::string getInfo() const noexcept override;

    /// Set ip address
    void setIp(std::string ip_addr);
    /// Returns ip address
    std::string getIp() const { return m_ip_addr; }

    /// Set port number
    void setPort(unsigned t_port);
    /// Returns port number
    unsigned getPort() const { return m_port; }

    // Set baud rate for serial interface
    void setBaud(BaudRate t_baud) override;

    // Set number of data bits per packet
    void setCharSize(CharSize t_csize) override;

    // Send 1 or 2 stop bits
    void setStopBits(StopBits t_sbits) override;

    // Enable and set parity
    void setParity(Parity t_par) override;

    // Apply termios settings
    void applySettings() override;

    // Enable and set parity
    void enableRtsCts() override;
    void disableRtsCts() override;
    void enableDtrDsr() override;
    void disableDtrDsr() override;
    void enableXOnXOff(char xon = 0x11, char xoff = 0x13) override;
    void disableXOnXOff() override;

    // Data Terminal Ready (DTR) for manual flow control
    void setDtr() override;
    void clearDtr() override;

    // Request To Send (RTS) for manual flow control
    void setRts() override;
    void clearRts() override;

private:
    TcpipComm m_tcpip_cfg, m_tcpip_ser;
    std::string m_ip_addr {"127.0.0.1"};
    unsigned m_port {0};
    unsigned m_flc {0};
    bool m_update_settings {false};

    static constexpr unsigned HTTP_PORT = 80;

    /// Convert baud rate to bdr parameter for http setup
    static std::string getBdr(BaudRate t_baud);
    /// Convert char size to dtb parameter for http setup
    static unsigned getDtb(CharSize t_csize);
    /// Convert parity to prt parameter for http setup
    static unsigned getPrt(Parity t_par);
    /// Convert stop bits to stb parameter for http setup
    static unsigned getStb(StopBits t_sbits);
};

}

#endif