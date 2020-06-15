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
    bool m_Running;

    long long int m_StartTime;
    long long int m_CurrentTime;
    long long int m_PreviousTime;

    float m_SecondsPerTick;
    float m_Delta;
};
