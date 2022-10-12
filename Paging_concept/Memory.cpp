# include <cassert>
# include <iostream>
# include "Memory.h"
# include "Page.h"

//Memory::~Memory() {
//    for(unsigned i=0; i<_memory.size();i++)delete _memory[i];
//    delete this;
//}

void Memory::addPage(Page *p) {
    assert(!isFull());

    _memory.push_back(p);
}

void Memory::evictPages(Process *process) {
    unsigned pId = process->getProcessId();
    for (unsigned i = 0; i < _memory.size(); i++) {
        if (pId == _memory[i]->getParentId()) {
            _memory.erase(_memory.begin() + i);
            evictPages(process);
            return;
        }
    }
}

void Memory::replacePage(Page *oldPage, Page *newPage) {
    for (unsigned i = 0; i < _memory.size(); i++)
        if (_memory[i] == oldPage)
            _memory[i] = newPage;
}

void Memory::removePage(Page *oldPage){
    for (unsigned i = 0; i < _memory.size(); i++){
        if (_memory[i] == oldPage){
            _memory.erase(_memory.begin() + i);
            return;
        }
    }
}

void Memory::printMap(Process *process) {
    unsigned pId = process->getProcessId();

    std::cout << "<";
    for (unsigned i = 0; i < _memory.size(); i++) {
        if (pId != _memory[i]->getParentId())
            std::cout << _memory[i]->getParentId();
        else std::cout << ".";
        if (i != 99) std::cout << ", ";
    }

    for (unsigned i = _memory.size(); i < 100; i++) {
        std::cout << ".";
        if (i != 99) std::cout << ", ";
    }
    std::cout << ">";
}
