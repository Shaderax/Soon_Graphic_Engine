#pragma once

#include "Id.hpp"

namespace Soon
{
	template < typename TBase >
		class ClassTypeId
		{
			public:
				template < typename T >
					static TypeId GetId( void )
					{
						static const TypeId id = _nextId++;
						return (id);
					}

			private:
				static TypeId _nextId;
		};

	template < typename TBase >
		TypeId ClassTypeId<TBase>::_nextId = 0;
}
