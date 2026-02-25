#ifndef DIRECTX_HELPER_H
#define DIRECTX_HELPER_H

#include <stdexcept>

namespace URay
{
	inline void ThrowIfFailed(HRESULT hr)
	{
		if (FAILED(hr))
		{
			throw std::runtime_error("DirectX Error Occurred.");
		}
	}
}

#endif
