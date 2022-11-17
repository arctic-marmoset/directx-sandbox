#include "gsspch.hpp"
#include "Clock.hpp"

Clock::Clock()
{
    LARGE_INTEGER freq{ };
    QueryPerformanceFrequency(&freq);
    m_SecondsPerTick = 1.0f / static_cast<float>(freq.QuadPart);
}

void Clock::Start()
{
    if (m_Running)
    {
        return;
    }

    LARGE_INTEGER now{ };
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
    LARGE_INTEGER now{ };
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

    LARGE_INTEGER now{ };
    QueryPerformanceCounter(&now);
    m_CurrentTime = now.QuadPart;
    m_Delta = static_cast<float>(m_CurrentTime - m_PreviousTime) * m_SecondsPerTick;
    m_PreviousTime = m_CurrentTime;

    if (m_Delta < 0.0) m_Delta = 0.0;
}

float Clock::TimeTotal() const
{
    return static_cast<float>(m_CurrentTime - m_StartTime) * m_SecondsPerTick;
}

float Clock::TimeDelta() const
{
    return m_Delta;
}
