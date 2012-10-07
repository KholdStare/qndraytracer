#ifndef _COLOUR_H
#define _COLOUR_H

#include <algorithm>
#include <ostream>

// ===========================================
class Colour {
public:
    Colour() {
        std::fill_n(m_data, 3, 0.0);
    }

    Colour(double grey) { 
        m_data[0] = grey;
        m_data[1] = grey;
        m_data[2] = grey;
    }

    Colour(double r, double g, double b) { 
        m_data[0] = r;
        m_data[1] = g;
        m_data[2] = b;
    }

    Colour(const Colour& other) {
        std::copy(other.m_data, other.m_data+3, m_data);
    }

    Colour& operator =(const Colour& other); 
    Colour& operator /=(const double c); 
    Colour operator /(const double c) const; 
    Colour& operator +=(const Colour& other); 
    Colour operator *(const Colour& other) const; 
    Colour operator *(const double c) const; 
    double& operator[](int i); 
    double operator[](int i) const; 
    bool isBlack() const; 
    Colour inverse() const; 
	void clamp(); 	

private:
	double m_data[3];
};


// inline methods for better performance
inline Colour& Colour::operator =(const Colour& other) {
    std::copy(other.m_data, other.m_data+3, m_data);
    return *this;
}

inline Colour& Colour::operator /=(const double c) {
    m_data[0] /= c;
    m_data[1] /= c;
    m_data[2] /= c;
    return *this;
}

inline Colour Colour::operator /(const double c) const {
    return Colour(*this) /= c;
}

inline Colour& Colour::operator +=(const Colour& other) {
    m_data[0] += other.m_data[0];
    m_data[1] += other.m_data[1];
    m_data[2] += other.m_data[2];
    return *this;
}

inline Colour Colour::operator *(const Colour& other) const {
    return Colour(m_data[0]*other.m_data[0], 
            m_data[1]*other.m_data[1], 
            m_data[2]*other.m_data[2]);
}

inline Colour Colour::operator *(const double c) const {
    return Colour(m_data[0]*c, 
            m_data[1]*c, 
            m_data[2]*c);
}

inline double& Colour::operator[](int i) {
    return m_data[i];
}

inline double Colour::operator[](int i) const {
    return m_data[i];
}

inline bool Colour::isBlack() const {
    return (m_data[0] + m_data[1] + m_data[2]) == 0.0;
}

inline Colour Colour::inverse() const {
    return Colour(1.0 - m_data[0],
            1.0 - m_data[1],
            1.0 - m_data[2]);
}

inline void Colour::clamp() {
    for (int i = 0; i < 3; i++) {
        if (m_data[i] > 1.0) m_data[i] = 1.0; 
        if (m_data[i] < 0.0) m_data[i] = 0.0; 
    }
}

inline Colour operator *(double s, const Colour& c)
{
    return Colour(s*c[0], s*c[1], s*c[2]);
}

inline Colour operator +(const Colour& u, const Colour& v)
{
    return Colour(u[0]+v[0], u[1]+v[1], u[2]+v[2]);
}

Colour operator *(double s, const Colour& c); 
Colour operator +(const Colour& u, const Colour& v); 
std::ostream& operator <<(std::ostream& o, const Colour& c); 
// ===========================================


template <typename T>
inline T gammaCorrect(T intensity, T gamma) {
    return pow(intensity, 1.0/gamma);
}

inline Colour attenuateByAbsorption(Colour const& col, double dist, Colour const& absorptionCoeff) {
    return Colour(col[0]/std::exp(absorptionCoeff[0]*10*dist),
                  col[1]/std::exp(absorptionCoeff[1]*10*dist),
                  col[2]/std::exp(absorptionCoeff[2]*10*dist));
}

#endif // _COLOUR_H
