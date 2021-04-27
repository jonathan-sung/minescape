#pragma once

#include "ecm.h"
#include <Box2D/Box2D.h>

using namespace sf;

class RopeComponent : public Component {
private:
	Vector2f clickPos;
	Vector2f directionVector;
	bool buttonPressed;

	float launchSpeed;
	float ropeMaxLength;

	Vector2f initialRopePosition;
	Vector2f finalRopePosition;
	
	b2Body* _endBody;
	b2Fixture* _endfixture;
	b2Body* _pbody;
	b2Fixture* _pfixture;
	b2RopeJointDef rjDef;
	b2Joint* ropeJoint;

	enum RopeState
	{
		Ready,
		InAir,
		Latched,
		Withdrawing,
		Unusable
	};

	RopeState ropeState;

	void updateClickPos();
	bool mouseClick();
	void setDirectionVector(Vector2f initialPos, Vector2f targetPos);
	bool isTouchingWall();
	void createRopeJoint();
	void disposeOfDistanceJoint();
	
public:

	explicit RopeComponent(Entity* p,float launchspeed,float maxlength);
	RopeComponent() = delete;

	void update(double dt) override;
	void render() override;
};
