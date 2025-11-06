#include <iostream>
#include "bitarray.h"

BitArray::BitArray()
{
    SIZE = 0;
    BITS_NUM = 0;
    ARR = nullptr;
}

BitArray::~BitArray()
{
    delete[] ARR;
}

BitArray::BitArray(unsigned int num_bits, unsigned long value)
{
    SIZE = ((num_bits + 7)/8);
    BITS_NUM = num_bits;
    ARR = new unsigned char[SIZE]();

    for (unsigned int i = 0; i < num_bits; i++)
    {
        if (value&1)
        {
            set(i);
        }
        else
        {
            reset(i);
        }
        value >>= 1;
    }
}
BitArray::BitArray(const BitArray& b)
{
    ARR = new unsigned char[b.SIZE];
    for (int i = 0; i < b.SIZE; i++)
    {
        ARR[i] = b.ARR[i];
    }
}


//Обменивает значения двух битовых массивов.
void BitArray::swap(BitArray& b)
{
    unsigned char* TMP_ARR = ARR;
    ARR = b.ARR;
    b.ARR = TMP_ARR;

    int TMP_SIZE = SIZE;
    SIZE = b.SIZE;
    b.SIZE = TMP_SIZE;

    int TMP_BITS_NUM = BITS_NUM;
    BITS_NUM = b.BITS_NUM;
    b.BITS_NUM = TMP_BITS_NUM;
}

BitArray& BitArray::operator=(const BitArray& b)
{
    if (b == *this)
    {
        return *this;
    }
    delete[] ARR;
    BITS_NUM = b.BITS_NUM;
    SIZE = b.SIZE;
    ARR = new unsigned char[SIZE];
    for (int i = 0; i < SIZE; i++)
    {
        ARR[i] = b.ARR[i];
    }
    return *this;
}


//Изменяет размер массива. В случае расширения, новые элементы 
//инициализируются значением value.
void BitArray::resize(int num_bits, bool value)
{
    
    if (num_bits == BITS_NUM)
    {
        return;
    }
    int NEW_SIZE = (num_bits + 7)/8;         
    unsigned char* TMP = new unsigned char[NEW_SIZE]();        
    int BITS_TMP = std::min(BITS_NUM, num_bits);
    for (int i = 0; i < BITS_TMP; i++)
    {
        if ((ARR[i/8] >> (i%8)) & 1)
        {
            TMP[i/8] |= (1 << (i%8));
        }
    }
    for (int i = BITS_NUM; i < num_bits; i++)
    {
        if (value)
        {
            TMP[i/8] |= (1 << (i%8));
        }
        else
        {
            TMP[i/8] &= ~(1 << (i%8));
        }
    }
    
    delete[] ARR;
    ARR = TMP;
    SIZE = NEW_SIZE;
    BITS_NUM = num_bits;
}
//Добавляет новый бит в конец массива. В случае необходимости 
//происходит перераспределение памяти.
void BitArray::push_back(bool bit)
{
    resize(BITS_NUM+1, bit);
}


//Битовые операции над массивами.
//Работают только на массивах одинаковоsго размера.
//Обоснование реакции на параметр неверного размера входит в задачу.
BitArray& BitArray::operator&=(const BitArray& b)
{
    if (BITS_NUM == b.BITS_NUM)
    {
        if (BITS_NUM%8 != 0)
        {
            for (int i = 0; i < SIZE - 1; i++)
            {
                ARR[i] &= b.ARR[i];
            }
            for (int i = (SIZE - 1) * 8; i < BITS_NUM; i++)
            {
                //std::cout<<" "<<i;
                if (((ARR[i/8] >> i%8)&1) != ((b.ARR[i/8] >> i%8)&1))
                {
                    set(i, 0);
                }
            }
        }
        else
        {
            for (int i = 0; i < SIZE; i++)
            {
                ARR[i] &= b.ARR[i];
            }
        }
    }
    return *this;
}
BitArray& BitArray::operator|=(const BitArray& b)
{
    if (BITS_NUM == b.BITS_NUM)
    {
        // std::cout<<SIZE<<std::endl;
        if (BITS_NUM%8 != 0)
        {
            for (int i = 0; i < SIZE - 1; i++)
            {
                ARR[i] |= b.ARR[i];
            }
            for (int i = (SIZE - 1) * 8; i < BITS_NUM; i++)
            {
                //std::cout<<" "<<i;
                if (((ARR[i/8] >> i%8)&1) || ((b.ARR[i/8] >> i%8)&1))
                {
                    set(i, 1);
                }
                else
                {
                    set(i, 0);
                }
            }
        }
        else
        {
            for (int i = 0; i < SIZE; i++)
            {
                ARR[i] |= b.ARR[i];
            }
        }
    }
    return *this;
}
BitArray& BitArray::operator^=(const BitArray& b)
{
    if (BITS_NUM == b.BITS_NUM)
    {
        if (BITS_NUM%8 != 0)
        {
            for (int i = 0; i < SIZE - 1; i++)
            {
                ARR[i] ^= b.ARR[i];
            }
            for (int i = (SIZE - 1) * 8; i < BITS_NUM; i++)
            {
                //std::cout<<" "<<i;
                if (((ARR[i/8] >> i%8)&1) == ((b.ARR[i/8] >> i%8)&1))
                {
                    set(i, 0);
                }
                else
                {
                    set(i, 1);
                }
            }
        }
        else
        {
            for (int i = 0; i < SIZE; i++)
            {
                ARR[i] ^= b.ARR[i];
            }
        }
        
    }
    return *this;
}

