#include <labcomm/scpi/Scpi.hh>
#include <labcomm/Exceptions.hh>
#include <labcomm/Utils.hh>

#include <unistd.h>

using namespace std;

namespace labcomm {

void Scpi::setEse(uint8_t t_event_status)
{
    string msg = "*ESE " + to_string(t_event_status) + "\n";
    m_comm->write(msg);
    return;
}

uint8_t Scpi::getEse()
{
    string resp = m_comm->query("*ESE?\n");
    uint8_t ese = convertTo<uint8_t>(resp);
    return ese;
}

uint8_t Scpi::getEsr()
{
    string resp = m_comm->query("*ESR?\n");
    uint8_t esr = convertTo<uint8_t>(resp);
    return esr;
}

bool Scpi::getOpc()
{
    string resp = m_comm->query("*OPC?\n");
    bool opc {false};
    opc = convertTo<uint8_t>(resp);
    return opc;
}

void Scpi::waitForOpc(unsigned t_interval_ms)
{
    bool opc {false};
    while (!opc) {
        opc = this->getOpc();
        usleep(t_interval_ms * 1e3);  // To avoid excessive polling
    }
    return;
}

void Scpi::setSre(uint8_t service_request)
{
    string msg = "*SRE " + to_string(service_request) + "\n";
    m_comm->write(msg);
    return;
}

uint8_t Scpi::getSre()
{
    string resp = m_comm->query("*OPC?\n");
    uint8_t sre = convertTo<uint8_t>(resp);
    return sre;
}

uint8_t Scpi::getStb()
{
    string resp = m_comm->query("*OPC?\n");
    uint8_t stb = convertTo<uint8_t>(resp);
    return stb;
}

bool Scpi::tst()
{
    m_comm->write("*TST?\n");
    string resp {};
    while (resp.empty()) {  // Wait until self test is done
        resp = m_comm->read();
    }
    bool tst {false};
    tst = convertTo<uint8_t>(resp);
    return !tst;    // 0 = success, everything else means failed
}

}