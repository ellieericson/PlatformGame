#pragma once

class World;
class GameObject;

class Event {
public:
    virtual ~Event() = default;
    virtual void perform(World& world, GameObject& obj) = 0;
};

class NextLevel : public Event {
public:
    void perform(World& world, GameObject& obj) override;
};

class Drown : public Event {
public:
    void perform(World& world, GameObject& obj) override;
};

class Gather : public Event {
public:
    void perform(World& world, GameObject& obj) override;
};