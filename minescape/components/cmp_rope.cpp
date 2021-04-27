#include "cmp_rope.h"
#include <engine.h>
#include <SFML/Window/Mouse.hpp>
#include <SFML/Graphics/Vertex.hpp>
#include <system_physics.h>
#include "cmp_physics.h"
#include <Box2D/Box2D.h>
#include <system_renderer.h>
#include "Box2D/Common/b2Math.h"

using namespace std;

RopeComponent::RopeComponent(Entity* p, float launchspeed,float maxlength) :Component(p)
{
	buttonPressed = false;
	ropeState = RopeState::Ready;
	launchSpeed = launchspeed;
	ropeMaxLength = maxlength;
}

void RopeComponent::updateClickPos()
{
	//get window relative mouse coordinates
	Vector2i mousePos = Mouse::getPosition(Engine::GetWindow());
	clickPos = Engine::GetWindow().mapPixelToCoords(mousePos);
}

bool RopeComponent::mouseClick() 
{
	if (Mouse::isButtonPressed(Mouse::Button::Left))
	{
		if (!buttonPressed)
		{
			buttonPressed = true;
			return true;
		}
		else
		{
			return false;
		}
	}
	else 
	{
		if (buttonPressed)
		{
			buttonPressed = false;
		}
		return false;
	}
}

void RopeComponent::setDirectionVector(Vector2f initialPos,Vector2f targetPos)
{
	//set distance to direction vector
	directionVector = Vector2f(
		targetPos.x- initialPos.x,
		targetPos.y- initialPos.y);

	//get the length
	float directionVectorLength = length(directionVector);

	//divide by the length to get size 1 vector
	directionVector = Vector2f(directionVector.x/directionVectorLength,
		directionVector.y/directionVectorLength);
}

bool RopeComponent::isTouchingWall()
{
	vector<const b2Contact const*> ret;
	b2ContactEdge* edge = ropeJoint->GetBodyB()->GetContactList();
	
	while (edge != NULL) {
		const b2Contact* contact = edge->contact;
		if (contact->IsTouching()) {
			cout << "edge touching" << endl;
			ret.push_back(contact);
			return true;
		}
		else { cout << "edge not touching" << endl; }
		edge = edge->next;
	}

	auto touch = ret;
	const auto& pos = ropeJoint->GetBodyB()->GetPosition();
	const float halfPlrHeigt =  .5f;
	const float halfPlrWidth =  .52f;

	//recheck world build

	b2WorldManifold manifold;
	for (const auto& contact : touch) {
		contact->GetWorldManifold(&manifold);
		const int numPoints = contact->GetManifold()->pointCount;
		// If all contacts are below the player.
		for (int j = 0; j < numPoints; j++) {
			cout << "points touched" << numPoints << endl;
		}
	}
	cout << "no touches" << endl;
	return false;
}

void RopeComponent::createRopeJoint()
{
	//create joint definition
	//set the first body
	auto pyco = _parent->GetCompatibleComponent<PhysicsComponent>();
	_pbody = pyco[0].get()->getFixture()->GetBody();
	_pbody->SetGravityScale(1.0f);
	rjDef.bodyA = _pbody;
	//world vector of the direction of rope launch
	b2Vec2 directionBVec = b2Vec2(directionVector.x, -directionVector.y);
	directionBVec.Normalize();
	//create the second body definition
	b2BodyDef endbodydef;
	//set position to player + direction 
	b2Vec2 offset = b2Vec2(directionBVec.x * (ropeMaxLength * Physics::physics_scale_inv),
		directionBVec.y * (ropeMaxLength * Physics::physics_scale_inv));
	endbodydef.position = rjDef.bodyA->GetPosition()+ (offset);
	//make the body static
	endbodydef.type = b2BodyType::b2_dynamicBody;
	//create second body
	_endBody = Physics::GetWorld()->CreateBody(&endbodydef);
	//set the second body
	rjDef.bodyB = _endBody;

	//make it a rope joint
	rjDef.type = b2JointType::e_ropeJoint;
	rjDef.localAnchorA = b2Vec2(0, 0);
	rjDef.localAnchorB = b2Vec2(0, 0);
	rjDef.collideConnected = true;
	//set the maximum length
	rjDef.maxLength = ropeMaxLength*Physics::physics_scale_inv;

	//create joint in world
	auto joint = Physics::GetWorld().get()->CreateJoint(&rjDef);
	//set it to instance
	ropeJoint = joint;
}

