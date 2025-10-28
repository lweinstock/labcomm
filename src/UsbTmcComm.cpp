#include <labcomm/UsbTmcComm.hh>
#include <labcomm/Exceptions.hh>
#include <labcomm/Debug.hh>

using namespace std;

namespace labcomm {

int UsbTmcComm::writeRaw(const uint8_t* t_data, size_t t_len) 
{
    return this->writeDevDepMsg(t_data, t_len);
}

int UsbTmcComm::readRaw(uint8_t* t_data, size_t t_max_len, unsigned t_timeout_ms) 
{
    return this->readDevDepMsg(t_data, t_max_len, t_timeout_ms);
}

int UsbTmcComm::writeDevDepMsg(const uint8_t* t_msg, size_t t_len,
    uint8_t transfer_attr) 
{
    // add space for header + total length must be multiple of 4
    size_t tot_len = HEADER_LEN + t_len;
    if (tot_len%4 > 0)
        tot_len += 4 - tot_len%4;
    uint8_t* usbtmc_message = new uint8_t[tot_len];
    this->createUsbTmcHeader(usbtmc_message, DEV_DEP_MSG_OUT,
        transfer_attr, t_len);

    // Append data
    for (size_t i = HEADER_LEN; i < tot_len; i++) {
        usbtmc_message[i] = t_msg[i-HEADER_LEN];
        if (i > t_len+HEADER_LEN)
            usbtmc_message[i] = 0x00;   // zero padding
    }
    DEBUG_PRINT("%s\n", "Sending device dependent message");
    int nbytes = this->writeBulk((const uint8_t*)usbtmc_message, tot_len);
    DEBUG_PRINT_BYTE_DATA(usbtmc_message, nbytes, "Written %zu bytes: ", nbytes);

    // cleanup
    delete[] usbtmc_message;

    return nbytes;
}

int UsbTmcComm::readDevDepMsg(uint8_t* t_data, size_t t_max_len,
    int t_timeout_ms, uint8_t t_transfer_attr, uint8_t t_term_char) 
{
    uint8_t read_request[HEADER_LEN];
    uint8_t rbuf[DFLT_BUF_SIZE] = { 0x00 };

    // Send read request
    DEBUG_PRINT("%s\n", "Sending read request");
    this->createUsbTmcHeader(read_request, REQUEST_DEV_DEP_MSG_IN,
        t_transfer_attr, sizeof(rbuf), t_term_char);
    this->writeBulk((const uint8_t*)read_request, HEADER_LEN);

    // Read from bulk endpoint
    DEBUG_PRINT("%s\n", "Reading device dependent message");
    int len = this->readBulk(rbuf, sizeof(rbuf), t_timeout_ms);

    // If an empty message was received, return immediatly
    if (len == 0)
        return len;

    // Check header
    int transfer_size = checkUsbUmcHeader(rbuf, DEV_DEP_MSG_IN);

    // Copy data into output array
    std::copy(rbuf + HEADER_LEN, rbuf + len, t_data);
    int bytes_received = len - HEADER_LEN;
    while (bytes_received < transfer_size) {
        int nbytes = this->readBulk(rbuf, sizeof(rbuf), t_timeout_ms);
        if (bytes_received > static_cast<int>(t_max_len))
            throw BadIo(this->getInfo() + " - Buffer size too small");
        std::copy(rbuf, rbuf + nbytes, t_data + bytes_received);
        bytes_received += nbytes;
    }
    DEBUG_PRINT_BYTE_DATA(t_data, bytes_received, "Read %zu bytes: ", bytes_received);

    // Increase bTag for next communication
    m_cur_tag++;
    
    return bytes_received;
}

int UsbTmcComm::writeVendorSpecific(string t_msg) 
{
    // add space for header + total length must be multiple of 4
    size_t tot_len = HEADER_LEN + t_msg.size() + 4 - t_msg.size()%4;
    uint8_t* usbtmc_message = new uint8_t[tot_len];
    this->createUsbTmcHeader(usbtmc_message, VENDOR_SPECIFIC_OUT, 0x00,
        t_msg.size());

    // Append data
    for (size_t i = HEADER_LEN; i < tot_len; i++) 
    {
        usbtmc_message[i] = t_msg.c_str()[i-HEADER_LEN];
        if (i > t_msg.size()+HEADER_LEN)
            usbtmc_message[i] = 0x00;   // zero padding
    }

    int nbytes = this->writeBulk((const uint8_t*)usbtmc_message, tot_len);
    // cleanup
    delete[] usbtmc_message;

    return nbytes;
}

string UsbTmcComm::readVendorSpecific(int t_timeout_ms) 
{
    uint8_t read_request[HEADER_LEN], rbuf[DFLT_BUF_SIZE];
    // Send read request
    DEBUG_PRINT("%s\n", "Sending vendor specific read request\n");
    this->createUsbTmcHeader(read_request, REQUEST_VENDOR_SPECIFIC_IN,
        0x00, sizeof(rbuf), 0x00);
    this->writeBulk((const uint8_t*)read_request, HEADER_LEN);

    // Read from bulk endpoint
    DEBUG_PRINT("%s\n", "Reading...\n");
    int len = this->readBulk(rbuf, sizeof(rbuf), t_timeout_ms);

    // If an empty message was received, return immediatly
    if (len == 0)
        return "";

    // Check header
    int transfer_size = checkUsbUmcHeader(rbuf, DEV_DEP_MSG_IN);
    // Remove header from return value
    len -= HEADER_LEN;
    string ret((const char*)rbuf + HEADER_LEN, len);

    // If more data than received was anounced in the header, keep reading
    int bytes_left = transfer_size - len;
    while (bytes_left > 0) {
        int nbytes = this->readBulk(rbuf, sizeof(rbuf), t_timeout_ms);
        ret.append((char*)rbuf, min(bytes_left, nbytes));
        bytes_left -= nbytes;
    }
    // Increase bTag for next communication
    m_cur_tag++;
    DEBUG_PRINT("Received vendor specific message (%lu) '%s'\n",
        ret.size(), ret.c_str());

    return ret;
}

/*
 *      P R I V A T E   M E T H O D S
 */

void UsbTmcComm::createUsbTmcHeader(uint8_t* t_header, uint8_t t_message_id, 
    uint8_t t_transfer_attr, uint32_t t_transfer_size, uint8_t t_term_char) 
{
    // Create USBTMC t_header
    t_header[0] = t_message_id;
    t_header[1] = m_cur_tag;
    t_header[2] = ~m_cur_tag;
    t_header[3] = 0x00;
    t_header[4] = 0xFF & t_transfer_size;
    t_header[5] = 0xFF & (t_transfer_size >> 8);
    t_header[6] = 0xFF & (t_transfer_size >> 16);
    t_header[7] = 0xFF & (t_transfer_size >> 24);
    switch (t_message_id) {
        case DEV_DEP_MSG_OUT:
        t_header[8] = 0x01 & t_transfer_attr;
        t_header[9] = 0x00;
        t_header[10] = 0x00;
        t_header[11] = 0x00;
        break;

        case REQUEST_DEV_DEP_MSG_IN:
        t_header[8] = 0x02 & t_transfer_attr;
        t_header[9] = t_term_char;
        t_header[10] = 0x00;
        t_header[11] = 0x00;
        if (t_transfer_attr & TERM_CHAR)
            m_term_char = t_term_char;
        break;

        case VENDOR_SPECIFIC_OUT:
        case REQUEST_VENDOR_SPECIFIC_IN:
        t_header[8] = 0x00;
        t_header[9] = 0x00;
        t_header[10] = 0x00;
        t_header[11] = 0x00;
        break;
    }

    return;
}

int UsbTmcComm::checkUsbUmcHeader(uint8_t* t_message, uint8_t t_message_id) 
{
    // Check MsgID field
    if ( t_message_id != t_message[0] ) {
        DEBUG_PRINT("Wrong MsgID returned : expected 0x%02X, received 0x%02X\n",
            t_message_id, t_message[0]);
        throw BadProtocol(this->getInfo() + " - Wrong MsgID received");
    }

    // Check bTag and ~bTag fields
    uint8_t inv_cur_tag = (uint8_t)(~m_cur_tag);
    if ( (t_message[1] != m_cur_tag) || (t_message[2] != inv_cur_tag) ) {
        DEBUG_PRINT("Wrong bTag/~bTag returned : expected 0x%02X/0x%02X, "
            "received 0x%02X/0x%02X\n", m_cur_tag, inv_cur_tag, t_message[1], 
            t_message[2]);
        throw BadProtocol(this->getInfo() + " - Wrong bTag/~bTag received");
    }

    // Check transfer size
    uint32_t transfer_size = ((uint32_t)t_message[4] <<  0) |
                             ((uint32_t)t_message[5] <<  8) |
                             ((uint32_t)t_message[6] << 16) |
                             ((uint32_t)t_message[7] << 24);

    DEBUG_PRINT("MsgID 0x%02X, bTag 0x%02X/0x%02X, TransferSize 0x%08X (%u)\n",
        t_message_id, m_cur_tag, inv_cur_tag, transfer_size, transfer_size);
    return transfer_size;
}

}
