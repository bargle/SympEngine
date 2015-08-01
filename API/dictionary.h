#ifndef _DICTIONARY_H_
#define _DICTIONARY_H_

#include <map>

template< typename key, typename value >
class Dictionary {
public:
	Dictionary(){}
	~Dictionary(){}

	value& operator[] ( const key& _key ) {
		std::map< key, value >::iterator iter = elements.find( _key );
		if ( iter == elements.end() ){
			value val;
			std::pair< key, value > _pair( _key, val );
			std::pair< std::map< key, value >::iterator, bool > ret = elements.insert( _pair );
			return ret.first->second;
		} else {
			return iter->second;
		}
	}

private:
	std::map< key, value > elements;
};

#endif
