# include "Page.h"

static unsigned _globalIdCounter = 0;

Page::Page(unsigned localId, unsigned parentId)
        : _globalId(_globalIdCounter++), _localId(localId), _parentId(parentId) { _frequency=0;}
