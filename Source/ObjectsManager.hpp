#include <vector>
#include "Utilities/Error.hpp"

namespace Soon
{
    template<typename T>
    class ObjectsManager
    {
    public:
        ObjectsManager( void );
        ~ObjectsManager( void );

        ObjectsManager( const ObjectsManager& obj );
        ObjectsManager( const ObjectsManager&& obj );

        ObjectsManager& operator=(const ObjectManager& obj);
        ObjectsManager& operator=(ObjectManager&& obj);

        T& Get(std::uint32_t id);

        template<typename ... N>
        T& GetNew(N... args)
        {
            m_Objects.emplace_back(args);
            return m_Objects.back();
        }
    private:
        std::vector<T> m_Objects;
        std::uint32_t m_ObjectsCounter = 0;
		std::vector<uint32_t> m_FreeIds;
    };
}