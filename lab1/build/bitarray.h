#define BITARRAY_H

#include <string>

class BitArray
{
private:
    unsigned char* ARR;
    int SIZE;
    int BITS_NUM;

public:
    BitArray();
    explicit BitArray(unsigned int num_bits, unsigned long value = 0);
    BitArray(const BitArray& b);
    ~BitArray();
    BitArray& operator=(const BitArray& b);
    int size() const;
    bool empty() const;
    bool operator[](int i) const;
    void resize(int num_bits, bool value = false);
    void push_back(bool bit);
    void clear();
    void swap(BitArray& b);
    BitArray& set(int n, bool val = true);
    BitArray& set();
    BitArray& reset(int n);
    BitArray& reset();
    BitArray& operator&=(const BitArray& b);
    BitArray& operator|=(const BitArray& b);
    BitArray& operator^=(const BitArray& b);
    BitArray& operator<<=(int n);
    BitArray& operator>>=(int n);
    
    BitArray operator<<(int n) const;
    BitArray operator>>(int n) const;
    BitArray operator~() const;

    bool any() const;
    bool none() const;
    int count() const;
    std::string to_string(bool separate = false, unsigned char separator = ' ') const;
};

bool operator==(const BitArray& a, const BitArray& b);
bool operator!=(const BitArray& a, const BitArray& b);
BitArray operator&(const BitArray& b1, const BitArray& b2);
BitArray operator|(const BitArray& b1, const BitArray& b2);
BitArray operator^(const BitArray& b1, const BitArray& b2);
