#include <limits>
#include <ostream>
#include <stdexcept>
#include <string>
#include <string_view>
#include "DSS/Marking.h"

Marking::Marking(const size_t place_count) : m_data(place_count) {
}

Marking::Marking(const Marking &other) : m_data(other.m_data) {
}

Marking &Marking::operator=(const Marking &other) {
    m_data = other.m_data;
    return *this;
}

void Marking::setTokens(const size_t place, const uint32_t tokens) {
    if (tokens > std::numeric_limits<token_t>::max()) {
        throw std::overflow_error("Marking: " + std::to_string(tokens) + " tokens in place " + std::to_string(place)
                                  + " (at most " + std::to_string(std::numeric_limits<token_t>::max()) + " are supported)");
    }
    m_data[place] = static_cast<token_t>(tokens);
}

size_t Marking::hash() const {
    return std::hash<std::string_view>{}({reinterpret_cast<const char *>(m_data.data()), m_data.size()});
}

void Marking::addSucc(Transition *t, Marking *m) {
    m_succ.emplace_back(t, m);
}

std::ostream &operator<<(std::ostream &stream, const Marking &m) {
    stream << '(';
    for (size_t place {}; place < m.m_data.size(); ++place) {
        if (place) stream << ',';
        stream << static_cast<unsigned>(m.m_data[place]);
    }
    return stream << ')';
}
