#ifndef TOOLS_H
#define TOOLS_H

#include <QtGlobal>

bool GetBit(int word, unsigned int bits) {
    Q_ASSERT(bits < 16);
    word >>= bits;
    return (bool) (word & 1);
}
bool GetBit(unsigned int word, unsigned int bits) {
    Q_ASSERT(bits < 16);
    word >>= bits;
    return (bool) (word & 1);
}
bool GetBit(long word, unsigned int bits) {
    Q_ASSERT(bits < 32);
    word >>= bits;
    return (bool) (word & 1);
}
bool GetBit(unsigned long word, unsigned int bits) {
    Q_ASSERT(bits < 16);
    word >>= bits;
    return (bool) (word & 1);
}
#endif // TOOLS_H
