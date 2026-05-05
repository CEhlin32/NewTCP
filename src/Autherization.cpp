#include <Autherization.h>
#include <cstdlib>
#include <time.h>
#include <unistd.h>
#include <vector>
#include <cstring>
#include <DebugSupport.h>
#include <Logger.h>
#include <LCD_Display.h>

using namespace Devices_CE;

namespace CE::tcp
{
    Autherization *Autherization::theInstance = nullptr;

    Autherization *Autherization::Get()
    {
        if (nullptr == Autherization::theInstance)
        {
            Autherization::theInstance = new Autherization();
        }
        return Autherization::theInstance;
    }

    Autherization::Autherization() :    m_Key(), m_IV(), InProgress(false)
    {
        srand((unsigned)time(NULL));

        InitPin();
    }

    Autherization::~Autherization()
    {
    }

    void Autherization::StartAutherizationMode(const std::string& clientPinStr)
    {
        InProgress = true;
        InitPin();
        m_clientPinStr = clientPinStr;
        std::string KeyStr = m_serverPinStr + m_clientPinStr;
        std::string IVStr = m_clientPinStr + m_serverPinStr;

        m_Key.SetBuf(KeyStr);
        m_IV.SetBuf(IVStr);


        // Stop and Display message from anyhere but her

        // Display Pin Number with Prompt
        ILCD_Display *pDisplay = LCD_Display::Get();
        pDisplay->SwitchDisplayScreen(1);

        pDisplay->ClearDisplay(1);
        pDisplay->WriteLineRow(" AUTHENICATION MODE ", 0, 1);

        pDisplay->WriteLineRow("Please  Enter  This ", 1, 1);
        pDisplay->WriteLineRow("Pin On Your Device..", 2, 1);
        pDisplay->WriteLineRow("       ", 3, 1);
        pDisplay->WriteLineRow(m_serverPinStr.c_str(), 3, 1);
        TheAppLogger.LogMsgWithTime(LogAlways::instance(), "Pin string:  %s\n", m_serverPinStr.c_str());
    }

    bool Autherization::IsInAutherization() const
    {
        return InProgress;
    }

    void Autherization::StopAutherizationMode()
    {
        InProgress = false;
        LCD_Display::Get()->SwitchDisplayScreen(0);
    }
    void Autherization::InitPin()
    {
        std::vector<char> pinChars(8);
        for (int n = 0; n < 8; n++)
        {
            int r = rand();
            float rFloat = r;
            int rInt = 10 * rFloat / RAND_MAX;
            unsigned char c = (unsigned char)rInt;
            pinChars[n] = '0' + c;
        }
        m_serverPinStr = std::string(pinChars.data(), pinChars.size());
    }


    CryptoBlockVector Autherization::GetKey()
    {
        return m_Key;
    }

    CryptoBlockVector Autherization::GetIV()
    {
        return m_IV;
    }
}   // namespace CE::tcp
