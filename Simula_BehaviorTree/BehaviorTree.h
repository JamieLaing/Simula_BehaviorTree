// BehaviorTree.h

#ifndef _BEHAVIORTREE_h
#define _BEHAVIORTREE_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

#include "Motor.h"
#include "Sensor_State.h"
#include <StandardCplusplus.h>
#include <list>
#include <vector>
#include <initializer_list>
#include <algorithm>

class BehaviourTree {  // Note:  A proper copy constructor and assignment operator should be defined, since the implicit ones use shallow copies only.
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
		std::vector<Node*> childrenShuffled() const { std::vector<Node*> temp = children;  std::random_shuffle(temp.begin(), temp.end());  return temp; }
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
		friend class BehaviourTree;
		void setChild(Node* newChild) { child = newChild; }
		virtual bool run() override { return child->run(); }
	};
private:
	Root* root;
public:
	BehaviourTree() : root(new Root) {}
	void setRootChild(Node* rootChild) const { root->setChild(rootChild); }
	bool run() const { return root->run(); }
};

struct TREE_STATE {
	String currentNode;
	unsigned long currentTime;
	unsigned long nodeStartTime;
	bool nodeTask1Started;
};
extern struct TREE_STATE treeState;

class Action : public BehaviourTree::Node {
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

class ButtonStop : public BehaviourTree::Node {
private:
	String name = "Button";
	bool buttonState = false;
	int lastButtonState = HIGH;
	unsigned long debounceTime;
	const long debounceDelay = 10;
	virtual bool run() override{
		int reading = digitalRead(hardware.pinButton);
		if (reading != lastButtonState) {
			debounceTime = millis();
		}

		if ((millis() - debounceTime) > debounceDelay) {
			if (reading != buttonState) {
				buttonState = reading;
				if (buttonState == HIGH) {
					sensorState.buttonPressed = !sensorState.buttonPressed;
					Serial.print(F("Button toggle: "));
					Serial.println(sensorState.buttonPressed);
					if (sensorState.buttonPressed)
					{
						treeState.currentNode = name;
						motors.motorLeft->powerOff();
						motors.motorRight->powerOff();
					}
					else {
						treeState.currentNode = "";
					}
				}
			}
		}
		lastButtonState = reading;
		return sensorState.buttonPressed;
	}
};

class CliffCenter : public BehaviourTree::Node {
private:
	String name = "Cliff Center";
	const long duration = 200;
	virtual bool run() override {
		if (!name.equals(treeState.currentNode))
		{
			if (sensorState.irLeftCliff && sensorState.irRightCliff) {
				Serial.println(F("Cliff center detected."));
				treeState.currentNode = name;
				treeState.nodeStartTime = treeState.currentTime;
				motors.motorLeft->setPower(-120);
				motors.motorRight->setPower(-120);
				return true;
			}
		}
		else {
			Serial.println(F("Running cliff center."));
			if ((treeState.nodeStartTime + duration < treeState.currentTime) && (!sensorState.irLeftCliff && !sensorState.irRightCliff)) {
				treeState.currentNode = "";
				treeState.nodeStartTime = 0;
				motors.motorLeft->powerOff();
				motors.motorRight->powerOff();
			}
		}
		return false;
	}
};

class CliffLeft : public BehaviourTree::Node {
private:
	String name = "Cliff Left";
	const long backMillis = 400;
	const long turnMillis = 400;
	bool turnStarted;
	virtual bool run() override {
		if (!name.equals(treeState.currentNode)) {
			if (sensorState.irLeftCliff && !sensorState.irRightCliff) {
				Serial.println(F("Cliff left detected."));
				treeState.currentNode = name;
				turnStarted = false;
				treeState.nodeStartTime = treeState.currentTime;
				motors.motorLeft->setPower(-120);
				motors.motorRight->setPower(-120);
				return true;
			}
		}
		else {
			if ((treeState.nodeStartTime + backMillis < treeState.currentTime) && !turnStarted) {
				Serial.println(F("Cliff left turning."));
				turnStarted = true;
				motors.motorLeft->setPower(120);
				motors.motorRight->setPower(-120);
			}
			if (treeState.nodeStartTime + backMillis + turnMillis < treeState.currentTime) {
				Serial.println(F("Cliff left stopping."));
				treeState.currentNode = "";
				treeState.nodeStartTime = 0;
				motors.motorLeft->powerOff();
				motors.motorRight->powerOff();
			}
		}
		return false;
	}
};

class CliffRight : public BehaviourTree::Node {
private:
	String name = "Cliff Right";
	const long backMillis = 400;
	const long turnMillis = 400;  //durations not additive, subtract duration1 to find execution time
	bool turnStarted;
	virtual bool run() override {
		if (!name.equals(treeState.currentNode)) {
			if (!sensorState.irLeftCliff && sensorState.irRightCliff) {
				Serial.println(F("Cliff right detected."));
				treeState.currentNode = name;
				turnStarted = false;
				treeState.nodeStartTime = treeState.currentTime;
				motors.motorLeft->setPower(-120);
				motors.motorRight->setPower(-120);
				return true;
			}
		}
		else {
			if ((treeState.nodeStartTime + backMillis < treeState.currentTime) && !turnStarted) {
				Serial.println(F("Cliff right turning."));
				turnStarted = true;
				motors.motorLeft->setPower(-120);
				motors.motorRight->setPower(120);
			}
			if (treeState.nodeStartTime + backMillis + turnMillis < treeState.currentTime) {
				Serial.println(F("Cliff right stopping."));
				treeState.currentNode = "";
				treeState.nodeStartTime = 0;
				motors.motorLeft->powerOff();
				motors.motorRight->powerOff();
			}
		}
		return false;
	}
};

class Cruise : public BehaviourTree::Node {
private:
	String name = "Cruise";
	//bool nodeActivated = false;
	virtual bool run() override {
		if (treeState.currentNode.equals("")) {
			treeState.currentNode = name;
			Serial.println(F("Cruising."));
			motors.motorLeft->setPower(120);
			motors.motorRight->setPower(120);
		}
		return true;
	}
};

#endif

