#ifndef LC_USB_COMM_HH
#define LC_USB_COMM_HH

#include <labkit/comms/basiccomm.hh>
#include <libusb.h>

namespace labkit 
{

/**
 * @brief USB communication interface
 *
 *  The usb_comm is based on libusb-1.0 enabling driverless USB communication.
 *
 *  For setting up the USB communication to the device the devices vendor ID,
 *  product ID, interface number, endpoint addresses and types, and the max
 *  package size have to be provided using claim_interface(...), 
 *  config_endpoint_in(...), and config_endpoint_out(...). This information
 *  can be gathered using tools like 'lsusb' (usbutils).
 * 
 *  For more information on the USB protocol please refer to the wonderful
 *  article 'USB in a NutShell':
 *
 *  https://www.beyondlogic.org/usbnutshell/usb1.shtml
 *
 */
class UsbComm : public BasicComm {
public:
    /// Default constructor
    UsbComm() : BasicComm() {};
    
    /**
     * @brief Open USB communication to device with given VID, PID, and serial
     * 
     * @param t_vid Vendor ID
     * @param t_pid Product ID
     * @param t_serno Serial number
     */
    UsbComm(uint16_t t_vid, uint16_t t_pid, std::string t_serno = "");

    /// Destructor
    virtual ~UsbComm();

    /** \brief C-style raw byte write.
     *  \param [in] t_data Output byte array.
     *  \param [in] t_len Length of byte array.
     *  \return Number of successfully written bytes.
     */
    int writeRaw(const uint8_t* t_data, size_t t_len) override;

    /** \brief C-style raw byte read.
     *  \param [out] t_data Input byte array.
     *  \param [in] t_max_len Maximum length of byte array.
     *  \return Number of successfully read bytes.
     */
    int readRaw(uint8_t* t_data, size_t t_max_len, 
        unsigned t_timeout_ms = DFLT_TIMEOUT_MS) override;

    /// Open USB communication with stored settings
    void open() override;
    /// Open USB communication with provided settings
    void open(uint16_t t_vid, uint16_t t_pid, std::string t_serno = "");
    /// Close USB communication
    void close() override;

    /// Returns human readable string with information.
    std::string getInfo() const noexcept override;

    /// Returns USB communication type
    CommType type() const noexcept override { return USB; }

    /**
     * @brief USB control transfer (i.e. setup package)
     *
     *  For detailed description of all fields please refer to "USB in a nutshell":
     *  https://www.beyondlogic.org/usbnutshell/usb6.shtml#SetupPacket
     * 
     * @param t_request_type Sets direction, type, and recipient of transfer
     * @param t_request Sets the actual request beeing made (standard, vendor, ...)
     * @param t_value Parameter of request
     * @param t_index Parameter of request
     * @param t_data Bytes to be transferred in data phase (read or write)
     * @param t_len No of bytes to be transferred
     * @return int 
     */
    int controlTransfer(uint8_t t_request_type, uint8_t t_request, uint16_t t_value, 
        uint16_t t_index, const uint8_t* t_data, int t_len, int t_timeout_ms = DFLT_TIMEOUT_MS);

    /// Send data to bulk endpoint
    int writeBulk(const uint8_t* t_data, int t_len);
    /// Read data from bulk endpoint
    int readBulk(uint8_t* t_data, int t_max_len, 
        int t_timeout_ms = DFLT_TIMEOUT_MS);

    /// Send data to interrupt endpoint
    int writeInterrupt(const uint8_t* t_data, int t_len);
    /// Read data from interrupt endpoint
    int readInterrupt(uint8_t* t_data, int t_max_len, 
        int t_timeout_ms = DFLT_TIMEOUT_MS);

    /**
     * @brief Claim usb interface
     * 
     * @param t_iface Interface number
     * @param t_alt Alternative settings number (typ. not used)
     */
    void configInterface(int t_iface = 0, int t_alt = 0);

    /// Enum for endpoint comminucation type; control, bulk or interrupt transfer
    enum EndpointType {CONTROL, BULK, INTERRUPT};

    /**
     * @brief Configure endpoint in (device to host)
     * 
     * @param t_ep_addr Endpoint address
     * @param t_max_size Maximum packet size
     */
    void configEndpointIn(uint8_t t_ep_addr, EndpointType t_type = BULK, 
        size_t t_max_size = 64);

    /**
     * @brief Configure endpoint in (host to device)
     * 
     * @param t_ep_addr Endpoint address
     * @param t_max_size Maximum packet size
     */
    void configEndpointOut(uint8_t t_p_addr, EndpointType t_type = BULK, 
        size_t t_max_size = 64);

    // Clear endpoint buffers
    void clear();

    /// Set vendor ID
    void setVid(uint16_t t_vid) { m_vid = t_vid; }
    /// Returns vendor ID
    uint16_t getVid() const { return m_vid; }
    /// Set product ID
    void setPid(uint16_t t_pid) { m_pid = t_pid; }
    /// Returns product ID
    uint16_t getPid() const { return m_pid; }
    /// Set serial number
    void setSerial(std::string t_serno) { m_serno = t_serno; }
    /// Returns serial number
    std::string getSerial() const { return m_serno; }

protected:
    static libusb_context* s_default_ctx;
    static int s_dev_count;
    libusb_device* m_usb_dev {NULL};
    libusb_device_handle* m_usb_handle {NULL};

    // Current device I/O information
    int m_cur_iface {-1};
    EndpointType m_ep_in_type {BULK}, m_ep_out_type {BULK};
    uint8_t m_ep_in_addr {0x80}, m_ep_out_addr {0x00};
    size_t m_max_pkt_size_in {64}, m_max_pkt_size_out {64};

    // Device info
    uint16_t m_vid {0x0000}, m_pid {0x0000};
    std::string m_serno {""};

    void check_and_throw(int status, const std::string& msg) const;
};

}

#endif
