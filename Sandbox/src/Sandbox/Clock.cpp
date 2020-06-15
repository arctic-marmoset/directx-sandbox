#include "gspch.h"
#include "Clock.h"

Clock::Clock()
    :
    m_Running(true),
    m_StartTime(0),
    m_CurrentTime(0),
    m_PreviousTime(0),
    m_SecondsPerTick(0.0),
    m_Delta(0.0)
{
    LARGE_INTEGER freq = { };
    QueryPerformanceFrequency(&freq);
    m_SecondsPerTick = 1.0 / freq.QuadPart;
}

void Clock::Start()
{
    if (m_Running)
    {
        return;
    }

    LARGE_INTEGER now = { };
    QueryPerformanceCounter(&now);
    m_PreviousTime = now.QuadPart;
    m_Running = true;
}

void Clock::Stop()
{
    if (!m_Running)
    {
        return;
    }

    m_Running = false;
}

void Clock::Set()
{
    LARGE_INTEGER now = { };
    QueryPerformanceCounter(&now);
    m_StartTime = now.QuadPart;
    m_PreviousTime = now.QuadPart;
    m_Running = true;
}

void Clock::Tick()
{
    if (!m_Running)
    {
        m_Delta = 0.0;
        return;
    }

    LARGE_INTEGER now = { };
    QueryPerformanceCounter(&now);
    m_CurrentTime = now.QuadPart;
    m_Delta = (m_CurrentTime - m_PreviousTime) * m_SecondsPerTick;
    m_PreviousTime = m_CurrentTime;

    if (m_Delta < 0.0) m_Delta = 0.0;
}

double Clock::TimeTotal() const
{
    return (m_CurrentTime - m_StartTime) * m_SecondsPerTick;
}

double Clock::TimeDelta() const
{
    return m_Delta;
}
