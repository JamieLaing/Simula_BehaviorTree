// BehaviorTree.h

#ifndef _BEHAVIORTREE_h
#define _BEHAVIORTREE_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

#include "Motor.h"
#include "CRC_Sensors.h"
#include "CRC_AudioManager.h"
#include "CRC_Lights.h"
#include <StandardCplusplus.h>
#include <list>
#include <vector>
#include <initializer_list>
#include <algorithm>

struct TREE_STATE {
	bool treeActive = false;
};
extern struct TREE_STATE treeState;

class Behavior_Tree {  // Note:  A proper copy constructor and assignment operator should be defined, since the implicit ones use shallow copies only.
private:
	
public:
	class Node {  // This class represents each node in the behaviour tree.
	public:
		virtual bool run() = 0;
	};

	class CompositeNode : public Node {  //  This type of Node follows the Composite Pattern, containing a list of other Nodes.
	private:
		std::vector<Node*> children;
	public:
		const std::vector<Node*>& getChildren() const { return children; }
		void addChild(Node* child) { children.push_back(child); }
		void addChildren(std::initializer_list<Node*>&& newChildren) { for (Node* child : newChildren) addChild(child); }
		template <typename CONTAINER>
		void addChildren(const CONTAINER& newChildren) { for (Node* child : newChildren) addChild(child); }
	protected:
		std::vector<Node*> childrenShuffled() const { 
			std::vector<Node*> temp = children;  
			std::random_shuffle(temp.begin(), temp.end());  
			return temp; 
		}
	};

	class Selector : public CompositeNode {
	public:
		virtual bool run() override {
			for (Node* child : getChildren()) {  // The generic Selector implementation
				if (child->run())  // If one child succeeds, the entire operation run() succeeds.  Failure only results if all children fail.
					return true;
			}
			return false;  // All children failed so the entire run() operation fails.
		}
	};

	class RandomSelector : public CompositeNode {  // RandomSelector operates as a Selector, but in a random order instead of from first child to last child.
	public:
		virtual bool run() override {
			for (Node* child : childrenShuffled()) {  // The order is shuffled
				if (child->run())
					return true;
			}
			return false;
		}
	};

	class Sequence : public CompositeNode {
	public:
		virtual bool run() override {
			for (Node* child : getChildren()) {  // The generic Sequence implementation.
				if (!child->run())  // If one child fails, the entire operation run() fails.  Success only results if all children succeed.
					return false;
			}
			return true;  // All children suceeded, so the entire run() operation succeeds.
		}
	};

	class Root : public Node {
	private:
		Node* child;
		friend class Behavior_Tree;
		void setChild(Node* newChild) { child = newChild; }
		virtual bool run() override { return child->run(); }
	};
private:
	Root* root;
public:
	Behavior_Tree() : root(new Root) {}
	void setRootChild(Node* rootChild) const { root->setChild(rootChild); }
	bool run() const { return root->run(); }
};
class Action : public Behavior_Tree::Node {
private:
	String name;
	int probabilityOfSuccess;
public:
	Action(const String newName, int prob) : name(newName), probabilityOfSuccess(prob) {}
private:
	virtual bool run() override {
		long randNum = random(1, 101);

		Serial.print(F("random number: "));
		Serial.println(randNum);
		Serial.print(name);
		if (randNum <= probabilityOfSuccess) {
			Serial.println(F(" succeeded."));
			Serial.println("");
			return true;
		}
		Serial.println(F(" failed."));
		Serial.println(F(""));
		return false;
	}
};
class Button_Stop : public Behavior_Tree::Node {
private:
	bool buttonState = true;
	int lastButtonState = HIGH;
	unsigned long debounceTime;
	const long debounceDelay = 10;
	virtual bool run() override {
		int reading = digitalRead(hardware.pinButton);
		/*Serial.print("reading:");
		Serial.println(reading);
		Serial.print("lastButtonState:");
		Serial.println(lastButtonState);*/
		if (reading != lastButtonState) {
			debounceTime = millis();
		}

		if ((millis() - debounceTime) > debounceDelay) {
			if (reading != buttonState) {
				buttonState = reading;
				if (buttonState == HIGH) {
					treeState.treeActive = !treeState.treeActive;
					if (!treeState.treeActive)
					{
						Serial.println(F("Behavior tree off."));
						motors.motorLeft->powerOff();
						motors.motorRight->powerOff();
						motors.motorsActive = false;
						sensors.deactivate();
						crcLights.showNothing();
					}
					else {
						Serial.println(F("Activating behavior tree."));
						sensors.activate();
						crcLights.showBreathing();
						delay(50);
						//return true to allow sensors to read before next tree loop.
						return true;
					}
				}
			}
		}
		lastButtonState = reading;
		return !treeState.treeActive;
	}
};
class Battery_Check : public Behavior_Tree::Node {
private:
	bool nodeActive = false;
	unsigned long now;
	unsigned long lastCheck = 0;
	int interval = 10000;
	virtual bool run() override {
		now = millis();
		if (!nodeActive) {
			if ((lastCheck == 0) || (now > lastCheck + interval)) {
				lastCheck = now;
				float postVoltage = hardware.readBatteryVoltage();
				if (postVoltage < hardware.lowBatteryVoltage) {
					nodeActive = true;
				}
			}
		}
		else
		{
			Serial.println(F("Batteries low."));
		}
		return nodeActive;
	}
};

