#ifndef LC_EXCEPTION_HH
#define LC_EXCEPTION_HH

#include <exception>
#include <string>

namespace labkit 
{

/// Basic labkit exception class
class Exception : public std::exception {
public:
    Exception(const std::string& msg, int err = 0):
        std::exception(), m_err_message(msg), m_err_number(err) {};

    virtual ~Exception() noexcept {};

    virtual const char* what() const noexcept override 
        { return m_err_message.c_str(); }

    int errorNumber() const noexcept { return m_err_number; }

private:
    std::string m_err_message;
    int m_err_number;
};

/// Timeout condition
class Timeout : public Exception {
public:
    Timeout(const std::string& msg, int err = 0):
        Exception(msg, err) {};
    virtual ~Timeout() {};
};

/// Connection lost or cannot be established
class BadConnection : public Exception {
public:
    BadConnection(const std::string& msg, int err = 0):
        Exception(msg, err) {};
    virtual ~BadConnection() {};
};

/// Generic IO error
class BadIo : public Exception {
public:
    BadIo(const std::string& msg, int err = 0):
        Exception(msg, err) {};
    virtual ~BadIo() {};
};

/// Communication protocol violation
class BadProtocol : public Exception {
public:
    BadProtocol(const std::string& msg, int err = 0):
        Exception(msg, err) {};
    virtual ~BadProtocol() {};
};

/// Device specific error condition
class DeviceError : public Exception {
public:
    DeviceError(const std::string& msg, int err = 0):
        Exception(msg, err) {};
    virtual ~DeviceError() {};
};

/// Error during type converion
class ConversionError : public Exception {
public:
    ConversionError(const std::string& msg, int err = 0):
        Exception(msg, err) {};
    virtual ~ConversionError() {};
};

}

#endif
