#pragma once
#include "../System/Vector3.h"
#include "Character.h"
#include <memory>
class Player;
class Alien : public Character
{
public:
	Alien();
	~Alien()override;
	void Init()override;
	void Update()override;
	void Draw()override;
	void ApplyDamage(int damage)override;

	CharacterType GetCharacterType()const override;

private:

	enum class AlienState
	{
		Idle,
		Move,
		Attack,
		Down,
		StandUp,
		Up
	};

	//Œ»İ‚Ìó‘Ô
	AlienState m_currentState;

	//‘O‰ñ‚Ìó‘Ô
	AlienState m_prevState;

	//ƒnƒ“ƒhƒ‹
	int m_ailenHandle;


	void TransitionTo(AlienState nextState);

	std::shared_ptr<Player>m_pPlayer;

};