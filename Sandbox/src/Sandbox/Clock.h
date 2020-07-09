#pragma once

class Clock
{
public:
    Clock();
    Clock(const Clock &) = delete;
    ~Clock() = default;

    void Start();
    void Stop();
    void Set();
    void Tick();

    float TimeTotal() const;
    float TimeDelta() const;

    Clock &operator=(const Clock &) = delete;

private:
    using TickCount = decltype(LARGE_INTEGER().QuadPart);

    bool m_Running;

    TickCount m_StartTime;
    TickCount m_CurrentTime;
    TickCount m_PreviousTime;

    float m_SecondsPerTick;
    float m_Delta;
};
