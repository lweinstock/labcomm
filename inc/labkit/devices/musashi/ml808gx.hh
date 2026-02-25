#ifndef LK_ML808GX_HH
#define LK_ML808GX_HH

#include <labkit/devices/basicdevice.hh>
#include <labkit/comms/serialcomm.hh>

#include <tuple>

namespace labkit {

class Ml808Gx : public BasicDevice {
public:
    Ml808Gx() : BasicDevice("ML-808GX") {};
    Ml808Gx(std::unique_ptr<SerialComm> t_ser) 
        { this->connect(std::move(t_ser)); }
    ~Ml808Gx() {};

    /// Connect to ML-808GX via serial port
    void connect(std::unique_ptr<SerialComm> t_ser);

    /// Dispense glue using parameters from current channel
    void dispense();

    // Select dispensing parameters from channel/recipe
    void setChannel(unsigned ch);
    unsigned getChannel();

    // Set parameters for current channel recipe (see manual p. 61):
    void setChannelParams(float pressure_kPa, float dur_ms, float on_delay_ms, 
        float off_delay_ms);
    void getChannelParams(float& pressure_kPa, float& dur_ms, 
        float& on_delay_ms, float& off_delay_ms);
    // Python-style tuple return (pressure, duration, on/off delay)
    std::tuple<float, float, float, float> getChannelParams();

    // Setter and getter methods for single parameters of current channel
    void setPressure(float pressure_kPa);
    float getPressure();
    void setDuration(float dur_ms);
    float getDuration();
    void setDelays(float on_delay_ms, float off_delay_ms);
    void getDelays(float &on_delay_ms, float &off_delay_ms);
    std::tuple<float, float> getDelays();

    // Manual and timed despense modes (see manual p. 16)
    void manualMode();
    void timedMode();

    // Vacuum interval feature (see manual p. 35)
    void enableVacuum(bool ena = true);
    void disableVacuum() { this->enableVacuum(false); }
    void setVacuumInterval(unsigned on_ms, unsigned off_ms);


private:
    // Protocol definitions (see manual p. 57)
    const static std::string STX;   /// Start of text (ASCII)
    const static std::string ETX;   /// End of text (ASCII)
    const static std::string EOT;   /// End of transmission (ASCII)
    const static std::string ENQ;   /// Start enquiry (ASCII)
    const static std::string ACK;   /// Acknowledge (ASCII)
    const static std::string A0;    /// Success
    const static std::string A2;    /// Error
    const static std::string CAN;   /// No idea what this is

    void init();
    unsigned m_cur_ch {0};
    
    /// Send formatted command (see manual p. 55)
    void sendCommand(std::string cmd, std::string data = "");

    /// Send data host -> device (see manual p. 56, 58ff)
    void downloadCommand(std::string cmd, std::string data = "");

    /// Query data device -> host (see manual p. 57, 67ff)
    void uploadCommand(std::string cmd, std::string& payload);


};

}

#endif