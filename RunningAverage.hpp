
// #include <cmath>

class RunningAverage
{
public:
    RunningAverage(uint8_t nValues)
    {
        m_nValues = nValues;
        m_buf = new int[m_nValues];
        clearBuffer();
    }

    ~RunningAverage()
    {
        delete[] m_buf;
    }

    void addValue(int val)
    {
        m_buf[m_idx] = val;
        m_idx = (m_idx + 1) % m_nValues;
    }

    int getAverage()
    {
        int sum = 0;
        for (uint8_t i = 0; i < m_nValues; i++)
        {
            sum += m_buf[i];
        }
        return static_cast<int>(sum / m_nValues);
    }

    void clearBuffer()
    {
        for (uint8_t i = 0; i < m_nValues; i++)
        {
            m_buf[i] = 0;
        }
        m_idx = 0;
    }

private:
    uint8_t m_nValues;
    int *m_buf;
    uint8_t m_idx = 0;
};