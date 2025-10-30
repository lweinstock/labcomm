#ifndef LC_SERIAL_COMM_HH
#define LC_SERIAL_COMM_HH

#include <labkit/comms/basiccomm.hh>
#include <termios.h>

namespace labkit
{

/** \brief Communication interface based on the UNIX serial port
 *
 *  This class is a C++ wrapper for the C UNIX serial port (termios).
 *  It can be used for all serial devices (RS232, RS422, RS485, UART, 
 *  USB-UART bridged, etc.) that create a tty device file.
 */
class SerialComm : public BasicComm {
public:
    /// Default constructor
    SerialComm() : BasicComm() {};
    /// Default destructor
    ~SerialComm();

    enum BaudRate : int 
    { 
        BAUD_0 = B0,
        BAUD_50 = B50,
        BAUD_75 = B75,
        BAUD_110 = B110,
        BAUD_134 = B134,
        BAUD_150 = B150,
        BAUD_200 = B200,
        BAUD_300 = B300,
        BAUD_600 = B600,
        BAUD_1200 = B1200,
        BAUD_1800 = B1800,
        BAUD_2400 = B2400,
        BAUD_4800 = B4800,
        BAUD_9600 = B9600,
        BAUD_19200 = B19200,
        BAUD_38400 = B38400,
        BAUD_57600 = B57600,
        BAUD_115200 = B115200,
        BAUD_230400 = B230400
    };

    enum CharSize : int 
    {
        CHAR_5 = CS5, 
        CHAR_6 = CS6, 
        CHAR_7 = CS7, 
        CHAR_8 = CS8
    };

    enum Parity : int 
    {
        PAR_NONE = 0, 
        PAR_EVEN, 
        PAR_ODD
    };

    enum StopBits : int
    {
        STOP_1 = 1, 
        STOP_2
    };

    /** \brief Open device file with specified baud rate and frame format.
     *  
     *  \param t_path Path to device file (e.g. "/dev/ttyUSB0").
     *  \param t_baud Baud rate in bits per second.
     *  \param t_csize Number of data bits (8/7/6/5) per the frame.
     *  \param t_par Parity for the frame (none/even/odd).
     *  \param t_sbits Number of stop bits (1/2) per frame.
     */
    SerialComm(std::string t_path, BaudRate t_baud = BAUD_9600, CharSize t_csize = CHAR_8,
        Parity t_par = PAR_NONE, StopBits t_sbits = STOP_1);

    /** \brief C-style raw byte write.
     *  \param [in] t_data Output byte array.
     *  \param [in] t_len Length of byte array.
     *  \return Number of successfully written bytes.
     */
    virtual int writeRaw(const uint8_t* t_data, size_t t_len) override;

    /** \brief C-style raw byte read.
     *  \param [out] t_data Input byte array.
     *  \param [in] t_max_len Maximum length of byte array.
     *  \return Number of successfully read bytes.
     */
    virtual int readRaw(uint8_t* t_data, size_t t_max_len,
        unsigned t_timeout_ms = DFLT_TIMEOUT_MS) override;

    // Returns human readable info string
    std::string getInfo() const noexcept override;

    /// Open serial communication with stored settings
    virtual void open() override;
    /// Open serial communication with provided settings
    void open(std::string t_path, BaudRate t_baud = BAUD_9600, CharSize t_csize = CHAR_8,
        Parity t_par = PAR_NONE, StopBits t_sbits = STOP_1);
    /// Close serial communication
    virtual void close() override;

    /// Set baud rate for serial interface.
    virtual void setBaud(BaudRate t_baud);
    /// Get baud rate for serial interface.
    virtual BaudRate getBau() const { return m_baud; }

    /// Set number of data bits per packet
    virtual void setCharSize(CharSize t_csize);
    /// Get number of data bits per packet
    virtual CharSize getCharSize() const { return m_csize; }

    /// Send 1 or 2 stop bits
    virtual void setStopBits(StopBits t_sbits = STOP_1);
    /// Returns the number of stop bits
    virtual StopBits getStopBits() const { return m_sbits; }

    /// Set parity to none, even, or odd
    virtual void setParity(Parity m_par);
    /// Returns true if parity is used.
    virtual bool getParity() const { return m_par; }

    /// Apply changed settings.
    virtual void applySettings();

    /// Enable hardware flow control; use Request To Send (RTS) and 
    /// Clear To Send (CTS) signals for flow control.
    virtual void enableRtsCts();
    /// Disable Request To Send (RTS) and Clear To Send (CTS) flow control
    virtual void disableRtsCts();

    /// Enable hardware flow control; use Data Terminal Ready (DTR) and 
    /// Data Set Ready (DSR) signals for flow control.
    virtual void enableDtrDsr();
    /// Disable Data Terminal Ready (DTR) and Data Set Ready (DSR) flow control
    virtual void disableDtrDsr();

    /// Enable XON/XOFF software flow control
    virtual void enableXOnXOff(char t_xon = 0x11, char t_xoff = 0x13);
    /// Disable XON/XOFF software flow control
    virtual void disableXOnXOff();

    /// Set Data Terminal Ready (DTR) for manual flow control.
    virtual void setDtr();
    /// Clear Data Terminal Ready (DTR) for manual flow control.
    virtual void clearDtr();

    /// Set Request To Send (RTS) for manual flow control
    virtual void setRts();
    /// Clear Request To Send (RTS) for manual flow control
    virtual void clearRts();

    CommType type() const noexcept override { return SERIAL; }

protected:
    int m_fd {-1};
    BaudRate m_baud {BAUD_9600};
    CharSize m_csize {CHAR_8};
    StopBits m_sbits {STOP_1};
    Parity m_par {PAR_NONE};
    bool m_update_settings {true};

    static int cSizeToInt(CharSize t_csize);
    static std::string parToStr(Parity t_par);
    static char parToChar(Parity t_par);

private:
    std::string m_path {"/dev/tty0"};
    struct termios m_term_settings {};
    struct timeval m_timeout {};

    /// Check return value and throw corresponding exception
    void checkAndThrow(int t_status, const std::string &t_msg) const;
};

}

#endif