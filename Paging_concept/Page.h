# ifndef PAGE_H
# define PAGE_H
# include <vector>
# include <ostream>

class Page {
    unsigned _globalId, _localId, _parentId, _frequency, _lastAccessTime;

public:
    Page(unsigned parentId, unsigned localId);
    
    unsigned getGlobalId() { return _globalId; }
    unsigned getLocalId() { return _localId; }
    unsigned getParentId() { return _parentId; }
    unsigned getFrequency() { return _frequency; }
    unsigned getAccessTime() { return _lastAccessTime;}

    void incrementFrequency() {++this->_frequency;}
    void setAccessTime(unsigned t) { this->_lastAccessTime = t; }
};

# endif /* PAGE_H */
