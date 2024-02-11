#ifndef TRANSLATOR_WORLD_STOPPING_SCOPE_H
#define TRANSLATOR_WORLD_STOPPING_SCOPE_H

class WorldStoppingScope {
public:
    WorldStoppingScope();
    ~WorldStoppingScope();

    WorldStoppingScope(const WorldStoppingScope &other) = delete;
    WorldStoppingScope &operator =(const WorldStoppingScope &other) = delete;

private:
    bool m_stoppedByThis;
};

#endif

