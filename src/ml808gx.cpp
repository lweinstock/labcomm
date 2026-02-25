#include <sstream>
#include <iomanip>
#include <tuple>

#include <labkit/devices/musashi/ml808gx.hh>
#include <labkit/exceptions.hh>
#include <labkit/debug.hh>

using namespace std;

namespace labkit {

// Static variables
const string Ml808Gx::STX = "\x02";
const string Ml808Gx::ETX = "\x03";
const string Ml808Gx::EOT = "\x04";
const string Ml808Gx::ENQ = "\x05";    
const string Ml808Gx::ACK = "\x06";
const string Ml808Gx::A0 = STX + "02A02D" + ETX;
const string Ml808Gx::A2 = STX + "02A22B" + ETX; 
const string Ml808Gx::CAN = STX + "0218186E" + ETX;

void Ml808Gx::connect(std::unique_ptr<SerialComm> t_ser)
{
    this->setComm(std::move(t_ser));
    this->init(); 
    return;
}

void Ml808Gx::dispense() 
{
    DEBUG_PRINT("%s\n", "Dispensing");
    string cmd = "DI  ";
    this->downloadCommand(cmd);
    return;
}

void Ml808Gx::setChannel(unsigned ch)
{
    if (ch > 399) {
        printf("Invalid channel %i (allowed 0 - 399)\n", ch);
        abort();
    }
    DEBUG_PRINT("Switching channel to %i ...\n", ch);
    string cmd = "CH  ";
    stringstream data("");
    data << setfill('0') << setw(3) << ch;
    this->downloadCommand(cmd, data.str());
    m_cur_ch = ch;
    return;
}

unsigned Ml808Gx::getChannel()
{
    DEBUG_PRINT("%s\n", "Reading current channel number...");
    string resp("");
    this->uploadCommand("UA   ", resp);
    resp = resp.substr(resp.find("D0")+2, 3);
    m_cur_ch = strtoul(resp.c_str(), NULL, 10);
    return m_cur_ch;
}

void Ml808Gx::setChannelParams(float pressure_kPa, float dur_ms, 
    float on_delay_ms, float off_delay_ms) 
{
    // Pressure in units of 100 Pa, duratio in ms, on/off delay in 0.1ms
    unsigned p = static_cast<unsigned>(10*pressure_kPa);
    unsigned dt = static_cast<unsigned>(dur_ms);
    unsigned ton = static_cast<unsigned>(10*on_delay_ms);
    unsigned toff = static_cast<unsigned>(10*off_delay_ms);
    if ( (p < 200) || (p > 8000) ) {
        fprintf(stderr, "Invalid pressure %.2f kPa (valid range: 0.2 - 8.0 kPa)\n", 
            pressure_kPa);
        abort();
    }
    if ( (dt < 10) || (dt > 9999) ) {
        fprintf(stderr, "Invalid duration %.2f ms (valid range: 10 - 9999 ms)\n", 
            dur_ms);
        abort();
    }
    if ( (ton < 0) || (ton > 99999) || (ton < 0) || (toff > 99999) ) {
        fprintf(stderr, "Invalid delay %.2f/%.2f (valid range: 0 - 9.9999 ms)\n", 
            on_delay_ms, off_delay_ms);
        abort();
    }

    DEBUG_PRINT("Setting parameters of ch %i to:\n", m_cur_ch);
    DEBUG_PRINT("p = %i x 100Pa\n", p);
    DEBUG_PRINT("t = %i ms\n", dt);
    DEBUG_PRINT("don/off = %i/%i x 0.1ms\n", ton, toff);
    string cmd = "SC  ";
    stringstream data("");
    data << "CH" << setfill('0') << setw(3) << m_cur_ch;
    data << "P"  << setfill('0') << setw(4) << p;
    data << "T"  << setfill('0') << setw(4) << dt;
    data << "OD" << setfill('0') << setw(5) << ton;
    data << "OF" << setfill('0') << setw(5) << toff;
    this->downloadCommand(cmd, data.str());
    return;
}

void Ml808Gx::getChannelParams(float& pressure_kPa, float& dur_ms, 
    float& on_delay_ms, float& off_delay_ms) 
{
    DEBUG_PRINT("Reading parameters of ch %i...\n", m_cur_ch);
    string resp("");
    stringstream cmd("");
    cmd << "GC" << setfill('0') << setw(3) << m_cur_ch;

    this->uploadCommand(cmd.str(), resp);

    // Extract data from string
    unsigned p    = stoi( resp.substr(resp.find('P')+1, 4) );
    unsigned dt   = stoi( resp.substr(resp.find('T')+1, 4) );
    unsigned ton  = stoi( resp.substr(resp.find("OD")+2, 7) );
    unsigned toff = stoi( resp.substr(resp.find("OF")+2, string::npos) );
    DEBUG_PRINT("p = %i x 100Pa\n", p);
    DEBUG_PRINT("t = %i ms\n", dt);
    DEBUG_PRINT("don/off = %i/%i x 0.1ms\n", ton, toff);

    // Convert into floats
    pressure_kPa = 1e-1*static_cast<float>(p);
    dur_ms       = static_cast<float>(dt);
    on_delay_ms  = 1e-1*static_cast<float>(ton);
    off_delay_ms = 1e-1*static_cast<float>(toff);
    
    return;
}

tuple<float, float, float, float> Ml808Gx::getChannelParams()
{
    float p = .0, dt = .0, on = .0, off = .0;
    this->getChannelParams(p, dt, on, off);
    return make_tuple(p, dt, on, off);
}

void Ml808Gx::setPressure(float pressure_kPa)
{
    unsigned p = static_cast<unsigned>(10*pressure_kPa);
    if ( (p < 200) || (p > 8000) ) {
        fprintf(stderr, "Invalid pressure %.2f kPa (valid range: 0.2 - 8.0 kPa)\n", 
            pressure_kPa);
        abort();
    }

    DEBUG_PRINT("Setting pressure of ch %i to %i x 100 Pa\n", m_cur_ch, p);
    string cmd = "PH  ";
    stringstream data("");
    data << "CH" << setfill('0') << setw(3) << m_cur_ch;
    data << "P"  << setfill('0') << setw(4) << p;
    this->downloadCommand(cmd, data.str());
    return;
}

float Ml808Gx::getPressure()
{
    float p = .0, dt = .0, on = .0, off = .0;
    this->getChannelParams(p, dt, on, off);
    return p;
}

void Ml808Gx::setDuration(float dur_ms)
{
    unsigned dt = static_cast<unsigned>(dur_ms);
    if ( (dt < 9.999) || (dt > 9999) ) {
        fprintf(stderr, "Invalid duration %.2f ms (valid range: 10 - 9999 ms)\n", 
            dur_ms);
        abort();
    }

    DEBUG_PRINT("Setting duration of ch %i to %i ms\n", m_cur_ch, dt);
    string cmd = "DH  ";
    stringstream data("");
    data << "CH" << setfill('0') << setw(3) << m_cur_ch;
    data << "T"  << setfill('0') << setw(4) << dt;
    this->downloadCommand(cmd, data.str());
    return;
}

float Ml808Gx::getDuration()
{   
    float p = .0, dt = .0, on = .0, off = .0;
    this->getChannelParams(p, dt, on, off);
    return dt;
}

void Ml808Gx::setDelays(float on_delay_ms, float off_delay_ms)
{
    unsigned ton = static_cast<unsigned>(10*on_delay_ms);
    unsigned toff = static_cast<unsigned>(10*off_delay_ms);
    if ( (ton < 0) || (ton > 99999) || (ton < 0) || (toff > 99999) ) {
        fprintf(stderr, "Invalid delay %.2f/%.2f (valid range: 0 - 9.9999 ms)\n", 
            on_delay_ms, off_delay_ms);
        abort();
    }


    DEBUG_PRINT("Setting delays of ch %i to %i (on) %i (off) x 0.1 ms\n", 
        m_cur_ch, ton, toff);
    string cmd = "DD  ";
    stringstream data("");
    data << "CH" << setfill('0') << setw(3) << m_cur_ch;
    data << "N"  << setfill('0') << setw(5) << ton;
    data << "F"  << setfill('0') << setw(5) << toff;
    this->downloadCommand(cmd, data.str());
    return;
}

void Ml808Gx::getDelays(float &on_delay_ms, float &off_delay_ms)
{
    float p = 0, dt = 0;
    this->getChannelParams(p, dt, on_delay_ms, off_delay_ms);
}

tuple<float, float> Ml808Gx::getDelays()
{
    float ton = .0, toff = .0;
    this->getDelays(ton, toff);
    return make_tuple(ton, toff);
}


void Ml808Gx::manualMode() 
{
    this->downloadCommand("MT  ");
    return;
}
void Ml808Gx::timedMode() 
{
    this->downloadCommand("TT  ");
    return;
}

void Ml808Gx::enableVacuum(bool ena) 
{
    this->downloadCommand("VO  ", (ena ?  "1" : "0"));
    return;
}

void Ml808Gx::setVacuumInterval(unsigned on_ms, unsigned off_ms) 
{
    if (on_ms > 4000) {
        printf("Vacuum time %i out of range\n", on_ms);
        abort();
    }
    if (off_ms > 4000) {
        printf("Vacuum interval time %i out of range\n", off_ms);
        abort();
    }

    stringstream data("");
    data << "V" << setw(4) << setfill('0') << on_ms;
    data << "I" << setw(4) << setfill('0') << off_ms;
    this->downloadCommand("VI  ", data.str());
    return;
}

/*
 *      P R I V A T E   M E T H O D S
 */

void Ml808Gx::init() 
{   
    // Update current channel
    this->getChannel();
    return;
}

void Ml808Gx::sendCommand(string cmd, string data) 
{
    if (cmd.size() < 4)
        throw BadProtocol("Invalid command", cmd.size());
    
    // Build message: 
    //      stx(1) + nchars(2) + cmd(4) + data(n) + checksum(2) + etx(1)
    unsigned nchars = data.size() + cmd.size();
    stringstream msg("");
    msg << STX;
    msg << setw(2) << setfill('0') << uppercase << hex << nchars;
    msg << cmd;
    msg << data;
    // Calculate and add checksum to message
    uint8_t checksum = 0x00;
    for (size_t i = 1; i < msg.str().size(); i++)
        checksum -= msg.str()[i];
    msg << uppercase << hex << (int)checksum;
    msg << ETX;
    this->getComm()->write(msg.str());  

    return;
}

void Ml808Gx::downloadCommand(string cmd, string data) 
{
    // Initialize enquary
    this->getComm()->write(ENQ);
    string resp = this->getComm()->read();
    if (resp != ACK)
        throw BadProtocol("Did not receive ACK", resp.size());
    
    this->sendCommand(cmd, data);
    resp = this->getComm()->readUntil(ETX);
    
    // Check response
    if ( resp == A2) {
        // Handle error and throw
        this->getComm()->write(CAN);
        throw BadProtocol("Received error A2", -1);
    }

    // End transmission
    this->getComm()->write(EOT);
    return;
}

void Ml808Gx::uploadCommand(string cmd, string& payload) 
{    
    // Initialize enquary
    this->getComm()->write(ENQ);
    string resp = this->getComm()->read();
    if (resp != ACK)
        throw BadProtocol("Did not receive ACK", resp.size());
    
    this->sendCommand(cmd);
    resp = this->getComm()->readUntil(ETX);

    // Check response
    if (resp == A2) {
        // Handle error and throw
        this->getComm()->write(CAN);
        throw BadProtocol("Received error A2", -1);
    }

    this->getComm()->write(ACK);
    payload = this->getComm()->readUntil(ETX);

    // End transmission
    this->getComm()->write(EOT);
    return;
}

}
