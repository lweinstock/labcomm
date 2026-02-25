#ifndef LK_XENAX_XVI_HH
#define LK_XENAX_XVI_HH

#include <labkit/devices/basicdevice.hh>
#include <labkit/comms/tcpipcomm.hh>
#include <labkit/comms/serialcomm.hh>

#include <memory>

namespace labkit {

class XenaxXvi75V8 : public BasicDevice {
public:
    XenaxXvi75V8() : BasicDevice("XENAX Xvi 75v8") {};
    XenaxXvi75V8(std::unique_ptr<TcpipComm> t_tcpip);
    XenaxXvi75V8(std::unique_ptr<SerialComm> t_ser);
    ~XenaxXvi75V8() {};

    // XENAX default port 10001
    static constexpr unsigned PORT = 10001;

    /// Connect to Xenax Xvi 75v8 via TCP/IP
    void connect(std::unique_ptr<TcpipComm> t_tcpip);

    /// Connect to Xenax Xvi 75v8 via USBTMC
    void connect(std::unique_ptr<SerialComm> t_ser);

    /// En-/disable power of motor controller
    void powerOn(bool t_enable = true) { this->queryCmd( t_enable? "PW" : "PQ"); }
    /// Disable power of motor controller
    void powerOff() { powerOn(false); }
    /// Continue power of motor controller
    void powerContinue() { this->queryCmd("PWC"); }
    /// Returns true if the motor is on
    bool isOn() { return std::stoi(this->queryCmd("TS")); }

    /// Referencing for absolute position measurements (see manual p.48)
    void referenceAxis();

    /// Reference direction for axis and gantry referencing 
    enum ReferenceDirection : int {REF_POS = 0, REF_NEG, GANTRY_POS, 
        GANTRY_NEG, GANTRY_POS_NEG, GANTRY_NEG_POS};
    /// Change reference direction (see manual p. 48)
    void setReferenceDir(ReferenceDirection dir);
    /// Returns the current reference direction
    ReferenceDirection getReferenceDir();

    /// Go to absolute position in [inc]
    void movePosition(int t_pos) { this->queryCmd("G" + std::to_string(t_pos)); }
    /// Returns the current position in [inc]
    int getPosition();

    /// Move in positive direction with constant speed
    void jogPos() { this->queryCmd("JP"); }
    /// Move in negative direction with constant speed
    void jogNeg() { this->queryCmd("JN"); }
    /// Stop all motion now
    void stopMotion();

    /// Returns true if a motion command has been completed
    bool motionCompleted();
    /// Returns true if a reference motion has been completed
    bool referenceCompleted();
    /// Returns true if the gantry has been initialised properly
    bool gantryInitialized();
    /// Returns true if there is an error in the error queue
    bool errorPending();
    
    /// Read the Process Status Register (PSR) & update status
    uint32_t getStatusRegister();
    /// Process Status Register definition (manual p. 56)
    enum ProcessStatusRegister : uint32_t {
        ERROR                     = (1 << 0),
        REF                       = (1 << 1),
        IN_MOTION                 = (1 << 2),
        IN_POSITION               = (1 << 3),
        END_OF_PROGRAM            = (1 << 4),
        IN_FORCE                  = (1 << 5),
        IN_SECTO                  = (1 << 6),
        FORCE_IN_SECTOR           = (1 << 7),
        INVERTER_VOLTAGE          = (1 << 8),
        END_OF_GANTRY_INIT        = (1 << 9),
        NEGATIVE_LIMIT_SWITC      = (1 << 10),
        POSITIVE_LIMIT_SWITC      = (1 << 11),
        REMAIN_POWER_ON           = (1 << 12),
        POWER_OFF                 = (1 << 13),
        FORCE_CALIBRATION_ACTIVE  = (1 << 14),
        I_FORCE_LIMIT_REACHED     = (1 << 15),
        STO_PRIMED_HIT            = (1 << 16),
        SS1_PRIMED_HIT            = (1 << 17),
        SS2_PRIMED                = (1 << 18),
        SS2_HIT                   = (1 << 19),
        SLS_PRIMED                = (1 << 20),
        SLS_SPEED_HIT             = (1 << 21),
        SLS_POSITION_HIT          = (1 << 22),
        WARNING                   = (1 << 23),
        INFO                      = (1 << 24),
        PHASING_DONE              = (1 << 25),
        I_FORCE_DRIFT_COMP_ACTIVE = (1 << 26)
    };

    /// Set speed in [inc/s]
    void setSpeed(unsigned t_inc_per_sec);
    /// Returns current speed in [inc/s]
    unsigned getSpeed();
    /// Set acceleration in [inc/s2]
    void setAcceleration(unsigned t_inc_per_sec2);
    /// Return current acceleration in [inc/s2]
    unsigned getAcceleration();
    /// Set S-curve parameter in [%]
    void setSCurve(unsigned t_percent);
    /// Returns current S-curve parameter in [%]
    unsigned getSCurve();

    /// Calibration for more precise force measurements (dF ~ 0.5 - 1.0 N)
    void forceCalibration(unsigned t_distance);
    /// Returns the motor current in [mA]
    int getMotorCurrent();
    /// Returns the motors force constant in [N/mA]
    float getForceConstant();
    /// Returns the motor force in [N]
    float getMotorForce();
    /// Set force limit (manual p. 54)
    void setForceLimit(float t_fmax_n);
    /// Returns the force limit
    float getForceLimit();
    /// Returns true if force limit has been reached
    bool forceLimitReached();

