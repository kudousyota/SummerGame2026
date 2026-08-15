#pragma once
#include "DxLib.h"
class Model
{
	//ƒ‚ƒfƒ‹‚ğŠÇ—‚·‚éƒNƒ‰ƒX
public:
	static Model& Instance()
	{
		static Model instance;
		return instance;
	}
	void PreloadAll();

	int CreateCreatureModel() const { return MV1DuplicateModel(m_creatureBaseHandle); }
	int CreateAngelModel()    const { return MV1DuplicateModel(m_angelBaseHandle); }
	int CreatAlienModel() const { return MV1DuplicateModel(m_alienBaseHandle); }
	int CreatPlayerModel() const { return MV1DuplicateModel(m_playerBaseHandle); }
	int CreatWitchTimeHandModel() const { return MV1DuplicateModel(m_witchTimeHandBaseHandle); }
	int CreatSkyDomeModel() const { return MV1DuplicateModel(m_skyDomeBaseHandle); }
	void ReleseAll();

private:
	int m_creatureBaseHandle = -1;
	int m_angelBaseHandle = -1;
	int m_alienBaseHandle = -1;
	int m_playerBaseHandle = -1;
	int m_witchTimeHandBaseHandle = -1;
	int m_skyDomeBaseHandle = -1;

};