void RopeComponent::disposeOfDistanceJoint()
{
	Physics::GetWorld().get()->DestroyJoint(ropeJoint);
	//Physics::GetWorld().get()->DestroyBody(ropeJoint->GetBodyB());
	
}

void RopeComponent::update(double dt)
{
	switch (ropeState)
	{
		//rope is ready to be used
	case RopeState::Ready:
	{
		//on mouse click
		if (mouseClick())
		{
			//handle calculation for rope firing
			updateClickPos();
			//set the direction vector
			setDirectionVector(_parent->getPosition(), clickPos);
			//create the rope joint
			createRopeJoint();

			//set the endpoint to the bodyB point
			Vector2f dir = Vector2f(
				ropeJoint->GetBodyB()->GetPosition().x - ropeJoint->GetBodyA()->GetPosition().x,
				ropeJoint->GetBodyB()->GetPosition().y - ropeJoint->GetBodyA()->GetPosition().y
				);
			dir = Vector2f(dir.x * Physics::physics_scale,
				-dir.y * Physics::physics_scale);
			finalRopePosition = _parent->getPosition() + dir;

			//fire rope
			ropeState = RopeState::InAir;
		}
		break;
	}
	case RopeState::InAir:
	{
		initialRopePosition = _parent->getPosition();

		//set the endpoint to the bodyB point
		Vector2f dir = Vector2f(
			ropeJoint->GetBodyB()->GetPosition().x - ropeJoint->GetBodyA()->GetPosition().x,
			ropeJoint->GetBodyB()->GetPosition().y - ropeJoint->GetBodyA()->GetPosition().y
		);
		dir = Vector2f(dir.x * Physics::physics_scale,
			-dir.y * Physics::physics_scale);
		finalRopePosition = _parent->getPosition() + dir;
		if (isTouchingWall())
		{
			cout << "touched" << endl;
			ropeState = RopeState::Latched;
		}
		//once it touched a wall tile go to latch
		//if (isTouchingWall()) ropeState = RopeState::Latched;
		//else ropeState = RopeState::Withdrawing;

		break;
	}
	case RopeState::Latched:
	{
		//once latched make it controllable by player
		if (Keyboard::isKeyPressed(Keyboard::Space))
		{
			cout << "changing states" << endl;
			ropeState = RopeState::Withdrawing;
		}
		//when player presses jump button go to withdraw and make player jump
		break;
	}
	case RopeState::Withdrawing:
	{
		disposeOfDistanceJoint();

		//once it has fully reduced go to unusable
		ropeState = RopeState::Unusable;

		break;
	}
	case RopeState::Unusable:
	{
		//stay here for a bit (cooldown?)
		//go back to ready
		ropeState = RopeState::Ready;
		//reset mouse click for reuse
		buttonPressed = false;

		break;
	}
	default:
		break;
	}
}

void RopeComponent::render() 
{
	//only render if rope is out
	if (ropeState == RopeState::InAir || ropeState == RopeState::Latched || ropeState == RopeState::Withdrawing) {
		
		//create vertex array (this should be a temporary solution)
		sf::Vertex ropeLines[] = { sf::Vertex(initialRopePosition),
			sf::Vertex(finalRopePosition)};
		//render rope
		Engine::GetWindow().draw( ropeLines , 2, sf::Lines);
	}
}