class Orientation_Check : public Behavior_Tree::Node {
private:
	bool nodeActive = false;
	const int Z_Orient_Min = 15000;
	virtual bool run() override {


		if (sensors.lsm.accelData.z < Z_Orient_Min) {
			crcAudio.playRandomAudio("emotions/scare_", 9, ".mp3");
			//Serial.print("Z: ");
			//Serial.println(sensors.lsm.accelData.z);
			nodeActive = true;
		}
		else {
			nodeActive = false;
		}
		return nodeActive;

		/*Serial.print("Accel X: "); Serial.print((int)sensors.lsm.accelData.x); Serial.print(" ");
		Serial.print("Y: "); Serial.print((int)sensors.lsm.accelData.y);       Serial.print(" ");
		Serial.print("Z: "); Serial.println((int)sensors.lsm.accelData.z);     Serial.print(" ");
		Serial.print("Mag X: "); Serial.print((int)sensors.lsm.magData.x);     Serial.print(" ");
		Serial.print("Y: "); Serial.print((int)sensors.lsm.magData.y);         Serial.print(" ");
		Serial.print("Z: "); Serial.println((int)sensors.lsm.magData.z);       Serial.print(" ");
		Serial.print("Gyro X: "); Serial.print((int)sensors.lsm.gyroData.x);   Serial.print(" ");
		Serial.print("Y: "); Serial.print((int)sensors.lsm.gyroData.y);        Serial.print(" ");
		Serial.print("Z: "); Serial.println((int)sensors.lsm.gyroData.z);      Serial.print(" ");
		Serial.print("Temp: "); Serial.print((int)sensors.lsm.temperature);    Serial.println(" ");*/
	}
};