//Битовый сдвиг с заполнением нулями.
BitArray& BitArray::operator<<=(int n)
{
    // this->resize(BITS_NUM + n);
    for (int i = BITS_NUM - 1; i >= n; i--)
    {
        //set(i, ARR[(i - n)/8] >> (i - n)%8);
        set(i, (ARR[(i - n)/8] >> (i - n)%8) & 1);
    }
    for (int i = 0; i < n; i++)
    {
        set(i, 0);
    }
    return *this;
}
BitArray& BitArray::operator>>=(int n)
{
    for (int i = 0; i < BITS_NUM - n; i++)
    {
        //set(i, ARR[(i - n)/8] >> (i - n)%8);
        set(i, (ARR[(i + n)/8] >> (i + n)%8) & 1);
    }
    for (int i = BITS_NUM - n; i < BITS_NUM; i++)
    {
        set(i, 0);
    }
    return *this;
}

BitArray BitArray::operator<<(int n) const
{
    BitArray result(BITS_NUM);
    result = *this;
    result <<= n;
    return result;
}
BitArray BitArray::operator>>(int n) const
{
    BitArray result(BITS_NUM);
    result = *this;
    result >>= n;
    return result;
}


//Устанавливает бит с индексом n в значение val.
BitArray& BitArray::set(int n, bool val)
{
    if (n >= BITS_NUM)
    {
        resize(n+1, 0);
    }
    if (val)
    {
        ARR[n/8] |= (1 << (n%8));
    }
    else
    {
        ARR[n/8] &= ~(1 << (n%8));
    }
    return *this;
}
//Заполняет массив истиной.
BitArray& BitArray::set()
{
    for (int i = 0; i < SIZE; i++)
    {
        for (int j = 0; j < 8; j++)
        {
            ARR[i] |= 255;
        }
    }
    return *this;
}

//Устанавливает бит с индексом n в значение false.
BitArray& BitArray::reset(int n)
{
    ARR[n/8] &= ~(1 << (n%8));
    return *this;
}
//Заполняет массив ложью.
BitArray& BitArray::reset()
{
    for (int i = 0; i < SIZE; i++)
    {
        for (int j = 0; j < 8; j++)
        {
            ARR[i] &= 0;
        }
    }
    return *this;
}

//true, если массив содержит истинный бит.
bool BitArray::any() const
{
    for (int i = 0; i < SIZE; i++)
    {
        if (ARR[i] != 0)
        {
            return 1;
        }
    }
    return 0;
}
//true, если все биты массива ложны.
bool BitArray::none() const
{
    return !any();
}
//Битовая инверсия
BitArray BitArray::operator~() const
{
    BitArray res(BITS_NUM);
    for (int i = 0; i <= BITS_NUM; i++)
    {
        int curbit = ((ARR[i/8] >> i%8) & 1);
        if (curbit)
        {
            res.ARR[i/8] &= ~(1 << (i%8));
        }
        else
        {
            res.ARR[i/8] |= (1 << (i%8));
        }
    }
    return res;
}
//Подсчитывает количество единичных бит.
int BitArray::count() const
{
    int res = 0;
    for (int i = 0; i <= BITS_NUM; i++)
    {
        res += ((ARR[i/8] >> i%8) & 1);
    }
    return res;
}


//Возвращает значение бита по индексу i.
bool BitArray::operator[](int i) const
{
    if (i < BITS_NUM && i > 0)
    {
        return ARR[i/8] & (1 << (i%8));
    }
    return false;
}
int BitArray::size() const
{
    return SIZE;
}
// int bnum() const
// {
//     return BITS_NUM;
// }
bool BitArray::empty() const
{
    return (SIZE == 0);
}

//Возвращает строковое представление массива.

std::string BitArray::to_string(bool separate, unsigned char separator) const
{
    std::string res;
    for (int i = BITS_NUM - 1; i >= 0; i--)
    {
        res += ((ARR[i/8] >> i%8) & 1) ? '1' : '0';
        if (i % 8 == 0 && separate && i > 0) 
        {
            res += separator;
        }
    }
    return res;
}

bool operator==(const BitArray & a, const BitArray & b)
{
    int size_a = a.size();
    int size_b = b.size();
    if (size_a != size_b)
    {
        return false;
    }
    for (int i = 0; i < size_a * 8; i++)
    {
        //std::cout<<a[i]<<" "<<b[i]<<std::endl;
        if (a[i] != b[i])
        {
            return false;
        }
    }
    return true;
}

bool operator!=(const BitArray & a, const BitArray & b)
{
    return !(a == b);
}

BitArray operator&(const BitArray& b1, const BitArray& b2)
{
    BitArray result(b1.size());
    result = b1;
    result &= b2;
    return result;
}
BitArray operator|(const BitArray& b1, const BitArray& b2)
{
    BitArray result(b1.size());
    result = b1;
    result |= b2;
    return result;
}

BitArray operator^(const BitArray& b1, const BitArray& b2)
{
    BitArray result(b1.size());
    result = b1;
    result ^= b2;
    return result;
}
