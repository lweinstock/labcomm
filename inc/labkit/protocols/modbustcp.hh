#ifndef LK_MODBUS_TCP_HH
#define LK_MODBUS_TCP_HH

#include <labkit/protocols/modbus.hh>

namespace labkit
{

/** \brief Implementation of MODBUS TCP
 *
 */
class ModbusTcp : public Modbus
{
public:
    ModbusTcp() : Modbus() {};
    ModbusTcp(std::shared_ptr<BasicComm> t_comm) : Modbus(t_comm) {};
    ~ModbusTcp() {};

    /// Function Code 01; read coils -> returns true = on, false = off
    std::vector<bool> readCoils(uint8_t t_unit_id, uint16_t t_addr, 
        uint16_t t_len) override;

    /// Function Code 02; read discrete inputs
    std::vector<bool> readDiscreteInputs(uint8_t t_unit_id, uint16_t t_addr, 
        uint16_t t_len) override;

    /// Function Code 03; read multiple holding registers
    std::vector<uint16_t> readMultipleHoldingRegs(uint8_t t_unit_id, 
        uint16_t t_addr, uint16_t t_len) override;

    /// Function Code 04; read input registers
    std::vector<uint16_t> readInputRegs(uint8_t t_unit_id, uint16_t t_addr, 
        uint16_t t_len) override;

    /// Function Code 05; write single coil -> on = true, off = false
    void writeSingleCoil(uint8_t t_unit_id, uint16_t t_addr, bool t_ena) override;

    /// Function Code 06; write single holding register
    void writeSingleHoldingReg(uint8_t t_unit_id, uint16_t t_addr, 
        uint16_t t_reg) override;

    /// Function Code 15; write multiple coils -> on = true, off = false
    void writeMultipleCoils(uint8_t t_unit_id, uint16_t t_addr, 
        std::vector<bool> t_ena) override;

    /// Function Code 16; write multiple holding registers
    void writeMultipleHoldingRegs(uint8_t t_unit_id, uint16_t t_addr, 
        std::vector<uint16_t> t_regs) override;

private:
    /// Transaction ID used by MODBUS TCP
    uint16_t m_tid {0x0000};

    /// Returns MODBUS packet
    std::vector<uint8_t> createPacket(uint8_t t_unit_id, 
        uint8_t t_function_code, std::vector<uint8_t> &t_data);

    /// Read 16 bit registers; used by FC03 & FC04
    std::vector<uint16_t> read16BitRegs(uint8_t t_unit_id, 
        uint8_t t_function_code, uint16_t t_start_addr, uint16_t t_len);

};

}

#endif