class Cliff_Center : public Behavior_Tree::Node {
private:
	bool nodeActive = false;
	const long duration = 200;
	unsigned long currentTime;
	unsigned long nodeStartTime = 0;
	virtual bool run() override {

		currentTime = millis();

		if(!nodeActive){
			if (sensors.irLeftCliff && sensors.irRightCliff) {
				nodeActive = true;
				Serial.println(F("Cliff center detected."));
				nodeStartTime = currentTime;
				motors.motorLeft->setPower(-120);
				motors.motorRight->setPower(-120);
			}
		}
		else
		{
			if ((nodeStartTime + duration < currentTime) && (!sensors.irLeftCliff && !sensors.irRightCliff)) {
				Serial.println(F("Cliff center complete."));
				nodeActive = false;
				nodeStartTime = 0;
				motors.motorLeft->powerOff();
				motors.motorRight->powerOff();
				motors.motorsActive = false;
			}
		}
		return nodeActive;
	}
};
class Cliff_Left : public Behavior_Tree::Node {
private:
	bool nodeActive = false;
	const long backDuration = 400;
	const long turnDuration = 400;
	unsigned long currentTime;
	unsigned long nodeStartTime = 0;
	bool turnStarted = false;
	virtual bool run() override {

		currentTime = millis();

		if (!nodeActive) {
			if (sensors.irLeftCliff && !sensors.irRightCliff) {
				Serial.println(F("Cliff left detected."));
				nodeStartTime = currentTime;
				nodeActive = true;
				motors.motorLeft->setPower(-120);
				motors.motorRight->setPower(-120);
			}
		}
		else {
			if ((nodeStartTime + backDuration < currentTime) && !turnStarted) {
				Serial.println(F("Cliff left turning."));
				turnStarted = true;
				motors.motorLeft->setPower(180);
				motors.motorRight->setPower(-180);
			}
			if (nodeStartTime + backDuration + turnDuration < currentTime) {
				Serial.println(F("Cliff left stopping."));
				nodeStartTime = 0;
				motors.motorsActive = false;
				motors.motorLeft->powerOff();
				motors.motorRight->powerOff();
				nodeActive = false;
				turnStarted = false;
			}
		}
		return nodeActive;
	}
};
class Cliff_Right : public Behavior_Tree::Node {
private:
	bool nodeActive = false;
	const long backDuration = 400;
	const long turnDuration = 400;
	unsigned long currentTime;
	unsigned long nodeStartTime = 0;
	bool turnStarted = false;
	virtual bool run() override {

		currentTime = millis();
		if (!nodeActive) {
			if (!sensors.irLeftCliff && sensors.irRightCliff) {
				Serial.println(F("Cliff right detected."));
				nodeStartTime = currentTime;
				nodeActive = true;
				motors.motorLeft->setPower(-120);
				motors.motorRight->setPower(-120);
			}
		}
		else {
			if ((nodeStartTime + backDuration < currentTime) && !turnStarted) {
				Serial.println(F("Cliff right turning."));
				turnStarted = true;
				motors.motorLeft->setPower(-180);
				motors.motorRight->setPower(180);
			}
			if (nodeStartTime + backDuration + turnDuration < currentTime) {
				Serial.println(F("Cliff right stopping."));
				nodeStartTime = 0;
				motors.motorsActive = false;
				motors.motorLeft->powerOff();
				motors.motorRight->powerOff();
				nodeActive = false;
				turnStarted = false;
			}
		}
		return nodeActive;
	}
};

