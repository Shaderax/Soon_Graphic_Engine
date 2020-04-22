#pragma once

#include "Core/Scene/3D/Components/Mesh.hpp"
#include <vector>

namespace Soon
{
	class MeshArray
	{
		public:
			std::string		_path;
			std::vector<Mesh>	_meshArray;

			  MeshArray& operator+=(const MeshArray& rhs)
			  {
				  _meshArray.insert(_meshArray.end(), rhs._meshArray.begin(), rhs._meshArray.end());

				  return (*this);
			  }

			  MeshArray& operator=(const MeshArray& rhs)
			  {
				  _meshArray = rhs._meshArray;

				  return (*this);
			  }
			  
			  Mesh& operator[](uint32_t index)
			  {
				  if (index >= _meshArray.size())
				  {
						std::cout << "Out of range MeshArray[]" << std::endl;
				  }
				  return (_meshArray[index]);
			  }
	};
};
