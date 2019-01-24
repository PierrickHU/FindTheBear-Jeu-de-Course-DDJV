#ifndef I_SERIALIZABLE_H
#define I_SERIALIZABLE_H

struct ISerializable {
    virtual void Unserialize() = 0;
    virtual void Serialize() = 0;
};

#endif // I_SERIALIZABLE_H