class Perimeter_Center : public Behavior_Tree::Node {
private:
	bool nodeActive = false;
	uint8_t alarmCM = 11;
	const long duration = 200;
	unsigned long currentTime;
	unsigned long nodeStartTime = 0;
	virtual bool run() override {
		currentTime = millis();
		if (!nodeActive) {
			if (sensors.irFrontCM < alarmCM && sensors.irFrontCM > hardware.irMinimumCM) {
				nodeStartTime = currentTime;
				nodeActive = true;
				Serial.print(F("Permimeter center alarm: "));
				Serial.println(sensors.irFrontCM);
				//50% chance of turning either directon
				long randNum = random(1, 101);

				Serial.print(F("random number: "));
				Serial.println(randNum);
				if (randNum <= 50) {
					Serial.println(F("Turning left."));
					motors.motorLeft->setPower(-160);
					motors.motorRight->setPower(160);
				}
				else
				{
					Serial.println(F("Turning right."));
					motors.motorLeft->setPower(160);
					motors.motorRight->setPower(-160);
				}
				return nodeActive;
			}
		}
		else {
			if ((nodeStartTime + duration < currentTime) && (sensors.irFrontCM >= alarmCM)) {
				Serial.println(F("Perimeter center complete."));
				motors.motorLeft->powerOff();
				motors.motorRight->powerOff();
				motors.motorsActive = false;
				nodeStartTime = 0;
				nodeActive = false;
			}
		}
		return nodeActive;
	}
};
class Perimeter_Left : public Behavior_Tree::Node {
private:
	bool nodeActive = false;
	uint8_t alarmCM = 11;
	const long duration = 200;
	unsigned long currentTime;
	unsigned long nodeStartTime = 0;
	virtual bool run() override {
		currentTime = millis();
		if (!nodeActive) {
			if (sensors.irLeftFrontCM < alarmCM && sensors.irLeftFrontCM > hardware.irMinimumCM) {
				nodeStartTime = currentTime;
				nodeActive = true;
				Serial.print(F("Permimeter left front alarm: "));
				Serial.println(sensors.irLeftFrontCM);
				motors.motorLeft->setPower(160);
				motors.motorRight->setPower(-160);
				motors.motorsActive = true;
			}
		}
		else {
			if ((nodeStartTime + duration < currentTime) && sensors.irLeftFrontCM >= alarmCM) {
				Serial.println(F("Perimeter left front complete."));
				motors.motorLeft->powerOff();
				motors.motorRight->powerOff();
				motors.motorsActive = false;
				nodeStartTime = 0;
				nodeActive = false;
			}
		}
		return nodeActive;
	}
};
class Perimeter_Right : public Behavior_Tree::Node {
private:
	bool nodeActive = false;
	uint8_t alarmCM = 11;
	const long duration = 200;
	unsigned long currentTime;
	unsigned long nodeStartTime = 0;
	virtual bool run() override {
		currentTime = millis();
		if (!nodeActive) {
			if (sensors.irRightFrontCM < alarmCM && sensors.irRightFrontCM > hardware.irMinimumCM) {
				nodeStartTime = currentTime;
				nodeActive = true;
				Serial.print(F("Permimeter right front alarm: "));
				Serial.println(sensors.irRightFrontCM);
				motors.motorLeft->setPower(-160);
				motors.motorRight->setPower(160);
				motors.motorsActive = true;
			}
		}
		else {
			if ((nodeStartTime + duration < currentTime) && sensors.irRightFrontCM >= alarmCM) {
				Serial.println(F("Perimeter right front complete."));
				motors.motorLeft->powerOff();
				motors.motorRight->powerOff();
				motors.motorsActive = false;
				nodeStartTime = 0;
				nodeActive = false;
			}
		}
		return nodeActive;
	}
};

class Random_Action : public Behavior_Tree::Node {
	//This function is checked every checkInterval to see if it should run.
	//The interval is randomized, as is the duration of the time doing nothing.
private:
	bool nodeActive = false;
	long checkInterval = 5000;
	long duration = 4000;
	unsigned long currentTime;
	unsigned long lastCheck = 0;
	unsigned long nodeStartTime = 0;
	int percentChance = 10;
	virtual bool run() override {
		currentTime = millis();
		if (lastCheck == 0) {
			lastCheck = currentTime;
		}
		if (!nodeActive){
			if (lastCheck + checkInterval < currentTime) {
				lastCheck = currentTime;
				long randNum = random(1, 101);
				Serial.print(F("random number: "));
				Serial.println(randNum);
				if (randNum <= percentChance) {
					nodeActive = true;
					nodeStartTime = currentTime;
					Serial.println(F("Doing nothing."));
					motors.motorLeft->powerOff();
					motors.motorRight->powerOff();
					motors.motorsActive = false;
				}
			}
		}
		else {
			if (nodeStartTime + duration < currentTime) {
				Serial.println(F("Doing nothing finished."));
				nodeActive = false;
				nodeStartTime = 0;
			}
		}
		return nodeActive;
	}
};

class Cruise_Forward : public Behavior_Tree::Node {
private:
	bool nodeActive = true;
	virtual bool run() override {
		if (!motors.motorsActive)
		{
			Serial.println(F("Cruising."));
			motors.motorsActive = true;
			//motors.motorLeft->setPower(120);
			//motors.motorRight->setPower(120);
		}
		return nodeActive;
	}
};

#endif