    /// Set lower and upper motion soft limits in [inc]
    void setLimits(unsigned t_lower, unsigned t_upper);
    /// Returns the lower soft limit in [inc]
    unsigned getLowerLimit();
    /// Returns the upper soft limit in [inc]
    unsigned getUpperLimit();

    // Programmable Logic Controller (PLC) GPIO settings (manual p. 51ff)

    /// Output type for the PLC GPIO pins
    enum OutputType : uint8_t {
        SINK = 0b00,
        SOURCE = 0b01,
        SINK_SOURCE = 0b10
    };
    /// Set output type of GPIO pin
    void setOutputType(unsigned t_output_no, OutputType t_type);
    /// Set GPIO pin to active low or active high
    void setOutputActivity(unsigned t_output_no, bool t_active_hi);
    /// Set GPIO pin high or low
    void setOutput(unsigned t_output_no, bool t_high);
    /// Returns GPIO output state (true = high, false = low)
    bool getOutput(unsigned t_output_no);
    /// Returns GPIO input state (true = high, false = low)
    bool getInput(unsigned t_input_no);
    /// Returns the raw output state register for all GPIOs
    uint8_t getOutputStateReg() 
        { return static_cast<uint8_t>(std::stoi(this->queryCmd("TOX"), 0, 16)); }
    /// Returns the raw input state register for all GPIOs
    uint16_t getInputStateReg() 
        { return static_cast<uint8_t>(std::stoi(this->queryCmd("TIX"), 0, 16)); }

    /// Motor type reset (in response to error 59)
    void resetMotorType() { this->queryCmd("RESM", 10000); }
    /// Disable motion blocked by unconfigured Safety Motion Unit (SMU)
    void disableSMU() { this->queryCmd("DMBUS"); }

    /// Set custom servo identifier (max. 16 bytes)
    void setServoId(std::string t_sid);
    /// Clear current servo ID
    void clearServoId() { this->setServoId(""); }
    /// Returns the current servo ID
    std::string getServoId() { return this->queryCmd("SID?"); }

    /// Set card identifier (used for master/slave + gantry, manual p. 94)
    void setCardId(unsigned t_cid);
    /// Returns the card identifier
    unsigned getCardId();
    /// Set gantry slave id in master controller
    void setGantrySlaveId(unsigned t_gsid);
    /// Returns the slave identifier
    unsigned getGantrySlaveId();
    /// Set gantry offset master/slave
    void setGantryMasterSlaveOffs(int t_gmso);
    /// Returns the gantry offser master/slave
    int getGantryMasterSlaveOffs();
    /// Returns the measured offset master/slave after init and reference
    int detectedGantryMasterSlaveOffs();

    // Controller settings (manual p. 46/47)
    /// Set payload in [g]
    void setPayload(unsigned t_payload_g);
    /// Returns payload in [g]
    unsigned getPayload();
    /// Set POS gain parameter
    void setGainPos(unsigned t_gain_pos);
    /// Returns POS gain parameter
    unsigned getGainPos();
    /// Set CUR gain parameter
    void setGainCur(unsigned t_gain_cur);
    /// Returns CUR gain parameter
    unsigned getGainCur();
    /// Set maximum deviation in [inc]
    void setMaxDeviation(unsigned t_max_dev);
    /// Returns maximum deviation in [inc]
    unsigned getMaxDeviation();
    /// Set target deviation in [inc]
    void setTargetDeviation(unsigned t_ar_dev);
    /// Returns target deviation in [inc]
    unsigned getTargetDeviation();

    /// Get error information (C-style)
    unsigned getError(std::string &t_strerror);
    /// Get error information (python-style)
    std::tuple<unsigned,std::string> getError();

private:
    std::string m_input_buffer;
    float m_force_const {0};   // I->F conversion factor [N/mA]
    int m_error {0};
    uint16_t m_output_type {0x5555};
    uint8_t m_output_activity {0xFF};
    bool m_error_pending {false};

    /// GPIO Set Output Type (SOT) definitions (manual p. 26)
    enum SOT : uint16_t {
        SOT10 = (1 << 0),
        SOT11 = (1 << 1),
        SOT20 = (1 << 2),
        SOT21 = (1 << 3),
        SOT30 = (1 << 4),
        SOT31 = (1 << 5),
        SOT40 = (1 << 6),
        SOT41 = (1 << 7),
        SOT50 = (1 << 8),
        SOT51 = (1 << 9),
        SOT60 = (1 << 10),
        SOT61 = (1 << 11),
        SOT70 = (1 << 12),
        SOT71 = (1 << 13),
        SOT80 = (1 << 14),
        SOT81 = (1 << 15)
    };

    /// GPIO Set Output Activity (SOA) definitions (manual p. 26)
    enum SOA : uint8_t {
        SOA1 = (1 << 0),
        SOA2 = (1 << 1),
        SOA3 = (1 << 2),
        SOA4 = (1 << 3),
        SOA5 = (1 << 4),
        SOA6 = (1 << 5),
        SOA7 = (1 << 6),
        SOA8 = (1 << 7)
    };

    void init();
    void flushBuffer();

    /// General command query
    std::string queryCmd(std::string t_cmd, unsigned t_timeout_ms = 1000);

    /// Wait until status bits are set
    void waitStatusSet(uint32_t t_status, unsigned t_interval_ms = 500,
        unsigned t_timeout_ms = 10000);

    /// Wait until status bits are cleared
    void waitStatusClr(uint32_t t_status, unsigned t_interval_ms = 500,
        unsigned t_timeout_ms = 10000);

    /// GPIO register access
    void setOutputTypeReg(uint16_t t_mask);
    void setOutputStateReg(uint8_t t_mask);

    void readErrorQueue();

};

}

#endif
