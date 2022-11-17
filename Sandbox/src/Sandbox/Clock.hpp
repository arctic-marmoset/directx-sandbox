#pragma once

class Clock
{
public:
    Clock();

    void Start();
    void Stop();
    void Set();
    void Tick();

    float TimeTotal() const;
    float TimeDelta() const;

private:
    using TickCount = decltype(LARGE_INTEGER().QuadPart);

    bool m_Running{ true };

    TickCount m_StartTime{ };
    TickCount m_CurrentTime{ };
    TickCount m_PreviousTime{ };

    float m_SecondsPerTick{ };
    float m_Delta{ };
};
