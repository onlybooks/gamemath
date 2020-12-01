#pragma once

namespace CK
{

class Texture
{
public:
	Texture() = default;
	Texture(std::string InFileName);
	Texture(std::wstring InFileName);
	~Texture() { }

public:
	void Release();
	bool IsIntialized() const { return (_Buffer.size() > 0); }
	UINT32 GetWidth() const { return _Width; }
	UINT32 GetHeight() const { return _Height; }
	UINT32 GetSize() const { return _Width * _Height; }
	LinearColor GetSample(Vector2 InUV) const;
	TextureFilterMode GetMagFilterMode() const { return _MagFilter; }
	TextureFilterMode GetMinFilterMode() const { return _MinFilter; }
	TextureAddressMode GetAddressMode() const { return _AddressMode; }
	bool IsAnisotropyEnabled() const { return _AnisotropyEnable; }
	float GetMaxAnisotropy() const { return _MaxAnisotropy; }

private:
	void LoadFromFile(FILE* f);

private:
	static constexpr BYTE _Channel = 4;
	std::vector<LinearColor> _Buffer;
	UINT32 _Width = 0;
	UINT32 _Height = 0;

	TextureFilterMode _MagFilter = TextureFilterMode::Nearest;
	TextureFilterMode _MinFilter = TextureFilterMode::Nearest;
	TextureAddressMode _AddressMode = TextureAddressMode::Repeat;
	bool _AnisotropyEnable = false;
	float _MaxAnisotropy = 16.0f;
};

}